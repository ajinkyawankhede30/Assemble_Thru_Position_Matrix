#pragma once
#define	_CRT_SECURE_NO_WARNINGS
#define	CURL_STATICLIB

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <filesystem>
#include <map>
#include <memory>
#include <thread>

#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProAssembly.h>
#include <ProSolid.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <ProUI.h>
#include <ProUIMessage.h>
#include <TestError.h>
#include <ProWchar.h>
#include <ProUICmd.h>
#include <ProFeatType.h>
#include <ProFamtable.h>
#include <ProFaminstance.h>
#include <ProUIDialog.h>
#include <ProUIPushbutton.h>
#include <ProUIInputpanel.h>
#include <ProUILabel.h>
#include <ProShrinkwrap.h>


#include "curl.h"


static FILE* file = nullptr;
static const char logpath[PRO_PATH_SIZE] = "C:\\tmp\\DPDSCreoTest.txt";
static wchar_t UserMsg[50];

ProError APL_Assembly_Creation_Dialog();


