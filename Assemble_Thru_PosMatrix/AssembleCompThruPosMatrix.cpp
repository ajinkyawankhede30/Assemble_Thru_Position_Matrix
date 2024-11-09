#include "AssembleCompThruPosMatrix.h"
 
//using namespace libxl;

//Get the current assembly info

	//Name

	//Type

 
//Get the sub-components and its position matrix

	//Assemble each model in parent assembly

	//Apply fix constraint
 
struct ExcelData

{

	std::string itemNumber, itemDescp;

};
 
ProError AssembleComponents()

{

	fopen_s(&fp, logFile, "w");

	fprintf(fp, "----------------------Assemble Components through position matrix--------------------------------\n\n");

	fflush(fp);
 
	ProError			status = PRO_TK_NO_ERROR;

	ProMdl				MainAssemblyMld;

	ProName				w_asm_name;

	ProUIMessageButton* buttons; 

	ProUIMessageButton	user_choice;
 
	//Get current model

	status = ProMdlCurrentGet(&MainAssemblyMld);

	fprintf(fp, "ProMdlCurrentGet = %d\n", status);

	fflush(fp);

	if (status != PRO_TK_NO_ERROR) 

		return status;
 
	//Get Mdl Name

	status = ProMdlNameGet(MainAssemblyMld, w_asm_name);

	char c_asm_name[PRO_NAME_SIZE];

	ProWstringToString(c_asm_name, w_asm_name);

	fprintf(fp, "ProMdlCurrentGet: %d\t\tc_asm_name: %s\n", status, c_asm_name);

	fflush(fp);
 
	ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);

	buttons[0] = PRO_UI_MESSAGE_OK;
 
	ProMdlType cur_mdl_type;

	status = ProMdlTypeGet(MainAssemblyMld, &cur_mdl_type);

	if (cur_mdl_type != PRO_MDL_ASSEMBLY)

	{

		status = ProUIMessageDialogDisplay(PROUIMESSAGE_WARNING, (wchar_t*)L"Invalid", (wchar_t*)L"This tool works only for assembly models.", buttons, PRO_UI_MESSAGE_OK, &user_choice);

		ProArrayFree((ProArray*)&buttons);

		return PRO_TK_NO_ERROR;

	}
 
	//1. Read the exported data from excel file [item no, item descp.] and write to .txt file

	//int error = ReadXLData(c_asm_name);

	char temp[PRO_NAME_SIZE] = "TC_";

	strcat(temp, c_asm_name);

	char BOM_file[PRO_PATH_SIZE] = "C:\\tmp\\";

	strcat(BOM_file, temp);

	strcat(BOM_file, ".csv");

	fprintf(fp, "BOM_file: %s\n", BOM_file);
 
	MapPositionMatrix(BOM_file);
 
	//Get the sub-components and its position matrix from file

	std::ifstream rstream;

	//rstream.open("D:\\AW_downloads\\Work\\DPDSCreoTest\\ReadPositionMatrix.txt");

	rstream.open("C:\\tmp\\ReadPositionMatrix.txt");

	fprintf_s(fp, "\n****************************Reading ReadPositionMatrix.txt***********************\n\n");

	fflush(fp);
 
	std::vector<std::string> data;

	while (!rstream.eof())

	{

		std::string line;

		while (std::getline(rstream, line))

		{

			if (line != "")

			{

				data.push_back(line);

			}

		}
 
	}

	fprintf_s(fp, "\n****************************Closing ReadPositionMatrix.txt***********************\n\n");

	rstream.close();
 
	if (data.empty())

	{

		fprintf(fp, "Error in reading file. No data found in ReadPositionMatrix.txt.\n");

		fflush(fp);

		return PRO_TK_EMPTY;

	}
 
	std::vector<CompMatrix> CompDataVector;

	for(auto line : data)

	{

		std::istringstream ss(line);

		std::string token;

		std::vector<std::string> split_values;

		while (std::getline(ss, token, ';'))

		{

			split_values.push_back(token);

		}
 
		//Extract values

		CompMatrix compData;

		compData.PartNo = split_values[0]; //Part number

		compData.part_name = split_values[1];  //Part  Descrp.

		if (!split_values[2].empty())

		{

			std::istringstream matrix_str(split_values[2]);

			std::string matrixElem;

			int i = 0; int j = 0;

			while (std::getline(matrix_str, matrixElem, ' '))

			{

				compData.comp_matrix[i][j] = atof(matrixElem.c_str());

				//fprintf(fp, matrixElem.c_str());

				if (j == 3)

				{

					j = 0;

					i++; //row

					//fprintf(fp, "\n");

				}

				else

					j++; //column

				//for (int i = 0; i < 4; i++)

				//{

				//	for (int j = 0; j < 4; j++)

				//	{

				//		compData.comp_matrix[i][j] = atof(matrixElem.c_str());

				//	}

				//}

			}

			CompDataVector.push_back(compData);

		}

	}
 
	//Assemble Components

	if (!CompDataVector.empty())

	{

		status = AssembleAsmComponents(MainAssemblyMld, CompDataVector);

		fprintf(fp, "AssembleAsmComponents: %d \n", status);

		fflush(fp);

	}

	fclose(fp);

	return status;

}
 
ProError AssembleAsmComponents(ProMdl ParentAssembly, std::vector<CompMatrix> CompData)

{

	ProError status;

	ProFamilyMdlName wMdlPath;

	char cad_directory[PRO_PATH_SIZE] = "D:\\PLMDocs\\myuadata\\";

	ProAsmcomp out_asm_comp_hdl;

	ProPath current_dir,  newDirectory;

	char sCurrentDir[PRO_PATH_SIZE];
 
	//set FIX constraint

	ProAsmcompconstraint* constraint_arr;

	ProAsmcompconstraint constraint;

	status = ProArrayAlloc(0, sizeof(ProAsmcompconstraint), 1, (ProArray*)&constraint_arr);

	status = ProAsmcompconstraintAlloc(&constraint);

	status = ProAsmcompconstraintTypeSet(constraint, ProAsmcompConstrType::PRO_ASM_FIX);

	status = ProArrayObjectAdd((ProArray*)&constraint_arr, -1, 1, &constraint);
 
	status = ProDirectoryCurrentGet(current_dir);

	ProWstringToString(sCurrentDir, current_dir);

	fprintf(fp, "Current Directory: %s \n", sCurrentDir);

	fflush(fp);
 
	ProStringToWstring(newDirectory, cad_directory);
 
	ProDirectoryCurrentGet(newDirectory);

	fprintf(fp, "New Directory: %s \n", cad_directory);

	fflush(fp);
 
	//Loop through data to assemble sub-compoenents

	for (int i = 0; i < CompData.size(); i++)

	{

		const char* comp_id = CompData[i].PartNo.c_str();

		fprintf(fp, "%d. Component: %s\n", i+1, CompData[i].PartNo.c_str());

		char tempPath[PRO_PATH_SIZE];

		strcpy_s(tempPath, cad_directory);

		strcat_s(tempPath, comp_id);

		fprintf(fp, "Component_path: %s\n", tempPath);
 
		//strcat(cad_directory, comp_id);

		ProStringToWstring(wMdlPath, tempPath);
 
		ProMdl mdlhdl;

		//Retrieve model[part/asm] in memory

		status = ProMdlnameRetrieve(wMdlPath, (ProMdlfileType)ProMdlType::PRO_MDL_ASSEMBLY, &mdlhdl);

		fprintf(fp, "ProMdlnameRetrieve[ASSEMBLY]: %d \n", status);

		fflush(fp);
 
		//ProSolidretrievalerrs* errs = nullptr;

		//status = ProSolidRetrievalErrorsGet((ProSolid)mdlhdl, errs);

		if (status != PRO_TK_NO_ERROR)

		{

			status = ProMdlnameRetrieve(wMdlPath, (ProMdlfileType)ProMdlType::PRO_MDL_PART, &mdlhdl);

			fprintf(fp, "ProMdlnameRetrieve[PART]: %d \n", status);

			fflush(fp);
 
			//status = ProSolidRetrievalErrorsGet((ProSolid)mdlhdl, errs);

			if (status != PRO_TK_NO_ERROR)

			{

				fprintf(fp, "Part file not found in the myuadata dir.\n\n");

				fflush(fp);

				continue;

				//return PRO_TK_NO_ERROR;

			}

		}

		//delete[] comp_id; //free memory

		//Assemble Component

		status = ProAsmcompAssemble((ProAssembly)ParentAssembly, (ProSolid)mdlhdl, CompData[i].comp_matrix, &out_asm_comp_hdl);

		fprintf(fp, "ProAsmcompAssemble: %d \n", status);

		fflush(fp);
 
		if (status == PRO_TK_NO_ERROR)

		{

			//set fix constraint

			status = ProAsmcompConstraintsSet(NULL, &out_asm_comp_hdl, constraint_arr);

			fprintf(fp, "ProAsmcompConstraintsSet: %d \n\n", status);

			fflush(fp);

		}

	}

	status = ProArrayFree((ProArray*)&constraint_arr);

	fprintf(fp, "ProArrayFree: %d \n", status);
 
	status = ProSolidRegenerate((ProSolid)ParentAssembly, PRO_REGEN_NO_FLAGS);

	fprintf(fp, "ProSolidRegenerate: %d \n", status);

	fflush(fp);
 
	return status;

}
 
void MapPositionMatrix(std::string filepath)

{

	std::fstream read_stream;

	read_stream.open(filepath);

	std::vector<std::string> item_numbers;

	if (!read_stream.eof())

	{	

		std::string line;

		while (std::getline(read_stream, line))

		{

			if (line.find("BOM Line") != std::string::npos) //skip header line

				continue;

			else

			{

				size_t indx = line.find(","); //erase series number

				line.erase(0, indx+1);

				indx = line.find("/");

				line.erase(indx, line.size()-1); //erase string other than item number

				std::string newstr;

				for (int i = 0; i < line.size(); i++)

				{

					if (std::isalnum(line[i])) //remove " 

						newstr += line[i];

				}

				item_numbers.push_back(newstr);

			}

		}
 
	}

	 int item_count = item_numbers.size();

	 fprintf_s(fp, "Item_count: %d \n", item_count);

	 fflush(fp);
 
	//Read the ref assembly postion matrix file

	std::ifstream read_ref_stream;

	read_ref_stream.open("C:\\tmp\\PosMatrix_5554F9Y0131026.txt");

	fprintf_s(fp, "\n****************************Reading reference assembly PosMatrix_5554F9Y0131026.txt***********************\n\n");

	fflush(fp);

	std::multimap<std::string, std::string> mmap;	//container

	while (!read_ref_stream.eof())

	{

		std::string line;
 
		while (std::getline(read_ref_stream, line))

		{

			if (!line.empty())

			{

				std::istringstream ss(line);

				std::string token;

				std::vector<std::string> split_val;

				while (std::getline(ss, token, ';'))

				{

					split_val.push_back(token);

				}
 
				//mmap.insert([key]item_number | [value]position matrix)

				mmap.insert({ split_val[0], split_val[2] });

			}

		}
 
	}

	fprintf_s(fp, "\n****************************Closing PosMatrix_5554F9Y0131026.txt***********************\n\n");

	read_ref_stream.close();
 
	if (mmap.empty())

	{

		fprintf(fp, "Error in reading file. No data found in PosMatrix_5554F9Y0131026.txt.\n");

		fflush(fp);

		return;

	}
 
	//Write a file containiing only required item numbers along with its position matrix

	std::ofstream w_stream("C:\\tmp\\ReadPositionMatrix.txt", std::ios::trunc);

	fprintf(fp, "\n****************************Writing ReadPositionMatrix.txt***********************\n\n");

	fflush(fp);
 
	for (int i = 0;i < item_numbers.size(); i++)

	{

		if (mmap.find(item_numbers[i]) != mmap.end())

		{

			for (auto map : mmap)

			{

				if (map.first == item_numbers[i])

				{

					w_stream << map.first << ";-;" << map.second << "\n";

				}

			}

		}

	}

	fprintf(fp, "\n****************************Closing ReadPositionMatrix.txt***********************\n\n");

	fflush(fp);

	w_stream.close();
 
}
 