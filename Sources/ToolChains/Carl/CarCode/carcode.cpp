//
// File:        CarCode.exe
// Purpose:     Show a CAR component's fingerprint.
// Usage:       CarCode.exe ModuleName.dll
// Creat date:  2010-03-20
//

#include <stdio.h>
#include <malloc.h>
#include <clsbase.h>

#define COM_NULL 0
#define COM_GET_BUILDDATE 1
#define COM_GET_CARCODE 2
#define COM_GET_DEPENDENCE 3
#define COM_GET_URN 4

static int gComm;

#define FILETYPE_UNKNOWN 0
#define FILETYPE_CLS 1
#define FILETYPE_DEF 2
#define FILETYPE_DLL 3

#define _stricmp strcasecmp

typedef struct CarCode {
    const char *       cModuleName;
    unsigned int       nChecksum;
    unsigned int       nBarcode;
}   CarCode;

extern unsigned int rabin(const void *buf, int size);

int getCarCodeFromCls(const char *pszName, CarCode *pCarCode)
{
    int nRet;
    CLSModule *pModule;

    nRet = LoadCLSFromFile(pszName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. \n", pszName);
        return 0;
    }
    else if (nRet < 0)
        return 0;

    pCarCode->cModuleName = pModule->mUunm;
    pCarCode->nChecksum = pModule->mChecksum;
    pCarCode->nBarcode = pModule->mBarcode;

    return 1;
}

int getCarCodeFromDll(const char *pszName, CarCode *pCarCode)
{
    int nRet;
    CLSModule *pModule;

    nRet = LoadCLS(pszName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. \n", pszName);
        return 0;
    }
    else if (nRet < 0)
        return 0;

    pCarCode->cModuleName = pModule->mUunm;
    pCarCode->nChecksum = pModule->mChecksum;
    pCarCode->nBarcode = pModule->mBarcode;

    return 1;
}

int getCarCodeFromDef(const char *pszName, CarCode *pCarCode)
{
    FILE *pFile;
    int len;
    char *pBuf;
    unsigned int checksum;

    pFile = fopen(pszName, "r");
    if (!pFile) return 0;

    fseek(pFile, 0, SEEK_END);
    len=ftell(pFile);
    if (len <= 0) return 0;

    pBuf = (char*)malloc(len + 1);
    if (!pBuf) return 0;
    fseek(pFile, 0, SEEK_SET);
    fread(pBuf, sizeof(char), len, pFile);


    fclose(pFile);

    checksum = rabin(pBuf, len);

    pCarCode->cModuleName = "some.def";
    pCarCode->nChecksum = checksum;
    pCarCode->nBarcode = 0;

    free(pBuf);

    return 1;
}

int getDependenceFromDll(const char *pszName)
{
    int nRet;
    CLSModule *pModule;
    char *pStr;

    nRet = LoadCLS(pszName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. \n", pszName);
        return 0;
    }
    else if (nRet < 0)
        return 0;

    printf("Dependences: \n");
    for(int i = 0; i < pModule->mLibraryCount; i++) {
        pStr = pModule->mLibraryNames[i] + strlen(pModule->mLibraryNames[i]) - 4;
        if(!strcmp(pStr, ".dll") | !strcmp(pStr, ".eco"))
            printf("%s\n", pModule->mLibraryNames[i]);
    }

    return 1;
}

int getDependenceFromCls(const char *pszName)
{
    int nRet;
    CLSModule *pModule;
    char *pStr;

    nRet = LoadCLS(pszName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. \n", pszName);
        return 0;
    }
    else if (nRet < 0)
        return 0;

    printf("Dependences: \n");
    for(int i = 0; i < pModule->mLibraryCount; i++) {
        pStr = pModule->mLibraryNames[i] + strlen(pModule->mLibraryNames[i]) - 4;
        if(!strcmp(pStr, ".dll") | !strcmp(pStr, ".eco"))
            printf("%s\n", pModule->mLibraryNames[i]);
    }

    return 1;
}

void showUsage()
{
    printf("Copyright (C) 2012 The Elastos Open Source Project\n");
    printf("\n");
    printf("Licensed under the Apache License, Version 2.0 (the \"License\");\n");
    printf("you may not use this file except in compliance with the License.\n");
    printf("You may obtain a copy of the License at\n");
    printf("\n");
    printf("     http://www.apache.org/licenses/LICENSE-2.0\n");
    printf("\n");
    printf("Unless required by applicable law or agreed to in writing, software\n");
    printf("distributed under the License is distributed on an \"AS IS\" BASIS,\n");
    printf("WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n");
    printf("See the License for the specific language governing permissions and\n");
    printf("limitations under the License.\n\n");
    printf(" carcode.exe:    Show a CAR component's fingerprint: checksum.manufacturingdate.\n");
    printf(" Usage:          carcode.exe [-b | -c | -d | -u] [ModuleName.dll | ModuleName.eco | ClsName.cls | DefName.def]\n");
    printf(" -b              output build-date\n");
    printf(" -c              output fingerprint\n");
    printf(" -d              output dependence\n");
    printf(" -u              output urn\n");
}

int getFileType(const char *pFileName)
{
    int n = strlen(pFileName);

    if (!_stricmp(pFileName + n - 4, ".cls")) return FILETYPE_CLS;
    else if (!_stricmp(pFileName + n - 4, ".def")) return FILETYPE_DEF;
    else if (!_stricmp(pFileName + n - 4, ".dll")) return FILETYPE_DLL;
    else if (!_stricmp(pFileName + n - 4, ".eco")) return FILETYPE_DLL;

    return FILETYPE_UNKNOWN;
}

void print(CarCode *pCarCode)
{
    switch(gComm) {
    case COM_NULL:
        printf(" The %s's module fingerprint is:\n     %08x.%08x \n",
                    pCarCode->cModuleName, pCarCode->nChecksum, pCarCode->nBarcode);
        break;

    case COM_GET_BUILDDATE:
        printf("bd:%x\n", pCarCode->nBarcode);
        break;

    case COM_GET_CARCODE:
        printf("fp:%x\n", pCarCode->nChecksum);
        break;

    case COM_GET_URN:
        printf("urn:%s\n", pCarCode->cModuleName);
        break;
    }
}

int main(int argc, char *argv[])
{
    CarCode carCode;
    const char * pFileName;

    gComm = COM_NULL;

    if (*argv[1] == '-') {
        char c = *(argv[1] + 1);
        if (c == '?'){
            showUsage();
            return 1;
        }
        else if (c == 'b') {
            gComm = COM_GET_BUILDDATE;
        }
        else if (c == 'c') {
            gComm = COM_GET_CARCODE;
        }
        else if (c == 'd') {
            gComm = COM_GET_DEPENDENCE;
        }
        else if (c == 'u') {
            gComm = COM_GET_URN;
        }
        else {
            printf("Invalid argument!\n\n");
            showUsage();
            return 1;
        }

        pFileName = argv[2];
    }
    else {
        pFileName = argv[1];
    }

    int ftype = getFileType(pFileName);
    switch(ftype) {
    case FILETYPE_CLS:
        if (gComm == COM_GET_DEPENDENCE) {
            return !getDependenceFromCls(pFileName);
        }
        else if (getCarCodeFromCls(pFileName, &carCode)) {
            print(&carCode);
        }
        else {
            printf(" CarCode.exe failed!\n");
            return 1;
        }

        break;

    case FILETYPE_DEF:
        if (getCarCodeFromDef(pFileName, &carCode)){
            print(&carCode);
        }
        else {
            printf(" CarCode.exe failed!\n");
            return 1;
        }
        break;

    case FILETYPE_DLL:
        if (gComm == COM_GET_DEPENDENCE) {
            return !getDependenceFromDll(pFileName);
        }
        else if (getCarCodeFromDll(pFileName, &carCode)) {
            print(&carCode);
        }
        else {
            printf(" CarCode.exe failed!\n");
            return 1;
        }

        break;

    default:
        printf(" Invalid file name!\n\n");
        showUsage();
        return 1;
    }

    return 0;
}
