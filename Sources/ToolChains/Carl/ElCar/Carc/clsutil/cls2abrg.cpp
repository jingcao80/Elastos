
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <wtypes.h>
#include <malloc.h>
#include <assert.h>

typedef unsigned char  byte;
#include <elatypes.h>
#include <clsutil.h>
#include <abrgcls.h>

#ifdef _linux
#define _alloca alloca
#endif

void AbrgUuid(const GUID *puuid, FILE *pFile)
{
    fprintf(pFile,
            "//    [ %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x ]\n",
            puuid->mData1, puuid->mData2, puuid->mData3,
            puuid->mData4[0], puuid->mData4[1], puuid->mData4[2], puuid->mData4[3],
            puuid->mData4[4], puuid->mData4[5], puuid->mData4[6], puuid->mData4[7]
            );
}

#define ABRG_TYPE(_type_) \
case Param_Type_##_type_: return #_type_

const char *AbrgParamType(AbridgedParamsInfo param)
{
    switch (ParamType(param)) {
        ABRG_TYPE(uint8);
        ABRG_TYPE(uint16);
        ABRG_TYPE(uint32);
        ABRG_TYPE(uint64);
        ABRG_TYPE(guid);
        ABRG_TYPE(String);
        ABRG_TYPE(ArrayOf);
        ABRG_TYPE(eventhandler);
        ABRG_TYPE(interface);
        ABRG_TYPE(clsid);
        ABRG_TYPE(structure);

        ABRG_TYPE(puint8);
        ABRG_TYPE(puint16);
        ABRG_TYPE(puint32);
        ABRG_TYPE(puint64);
        ABRG_TYPE(pguid);
        ABRG_TYPE(peventhandler);
        ABRG_TYPE(pinterface);
        ABRG_TYPE(pclsid);
        ABRG_TYPE(pstructure);
        ABRG_TYPE(pString);

        default: break;
    }
    assert(TRUE == FALSE);
    return "__TypeUnknown__";
}

const char *AbrgParamAttrib(AbridgedParamsInfo param)
{
    if (param & Param_Attrib_in) {
        if (param & Param_Attrib_out) {
            return "in, out";
        }
        else {
            return "in";
        }
    }
    else {
        assert(param & Param_Attrib_out);
        if (param & Param_Attrib_callee) {
            return "out, callee";
        }
        else {
            return "out";
        }
    }
}

void AbrgMethod(FILE *pFile, int n, const AbridgedMethod *pMethod)
{
    fprintf(pFile, "//        [0X%08X, 0x%02x] Method_%02d(", pMethod->mAttribs, pMethod->nStackSize, n);

    for (n = 0; n < pMethod->mParamCount; n++) {
        if (0 != n) fputs(",", pFile);
        fprintf(pFile,
                "\n//                    [%s] %s",
                AbrgParamAttrib(pMethod->pParams[n]),
                AbrgParamType(pMethod->pParams[n]));
    }

    fputs(")\n", pFile);
}

const char *AbrgInterfaceName(const CLSModule *pModule, const GUID *pIID)
{
    for (int n = 0; n < pModule->mInterfaceCount; n++) {
        if (IsEqualUUID(pIID, &pModule->mInterfaceDirs[n]->mDesc->mIID)) {
            return pModule->mInterfaceDirs[n]->mName;
        }
    }
    assert(TRUE == FALSE);

    return "__INotFound__";
}

void AbrgInterface(FILE *pFile,
    const CLSModule *pModule, const AbridgedInterface *pInterface)
{
    AbrgUuid(&pInterface->iid, pFile);
    fprintf(pFile,
            "//    interface %s {\n",
            AbrgInterfaceName(pModule, &pInterface->iid));

    for (int n = 0; n < pInterface->mMethodCount; n++) {
        AbrgMethod(pFile, n, &pInterface->pMethods[n]);
    }

    fputs("//    }\n//\n", pFile);
}

const char *AbrgClassName(const CLSModule *pModule, const GUID *pIID)
{
    for (int n = 0; n < pModule->mClassCount; n++) {
        if (IsEqualUUID(pIID, &pModule->mClassDirs[n]->mDesc->mClsid)) {
            return pModule->mClassDirs[n]->mName;
        }
    }
    assert(TRUE == FALSE);

    return "__CNotFound__";
}

void AbrgClass(FILE *pFile,
    const CLSModule *pModule, const AbridgedClass *pClass)
{
    AbrgUuid(&pClass->clsid, pFile);
    fprintf(pFile,
            "//    uunm(\"%s\")\n"
            "//    class %s {\n",
            pClass->pszUunm,
            AbrgClassName(pModule, &pClass->clsid));

    for (int n = 0; n < pClass->mInterfaceCount; n++) {
        fprintf(pFile,
            "//        interface %s;\n",
            AbrgInterfaceName(pModule, &pClass->mInterfaces[n]->iid));
    }

    fputs("//    }\n//\n", pFile);
}

void Abrg2Comment(FILE * pFile,
    const CLSModule *pModule, const AbridgedCLS *pAbrg)
{
    int n;
    AbridgedCLS *pAbridged;

    pAbridged = (AbridgedCLS *)_alloca(pAbrg->nTotalSize);
    if (!pAbridged) return;
    memcpy(pAbridged, pAbrg, pAbrg->nTotalSize);

    RelocAbridgedCLS(pAbridged);

    fprintf(pFile,
            "//\n"
            "//    [ size(%d), class(%d), interface(%d) ]\n"
            "//    Text reverted from %s's Abridged-CLS\n"
            "//\n"
            "//\n",
            pAbridged->nTotalSize,
            pAbridged->mClassCount,
            pAbridged->mInterfaceCount,
            pModule->mName);

    for (n = 0; n < pAbridged->mInterfaceCount; n++) {
        AbrgInterface(pFile, pModule, &pAbridged->pInterfaces[n]);
    }

    for (n = 0; n < pAbridged->mClassCount; n++) {
        AbrgClass(pFile, pModule, &pAbridged->pClasses[n]);
    }
}

#ifdef _linux
static void _strupr(char* str)
{
    int i = 0;
    while(str[i] != '\0') {
        str[i] = toupper(str[i]);
        i++;
    }
}
#endif

int CLS2AbrgCpp_(FILE *pFile, const CLSModule *pModule)
{
    int n, i, nRet = CLSError_OutOfMemory;
    AbridgedCLS *pAbridged = NULL;
    char *pszName = NULL;
    char *pszUpperName = NULL;

    pszName = strdup(pModule->mName);
    if (!pszName) goto ErrorExit;
    n = strlen(pszName);
    for(i = 0; i < n; i++) {
        if (*(pszName + i) == '.') {
            *(pszName + i) = '_';
        }
    }

    pszUpperName = strdup(pszName);
    if (!pszUpperName) goto ErrorExit;
    _strupr(pszUpperName);

    nRet = AbridgeCLS(pModule, (void**)&pAbridged);
    if (nRet < 0) goto ErrorExit;

    fprintf(pFile,
            "#if !defined(__CAR_%sABRG_CPP__)\n"
            "#define __CAR_%sABRG_CPP__\n"
            "\n"
            "#include <clsinfo.h>\n"
            "\n"
            "static unsigned char s_%s_classInfo[%d] = {",
            pszUpperName, pszUpperName,
            pszName, pAbridged->nTotalSize);

    for (n = 0; n < (int)pAbridged->nTotalSize; n++) {
        if (0 == (n & 0x07)) fputs("\n    ", pFile);
        fprintf(pFile, "0x%02x, ", ((unsigned char *)pAbridged)[n]);
    }

    fprintf(pFile,
                "\n};\n"
                "\n"
                "CIClassInfo *g_%s_classInfo = "
                "(CIClassInfo *)s_%s_classInfo;\n"
                "\n"
                "#endif // __CAR_%sABRG_CPP__\n",
                pszName, pszName, pszUpperName);

    Abrg2Comment(pFile, pModule, pAbridged);

    DisposeAbridgedCLS(pAbridged);
    free(pszUpperName);
    free(pszName);
    return CLS_NoError;

ErrorExit:
    if (pAbridged) DisposeAbridgedCLS(pAbridged);
    if (pszUpperName) free(pszUpperName);
    if (pszName) free(pszName);

    return nRet;
}

int CLS2AbrgCpp(const char *pszName, const CLSModule *pModule)
{
    int nRet;
    FILE *pFile;
    pFile = fopen(pszName, "w+t");
    if (!pFile) return CLSError_OpenFile;

    nRet = CLS2AbrgCpp_(pFile, pModule);

    fclose(pFile);
    return nRet;
}
