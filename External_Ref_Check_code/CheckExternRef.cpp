#pragma once
#include <main.h>


typedef struct user_appdata
{
    FILE* fp;
    int level;
} UserAppdata;


static uiCmdAccessState AccessFunction(uiCmdAccessMode mode)
{
    return uiCmdAccessState::ACCESS_AVAILABLE;
}


/*====================================================================*\
    FUNCTION :  UserAsmCompFilter()
    PURPOSE  :  A filter used by ProSolidFeatVisit() to visit
                features which are assembly components
\*====================================================================*/
ProError UserAsmCompFilter(
    ProFeature* feature,
    ProAppData app_data)
{
    ProError status;
    ProFeattype ftype;

    /*--------------------------------------------------------------------*\
        Get the feature type
    \*--------------------------------------------------------------------*/
    status = ProFeatureTypeGet(feature, &ftype);

    /*--------------------------------------------------------------------*\
        If the feature is an assembly component,
            return NO ERROR,
        else
            return CONTINUE
    \*--------------------------------------------------------------------*/
    if (ftype == PRO_FEAT_COMPONENT)
        return(PRO_TK_NO_ERROR);
    return(PRO_TK_CONTINUE);
}

/*---------------------------------------------------------------------*\
Function : to write out the members of the current assembly, and display
        the result in an information window.
\*---------------------------------------------------------------------*/
ProError UserAsmCompVisit()
{
    char name[PRO_MDLNAME_SIZE];
    char type[PRO_MDLEXTENSION_SIZE];
    wchar_t wname[PRO_NAME_SIZE];
    ProError err;
    UserAppdata appdata;
    FILE* fp;
    ProMdl p_asm;
    ProMdlName modelName;
    ProMdlExtension modelExtension;
    /*---------------------------------------------------------------------*\
        Open the text file.
    \*---------------------------------------------------------------------*/
    char fileName[PRO_FILE_NAME_SIZE] = "C:\\tmp\\Chk_Ext_assembly_references.log";
    fopen_s(&fp, fileName, "w");

    err = ProMdlCurrentGet(&p_asm);

    err = ProMdlMdlnameGet(p_asm, modelName);

    err = ProMdlExtensionGet(p_asm, modelExtension);
    
    ProWstringToString(name, modelName);
    ProWstringToString(type, modelExtension);
    ProTKFprintf(fp, "%s	%s\n", name, type);
    appdata.fp = fp;
    appdata.level = 1;

    /*---------------------------------------------------------------------*\
        List the assembly members
    \*---------------------------------------------------------------------*/
    err = ProSolidFeatVisit((ProSolid)p_asm, UserAction, UserAsmCompFilter, &appdata);
    ProTKFprintf(fp, "ProSolidFeatVisit Error: %d\n", err);

    /*-------- ------------------------------------------------------------*\
        Close the file, and display it.
    \*---------------------------------------------------------------------*/
    fflush(fp);
    fclose(fp);
    ProStringToWstring(wname, fileName);
    err = ProInfoWindowDisplay(wname, NULL, NULL);
    return(PRO_TK_NO_ERROR);
}


ProError UserAction(ProFeature* feature, ProErr status, ProAppData app_data)
{
    FILE* fp;
    int i, level, n_info;
    UserAppdata* appd, appd1;
    //ProExtRefType reftype = ProExtRefType::PRO_ALL_REF_TYPES;
    ProExtRefInfo* ref_info;
    ProMdl mdl;
    ProName modelName;
    ProMdlExtension modelExtension;
    ProMdlType mdlType;
    char name[PRO_NAME_SIZE];
    char type[PRO_EXTENSION_SIZE];
    appd = (UserAppdata*)app_data;
    fp = appd->fp;
    level = appd->level;

    status = ProAsmcompMdlGet(feature, &mdl);

    status = ProMdlTypeGet(mdl, &mdlType);

    status = ProMdlMdlnameGet(mdl, modelName);

    status = ProMdlExtensionGet(mdl, modelExtension);

    ProWstringToString(name, modelName);
    ProWstringToString(type, modelExtension);
    for (i = 0; i < level; i++)
        ProTKFprintf(fp, "    ");
    ProTKFprintf(fp, "%s %s\n", name, type);

    if (strncmp(type, "ASM", 3) == 0 || strncmp(type, "PRT",3) == 0)
    {
        //appd1.fp = appd->fp;
        //appd1.level = appd->level + 1;

        //status = ProSolidExternParentsGet((ProSolid)feature, ProExtRefType::PRO_ALL_REF_TYPES, &ref_info, &n_info);
        status = ProSolidExternParentsGet((ProSolid)mdl, ProExtRefType::PRO_ALL_EXT_REF_TYPES, &ref_info, &n_info);
        ProTKFprintf(fp, "ProSolidExternParentsGet status: %d\n", status);

        if (status != PRO_TK_NO_ERROR)
            return PRO_TK_CONTINUE;

        if (n_info > 0)
        {
            ProTKFprintf(fp, "Number of elements in active Asm: %d\n", n_info);

            status = ProTestRefExtIfoArrProcess(fp, ref_info, n_info);
            ProTKFprintf(fp, "ProTestRefExtIfoArrProcess status: %d\n", status);

            status = ProExtRefInfoFree(&ref_info);
            ProTKFprintf(fp, "ProExtRefInfoFree status: %d\n\n", status);
        }
        //status = ProSolidFeatVisit((ProSolid)mdl, UserAction, UserAsmCompFilter, &appd1);
        //ProTKFprintf(fp, "ProSolidFeatVisit status: %d  Level: %d\n", status, level);

    }

    return status;
}

/*====================================================================*\
  Function : ProTestRefExtIfoArrProcess()
  Purpose  :
\*====================================================================*/
ProError ProTestRefExtIfoArrProcess(
    FILE* qcr_ref,
    ProExtRefInfo* infos,
    int n_infos)
{
    ProError err = PRO_TK_NO_ERROR;
    int i;

    for (i = 0; i < n_infos; i++)
        err = ProTestRefExtInfoProcess(qcr_ref, &(infos[i]));

    return (err);
}

/*====================================================================*\
  Function : ProTestRefExtInfoProcess()
  Purpose  :
\*====================================================================*/
ProError ProTestRefExtInfoProcess(
    FILE* qcr_ref,
    ProExtRefInfo* info)
{
    ProError err = PRO_TK_NO_ERROR;
    int i;

    ProTKFprintf(qcr_ref, "Ext. ref. info : type %d n_refs : %d\n",
        info->type, info->n_refs);

    for (i = 0; i < info->n_refs; i++)
    {
        ProTKFprintf(qcr_ref, "The number of ProExtFeatRef item : %d \n", i);
        err = ProTestExtFeatRefProcess(qcr_ref, info->ext_refs[i]);
    }

    ProTKFprintf(qcr_ref, "End of ext. ref. info\n\n\n");
    return (err);
}

/*====================================================================*\
  Function : ProTestExtFeatRefProcess()
  Purpose  :
\*====================================================================*/
ProError  ProTestExtFeatRefProcess(
    FILE* qcr_ref,
    ProExtFeatRef ref)
{
    ProError err = PRO_TK_NO_ERROR;
    ProRefState state;
    ProType type;
    ProAsmcomppath own_comp, ref_comp;
    ProSolid solid;
    ProFeature feature;
    ProModelitem modelitem;
    char mdlName[PRO_MDLNAME_SIZE];
    int i;

    own_comp.table_num = ref_comp.table_num = 0;
    feature.type = PRO_TYPE_UNUSED;
    modelitem.type = PRO_TYPE_UNUSED;

    err = ProExtRefStateGet(ref, &state);
    ProTKFprintf(qcr_ref, "Reference state : %d\n", state);

    err = ProExtRefTypeGet(ref, &type);
    ProTKFprintf(qcr_ref, "Reference type : %d\n", type);
    
    ///Retreive the asm compoent paths of owner and reference model///

    //err = ProExtRefAsmcompsGet(ref, &own_comp, &ref_comp);
    ////ProTKFprintf(qcr_ref, "ProExtRefAsmcompsGet status: %d\n", err);
    //
    //if (ref_comp.table_num != 0)
    //{
    //    ProTKFprintf(qcr_ref, "owner's asmcomppath model:\n");
    //    err = ProTestSolidInfoWrite(qcr_ref, own_comp.owner);
    //    ProTKFprintf(qcr_ref, "Reference's asmcomppath model:\n");
    //    err = ProTestSolidInfoWrite(qcr_ref, ref_comp.owner);
    //}
    
    //ProTKFprintf(qcr_ref, "\nOwner's asmcomppath:\n");
    //for (i = 0; i < own_comp.table_num; i++)
    //    ProTKFprintf(qcr_ref, "%d ", own_comp.comp_id_table[i]);
    //
    //ProTKFprintf(qcr_ref, "\nReference's asmcomppath:\n");
    //for (i = 0; i < ref_comp.table_num; i++)
    //    ProTKFprintf(qcr_ref, "%d ", ref_comp.comp_id_table[i]);

    err = ProExtRefOwnMdlGet(ref, &solid);
    ReturnMdlName(solid, mdlName);
    ProTKFprintf(qcr_ref, "\nOwner model: %s\n" , mdlName);
    //err = ProTestSolidInfoWrite(qcr_ref, solid);

    err = ProExtRefMdlGet(ref, &solid);
    ReturnMdlName(solid, mdlName);
    ProTKFprintf(qcr_ref, "Reference model: %s\n", mdlName);
    //err = ProTestSolidInfoWrite(qcr_ref, solid);

    //Get owner feature info
    err = ProExtRefOwnFeatGet(ref, &feature);

    if (feature.type != PRO_TYPE_UNUSED)
    {
        ProTKFprintf(qcr_ref, "Owner feature ID: %d\n", feature.id);
        ProTKFprintf(qcr_ref, "Owner feature type: %d\n", feature.type);
        //err = ProTestSolidInfoWrite(qcr_ref, (ProSolid)feature.owner);
    }

    //Get reference feature info
    err = ProExtRefFeatGet(ref, &feature);

    if (feature.type != PRO_TYPE_UNUSED)
    {
        ProTKFprintf(qcr_ref, "Reference feature ID: %d\n", feature.id);
        ProTKFprintf(qcr_ref, "Reference feature type: %d\n", feature.type);
        //err = ProTestSolidInfoWrite(qcr_ref, (ProSolid)feature.owner);
    }

    err = ProExtRefModelitemGet(ref, &modelitem);

    if (modelitem.type != PRO_TYPE_UNUSED)
    {
        ProTKFprintf(qcr_ref, "Reference modelitem ID: %d\n", modelitem.id);
        ProTKFprintf(qcr_ref, "Reference modelitem type: %d\n", modelitem.type);
        //err = ProTestSolidInfoWrite(qcr_ref, (ProSolid)modelitem.owner);
    }

    return (err);
}

void ReturnMdlName(ProSolid ref, char* mdl_name)
{
    ProName w_mdl_name;
    //ProCharLine mdl_name;
    ProError status = ProMdlNameGet((ProMdl)ref, w_mdl_name);
    ProWstringToString(mdl_name, w_mdl_name);
    // mdl_name;
}

ProError ProTestSolidInfoWrite(FILE* qcr_ref, ProSolid ref)
{
    ProName w_mdl_name;
    ProCharLine mdl_name;
    ProError status = ProMdlNameGet((ProMdl)ref, w_mdl_name);
    ProWstringToString(mdl_name, w_mdl_name);
    ProTKFprintf(qcr_ref, "COMPONENT NAME: %s \n\n", mdl_name);
    return status;
}

extern "C" int user_initialize()
{
    ProError status;
    uiCmdCmdId cmd_id_1;
    FILE* fp1;
    fopen_s(&fp1, "C:\\tmp\\Check_External_references.log", "w");

    wchar_t UserMsg2[PRO_FILE_NAME_SIZE];
    ProStringToWstring(UserMsg2, (char*)"UserMsg.txt");

    //status = ProCmdActionAdd((char*)"Check_Extrn_Ref", (uiCmdCmdActFn)CheckExtrnRef, uiProeImmediate, AccessFunction, PRO_B_TRUE, PRO_B_TRUE, &cmd_id_1);
    status = ProCmdActionAdd((char*)"Check_Extrn_Ref", (uiCmdCmdActFn)UserAsmCompVisit, uiProeImmediate, AccessFunction, PRO_B_TRUE, PRO_B_TRUE, &cmd_id_1);
    ProTKFprintf(fp1, "Status for ProCmdActionAdd Check_Extrn_Ref = %d \n", status);
    fflush(fp1);

    status = ProCmdIconSet(cmd_id_1, (char*)"plus.png");
    ProTKFprintf(fp1, "Status for ProCmdIconSet Check_External_References = %d \n", status);
    fflush(fp1);

    //char btn_lbl[] = "DPDSCreoApps# chckexternref_label";
    //char btn_hlp[] = "DPDSCreoApps# chckexternref_help";
    //char btn_desp[] = "DPDSCreoApps# chckexternref_descp";

     
    ProCmdItemIcon btn_lbl = "Check_Extern_Ref lbl";
    ProCmdLineHelp btn_hlp = "Check_Extern_Ref Help";
    ProCmdDescription btn_desp ="Check_Extern_Ref Desc";

    status = ProCmdDesignate(cmd_id_1, btn_lbl, btn_hlp, btn_desp, UserMsg2);
    ProTKFprintf(fp1, "Status for ProCmdDesignate Check_External_References = %d \n", status);
    fflush(fp1);

    fclose(fp1);
    
    return PRO_TK_NO_ERROR;
}

extern "C" void user_terminate()
{

    //printf("user_terminate\n");

}
