#pragma once
 
#include "AssembleCompThruPosMatrix.h"

#include "FetchTranfMatrix.h"

#include "LoadAssembly.h"
 
 
 
static uiCmdAccessState TestAccessDefault(uiCmdAccessMode access_mode)

{

	return (uiCmdAccessState::ACCESS_AVAILABLE);

}
 
extern "C" int user_initialize()

{

	ProError		status;

	uiCmdCmdId		cmd_id_1, cmd_id_2, cmd_id_3;

	//Log file

	fopen_s(&file, logpath, "w");

	fprintf(file, "\n------------------user_initialize------------------\n\n");

	fflush(file);
 
	ProStringToWstring(UserMsg, (char*)"DPDSCreoTest.txt");
 
	status = ProCmdActionAdd((char*)"User_Disp_Msg_1", (uiCmdCmdActFn)AssembleComponents, uiProeImmediate, TestAccessDefault, PRO_B_TRUE, PRO_B_TRUE, &cmd_id_1);

	fprintf(file, "Status for ProCmdActionAdd Apl_assemble_frame = %d \n", status);

	fflush(file);

	status = ProCmdIconSet(cmd_id_1, (char*)"aplasm.png");

	fprintf(file, "Status for ProCmdIconSet \'apl_asm\' = %d \n", status);

	fflush(file);

	status = ProCmdDesignate(cmd_id_1, (char*)"DPDSCreoTest# - Method1", (char*)"DPDSCreoTest Help", (char*)"DPDSCreoTest Desc", UserMsg);

	fprintf(file, "Status for ProCmdDesignate Apl_assemble_frame = %d \n", status);

	fflush(file);
 
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------*/

	//Fetch Assembly comp position matrix

	status = ProCmdActionAdd((char*)"User_Disp_Msg_2", (uiCmdCmdActFn)FetchTransfMatrix, uiProeImmediate, TestAccessDefault, PRO_B_TRUE, PRO_B_TRUE, &cmd_id_2);

	fprintf(file, "Status for ProCmdActionAdd FetchTransfMatrix = %d \n", status);

	fflush(file);
 
	status = ProCmdIconSet(cmd_id_2, (char*)"posmatrx.png");

	fprintf(file, "Status for ProCmdIconSet \'posmatrx.png\' = %d \n", status);

	fflush(file);
 
	status = ProCmdDesignate(cmd_id_2, (char*)"DPDSCreoTest# - Method2", (char*)"DPDSCreoTest Help2", (char*)"DPDSCreoTest Desc2", UserMsg);

	fprintf(file, "Status for ProCmdDesignate Fetch_Position_Matrix = %d \n", status);

	fflush(file);
 
	//Load Assembly in session from TCUA

	status = ProCmdActionAdd((char*)"User_Disp_Msg_3", (uiCmdCmdActFn)LoadAssemblyinSession, uiProeImmediate, TestAccessDefault, PRO_B_TRUE, PRO_B_TRUE, &cmd_id_3);

	fprintf(file, "ProCmdActionAdd: LoadAssemblyinSession %d", status);

	fflush(file);
 
	status = ProCmdIconSet(cmd_id_3, (char*)"download.png");

	fprintf(file, "ProCmdIconSet: LoadAssembly %d", status);

	fflush(file);
 
	status = ProCmdDesignate(cmd_id_3, (char*)"DPDSCreoTest# - Method3", (char*)"DPDSCreoTest Help3", (char*)"DPDSCreoTest Desc3", UserMsg);

	fprintf(file, "ProCmdDesignate:  LoadAssembly %d", status);

	fflush(file);
 
	return 0;

}
 
extern "C" void user_terminate()

{

	fprintf(file, "\n\n-------------------user_terminate-------------------------\n");

	fflush(file);

	fclose(file);

}
 