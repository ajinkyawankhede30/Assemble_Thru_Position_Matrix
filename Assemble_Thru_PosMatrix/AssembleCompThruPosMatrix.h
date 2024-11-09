#pragma once
#include "DPDSCreoTest.h"


struct CompMatrix
{
	std::string PartNo;
	std::string part_name;
	ProMatrix comp_matrix;
};

static FILE* fp = nullptr;
static char logFile[PRO_PATH_SIZE] = "C:\\tmp\\AssembleComponentsLog.txt";
//static ProUIMessageButton* buttons; //static added to prevent the ambiguity from other cpp files
//static ProUIMessageButton user_choice;

ProError AssembleComponents();
ProError AssembleAsmComponents(ProMdl ParentAssembly, std::vector<CompMatrix> CompData);
int ReadXLData(std::string module_number);
void MapPositionMatrix(const std::string filepath);
