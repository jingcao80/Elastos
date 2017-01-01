
#include <stdio.h>
#include <assert.h>
//#include <wtypes.h>

typedef unsigned char  byte;
#include <elatypes.h>
#include <clsutil.h>

void CGenerateCARAttribs(FILE *pFile, const CLSModule *pModule)
{
    BOOL bBracket;

    if (0 == CAR_ATTR(pModule->mAttribs)
        && !pModule->mUunm
        && 0 == pModule->mMajorVersion
        && 0 == pModule->mMinorVersion)
        return;

    bBracket = FALSE;

    if (0 != pModule->mMajorVersion || 0 != pModule->mMinorVersion) {
        if (!bBracket) {
            fputs("[\n", pFile);
            bBracket = TRUE;
        }
        fprintf(pFile, "    version(%d.%d)", pModule->mMajorVersion, pModule->mMinorVersion);
    }
    if (bBracket) {
        fputs("\n]\n", pFile);
    }
}

void CGenerateLibraries(FILE *pFile, const CLSModule *pModule)
{
    int n;

    for (n = 0; n < pModule->mLibraryCount; n++) {
        fprintf(pFile,
                "    importlib(\"%s\");\n",
                pModule->mLibraryNames[n]);
    }
}

void CGenerateInterfaceDecls(FILE *pFile, const CLSModule *pModule)
{
    int n;
    InterfaceDirEntry *pEntry;
    BOOL bFirst = TRUE;

    for (n = 0; n < pModule->mInterfaceCount; n++) {
        pEntry = pModule->mInterfaceDirs[n];

        if (!pEntry->mNameSpace \
            && !(pEntry->mDesc->mAttribs & InterfaceAttrib_dual)
            && !(pEntry->mDesc->mAttribs & InterfaceAttrib_clsobj)) {
            if (bFirst) {
                bFirst = FALSE;
                fputs("\n", pFile);
            }
            fprintf(pFile, "    interface %s;\n", pEntry->mName);
        }
    }
}

void CGenerateAliases(FILE *pFile, const CLSModule *pModule)
{
    int n;
    AliasDirEntry *pEntry;
    BOOL bFirst = TRUE;

    for (n = 0; n < pModule->mAliasCount; n++) {
        pEntry = pModule->mAliasDirs[n];
        if (!pEntry->mNameSpace) {
            if (bFirst) {
                bFirst = FALSE;
                fputs("\n", pFile);
            }
            fprintf(pFile,
                "    typedef %s%s %s;\n",
                (pEntry->mIsDummyType ? "[dummytype] ":""),
                Type2CString(pModule, &pEntry->mType),
                pEntry->mName);
        }
    }
}

void CGenerateEnums(FILE *pFile, const CLSModule *pModule)
{
    int n, m;
    EnumDescriptor *pDesc;

    for (n = 0; n < pModule->mEnumCount; n++) {
        if (!pModule->mEnumDirs[n]->mNameSpace) {
            fprintf(pFile,
                        "\n"
                        "    enum %s {\n",
                        pModule->mEnumDirs[n]->mName);

            pDesc = pModule->mEnumDirs[n]->mDesc;
            for (m = 0; m < pDesc->mElementCount; m++) {
                if (pDesc->mElements[m]->mIsHexFormat) {
                    fprintf(pFile,
                            "        %s = 0x%08x,\n",
                            pDesc->mElements[m]->mName,
                            pDesc->mElements[m]->mValue);
                }
                else {
                    fprintf(pFile,
                            "        %s = %d,\n",
                            pDesc->mElements[m]->mName,
                            pDesc->mElements[m]->mValue);
                }
            }
            fputs("    }\n", pFile);
        }
    }
}

void CGenerateConsts(FILE *pFile, const CLSModule *pModule)
{
    int n;

    for (n = 0; n < pModule->mConstCount; n++) {
        if (!pModule->mConstDirs[n]->mNameSpace) {
            fprintf(pFile,
                        "\n"
                        "    const %s = ",
                        pModule->mConstDirs[n]->mName);
            if (pModule->mConstDirs[n]->mType == TYPE_INTEGER32) {
                if (pModule->mConstDirs[n]->mV.mInt32Value.mIsHexFormat) {
                    fprintf(pFile,
                            "0x%08x;\n",
                            pModule->mConstDirs[n]->mV.mInt32Value.mValue);
                }
                else {
                    fprintf(pFile,
                            "%d;\n",
                            pModule->mConstDirs[n]->mV.mInt32Value.mValue);
                }
            }
            else {
                assert(pModule->mConstDirs[n]->mType == TYPE_STRING);
                if (pModule->mConstDirs[n]->mV.mStrValue.mValue != NULL) {
                     fprintf(pFile,
                            "\"%s\";\n",
                            pModule->mConstDirs[n]->mV.mStrValue.mValue);
                }
            }
        }
    }
}

void CGenerateStructs(FILE *pFile, const CLSModule *pModule)
{
    int n, m;
    StructDescriptor *pDesc;
    TypeDescriptor elemType;

    for (n = 0; n < pModule->mStructCount; n++) {
        if (!pModule->mStructDirs[n]->mNameSpace) {
            fprintf(pFile,
                    "\n"
                    "    struct %s {\n",
                    pModule->mStructDirs[n]->mName);

            pDesc = pModule->mStructDirs[n]->mDesc;
            for (m = 0; m < pDesc->mElementCount; m++) {
                elemType = pDesc->mElements[m]->mType;
                if (Type_Array == elemType.mType) { //Handle Array
                    fprintf(pFile,
                            "        %s;\n",
                            Array2CString(pModule, &elemType, pDesc->mElements[m]->mName));
                }
                else {
                    fprintf(pFile,
                            "        %s %s;\n",
                            Type2CString(pModule, &pDesc->mElements[m]->mType),
                            pDesc->mElements[m]->mName);
                }
            }
            fputs("    }\n", pFile);
        }
    }
}

void CGenerateInterfaceAttribs(
    FILE *pFile, const InterfaceDescriptor *pDesc)
{
    if (0 == INTERFACE_ATTR(pDesc->mAttribs))
        return;

    fputs("    [\n", pFile);
    if (pDesc->mAttribs & InterfaceAttrib_local) {
        fputs("        local", pFile);
    }
    if (pDesc->mAttribs & InterfaceAttrib_oneway) {
        fputs("        oneway", pFile);
    }
    if (pDesc->mAttribs & InterfaceAttrib_parcelable) {
        fputs("        parcelable", pFile);
    }

    fputs("\n    ]\n", pFile);
}

void CGenerateInterfaceConst(FILE *pFile,
    const CLSModule *pModule, InterfaceConstDescriptor *pConst)
{
    fprintf(pFile,
            "        const ");
    if (pConst->mType == TYPE_BOOLEAN) {
        fprintf(pFile, "Boolean %s = %s;\n", pConst->mName, pConst->mV.mBoolValue ? "TRUE" : "FALSE");
    }
    else if (pConst->mType == TYPE_CHAR32) {
        fprintf(pFile, "Char32 %s = \'%c\'\n", pConst->mName, pConst->mV.mInt32Value.mValue);
    }
    else if (pConst->mType == TYPE_BYTE) {
        fprintf(pFile, "Byte %s = ", pConst->mName);
        if (pConst->mV.mInt32Value.mFormat == FORMAT_HEX) {
            fprintf(pFile, "0x%08x;\n", (unsigned char)pConst->mV.mInt32Value.mValue);
        }
        else {
            fprintf(pFile, "%d;\n", (unsigned char)pConst->mV.mInt32Value.mValue);
        }
    }
    else if (pConst->mType == TYPE_INTEGER16) {
        fprintf(pFile, "Int16 %s = ", pConst->mName);
        if (pConst->mV.mInt32Value.mFormat == FORMAT_HEX) {
            fprintf(pFile, "0x%08x;\n", (short)pConst->mV.mInt32Value.mValue);
        }
        else {
            fprintf(pFile, "%d;\n", (short)pConst->mV.mInt32Value.mValue);
        }
    }
    else if (pConst->mType == TYPE_INTEGER32) {
        fprintf(pFile, "Int32 %s = ", pConst->mName);
        if (pConst->mV.mInt32Value.mFormat == FORMAT_HEX) {
            fprintf(pFile, "0x%08x;\n", pConst->mV.mInt32Value.mValue);
        }
        else {
            fprintf(pFile, "%d;\n", pConst->mV.mInt32Value.mValue);
        }
    }
    else if (pConst->mType == TYPE_INTEGER64) {
        fprintf(pFile, "Int64 ");
        fprintf(pFile, "%s = ", pConst->mName);
        if (pConst->mV.mInt64Value.mFormat == FORMAT_HEX) {
            fprintf(pFile, "0x%llx;\n", pConst->mV.mInt64Value.mValue);
        }
        else {
            fprintf(pFile, "%lld;\n", pConst->mV.mInt64Value.mValue);
        }
    }
    else if (pConst->mType == TYPE_FLOAT) {
        fprintf(pFile, "Float %s = %f;\n", pConst->mName, pConst->mV.mDoubleValue);
    }
    else if (pConst->mType == TYPE_DOUBLE) {
        fprintf(pFile, "Double %s = %f;\n", pConst->mName, pConst->mV.mDoubleValue);
    }
    else {
        assert(pConst->mType == TYPE_STRING);

        if (pConst->mV.mStrValue != NULL) {
             fprintf(pFile, "String %s = \"%s\";\n", pConst->mName, pConst->mV.mStrValue);
        }
    }
}

void CGenerateMethodAttribs(
    FILE *pFile, const MethodDescriptor *pMethod)
{
    if (pMethod->mAttribs & MethodAttrib_Oneway) {
        fprintf(pFile, "        [oneway] ");
    }
    else {
        fprintf(pFile, "        ");
    }
}

void CGenerateMethod(FILE *pFile,
    const CLSModule *pModule, MethodDescriptor *pMethod)
{
    int n;
    ParamDescriptor *pParam;

    CGenerateMethodAttribs(pFile, pMethod);
    fprintf(pFile,
            "%s(",
            pMethod->mName);
    for (n = 0; n < pMethod->mParamCount; n++) {
        if (0 != n) fputs(", ", pFile);
        pParam = pMethod->mParams[n];

        fprintf(pFile,
                "\n            [%s] %s %s",
                ParamAttrib2String(pParam->mAttribs),
                Type2CString(pModule, &pParam->mType),
                pParam->mName);
    }
    fputs(");\n", pFile);
}

void CGenerateInterfaces(FILE *pFile, const CLSModule *pModule)
{
    int n, m;
    InterfaceDescriptor *pDesc;

    for (n = 0; n < pModule->mInterfaceCount; n++) {
        pDesc = pModule->mInterfaceDirs[n]->mDesc;

        if (!pModule->mInterfaceDirs[n]->mNameSpace
            && !(pDesc->mAttribs & InterfaceAttrib_dual)
            && !(pDesc->mAttribs & InterfaceAttrib_clsobj)) {
            fputs("\n", pFile);
            CGenerateInterfaceAttribs(pFile, pDesc);

            fprintf(pFile, "    interface %s",
                    pModule->mInterfaceDirs[n]->mName);

            if (0 != pDesc->mParentIndex) {
                fprintf(pFile, " : %s",
                        pModule->mInterfaceDirs[pDesc->mParentIndex]->mName);
            }
            fputs(" {\n", pFile);

            for (m = 0; m < pDesc->mConstCount; m++) {
                if (0 != m) fputs("\n", pFile);
                CGenerateInterfaceConst(pFile, pModule, pDesc->mConsts[m]);
            }

            for (m = 0; m < pDesc->mMethodCount; m++) {
                if (0 != m) fputs("\n", pFile);
                CGenerateMethod(pFile, pModule, pDesc->mMethods[m]);
            }
            fputs("    }\n", pFile);
        }
    }
}

void CGenerateClassAttribs(
    FILE *pFile, const CLSModule *pModule, const ClassDescriptor *pDesc)
{
    int n;
    BOOL bComma;

    if (0 == CLASS_ATTR(pDesc->mAttribs))
        return;

    bComma = FALSE;
    fputs("    [\n", pFile);

    if (pDesc->mAttribs & ClassAttrib_singleton) {
        if (bComma) fputs(",\n", pFile);
        fputs("        singleton", pFile);
        bComma = TRUE;
    }
    if (pDesc->mAttribs & ClassAttrib_private) {
        if (bComma) fputs(",\n", pFile);
        fputs("        private", pFile);
        bComma = TRUE;
    }
    if (pDesc->mAttribs & ClassAttrib_aggregate) {
        if (bComma) fputs(",\n", pFile);
        fprintf(pFile,
                "        aggregate(%s",
                pModule->mClassDirs[pDesc->mAggrIndexes[0]]->mName);
        for (n = 1; n < pDesc->mAggregateCount; n++) {
            fprintf(pFile, ", %s",
                    pModule->mClassDirs[pDesc->mAggrIndexes[n]]->mName);
        }
        fputs(")", pFile);
    }

    if (pDesc->mAttribs & ClassAttrib_aspect) {
        if (bComma) fputs(",\n", pFile);
        fprintf(pFile,
                "        aspect(%s",
                pModule->mClassDirs[pDesc->mAspectIndexes[0]]->mName);
        for (n = 1; n < pDesc->mAspectCount; n++) {
            fprintf(pFile, ", %s",
                    pModule->mClassDirs[pDesc->mAspectIndexes[n]]->mName);
        }
        fputs(")", pFile);
    }
    fputs("\n    ]\n", pFile);
}

void CGenerateClasses(FILE *pFile, const CLSModule *pModule)
{
    int n, m;
    ClassDescriptor *pClass;
    InterfaceDirEntry *pIEntry;
    ClassInterface *pClsIntf;

    for (n = 0; n < pModule->mClassCount; n++) {
        pClass = pModule->mClassDirs[n]->mDesc;
        if (!pModule->mClassDirs[n]->mNameSpace
            && !(pClass->mAttribs & ClassAttrib_t_sink)
            && !(pClass->mAttribs & ClassAttrib_t_clsobj)
            && !(pClass->mAttribs & ClassAttrib_t_external)) {
            fputs("\n", pFile);
            CGenerateClassAttribs(pFile, pModule, pClass);

            switch (CLASS_TYPE(pClass->mAttribs)) {
                case ClassAttrib_t_aspect:
                    fputs("    aspect ", pFile);
                    break;
                case ClassAttrib_t_generic:
                    fputs("    generic ", pFile);
                    break;
                case ClassAttrib_t_regime:
                    fputs("    regime ", pFile);
                    break;
                case ClassAttrib_t_normalClass:
                    fputs("    class ", pFile);
                    break;
                default:
                    assert(TRUE == FALSE);
                    break;
            }
            fputs(pModule->mClassDirs[n]->mName, pFile);

            if (pClass->mAttribs & ClassAttrib_hasparent) {
                if (pModule->mClassDirs[pClass->mParentIndex]->mDesc->mAttribs
                    & ClassAttrib_t_generic) {
                    fprintf(pFile, " :: %s",
                        pModule->mClassDirs[pClass->mParentIndex]->mName);
                }
                else {
                    fprintf(pFile, " : %s",
                        pModule->mClassDirs[pClass->mParentIndex]->mName);
                }
            }

            fputs(" {\n", pFile);

// TODO: ctors

            for (m = 0; m < pClass->mInterfaceCount; m++) {
                pClsIntf = pClass->mInterfaces[m];
                pIEntry = pModule->mInterfaceDirs[pClsIntf->mIndex];

                if (pClsIntf->mAttribs & ClassInterfaceAttrib_outer)
                    continue;

                fputs("        ", pFile);
                if (pClsIntf->mAttribs & ClassInterfaceAttrib_virtual) {
                    fputs("virtual ", pFile);
                }
                else if (pClsIntf->mAttribs & ClassInterfaceAttrib_hidden) {
                    fputs("hidden ", pFile);
                }
                else if (pClsIntf->mAttribs & ClassInterfaceAttrib_callback) {
                    fputs("callback ", pFile);
                }

                fprintf(pFile, "interface %s;\n", pIEntry->mName);
            }
            fputs("    }\n", pFile);
        }
    }
}

int CLS2CAR_(FILE *pFile, const CLSModule *pModule)
{
    CGenerateCARAttribs(pFile, pModule);

    if (pModule->mAttribs & CARAttrib_library) {
        fprintf(pFile, "library");
    }
    else {
        fprintf(pFile, "module");
    }

    if (pModule->mUunm) {
        fprintf(pFile, " %s\n{\n", pModule->mUunm);
    }
    else {
        fprintf(pFile, "\n{\n");
    }

    CGenerateLibraries(pFile, pModule);
    CGenerateInterfaceDecls(pFile, pModule);
    CGenerateEnums(pFile, pModule);
    CGenerateConsts(pFile, pModule);
    CGenerateAliases(pFile, pModule);
    CGenerateStructs(pFile, pModule);
    CGenerateInterfaces(pFile, pModule);
    CGenerateClasses(pFile, pModule);

    fputs("}\n", pFile);

    return CLS_NoError;
}

int CLS2CAR(const char *pszName, const CLSModule *pModule)
{
    int nRet;
    FILE *pFile;

    pFile = fopen(pszName, "w+t");
    if (!pFile) return CLSError_OpenFile;

    nRet = CLS2CAR_(pFile, pModule);

    fclose(pFile);
    return nRet;
}
