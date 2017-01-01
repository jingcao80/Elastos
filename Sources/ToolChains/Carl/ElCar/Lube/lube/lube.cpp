
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <unistd.h>
#include "_lube.h"

extern int AddCLSLibrary(CLSModule *);
extern int CopyCLS(const CLSModule *, CLSModule *);
int ExecTemplates(PLUBEHEADER, CLSModule *, char *, DWORD);

extern FILE* g_RedirectFile;

void CloseRedirectFile()
{
    if (g_RedirectFile) {
        fclose(g_RedirectFile);
        g_RedirectFile = NULL;
    }
}

int main(int argc, char **argv)
{
    int nRet;
    CommandArgs args;
    PLUBEHEADER pLube;
    CLSModule *pModule;
    NameList nameList;
    DWORD dwLubeOpt = 0;

    if (ParseArgs(argc, argv, &args) < 0) return LUBE_FAIL;

    if (args.pszInputPath) {
        SetLubePath(args.pszInputPath);
        SetLibraryPath(args.pszInputPath);
    }
    if (args.mAttribs & Command_r_Overwrite) {
        dwLubeOpt |= LubeOpt_Overwrite;
    }
    if (args.mAttribs & Command_r_Redirect) {
        dwLubeOpt |= LubeOpt_Redirect;
    }
    if (args.mAttribs & Command_k_InKernel) {
        dwLubeOpt |= LubeOpt_InKernel;
    }
    if (args.mAttribs & Command_k_InSDK) {
        dwLubeOpt |= LubeOpt_InSDK;
    }
    if (args.mAttribs & Command_z_UseCustomClassObject) {
        dwLubeOpt |= LubeOpt_UseCustomClassObject;
    }
    if (args.mAttribs & Command_w_HaveSandwichMethod) {
        dwLubeOpt |= LubeOpt_HaveSandwichMethod;
    }
    if (args.mAttribs & Command_n_UseNakedMode) {
        dwLubeOpt |= LubeOpt_UseNakedMode;
    }
    if (args.mAttribs & Command_u_WeakRef) {
        dwLubeOpt |= LubeOpt_WeakRef;
    }

    if (LoadLube(args.pszInputLBO, &pLube) < 0) {
        return LUBE_FAIL;
    }
    if (args.mAttribs & Command_l_ListTemplates) {
        ListAllTemplates(pLube);
        if (!args.pszInputCLS) return LUBE_OK;
    }

    if (args.pszOutputPath) {
        SetOutputPath(args.pszOutputPath);
    }
    if (args.pszOutputFile) {
        dwLubeOpt |= LubeOpt_Redirect;
        if (!(dwLubeOpt & LubeOpt_Overwrite)
            && 0 == access(args.pszOutputFile, 0)) {
            fprintf(stderr, "[WARN] lube (0x1001) : File %s has existed.\n", args.pszOutputFile);
            return LUBE_FAIL;
        }
        g_RedirectFile = fopen(args.pszOutputFile, "w+t");
        if (!g_RedirectFile) {
            fprintf(stderr, "[ERROR] lube (0x1002) : Can't open file %s.\n", args.pszOutputFile);
            return LUBE_FAIL;
        }
    }
    if (args.pszInputCLS) {
        int n = strlen(args.pszInputCLS);
        if (!_stricmp(args.pszInputCLS + n - 4, ".cls")) {
            dwLubeOpt |= LubeOpt_FromExCls;
        }
    }

    nameList.Initialize(args.pszInputCLS);
    while (TRUE == nameList.MoveNext()) {
        nRet = LoadCLS(nameList.Current(), &pModule);
        if (nRet < 0) {
            fprintf(stderr, "[ERROR] lube (0x1007) : Can't load CLS file %s.\n", nameList.Current());
            goto LoadCLSError;
        }
        nRet = ExecTemplates(pLube, pModule, args.pszTemplates, dwLubeOpt);
        DisposeFlattedCLS(pModule);

        if (nRet < 0) {
            fprintf(stderr, "[ERROR] lube (0x1008) : Execute CLS file %s With Templete failed.\n", nameList.Current());
            goto LoadCLSError;
        }
    }
    CloseRedirectFile();
    return LUBE_OK;

LoadCLSError:
    CloseRedirectFile();
    fprintf(stderr, "[ERROR] lube (0x1003) : Can't load CLS resource from file %s.\n", args.pszOutputFile);
    fprintf(stderr, "[ERROR] lube (0x1004) : %s\n", CLSLastErrorMessage());
    return LUBE_FAIL;
}

int ExecTemplates(
    PLUBEHEADER pLube, CLSModule *pModule,
    char *pszTemplates, DWORD dwOption)
{
    NameList nameList;
    LubeContext ctx(pLube, pModule, dwOption);

    nameList.Initialize(pszTemplates);
    while (TRUE == nameList.MoveNext()) {
        if (ctx.ExecTemplate(nameList.Current()) < 0) {
            return LUBE_FAIL;
        }
    }
    nameList.Reset();

    return LUBE_OK;
}

int CalcNodeNumber(PSTATEDESC pDesc)
{
    int cNodes = 0;

    while (pDesc) {
        if (pDesc->pBlockette) {
            cNodes += CalcNodeNumber(pDesc->pBlockette);
        }
        cNodes++;
        pDesc = pDesc->pNext;
    }
    return cNodes;
}

int CalcNodeSize(PSTATEDESC pDesc)
{
    int nSize = 0;

    while (pDesc) {
        if (pDesc->pBlockette) {
            nSize += CalcNodeSize(pDesc->pBlockette);
        }
        nSize += sizeof(StateDesc) + pDesc->uDataSize;
        pDesc = pDesc->pNext;
    }
    return nSize;
}

void ListAllTemplates(PLUBEHEADER pLube)
{
    int n, cNodes, nSize;
    const char *pszInput;
    ObjectType obj;

    printf("%-6s%-16s%-10s%6s%8s\n",
            "ORDER", "NAME", "INPUT", "NODES", "SIZE");
    fputs("----------------------------------------------\n", stdout);

    for (n = 0; n < pLube->cTemplates; n++) {
        obj = pLube->ppTemplates[n]->tRoot.object;
        pszInput = Object_Module == obj ? "module":
                (Object_Class == obj ? "class":"interface");
        cNodes = CalcNodeNumber(pLube->ppTemplates[n]->tRoot.pBlockette);
        nSize = CalcNodeSize(pLube->ppTemplates[n]->tRoot.pBlockette);

        printf("%-6d%-16s%-10s%6d%8d\n",
                n, pLube->ppTemplates[n]->mName,
                pszInput, cNodes, nSize);
    }
}
