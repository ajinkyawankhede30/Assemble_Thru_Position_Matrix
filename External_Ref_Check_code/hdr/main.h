#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

#include <ProToolkit.h>
#include <ProAssembly.h>
#include <ProRefInfo.h>
//#include <ProMenu.h>
#include <ProUICmd.h>
#include <ProUtil.h>
#include <ProSolid.h>
#include <ProFeatType.h>
#include <ProCopyGeom.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProTKRunTime.h>
#include <TestError.h>
#include <ProAsmcomp.h>
#include <ProWchar.h>

//ProError CheckExtrnRef();
ProError UserAsmCompVisit();
ProError UserAsmCompFilter(ProFeature* feature,ProAppData app_data);
ProError UserAction(ProFeature* feature, ProErr status, ProAppData app_data);
ProError ProTestRefExtIfoArrProcess(FILE* qcr_ref,ProExtRefInfo* infos,int n_infos);
ProError ProTestRefExtInfoProcess(FILE* qcr_ref,ProExtRefInfo* info);
ProError ProTestExtFeatRefProcess(FILE* qcr_ref, ProExtFeatRef ref);
ProError ProTestSolidInfoWrite(FILE* qcr_ref, ProSolid ref);
void ReturnMdlName(ProSolid ref, ProCharLine mdl_name);
