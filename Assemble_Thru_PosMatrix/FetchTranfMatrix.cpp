#include "FetchTranfMatrix.h"
 
//#define ASSERT(x) if(!x) __debugbreak

//#define Status(x)
 
FILE* fp;

ProError status;

//const char log[PRO_PATH_SIZE] = "C:\\tmp\\FetchTransfMatrix.txt";
 
struct MatrixData 

{

	std::string comp_matrix;

	std::string comp_name;

};
 
std::vector<MatrixData> matrixData;
 
typedef struct user_appdata

{

	FILE* fp = nullptr;

	int level = 0;

	std::vector<std::string> asmCompNames;

	std::vector<ProAsmcomppath> asmcompPathArr;

	//std::vector<ProMatrix> asmcomp_matrices;
 
} UserAppdata;
 
 
ProError UserAsmCompFilter(ProFeature* feature, ProAppData app_data)

{

	ProError status;

	ProFeattype ftype;
 
	status = ProFeatureTypeGet(feature, &ftype);
 
	if (ftype == PRO_FEAT_COMPONENT)

		return(PRO_TK_NO_ERROR);
 
	return(PRO_TK_CONTINUE);

}
 
ProError visit_action(ProFeature* feature, ProError status, ProAppData appData)

{

	UserAppdata*	appd, appd1;

	appd =			(UserAppdata*)appData;

	FILE* fp =		appd->fp;

	int level =		appd->level;

	ProMdl			mdl;

	ProIdTable		comp_id_table;

	ProAsmcomppath	asmCompPath;

	ProName			w_mdlName;

	char			asmCompName[PRO_NAME_SIZE];

	char			asmCompName2[PRO_NAME_SIZE];

	char			type[PRO_MDLEXTENSION_SIZE];

	ProMdlExtension modelExtension;
 
	status = ProAsmcompMdlGet(feature, &mdl);

	fprintf(fp, "ProAsmcompMdlGet: %d \n", status);

	fflush(fp);
 
	status = ProMdlMdlnameGet(mdl, w_mdlName);

	ProWstringToString(asmCompName, w_mdlName);

	fprintf(fp, "Assembly Comp: %s \n", asmCompName);

	status = ProMdlExtensionGet(mdl, modelExtension);

	ProWstringToString(type, modelExtension);

	fprintf(fp, "Assembly Comp type: %s \n", type);

	fflush(fp);
 
	//for traveling upto n level in assembly

	if (strncmp(type, "ASM", 3) == 0)

	{

		appd1.fp = appd->fp;

		appd1.level = appd->level + 1;
 
		fprintf(fp, "At Level: %d \n ProSolidFeatVisit: %d \n", appd1.level, status);

		fflush(fp);

		status = ProSolidFeatVisit((ProSolid)mdl, visit_action, UserAsmCompFilter, &appd1);

	}
 
	ProMdl parent_asm = feature->owner;
 
	comp_id_table[0] = feature->id;

	comp_id_table[1] = -1;

	status = ProAsmcomppathInit((ProSolid)parent_asm, comp_id_table, 1, &asmCompPath);

	fprintf(fp, "ProAsmcomppathInit: %d \n\n", status);
 
	ProMatrix matrix;

	status = ProAsmcomppathTrfGet(&asmCompPath, PRO_B_TRUE, matrix);

	fprintf(fp, "ProAsmcomppathTrfGet: %d \n", status);
 
	std::string matrix_str;

	status = GetPostMatrixInString_(matrix, matrix_str);
 
	MatrixData data;

	data.comp_matrix = matrix_str;

	data.comp_name = asmCompName;
 
	matrixData.push_back(data);
 
	appd->asmCompNames.push_back(asmCompName); //comp name

	appd->asmcompPathArr.push_back(asmCompPath); //comp path

	//appd->asmcomp_matrices.push_back(matrix); //comp matrix

	return status;

}
 
ProError GetPostMatrixInString_(ProMatrix in_matrix, std::string& out_matrix)

{

	std::string str;

	for (int i = 0; i < 4; i++)

	{

		for (int j = 0; j < 4; j++)

		{

			str = str + std::to_string(in_matrix[i][j]) + " ";

		}

	}

	out_matrix = str;

	return (PRO_TK_NO_ERROR);

}
 
ProError GetPostMatrixInString(ProAsmcomppath asmCompPath, std::string& out_matrix)

{

	ProMatrix matrix;

	std::string str;

	status = ProAsmcomppathTrfGet(&asmCompPath, PRO_B_TRUE, matrix);

	fprintf(fp, "ProAsmcomppathTrfGet: %d \n", status);
 
	for (int i = 0; i < 4; i++)

	{

		for (int j = 0; j < 4; j++)

		{

			str = str + std::to_string(matrix[i][j]) + " ";

		}

	}

	out_matrix = str;

	return (PRO_TK_NO_ERROR);

}
 
ProError FetchTransfMatrix()

{

	fopen_s(&fp,"C:\\tmp\\FetchTransfMatrix.txt", "w");

	//Get Parent Assembly

	ProMdl				current_mdl;

	ProMdlName			asm_name;

	ProMdlType			mdl_type;

	UserAppdata			appD;
 
	appD.fp = fp;

	appD.level = 1;
 
	status = ProMdlCurrentGet(&current_mdl);

	fprintf(fp, "ProMdlCurrentGet: %d \n", status);
 
	status = ProMdlMdlnameGet(current_mdl, asm_name);

	fprintf(fp, "ProMdlMdlnameGet: %d \n", status);
 
	status = ProMdlTypeGet(current_mdl, &mdl_type);

	fprintf(fp, "ProMdlTypeGet: %d \n", status);
 
	if (mdl_type != ProMdlType::PRO_MDL_ASSEMBLY)

	{

		ProUIMessageButton* buttons;

		ProUIMessageButton user_choice;

		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
 
		buttons[0] = PRO_UI_MESSAGE_OK;

		status = ProUIMessageDialogDisplay(ProUIMessageType::PROUIMESSAGE_ERROR, (wchar_t*)L"Invalid", (wchar_t*)L"This tool works only for assembly models.", buttons, PRO_UI_MESSAGE_OK, &user_choice);
 
		ProArrayFree((ProArray*)buttons);

	}
 
	//Get asm component path in vector

	fprintf(fp, "At Level: %d \n ProSolidFeatVisit: %d \n", appD.level, status);

	fflush(fp);

	status = ProSolidFeatVisit(ProMdlToSolid(current_mdl), (ProFeatureVisitAction)visit_action, UserAsmCompFilter, &appD);

	fprintf(fp, "ProSolidFeatVisit: %d \n", status);

	fflush(fp);
 
	if (!matrixData.empty())

	{

		int comp_Count = matrixData.size();
 
		//Create a file

		std::ofstream write;

		char matrixfile[PRO_PATH_SIZE] = "C:\\tmp\\PosMatrix_";

		char s_asmName[PRO_NAME_SIZE];

		ProWstringToString(s_asmName, asm_name);

		//strcat(matrixfile, s_asmName);

		//strcat(matrixfile, (const char*)".txt");
 
		strcat_s(matrixfile, s_asmName);

		strcat_s(matrixfile, (const char*)".txt");
 
		write.open(matrixfile, std::ios_base::out | std::ios_base::trunc);

		if (write.is_open())

		{

			//write << "Loaded Assembly: " << s_asmName << "\n";

			for (int i = 0; i < comp_Count; i++)

			{

				fprintf(fp, "Processing %s \n", matrixData[i].comp_name.c_str());

				//GetPostMatrixInString_(asmcomp_matrices[i], matrix_str);

				if (!matrixData[i].comp_matrix.empty())

					write << matrixData[i].comp_name << ";" << "-" << ";" << matrixData[i].comp_matrix << "\n";

			}

		}

		write.close();

	}
 
#if 0

	if (!appD.asmcompPathArr.empty())

	{

		//int comp_Count = (int)appD.asmcompPathArr.size();

		int comp_Count = (int)appD.asmCompNames.size();

		int path_count = appD.asmcompPathArr.size();

		fprintf(fp, "Name Count: %d\n", comp_Count);

		fprintf(fp, "AsmCompPath Count: %d\n", path_count);
 
		//Create a file

		std::ofstream write;

		char matrixfile[PRO_PATH_SIZE] = "C:\\tmp\\PosMatrix_";

		char s_asmName[PRO_NAME_SIZE];

		ProWstringToString(s_asmName, asm_name);

		//strcat(matrixfile, s_asmName);

		//strcat(matrixfile, (const char*)".txt");
 
		strcat_s(matrixfile, s_asmName);

		strcat_s(matrixfile, (const char*)".txt");
 
		write.open(matrixfile, std::ios_base::out | std::ios_base::trunc);

		if (write.is_open())

		{

			//write << "Loaded Assembly: " << s_asmName << "\n";

			for (int i = 0; i < comp_Count; i++)

			{

				std::string matrix_str;

				fprintf(fp, "Processing %s \n", appD.asmCompNames[i].c_str());

				GetPostMatrixInString(appD.asmcompPathArr[i], matrix_str);

				//GetPostMatrixInString_(asmcomp_matrices[i], matrix_str);

				if (!matrix_str.empty())

					write << appD.asmCompNames[i] << ";" << "-" << ";" << matrix_str << "\n";

			}

		}

		write.close();

	}

#endif // 0
 
	fclose(fp);

	return PRO_TK_NO_ERROR;

}
 
///Code is runnig for n level components. It is able to fetch the matrix of all level components and later assemble them in any foriegn assembly.
 