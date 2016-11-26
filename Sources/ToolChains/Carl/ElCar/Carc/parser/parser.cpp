//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include "carc.h"
#include "clsutil.h"
#include "StringBuilder.h"

#ifdef _linux
#define _stricmp strcasecmp
#define _MAX_PATH 256
#endif

enum InternalReturnValue {
    Ret_Continue        = 0,
    Ret_ContinueOnError = -1,
    Ret_AbortOnError    = -2,
};

enum Properties {
    Prop_deprecated     = 0x0001,
    Prop_local          = 0x0002,
    Prop_oneway         = 0x0004,
    Prop_private        = 0x0008,
    Prop_parcelable     = 0x0010,
};

static FILE *s_pFile;
static CLSModule *s_pModule = NULL;
static const char *s_pszSourcePath = "";
static const char *s_pszFactoryUrl = "";
static char *s_pszCurrentPath = "";
static char *s_pszCarFullName = "";
ClassAttrib s_DefaultThreadModel = ClassAttrib_freethreaded;
bool s_bLenientNaming = false;
bool s_bInKernel = false;
bool s_bSupportWeakRef = false;
bool s_bInNakedMode = false;

static const IID EIID_IInterface = \
{0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x66}};

static const IID EIID_IObject = \
{0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x68}};

static const IID EIID_IAspect = \
{0x00010002,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x66}};

static const IID EIID_IClassObject = \
{0x00000001,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

static const IID EIID_IProxyDeathRecipient= \
{0x0001000D,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x66}};

static const IID EIID_IWeakReference= \
{0x00010008,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x66}};

static const IID EIID_IWeakReferenceSource= \
{0x0001000A,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x66}};

static const CLSID CLSID_XOR_CallbackSink = \
/* e724df56-e16a-4599-8edd-a97ab245d583 */
{0xe724df56,0xe16a,0x4599,{0x8e,0xdd,0xa9,0x7a,0xb2,0x45,0xd5,0x83}};

static char *s_pszNamespace = NULL;
static int s_nsCap = 0;
static int s_nsUsed = 0;

static char *s_pszUsingNS = NULL;
static int s_usingCap = 0;
static int s_usingUsed = 0;

static char *s_pszSubmodule = NULL;


typedef struct FrontDeclaration
{
    CARDataType type;
    char *pszFullName;
    struct FrontDeclaration *pNext;

    FrontDeclaration(const char *pszName, CARDataType t)
        : type(t)
        , pNext(NULL)
    {
        pszFullName = (char *)malloc(strlen(pszName) + 1);
        strcpy(pszFullName, pszName);
    }

    ~FrontDeclaration()
    {
        free(pszFullName);
        pNext = NULL;
    }

} FrontDeclaration;

static FrontDeclaration *s_pFrontDecl = NULL;

char* GetNamespace()
{
    return s_pszNamespace;
}

void InitNamespace()
{
    s_nsCap = 1024;
    s_pszNamespace = (char*)malloc(s_nsCap + 1);
    memset(s_pszNamespace, 0, s_nsCap + 1);

    s_usingCap = 1024;
    s_pszUsingNS = (char*)malloc(s_usingCap + 1);
    memset(s_pszUsingNS, 0, s_usingCap + 1);
}

void UninitNamespace()
{
    free(s_pszNamespace);
    s_nsCap = 0;
    s_nsUsed = 0;
    s_pszNamespace = NULL;

    free(s_pszUsingNS);
    s_usingCap = 0;
    s_usingUsed = 0;
    s_pszUsingNS = NULL;
}

void PushNamespace(const char *pszNamespace)
{
    int reqSize = strlen(pszNamespace);
    if (s_nsCap - s_nsUsed < reqSize) {
        char *newStr = (char*)malloc(s_nsCap * 2 + 1);
        strcpy(newStr, s_pszNamespace);
        free(s_pszNamespace);
        s_pszNamespace = newStr;
        s_nsCap *= 2;
    }

    if (s_nsUsed > 0) strcat(s_pszNamespace, ".");
    strcat(s_pszNamespace, pszNamespace);
    s_nsUsed += reqSize;
}

void PopNamespace()
{
    char *dot = strrchr(s_pszNamespace, '.');
    if (dot != NULL) {
        *dot = '\0';
        s_nsUsed = dot - s_pszNamespace;
    }
    else {
        *s_pszNamespace = '\0';
        s_nsUsed = 0;
    }
}

char* GetSubmodule()
{
    return s_pszSubmodule;
}

void InitSubmodule()
{
    s_pszSubmodule = NULL;
}

void UninitSubmodule()
{
    free(s_pszSubmodule);
}

void ChangeSubmodule(const char *pszPath)
{
    if (s_pszSubmodule) {
        free(s_pszSubmodule);
    }
    if (!pszPath) {
        s_pszSubmodule = NULL;
        return;
    }
    s_pszSubmodule = (char*)malloc(strlen(pszPath) + 1);
    strcpy(s_pszSubmodule, pszPath);
}

void AddUsingNS(const char *pszNamespace)
{
    int reqSize = strlen(pszNamespace);
    if (s_usingCap - s_usingUsed < reqSize) {
        char *newStr = (char*)malloc(s_usingCap * 2 + 1);
        strcpy(newStr, s_pszUsingNS);
        free(s_pszUsingNS);
        s_pszUsingNS = newStr;
        s_usingCap *= 2;
    }

    if (s_usingUsed > 0) strcat(s_pszUsingNS, ";");
    strcat(s_pszUsingNS, pszNamespace);
    s_usingUsed += reqSize;
}

void InitFrontDeclaration()
{
    FrontDeclaration *pDecl = s_pFrontDecl;
    while (pDecl != NULL) {
        if (pDecl->pNext != NULL) {
            s_pFrontDecl = pDecl->pNext;
        }
        else s_pFrontDecl = NULL;
        delete pDecl;
        pDecl = s_pFrontDecl;
    }
}

void AddFrontDeclaration(const char *pszName, CARDataType t)
{
    if (s_pFrontDecl == NULL) {
        s_pFrontDecl = new FrontDeclaration(pszName, t);
    }
    else {
        FrontDeclaration *pDeclNode = s_pFrontDecl;
        while (pDeclNode != NULL) {
            if (!strcmp(pDeclNode->pszFullName, pszName)) return;
            if (pDeclNode->pNext == NULL) break;
            pDeclNode = pDeclNode->pNext;
        }
        pDeclNode->pNext = new FrontDeclaration(pszName, t);
    }
}

int FindFrontDeclaration(const char *pszName, CARDataType t, FrontDeclaration **pDecl)
{
    *pDecl = NULL;
    int count = 0;
    FrontDeclaration *pDeclNode = s_pFrontDecl;
    while (pDeclNode != NULL) {
        char *dot = strrchr(pDeclNode->pszFullName, '.');
        if ((t == 0 || pDeclNode->type == t) && !strcmp(dot != NULL ? dot + 1 : pDeclNode->pszFullName, pszName)) {
            if (++count == 1) *pDecl = pDeclNode;
        }
        pDeclNode = pDeclNode->pNext;
    }
    return count;
}

void SetFactoryUrl(const char *pszUrl)
{
    s_pszFactoryUrl = pszUrl;
}

void SetSourcePath(const char *pszPath)
{
    s_pszSourcePath = pszPath;
}

void SetDefaultThreadModel(ClassAttrib attrib)
{
    s_DefaultThreadModel = attrib;
}

int GetCurrentCarPath(const char *pszPath, char **ppszDest)
{
    int n = strlen(pszPath) - 1;

    while (n >= 0) {
        if (IS_PATH_SEPARATOR(*(pszPath + n))) {
            if ((*ppszDest) && ('\0' != **ppszDest))
                delete[] *ppszDest;

            *ppszDest = new char[n + 1];
            if (NULL == *ppszDest) {
                ErrorReport(CAR_E_OutOfMemory);
                return Ret_AbortOnError;
            }

            memcpy(*ppszDest, pszPath, n);
            (*ppszDest)[n] = 0;

            break;
        }
        n--;
    }

    return Ret_Continue;
}

const char *GenerateInternalName(const char *s)
{
    static char s_szName[32];
    static int s_nSerial = 1;

    sprintf(s_szName, "__Internal_%s%d", s, s_nSerial++);
    return s_szName;
}

long long int Hexstr2Int(const char *pszHex)
{
    long long int nResult = 0;

    assert(pszHex[0] == '0' && (pszHex[1] == 'x' || pszHex[1] == 'X'));

    pszHex += 2; // skip prefix "0x"
    while (*pszHex) {
        nResult *= 16;

        if (isdigit(*pszHex)) {
            nResult += *pszHex - '0';
        }
        else if (*pszHex >= 'a' && *pszHex <= 'f') {
            nResult += *pszHex - 'a' + 10;
        }
        else {
            nResult += *pszHex - 'A' + 10;
        }
        pszHex++;
    }
    return nResult;
}

#define IS_ECODE(t)    ((t).type == Type_ECODE && (t).mPointer == 0)

BOOL IsValidParamTypeAttrib(
    const CLSModule *pModule, const TypeDescriptor *pType,
    const DWORD attribs, const BOOL isLocal, const BOOL isDeprecated)
{
    TypeDescriptor type;

    switch (pType->mType) {
        case Type_alias:
            GetOriginalType(pModule, pType, &type);
            return IsValidParamTypeAttrib(pModule, &type, attribs, isLocal, isDeprecated);

        /*
         * local: [in] PVoid ;
         * local: [out] PVoid;
         * local: [out] PVoid *;
         */
        case Type_PVoid:
            if (isLocal) {
                if (attribs & ParamAttrib_in) {
                    if (0 == pType->mPointer) return TRUE;
                    ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have '*' when it modifies [in] parameter.");
                    return FALSE;
                }
                if (attribs & ParamAttrib_out) {
                    if (attribs & ParamAttrib_callee) {
                        ErrorReport(CAR_E_IllegalTypeUsage, "Attribute [callee] is illegal.");
                        return FALSE;
                    }

                    if (1 >= pType->mPointer) return TRUE;

                    ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than "
                            "one '*' when it modifies [out] parameter.");
                    return FALSE;
                }
            }
            ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
            return FALSE;

        /*
         * local|unlocal: [in] IInterface *;
         * local|unlocal, deprecated: [in] IInterface **; // warning
         * local|unlocal: [out] IInterface **;
         */
        case Type_interface:
            if (pModule->mInterfaceDirs[pType->mIndex]->mDesc->mAttribs
                    & InterfaceAttrib_local) {
                if (!(pModule->mInterfaceDirs[pType->mIndex]->mDesc->mAttribs
                    & InterfaceAttrib_parcelable) && (!isLocal)) {
                    ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
                    return FALSE;
                }
            }

            if (attribs & ParamAttrib_in) {
                if (1 == pType->mPointer) return TRUE;
                if (2 == pType->mPointer) {
                    if (isDeprecated) {
                        ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                        return TRUE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than two '*' "
                        "when it modifies [in] parameter.");
                return FALSE;
            }

            if (attribs & ParamAttrib_out) {
                if ((2 == pType->mPointer)
                        && !(attribs & ParamAttrib_callee)) return TRUE;
                ErrorReport(CAR_E_IllegalTypeUsage, "Type should have only two '*' "
                        "when it modifies [out] parameter.");
                return FALSE;
            }

            return FALSE;

        /*
         * local|unlocal: [in|out] ArrayOf<Type>;
         * local|unlocal: [in|out] ArrayOf<Type, size>;
         * local|: [out, callee] ArrayOf<Type> *;
         * local|unlocal, deprecated: [in] ArrayOf<Type> *;   //warning
         * local|unlocal, deprecated: [in] ArrayOf<Type, size> *; //warning
         */
        case Type_ArrayOf:
            if (IsLocalCarQuintet(pModule, pType->mNestedType, attribs) && (!isLocal)) {
                ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
                return FALSE;
            }

            if (attribs & ParamAttrib_in) {
                if (0 == pType->mPointer) return TRUE;
                if (1 == pType->mPointer) {
                    if (isDeprecated) {
                        ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                        return TRUE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than one '*'.");
                return FALSE;
            }
            if (attribs & ParamAttrib_out) {
                if (0 == pType->mPointer) {
                    if (!(attribs & ParamAttrib_callee)) return TRUE;
                    ErrorReport(CAR_E_IllegalTypeUsage, "Attribute [callee] is illegal.");
                    return FALSE;
                }
                if (1 == pType->mPointer) {
                    if (attribs & ParamAttrib_callee) return TRUE;
                    ErrorReport(CAR_E_IllegalTypeUsage, "Attribute should be [callee].");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than one '*'.");
                return FALSE;
            }
            return FALSE;

        /*
         * local: [in] UInt16;
         * local, deprecated: [in] UInt16 *;   //warning
         * local: [out] UInt16 *;
         * local, deprecated: [out] UInt16 **;   //warning
         */
        case Type_Int8:
        case Type_UInt16:
        case Type_UInt32:
        case Type_UInt64:
            if (isLocal) {
                if (attribs & ParamAttrib_in) {
                    if (0 == pType->mPointer) return TRUE;
                    if (1 == pType->mPointer) {
                        if (isDeprecated) {
                            ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                            return TRUE;
                        }
                        ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                        return FALSE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than one '*'.");
                    return FALSE;
                }
                if (attribs & ParamAttrib_out) {
                    if (attribs & ParamAttrib_callee) {
                        ErrorReport(CAR_E_IllegalTypeUsage, "Attribute [callee] is illegal.");
                        return FALSE;
                    }

                    if (1 == pType->mPointer) return TRUE;

                    if (2 == pType->mPointer) {
                        if (isDeprecated) {
                            ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                            return TRUE;
                        }
                        ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                        return FALSE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than two '*'.");
                    return FALSE;
                }
            }
            ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
            return FALSE;

        /*
         * local|unlocal: [in] struct stname;
         * local|unlocal, deprecated: [in] struct stname *;  //warning
         * local|unlocal: [out] struct stname *;
         * local:         [out] struct stname **;
         */
        case Type_struct:
            //check if struct is local based on its elements;
            if (IsLocalStruct(pModule, pModule->mStructDirs[pType->mIndex]->mDesc)
                    && (!isLocal)) {
                ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
                return FALSE;
            }

            if (attribs & ParamAttrib_in) {
                if (0 == pType->mPointer) return TRUE;
                if (1 == pType->mPointer) {
                    if (isDeprecated) {
                        ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                        return TRUE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than one '*'.");
                return FALSE;
            }
            if (attribs & ParamAttrib_out) {
                if (attribs & ParamAttrib_callee) {
                    ErrorReport(CAR_E_IllegalTypeUsage, "Attribute [callee] is illegal.");
                    return FALSE;
                }

                if (1 == pType->mPointer) return TRUE;

                if (2 == pType->mPointer) {
                    if (isLocal) return TRUE;
                    else {
                        ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
                        return FALSE;
                    }
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than two '*'.");
                return FALSE;
            }
            return FALSE;

        /*
         * local|unlocal: [in] type;
         * local|unlocal, deprecated: [in] type *;   //warning
         * local|unlocal: [out] type *;
         * local, deprecated: [out] type **;   //warning
         */
        default:
            if (attribs & ParamAttrib_in) {
                if (0 == pType->mPointer) return TRUE;
                if (1 == pType->mPointer) {
                    if (isDeprecated) {
                        ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                        return TRUE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than one '*'.");
                return FALSE;
            }
            if (attribs & ParamAttrib_out) {
                if (attribs & ParamAttrib_callee) {
                    ErrorReport(CAR_E_IllegalTypeUsage, "Attribute [callee] is illegal.");
                    return FALSE;
                }

                if (0 == pType->mPointer) {
                    ErrorReport(CAR_E_IllegalTypeUsage, "Type should have one or two '*'.");
                    return FALSE;
                }

                if (1 == pType->mPointer) return TRUE;

                if (2 == pType->mPointer) {
                    if (!isLocal) {
                        ErrorReport(CAR_E_IllegalTypeUsage, "Type is a local type.");
                        return FALSE;
                    }
                    if (isDeprecated) {
                        ErrorReport(CAR_W_DecrepitTypeUsage, "The usage is deprecated, and will be removed.");
                        return TRUE;
                    }
                    ErrorReport(CAR_E_IllegalTypeUsage, "The usage is deprecated, you should use keyword \"deprecated\".");
                    return FALSE;
                }
                ErrorReport(CAR_E_IllegalTypeUsage, "Type cann't have more than two '*'.");
                return FALSE;
            }
            return FALSE;
    }
}

int P_BaseType(CARToken token, TypeDescriptor *pType);

// TYPE        -> < BASE_TYPE | DEF_TYPE > [ s_star [ s_star ] ]
// DEF_TYPE    -> [ k_struct | k_enum ] ident
//
int P_Type(TypeDescriptor *pType)
{
    int n;
    CARToken token;

    token = GetToken(s_pFile);
    switch (token) {
        case Token_ident:
        {
            const char *pszName = g_szCurrentToken;
            char *pszNamespaces = NULL;
            const char *dot = strrchr(pszName, '.');
            if (dot != NULL) {
                pszNamespaces = (char *)malloc(dot - pszName + 1);
                memset(pszNamespaces, 0, dot - pszName + 1);
                memcpy(pszNamespaces, pszName, dot - pszName);
                pszName = dot + 1;
            }
            else {
                FrontDeclaration* pDecl;
                int count = FindFrontDeclaration(pszName, (CARDataType)0, &pDecl);
                if (count > 1) {
                    ErrorReport(CAR_E_TypeAmbiguous, g_szCurrentToken);
                    return Ret_AbortOnError;
                }
                if (count == 1) {
                    pszName = pDecl->pszFullName;
                    dot = strrchr(pszName, '.');
                    if (dot != NULL) {
                        pszNamespaces = (char *)malloc(dot - pszName + 1);
                        memset(pszNamespaces, 0, dot - pszName + 1);
                        memcpy(pszNamespaces, pszName, dot - pszName);
                        pszName = dot + 1;
                    }
                }
                else {
                    int nsLen = strlen(GetNamespace());
                    int usingLen = strlen(s_pszUsingNS);
                    if (nsLen + usingLen != 0) {
                        int len = nsLen + 1 + usingLen + 1;
                        pszNamespaces = (char *)malloc(len);
                        memset(pszNamespaces, 0, len);
                        if (nsLen != 0) strcpy(pszNamespaces, GetNamespace());
                        if (usingLen != 0) {
                            if (pszNamespaces[0] != '\0') strcat(pszNamespaces, ";");
                            strcat(pszNamespaces, s_pszUsingNS);
                        }
                    }
                }
            }

            if (RetrieveIdentifyType(
                pszName, pszNamespaces, s_pModule, pType, FALSE) < 0) {
                ErrorReport(CAR_E_UndefType, g_szCurrentToken);
                // continue on error
            }
            break;
        }

        case Token_K_enum:
        {
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectEnumName);
                return Ret_AbortOnError;
            }

            const char *pszName = g_szCurrentToken;
            char *pszNamespaces = NULL;
            const char *dot = strrchr(pszName, '.');
            if (dot != NULL) {
                pszNamespaces = (char *)malloc(dot - pszName + 1);
                memset(pszNamespaces, 0, dot - pszName + 1);
                memcpy(pszNamespaces, pszName, dot - pszName);
                pszName = dot + 1;
            }
            else {
                int nsLen = strlen(GetNamespace());
                int usingLen = strlen(s_pszUsingNS);
                if (nsLen + usingLen != 0) {
                    int len = nsLen + 1 + usingLen + 1;
                    pszNamespaces = (char *)malloc(len);
                    memset(pszNamespaces, 0, len);
                    if (nsLen != 0) strcpy(pszNamespaces, GetNamespace());
                    if (usingLen != 0) {
                        if (pszNamespaces[0] != '\0') strcat(pszNamespaces, ";");
                        strcat(pszNamespaces, s_pszUsingNS);
                    }
                }
            }
            n = RetrieveEnum(pszName, pszNamespaces, s_pModule, FALSE);
            if (n < 0) {
                ErrorReport(CAR_E_NotFound, "enum", g_szCurrentToken);
                break; // continue on error
            }
            pType->mType = Type_enum;
            pType->mIndex = n;
            break;
        }

        case Token_K_struct:
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectStructName);
                return Ret_AbortOnError;
            }
            n = RetrieveStruct(g_szCurrentToken, s_pModule, FALSE);
            if (n < 0) {
                ErrorReport(CAR_E_NotFound, "struct", g_szCurrentToken);
                break; // continue on error
            }
            pType->mType = Type_struct;
            pType->mIndex = n;
            break;

        default:
            if (P_BaseType(token, pType) == Ret_AbortOnError)
                return Ret_AbortOnError;
            break;
    }

    if (PeekToken(s_pFile) == Token_S_star) {
        GetToken(s_pFile);
        pType->mPointer++;
        if (PeekToken(s_pFile) == Token_S_star) {
            GetToken(s_pFile);
            pType->mPointer++;
        }
    }
    return Ret_Continue;
}

// ArrayOf      -> k_ArrayOf s_langle TYPE, SIZE s_rangle
// BufferOf     -> k_BufferOf s_langle TYPE, SIZE s_rangle
// MemoryBuf    -> k_MemoryBuf s_langle SIZE s_rangle
// StringBuf   -> k_StringBuf s_langle SIZE s_rangle
//
int P_CarQuient(TypeDescriptor *pType)
{
    int s, token;

    pType->mSize = -1;
    if (GetToken(s_pFile) != Token_S_langle) {
        ErrorReport(CAR_E_ExpectSymbol, "<");
        return Ret_AbortOnError;
    }

    if (Type_ArrayOf == pType->mType){

        pType->mNestedType = new TypeDescriptor;
        if (!pType->mNestedType) {
            ErrorReport(CAR_E_OutOfMemory);
            return Ret_AbortOnError;
        }
        memset(pType->mNestedType, 0, sizeof(TypeDescriptor));
        pType->mNestedType->mNested = 1;

        if (P_Type(pType->mNestedType) == Ret_AbortOnError) {
            goto ErrorExit;
        }

        if (Type_ArrayOf == pType->mNestedType->mType) {
            ErrorReport(CAR_E_NestedCARQuient);
            return Ret_AbortOnError;
        }

        if (Token_S_comma == PeekToken(s_pFile)) {
            GetToken(s_pFile);
            token = PeekToken(s_pFile);
            if (!(Token_integer == token) && !(Token_ident == token)){
                ErrorReport(CAR_E_IlleagalSizeType);    //< n > n must be an integer
                return Ret_AbortOnError;
            }
        }
    }

    token = GetToken(s_pFile);
    if (token == Token_integer) {
        pType->mSize = atoi(g_szCurrentToken);

        token = GetToken(s_pFile);
    }
    else if (token == Token_ident) {
        s = RetrieveConst(g_szCurrentToken, s_pModule, FALSE);
        if (s < 0) {
            ErrorReport(CAR_E_NotFound, "const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        if (s_pModule->mConstDirs[s]->mType != TYPE_INTEGER32) {
            ErrorReport(CAR_E_NotIntegerConst, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        pType->mSize = s_pModule->mConstDirs[s]->mV.mInt32Value.mValue;

        token = GetToken(s_pFile);
    }

    if (token != Token_S_rangle) {
        ErrorReport(CAR_E_ExpectSymbol, ">");
        return Ret_AbortOnError;
    }

    return Ret_Continue;

ErrorExit:
    delete pType->mNestedType;
    pType->mNestedType = NULL;

    return Ret_AbortOnError;
}

// BASE_TYPE   -> C_TYPE | CAR_TYPE
//
int P_BaseType(CARToken token, TypeDescriptor *pType)
{

    switch (token) {
        case Token_K_ECode:
            pType->mType = Type_ECode;
            break;
        case Token_K_Byte:
            pType->mType = Type_Byte;
            break;
        case Token_K_Boolean:
            pType->mType = Type_Boolean;
            break;
        case Token_K_Int8:
            pType->mType = Type_Int8;
            break;
        case Token_K_Int16:
            pType->mType = Type_Int16;
            break;
        case Token_K_UInt16:
            pType->mType = Type_UInt16;
            break;
        case Token_K_Int32:
            pType->mType = Type_Int32;
            break;
        case Token_K_UInt32:
            pType->mType = Type_UInt32;
            break;
        case Token_K_Int64:
            pType->mType = Type_Int64;
            break;
        case Token_K_UInt64:
            pType->mType = Type_UInt64;
            break;
        case Token_K_PVoid:
            pType->mType = Type_PVoid;
            break;
        case Token_K_Char16:
            pType->mType = Type_Char16;
            break;
        case Token_K_Char32:
            pType->mType = Type_Char32;
            break;
        case Token_K_Float:
            pType->mType = Type_Float;
            break;
        case Token_K_Double:
            pType->mType = Type_Double;
            break;
        case Token_K_EMuid:
            pType->mType = Type_EMuid;
            break;
        case Token_K_EGuid:
            pType->mType = Type_EGuid;
            break;
        case Token_K_EventHandler:
            pType->mType = Type_EventHandler;
            break;
        case Token_K_String:
            pType->mType = Type_String;
            break;
        case Token_K_ArrayOf:
            pType->mType = Type_ArrayOf;
            return P_CarQuient(pType);
        default:
            ErrorReport(CAR_E_UndefType, g_szCurrentToken);
            // Don't break here
        case Token_Error:
            return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// DTYPE       -> BASE_TYPE | ident
//
int P_DType(TypeDescriptor *pType)
{
    int n;
    CARToken token;

    token = GetToken(s_pFile);
    switch (token) {
        case Token_ident:
        {
            const char *pszName = g_szCurrentToken;
            char *pszNamespaces = NULL;
            const char *dot = strrchr(pszName, '.');
            if (dot != NULL) {
                pszNamespaces = (char *)malloc(dot - pszName + 1);
                memset(pszNamespaces, 0, dot - pszName + 1);
                memcpy(pszNamespaces, pszName, dot - pszName);
                pszName = dot + 1;
            }
            else {
                int nsLen = strlen(GetNamespace());
                int usingLen = strlen(s_pszUsingNS);
                if (nsLen + usingLen != 0) {
                    int len = nsLen + 1 + usingLen + 1;
                    pszNamespaces = (char *)malloc(len);
                    memset(pszNamespaces, 0, len);
                    if (nsLen != 0) strcpy(pszNamespaces, GetNamespace());
                    if (usingLen != 0) {
                        if (pszNamespaces[0] != '\0') strcat(pszNamespaces, ";");
                        strcat(pszNamespaces, s_pszUsingNS);
                    }
                }
            }

            n = RetrieveIdentifyType(pszName, pszNamespaces, s_pModule, pType, FALSE);
            if (n < 0) {
                ErrorReport(CAR_E_UndefinedSymbol, g_szCurrentToken);
                // continue on error
            }
            break;
        }
        default:
            return P_BaseType(token, pType);
    }

    return Ret_Continue;
}

// ENUM_ELEM   -> ident [ s_assign < integer | hinteger | ident > ]
// The ident must has "EnumName_" as its prefix.
int P_EnumElement(const char *pszEnumName, EnumDescriptor *pDesc, int *pValue)
{
    int r;
    EnumElement *pElement;
    int nEnumNameLen;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    nEnumNameLen = strlen(pszEnumName);
    assert(nEnumNameLen);

    if ((g_nCurrentTokenLen <= nEnumNameLen + 1 /* EnumName_ */
        || strncmp(g_szCurrentToken, pszEnumName, nEnumNameLen)
        || g_szCurrentToken[nEnumNameLen] != '_') && !s_bLenientNaming) {
        ErrorReport(CAR_W_IllegalMemberName, g_szCurrentToken);
    }

    r = CreateEnumElement(g_szCurrentToken, s_pModule, pDesc);
    if (r < 0) {
        CreateError(r, "enum member", g_szCurrentToken);
        return Ret_ContinueOnError; // try to continue
    }

    if (PeekToken(s_pFile) == Token_S_assign) {
        GetToken(s_pFile); // skip "="

        switch (GetToken(s_pFile)) {
            case Token_integer:
                pDesc->mElements[r]->mValue = atoi(g_szCurrentToken);
                break;

            case Token_hinteger:
                pDesc->mElements[r]->mValue = Hexstr2Int(g_szCurrentToken);
                pDesc->mElements[r]->mIsHexFormat = TRUE;
                break;

            case Token_ident:
                pElement = GlobalSelectEnumElement(g_szCurrentToken, s_pModule);
                if (!pElement) {
                    ErrorReport(CAR_E_NotFound, "const", g_szCurrentToken);
                    break; // continue on error
                }
                pDesc->mElements[r]->mValue = pElement->mValue;
                pDesc->mElements[r]->mIsHexFormat = pElement->mIsHexFormat;
                break;

            default:
                ErrorReport(CAR_E_IllegalValue);
            case Token_Error:
                return Ret_AbortOnError;
        }

        *pValue = pDesc->mElements[r]->mValue;
    }
    else {
        pDesc->mElements[r]->mValue = *pValue;
    }

    return Ret_Continue;
}

// ENUM_BODY   -> s_lbrace ENUM_ELEM { s_comma ENUM_ELEM }
//                [ s_comma ] s_rbrace ]
//
int P_EnumBody(const char *pszEnumName, EnumDescriptor *pDesc)
{
    CARToken token;
    int value = 0;

    do {
        if (P_EnumElement(pszEnumName, pDesc, &value) == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
        value++;

        token = GetToken(s_pFile);
        if (Token_S_comma == token) {
            if (PeekToken(s_pFile) == Token_S_rbrace)
                token = GetToken(s_pFile);
        }
    } while (Token_S_comma == token);

    if (Token_S_rbrace != token) {
        ErrorReport(CAR_E_ExpectSymbol, "}");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// ENUM        -> k_enum ident [ ENUM_BODY | s_semicolon ]
//
int P_Enum()
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectEnumName);
        return Ret_AbortOnError;
    }

    int len = strlen(GetNamespace()) + strlen(g_szCurrentToken);
    char *pszFullName = (char*)malloc(len + 2);
    memset(pszFullName, 0, len + 2);
    if (GetNamespace()[0] != 0) {
        strcpy(pszFullName, GetNamespace());
        strcat(pszFullName, ".");
    }
    strcat(pszFullName, g_szCurrentToken);
    r = CreateEnumDirEntry(pszFullName, s_pModule);

    token = PeekToken(s_pFile);

    if (Token_S_lbrace == token) {
        free(pszFullName);
        if (r < 0) {
            CreateError(r, "enum", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        GetToken(s_pFile); // ignore "{"

        if (P_EnumBody(s_pModule->mEnumDirs[r]->mName,
            s_pModule->mEnumDirs[r]->mDesc) == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }

        s_pModule->mEnumDirs[r]->mFileIndex = CreateFileDirEntry(GetSubmodule(), s_pModule);
    }
    else if (Token_S_semicolon == token) {
        AddFrontDeclaration(pszFullName, Type_interface);
        free(pszFullName);
        if (r < 0 && r != CLSError_DupEntry) {
            CreateError(r, "enum", g_szCurrentToken);
            // continue on error
        }
        GetToken(s_pFile); // ignore ";"
    }
    else {
        free(pszFullName);
        ErrorReport(CAR_E_ExpectSymbol, "{ or ;");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// CONST       -> k_const ident s_assign < integer | hinteger > s_semicolon
//
int P_Const()
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateConstDirEntry(g_szCurrentToken, s_pModule);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "const", g_szCurrentToken);
            // continue on error
        }
    }
    token = GetToken(s_pFile);
    if (Token_S_assign != token) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }
    switch (GetToken(s_pFile)) {
        case Token_integer:
            s_pModule->mConstDirs[r]->mType = TYPE_INTEGER32;
            s_pModule->mConstDirs[r]->mV.mInt32Value.mValue = atoi(g_szCurrentToken);
            s_pModule->mConstDirs[r]->mV.mInt32Value.mIsHexFormat = FALSE;
            break;

        case Token_hinteger:
            s_pModule->mConstDirs[r]->mType = TYPE_INTEGER32;
            s_pModule->mConstDirs[r]->mV.mInt32Value.mValue = Hexstr2Int(g_szCurrentToken);
            s_pModule->mConstDirs[r]->mV.mInt32Value.mIsHexFormat = TRUE;
            break;

        case Token_string:
            {
                s_pModule->mConstDirs[r]->mType = TYPE_STRING;
                char *str = (char*)malloc(strlen(g_szCurrentToken) + 1);
                strcpy(str, g_szCurrentToken);
                s_pModule->mConstDirs[r]->mV.mStrValue.mValue = str;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// ARRAY -> {s_lbracket < Token_hinteger | ident > s_rbracket}+

int P_Array(TypeDescriptor &type)
{
    unsigned short elementCount;
    CARToken token, nextToken;
    int r;

    nextToken = PeekToken(s_pFile);
    assert(Token_S_lbracket == nextToken);

    // ignore '['
    token = GetToken(s_pFile);

    token = GetToken(s_pFile);
    if (token != Token_integer
        && token != Token_hinteger) {
        if (token != Token_ident) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            r = SelectConstDirEntry(g_szCurrentToken, s_pModule);
            if (CLSError_NotFound == r) {
                ErrorReport(CAR_E_NotFound, "const", g_szCurrentToken);
                return Ret_AbortOnError;
            }
            if (s_pModule->mConstDirs[r]->mType != TYPE_INTEGER32) {
                ErrorReport(CAR_E_NotIntegerConst, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            elementCount = s_pModule->mConstDirs[r]->mV.mInt32Value.mValue;
        }
    }
    else {
        if (Token_integer == token) {
            elementCount = (unsigned short)atoi(g_szCurrentToken);
        }
        else {
            assert(Token_hinteger == token);
            elementCount = (unsigned short)Hexstr2Int(g_szCurrentToken);
        }
    }
    nextToken = PeekToken(s_pFile);
    if (nextToken != Token_S_rbracket) {
        ErrorReport(CAR_E_UnexpectSymbol, nextToken);
        return Ret_AbortOnError;
    }

    if (elementCount <= 0) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    // ignore ']'
    token = GetToken(s_pFile);

    nextToken = PeekToken(s_pFile);
    if (Token_S_lbracket == nextToken) { //Handle the case of [m][n]...
        r = P_Array(type);
        if (r < 0)
            return r;
    }

    r = SelectArrayDirEntry(elementCount, type, s_pModule);
    if (r < 0) {
        r = CreateArrayDirEntry(s_pModule);
        if (r < 0) {
            ErrorReport(CAR_E_OutOfMemory);
            return Ret_AbortOnError;
        }
        memcpy(&s_pModule->mArrayDirs[r]->mType, &type, sizeof(TypeDescriptor));
        s_pModule->mArrayDirs[r]->mElementCount = elementCount;
    }

    memset(&type, 0, sizeof(TypeDescriptor));

    type.mType = Type_Array;
    type.mIndex = r;

    return r;
}

// STRUCT_ELEM -> DTYPE ELEM_NAME { s_comma ELEM_NAME } s_semicolon
// ELEM_NAME   -> [ s_star [ s_star] ] ident [ARRAY]
//
int P_StructElement(StructDescriptor *pDesc)
{
    int r;
    TypeDescriptor type, tempType;
    CARToken token, nextToken;
    char szElementName[c_nMaxTokenSize + 1];

    memset(&type, 0, sizeof(TypeDescriptor));
    if (P_DType(&type) == Ret_AbortOnError) return Ret_AbortOnError;

    do {
        memcpy(&tempType, &type, sizeof(type));
        token = GetToken(s_pFile);

        tempType.mPointer = 0;
        if (Token_S_star == token) {
            tempType.mPointer++;
            token = GetToken(s_pFile);
            if (Token_S_star == token) {
                tempType.mPointer++;
                token = GetToken(s_pFile);
            }
        }

        if (Token_ident != token) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }

        strcpy(szElementName, g_szCurrentToken);

        if (Type_struct == tempType.mType
            && s_pModule->mStructDirs[tempType.mIndex]->mDesc == pDesc
            && 0 == tempType.mPointer) {
            ErrorReport(CAR_E_NestedStruct, g_szCurrentToken);
            // continue on error
        }
        // In case of Array
        nextToken = PeekToken(s_pFile);
        if (Token_S_lbracket == nextToken) {
            r = P_Array(tempType);
            if (r < 0) {
                return Ret_AbortOnError;
            }
        }

        r = CreateStructElement(szElementName, pDesc);
        if (r < 0) {
            CreateError(r, "struct member", szElementName);
            // continue on error
        }
        else {
            memcpy(&pDesc->mElements[r]->mType, &tempType, sizeof(tempType));
        }

        token = GetToken(s_pFile);
    } while (Token_S_comma == token);

    if (Token_S_semicolon != token) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// STRUCT_BODY -> s_lbrace { STRUCT_ELEM { s_semicolon } }+ s_rbrace
//
int P_StructBody(StructDescriptor *pDesc)
{
    do {
        if (P_StructElement(pDesc) == Ret_AbortOnError)
            return Ret_AbortOnError;
        while (PeekToken(s_pFile) == Token_S_semicolon) GetToken(s_pFile);
    } while (PeekToken(s_pFile) != Token_S_rbrace);

    GetToken(s_pFile); // skip "}"
    return Ret_Continue;
}

// STRUCT      -> k_struct ident [ STRUCT_BODY | s_semicolon ]
//
int P_Struct()
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectStructName);
        return Ret_AbortOnError;
    }
    r = CreateStructDirEntry(g_szCurrentToken, s_pModule);

    token = PeekToken(s_pFile);

    if (Token_S_lbrace == token) {
        if (r < 0) {
            CreateError(r, "struct", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        GetToken(s_pFile); // ignore "{"

        if (P_StructBody(
            s_pModule->mStructDirs[r]->mDesc) == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
    }
    else if (Token_S_semicolon == token) {
        if (r < 0 && r != CLSError_DupEntry) {
            CreateError(r, "struct", g_szCurrentToken);
            // continue on error
        }
        GetToken(s_pFile); // ignore ";"
    }
    else {
        ErrorReport(CAR_E_ExpectSymbol, "{ or ;");
        return Ret_AbortOnError;
    }

    s_pModule->mStructDirs[r]->mFileIndex = CreateFileDirEntry(GetSubmodule(), s_pModule);

    return Ret_Continue;
}

// TYPEDEF     -> k_typedef [ k_dummytype ]
//                DTYPE ALIAS { s_comma ALIAS } s_semicolon
// ALIAS       -> [ s_star [ s_star ] ] ident
//
int P_Typedef()
{
    int r;
    CARToken token;
    BOOL isDummyType = FALSE;
    TypeDescriptor type;

    if (PeekToken(s_pFile) == Token_S_lbracket) {
        GetToken(s_pFile); // skip "["
        if (GetToken(s_pFile) != Token_K_dummytype) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        if (GetToken(s_pFile) != Token_S_rbracket) {
            ErrorReport(CAR_E_ExpectSymbol, "]");
            return Ret_AbortOnError;
        }
        isDummyType = TRUE;
    }

    memset(&type, 0, sizeof(type));
    if (P_DType(&type) == Ret_AbortOnError) return Ret_AbortOnError;

    do {
        token = GetToken(s_pFile);

        type.mPointer = 0;
        if (Token_S_star == token) {
            type.mPointer++;
            token = GetToken(s_pFile);
            if (Token_S_star == token) {
                type.mPointer++;
                token = GetToken(s_pFile);
            }
        }

        if (Token_ident != token) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }

        r = CreateAliasDirEntry(g_szCurrentToken, s_pModule, &type);
        if (r < 0) {
            CreateError(r, "alias", g_szCurrentToken);
            // continue on error
        }
        else {
            s_pModule->mAliasDirs[r]->mIsDummyType = isDummyType;
        }

        token = GetToken(s_pFile);
    } while (Token_S_comma == token);

    if (Token_S_semicolon != token) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// PRAGMA      -> k_pragma s_lparen PRAGMA_EXPS s_rparen
// PRAGMA_EXPS -> < k_disalbe | k_enable > s_colon < integer | hinteger >
//
int P_Pragma()
{
    int nWarningNo;
    CARToken token;

    if (GetToken(s_pFile) != Token_S_lparen) {
        ErrorReport(CAR_E_ExpectSymbol, "(");
        return Ret_AbortOnError;
    }

    token = GetToken(s_pFile);
    if (Token_K_disable != token && Token_K_enable != token) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }
    if (GetToken(s_pFile) != Token_S_colon) {
        ErrorReport(CAR_E_ExpectSymbol, ":");
        return Ret_AbortOnError;
    }

    switch (GetToken(s_pFile)) {
        case Token_integer:
            nWarningNo = atoi(g_szCurrentToken);
            break;
        case Token_hinteger:
            nWarningNo = Hexstr2Int(g_szCurrentToken);
            break;
        default:
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
    }

    if (Token_K_enable == token)
        EnableWarning(nWarningNo);
    else
        DisableWarning(nWarningNo);

    if (GetToken(s_pFile) != Token_S_rparen) {
        ErrorReport(CAR_E_ExpectSymbol, ")");
        return Ret_AbortOnError;
    }
    return Ret_Continue;
}


void AddConstClassInterface(
    const char *pszName,
    const char *pszNamespace,
    CLSModule *pModule,
    ClassDescriptor *pDesc)
{
    int n;

    n = RetrieveInterface(pszName, pszNamespace, pModule, FALSE);
    if (n < 0) {
        //ErrorReport(CAR_E_NotFound, "interface", pszName);
        return;
    }

    n = CreateClassInterface(n, pDesc);
    if (n < 0) {
        //if (n != CLSError_DupEntry)
        //CreateError(n, "class interface", pszName);
    }
    else {
        pDesc->mInterfaces[n]->mAttribs |= ClassInterfaceAttrib_autoadded;
        pDesc->mAttribs |= ClassAttrib_defined;
    }
}

int AddConstClassInterface(const char *pszName, const char *pszNamespace, ClassDescriptor *pDesc)
{
    int n;

    n = RetrieveInterface(pszName, pszNamespace, s_pModule, FALSE);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "interface", pszName);
        return n;
    }

    n = CreateClassInterface(n, pDesc);
    if (n < 0) {
        if (n != CLSError_DupEntry)
            CreateError(n, "class interface", pszName);
    }
    else {
        pDesc->mInterfaces[n]->mAttribs |= ClassInterfaceAttrib_autoadded;
        pDesc->mAttribs |= ClassAttrib_defined;
    }
    return n;
}


//
// Generate an IID seed string from interface info.
// The seed string is compose of alphabet, number or '_', we get them
// from name string, the selective order is:
//
//      (interface name) -> (method names) ->
//      (parent name) -> (parent method names) ->
//      (parent's parent name) -> ... and so on,
//
void GenIIDSeedString(
    CLSModule *pModule,
    InterfaceDirEntry *pInterface,
    char *pszBuf)
{
    int n, nSize, nTotalSize = 0;
    InterfaceDescriptor *pDesc = pInterface->mDesc;

    while (1) {
        nSize = strlen(pInterface->mName);
        if (nSize + nTotalSize + 1 > MAX_SEED_SIZE) break;
        memcpy(pszBuf, pInterface->mName, nSize);
        pszBuf += nSize;
        *pszBuf++ = '/';
        nTotalSize += nSize + 1;

        if (pInterface->mNameSpace != NULL) {
            nSize = strlen(pInterface->mNameSpace);
            if (nSize + nTotalSize + 1 > MAX_SEED_SIZE) break;
            memcpy(pszBuf, pInterface->mNameSpace, nSize);
            pszBuf += nSize;
            *pszBuf++ = '/';
            nTotalSize += nSize + 1;
        }

        for (n = 0; n < pDesc->mMethodCount; n++) {
            nSize = strlen(pDesc->mMethods[n]->mName);
            if (nSize + nTotalSize > MAX_SEED_SIZE) goto ExitEntry;
            memcpy(pszBuf, pDesc->mMethods[n]->mName, nSize);
            pszBuf += nSize;
            nTotalSize += nSize;
        }
        if (0 == pDesc->mParentIndex) break;

        pInterface = pModule->mInterfaceDirs[pDesc->mParentIndex];
        pDesc = pInterface->mDesc;
    }

ExitEntry:
    *pszBuf = 0;
}

void GenerateSinkClass(
    const char *pszName,
    CLSModule *pModule,
    ClassDirEntry *pClass)
{
    int n;
    ClassDescriptor *pSinkDesc;
    char szSinkName[c_nMaxTokenSize + 1];
    char szInterfaceName[c_nMaxTokenSize + 1];
    ClassDescriptor *pDesc = pClass->mDesc;
    CLSID *pClsid, *pSinkClsid;

    strcpy(szSinkName, pszName);
    strcat(szSinkName, "Sink");

    n = CreateClassDirEntry(szSinkName, pModule, ClassAttrib_t_sink);
    if (n < 0) {
        return;
    }
    if (NULL != pClass->mNameSpace) {
        pModule->mClassDirs[n]->mNameSpace =
                        (char *)malloc(strlen(pClass->mNameSpace) + 1);
        if (!pModule->mClassDirs[n]->mNameSpace) return;
        strcpy(pModule->mClassDirs[n]->mNameSpace, pClass->mNameSpace);
    }
    pSinkDesc = pModule->mClassDirs[n]->mDesc;

    pClsid = &pDesc->mClsid;
    pSinkClsid = &pSinkDesc->mClsid;
    pSinkClsid->mData1 = pClsid->mData1 ^ CLSID_XOR_CallbackSink.mData1;
    pSinkClsid->mData2 = pClsid->mData2 ^ CLSID_XOR_CallbackSink.mData2;
    pSinkClsid->mData3 = pClsid->mData3 ^ CLSID_XOR_CallbackSink.mData3;
    pSinkClsid->mData4[0] = pClsid->mData4[0] ^ CLSID_XOR_CallbackSink.mData4[0];
    pSinkClsid->mData4[1] = pClsid->mData4[1] ^ CLSID_XOR_CallbackSink.mData4[1];
    pSinkClsid->mData4[2] = pClsid->mData4[2] ^ CLSID_XOR_CallbackSink.mData4[2];
    pSinkClsid->mData4[3] = pClsid->mData4[3] ^ CLSID_XOR_CallbackSink.mData4[3];
    pSinkClsid->mData4[4] = pClsid->mData4[4] ^ CLSID_XOR_CallbackSink.mData4[4];
    pSinkClsid->mData4[5] = pClsid->mData4[5] ^ CLSID_XOR_CallbackSink.mData4[5];
    pSinkClsid->mData4[6] = pClsid->mData4[6] ^ CLSID_XOR_CallbackSink.mData4[6];
    pSinkClsid->mData4[7] = pClsid->mData4[7] ^ CLSID_XOR_CallbackSink.mData4[7];

    AddConstClassInterface("IObject", NULL, pModule, pSinkDesc);
    AddConstClassInterface("ICallbackSink", NULL, pModule, pSinkDesc);
    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback){
            strcpy(szInterfaceName, pModule->mInterfaceDirs[pDesc->mInterfaces[n]->mIndex]->mName);
            strcat(szInterfaceName, "Callback");
            int x = SelectInterfaceDirEntry(szInterfaceName, NULL, pModule);
            if (x < 0) continue;
            int m = CreateClassInterface(x, pSinkDesc);
            if (m > 0 && (pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_async)) {
                pSinkDesc->mInterfaces[m]->mAttribs |= ClassInterfaceAttrib_async;
            }
            if (m > 0 && (pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_delegate)) {
                pSinkDesc->mInterfaces[m]->mAttribs |= ClassInterfaceAttrib_delegate;
            }
            if (!(pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_async)
                && !(pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_delegate)) {
                szInterfaceName[strlen(szInterfaceName)-8] = '\0';
                strcat(szInterfaceName, "Handler");
                x = AddConstClassInterface(szInterfaceName, NULL, pSinkDesc);
                if (x > 0) pSinkDesc->mInterfaces[x]->mAttribs |= ClassInterfaceAttrib_handler;
            }
        }
    }
    pSinkDesc->mAttribs |= ClassAttrib_t_sink | ClassAttrib_naked;
    pModule->mAttribs |= CARAttrib_hasSinkObject;
}

int GenerateSinkInterface(
    const char *pszName,
    CLSModule *pModule,
    InterfaceDirEntry *pInterface)
{
    int i=0, n=0, x=0, m=0;
    InterfaceDescriptor *pSinkDesc = NULL;
    char szSinkName[c_nMaxTokenSize + 1] = {0};
    InterfaceDescriptor *pSrcDesc = pInterface->mDesc;
    ParamDescriptor *pParam = NULL;

    strcpy(szSinkName, pszName);
    strcat(szSinkName, "Callback");

    x = SelectInterfaceDirEntry(szSinkName, NULL, pModule);
    if (x >= 0)
        return x;
    x = n = CreateInterfaceDirEntry(szSinkName, pModule, InterfaceAttrib_sink);
    if (n < 0) {
        //CreateError(n, "interfcace", szSinkName);
        return 0;
    }
    pSinkDesc = pModule->mInterfaceDirs[n]->mDesc;
    pModule->mDefinedInterfaceIndexes[pModule->mDefinedInterfaceCount++] = n;
    InterfaceDescriptorCopy(pModule, pSrcDesc, pModule, pSinkDesc, TRUE);

    pSinkDesc->mAttribs |= InterfaceAttrib_sink;
    pSinkDesc->mAttribs &= ~InterfaceAttrib_t_callbacks;
    pSinkDesc->mAttribs |= InterfaceAttrib_t_normal;
    for (n = 0; n < pSrcDesc->mMethodCount; n++) {
        m = CreateMethodParam("cFlags", pSinkDesc->mMethods[n]);
        if (m < 0) return 0;
        pParam = pSinkDesc->mMethods[n]->mParams[m];

        for (i = m; i > 0;--i) {
            pSinkDesc->mMethods[n]->mParams[i] = pSinkDesc->mMethods[n]->mParams[i-1];
        }
        pSinkDesc->mMethods[n]->mParams[0] = pParam;
        pParam->mAttribs = ParamAttrib_in;
        memset(&pParam->mType, 0, sizeof(pParam->mType));
        pParam->mType.mType = Type_Int32;
    }

    return x;
}


int GenerateAsyncCallbackInterface(
    const char *pszName,
    CLSModule *pModule,
    InterfaceDirEntry *pInterface)
{
    int n=0, x=0;
    InterfaceDescriptor *pSinkDesc = NULL;
    char szSinkName[c_nMaxTokenSize + 1] = {0};
    InterfaceDescriptor *pSrcDesc = pInterface->mDesc;
    char szMethodName[c_nMaxTokenSize + 1];

    strcpy(szSinkName, pszName);
    strcat(szSinkName, "Async");

    x = SelectInterfaceDirEntry(szSinkName, NULL, pModule);
    if (x >= 0)
        return x;
    x = n = CreateInterfaceDirEntry(szSinkName, pModule, InterfaceAttrib_defined);
    if (n < 0) {
        //CreateError(n, "interfcace", szSinkName);
        return 0;
    }
    pSinkDesc = pModule->mInterfaceDirs[n]->mDesc;
    pModule->mDefinedInterfaceIndexes[pModule->mDefinedInterfaceCount++] = n;
    InterfaceDescriptorCopy(pModule, pSrcDesc, pModule, pSinkDesc, TRUE);

    for (n = 0; n < pSrcDesc->mMethodCount; n++) {
        strcpy(szMethodName, "begin");
        strcat(szMethodName, pSinkDesc->mMethods[n]->mName);
        delete [] pSinkDesc->mMethods[n]->mName;
        pSinkDesc->mMethods[n]->mName = new char[strlen(szMethodName)+1];
        strcpy(pSinkDesc->mMethods[n]->mName, szMethodName);
    }

    return x;
}

BOOL IsMethodDuplicated(InterfaceDescriptor* pIntfDesc, MethodDescriptor* pMethod);
int BuildMethodSignature(MethodDescriptor *pMethod);

int GenerateHandlerInterface(
    const char *pszName,
    CLSModule *pModule,
    InterfaceDirEntry *pInterface)
{
    int i=0, n=0, x=0, m=0, r=0;
    InterfaceDescriptor *pSinkDesc = NULL;
    char szSinkName[c_nMaxTokenSize + 1] = {0};
    InterfaceDescriptor *pSrcDesc = pInterface->mDesc;
    ParamDescriptor *pParam = NULL;
    char szMethodName[c_nMaxTokenSize + 1];

    strcpy(szSinkName, pszName);
    strcat(szSinkName, "Handler");

    x = RetrieveInterface(szSinkName, NULL, pModule, TRUE);
    if (x >= 0) return x;
    x = n = CreateInterfaceDirEntry(szSinkName, pModule, InterfaceAttrib_defined);
    if (n < 0) return 0;
    r = RetrieveInterface("ICallbackRendezvous", NULL, pModule, FALSE);
    if (r < 0) return 0;

    pSinkDesc = pModule->mInterfaceDirs[n]->mDesc;
    pModule->mDefinedInterfaceIndexes[pModule->mDefinedInterfaceCount++] = n;

    pSinkDesc->mAttribs |= InterfaceAttrib_defined;
    pSinkDesc->mAttribs |= InterfaceAttrib_sink;
    pSinkDesc->mAttribs |= InterfaceAttrib_t_normal;
    for (i = 0; i < pSrcDesc->mMethodCount; i++) {
        //////// AddXXXCallback(pThis, pFunc, pUserData) ////////
        strcpy(szMethodName, "Add");
        strcat(szMethodName, pSrcDesc->mMethods[i]->mName);
        strcat(szMethodName, "Callback");

        n = CreateInterfaceMethod(szMethodName, pSinkDesc);
        if (m < 0) return 0;

        pSinkDesc->mMethods[n]->mType.mType = Type_ECode;

        m = CreateMethodParam("handler", pSinkDesc->mMethods[n]);
        if (m < 0) return 0;
        pParam = pSinkDesc->mMethods[n]->mParams[m];

        pSinkDesc->mMethods[n]->mParams[0] = pParam;
        pParam->mAttribs = ParamAttrib_in;
        memset(&pParam->mType, 0, sizeof(pParam->mType));
        pParam->mType.mType = Type_EventHandler;
        pParam->mType.mPointer = 0;

        BuildMethodSignature(pSinkDesc->mMethods[n]);
        pSinkDesc->mMethodCount -= 1;
        if (IsMethodDuplicated(pSinkDesc, pSinkDesc->mMethods[n])) {
            return 0;
        }
        pSinkDesc->mMethodCount += 1;

        //////// RemoveXXXCallback(pThis, pFunc, pUserData) ////////
        strcpy(szMethodName, "Remove");
        strcat(szMethodName, pSrcDesc->mMethods[i]->mName);
        strcat(szMethodName, "Callback");

        n = CreateInterfaceMethod(szMethodName, pSinkDesc);
        if (m < 0) return 0;
        pSinkDesc->mMethods[n]->mType.mType = Type_ECode;

        m = CreateMethodParam("handler", pSinkDesc->mMethods[n]);
        if (m < 0) return 0;
        pParam = pSinkDesc->mMethods[n]->mParams[m];

        pSinkDesc->mMethods[n]->mParams[0] = pParam;
        pParam->mAttribs = ParamAttrib_in;
        memset(&pParam->mType, 0, sizeof(pParam->mType));
        pParam->mType.mType = Type_EventHandler;
        pParam->mType.mPointer = 0;

        BuildMethodSignature(pSinkDesc->mMethods[n]);
        pSinkDesc->mMethodCount -= 1;
        if (IsMethodDuplicated(pSinkDesc, pSinkDesc->mMethods[n])) {
            return 0;
        }
        pSinkDesc->mMethodCount += 1;

        //////// AcquireXXXRendezvous(pThis, pFunc, pUserData) ////////
        strcpy(szMethodName, "Acquire");
        strcat(szMethodName, pSrcDesc->mMethods[i]->mName);
        strcat(szMethodName, "Rendezvous");

        n = CreateInterfaceMethod(szMethodName, pSinkDesc);
        if (m < 0) return 0;
        pSinkDesc->mMethods[n]->mType.mType = Type_ECode;

        m = CreateMethodParam("ppRendezvous", pSinkDesc->mMethods[n]);
        if (m < 0) return 0;
        pParam = pSinkDesc->mMethods[n]->mParams[m];
        pSinkDesc->mMethods[n]->mParams[0] = pParam;
        pParam->mAttribs = ParamAttrib_out;
        memset(&pParam->mType, 0, sizeof(pParam->mType));
        pParam->mType.mType = Type_interface;
        pParam->mType.mPointer = 2;
        pParam->mType.mIndex = r;

        BuildMethodSignature(pSinkDesc->mMethods[n]);
        pSinkDesc->mMethodCount -= 1;
        if (IsMethodDuplicated(pSinkDesc, pSinkDesc->mMethods[n])) {
            return 0;
        }
        pSinkDesc->mMethodCount += 1;
    }

    return x;
}

static int s_nCallbackEnumValue = 0;
int CreateEnumElems(
    int methods,
    InterfaceDescriptor*pIntf,
    EnumDescriptor* pEnumDesc,
    char* pElemName,
    CLSModule *pModule)
{
    int i, m, n;
    bool flag = false;
    char tmp[c_nMaxTokenSize + 1];

    //if callback interface have parent
    if (pIntf->mParentIndex > 0) {
        CreateEnumElems(pModule->mInterfaceDirs[pIntf->mParentIndex]->mDesc->mMethodCount,
            pModule->mInterfaceDirs[pIntf->mParentIndex]->mDesc, pEnumDesc, pElemName, pModule);
    }

    // the name of the enum element
    for (i = 0; i < methods; i++) {
        strcpy(tmp, pElemName);
        strcat(tmp, pIntf->mMethods[i]->mName);
        for (n = 0; n < pEnumDesc->mElementCount; n++) {
            if (!strcmp(tmp, pEnumDesc->mElements[n]->mName)) {
                flag = true;
                break;
            }
        }
        if (flag == true)
            break;
        m = CreateEnumElement(tmp, pModule, pEnumDesc);
        if (m < 0) {
            //CreateError(m, "enum member", pElemName);
            return -1; // try to continue
        }
        pEnumDesc->mElements[m]->mValue = s_nCallbackEnumValue++;
    }

    return 0;
}

int CreateEnum(
    ClassDirEntry *pClass,
    InterfaceDescriptor *pIntf,
    char* pIntfName,
    CLSModule *pModule)
{
    char enumToken[c_nMaxTokenSize + 1];
    EnumDescriptor *pEnumDesc;
    int methods;
    char elemName[c_nMaxTokenSize + 1];
   // char szInterfaceName[c_nMaxTokenSize + 1];
    int m;

    //name of the enum entry
    strcpy(enumToken, "_");
    strcat(enumToken, pClass->mName);
    strcat(enumToken, "Event");

    m = SelectEnumDirEntry(enumToken, NULL, pModule);
    if (m < 0) {
        m = CreateEnumDirEntry(enumToken, pModule);
        if (m < 0) {
            //   CreateError(m, "enum Dir", enumToken);
            return -1;
        }
    }

    pEnumDesc = pModule->mEnumDirs[m]->mDesc;

    //begin of the callback interface enum
    strcpy(enumToken, pClass->mName);
    strcat(enumToken, "Sink_");
    strcat(enumToken, pIntfName);  //interface name
    strcat(enumToken, "Begin");

    m = CreateEnumElement(enumToken, pModule, pEnumDesc);
    if (m < 0) {
        //  CreateError(m, "enum member", enumToken);
        return 0; // try to continue
    }
    pEnumDesc->mElements[m]->mValue = s_nCallbackEnumValue++;

    methods = pIntf->mMethodCount;

    strcpy(elemName, pClass->mName);
    strcat(elemName, "_");

    CreateEnumElems(methods, pIntf, pEnumDesc, elemName, pModule);

    //end of the callback interface enum
    strcpy(enumToken, pClass->mName);
    strcat(enumToken, "Sink_");
    strcat(enumToken, pIntfName);  //interface name
    strcat(enumToken, "End");

    m = CreateEnumElement(enumToken, pModule, pEnumDesc);
    if (m < 0) {
        //  CreateError(m, "enum member", enumToken);
        return 0; // try to continue
    }
    pEnumDesc->mElements[m]->mValue = s_nCallbackEnumValue++;

    return 0;
}

int GenerateEnums(CLSModule *pModule)
{
    int j, m, n, k=0;
    ClassDirEntry *pClass=NULL;
    ClassDescriptor *pDesc=NULL;
    InterfaceDirEntry *pIntf=NULL;
    InterfaceDescriptor *pIntfDesc=NULL;
    char szInterfaceName[c_nMaxTokenSize + 1];
    EnumDescriptor *pEnumDesc;
    char enumToken[c_nMaxTokenSize + 1];

    for (k = 0; k < pModule->mClassCount; k++) {
        pClass = pModule->mClassDirs[k];
        pDesc = pClass->mDesc;
        if (pDesc->mAttribs & ClassAttrib_hascallback) {
            for (n = 0; n < pDesc->mInterfaceCount; n++) {
                if (pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback) {
                    pIntf = pModule->mInterfaceDirs[pDesc->mInterfaces[n]->mIndex];
                    pIntfDesc = pIntf->mDesc;

                    /*
                     * check callback interface of original class
                     */
                    strcpy(szInterfaceName, pIntf->mName);
                    j = RetrieveInterface(szInterfaceName, NULL, pModule, FALSE);
                    if (j < 0) {
                        return -1;
                    }

                    /*
                     * generate enum and interface for callback
                     */
                    CreateEnum(pClass, pIntfDesc, szInterfaceName, pModule);
                }
            }
            /*
             * generate Sink_MaxEvents enum
             */
            strcpy(enumToken, "_");
            strcat(enumToken, pClass->mName);
            strcat(enumToken, "Event");

            m = SelectEnumDirEntry(enumToken, NULL, pModule);
            if (m < 0) {
                return -2;
            }

            pEnumDesc = pModule->mEnumDirs[m]->mDesc;

            strcpy(enumToken, pClass->mName);
            strcat(enumToken, "Sink_MaxEvents");

            m = CreateEnumElement(enumToken, pModule, pEnumDesc);
            if (m < 0) {
                return -3;
            }

            pEnumDesc->mElements[m]->mValue = s_nCallbackEnumValue++;
            s_nCallbackEnumValue = 0;
        }
    }

    return 0;
}

int ExtendCLS(CLSModule *pModule, BOOL bNoElastos)
{
    int j, n, k, x=0;
    ClassDirEntry *pClass=NULL;
    ClassDescriptor *pDesc=NULL;
    InterfaceDirEntry *pIntf=NULL;
    char szInterfaceName[c_nMaxTokenSize + 1];
    char szSeedBuf[c_nMaxTokenSize + 1];
    CLSModule *pModuleTmp;

    for (k = 0; k < pModule->mClassCount; k++) {
        pClass = pModule->mClassDirs[k];
        pDesc = pClass->mDesc;
        if ((pDesc->mAttribs & ClassAttrib_hascallback) &&
                !(pDesc->mAttribs & ClassAttrib_t_generic) &&
                !(pDesc->mAttribs & ClassAttrib_t_external)) {
            if (!bNoElastos) {
                if ((CLS_NoError == LoadCLSFromDll("Elastos.Runtime.eco", &pModuleTmp))
                    || (LoadCLS("ElastosCore.cls", &pModule) == CLS_NoError)) {
                    int nIndex = SelectInterfaceDirEntry("ICallbackSink", NULL, pModuleTmp);
                    if (nIndex >= 0) InterfaceCopy(pModuleTmp, nIndex, pModule, FALSE);
                    DisposeFlattedCLS(pModuleTmp);
                }
            }

            for (n = 0; n < pDesc->mInterfaceCount; n++) {
                if (pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback) {
                    pIntf = pModule->mInterfaceDirs[pDesc->mInterfaces[n]->mIndex];

                    /*
                     * check callback interface of original class
                     */
                    strcpy(szInterfaceName, pIntf->mName);
                    j = RetrieveInterface(szInterfaceName, NULL, pModule, FALSE);
                    if (j < 0) {
                        return -1;
                    }

                    /*
                     * generate enum and interface for callback
                     */
                    x = GenerateSinkInterface(szInterfaceName, pModule, pModule->mInterfaceDirs[j]);

                    GenIIDSeedString(pModule, pModule->mInterfaceDirs[x], szSeedBuf);
                    GuidFromSeedString(szSeedBuf, &pModule->mInterfaceDirs[x]->mDesc->mIID);
                }
            }
            /*
             * generate sink class for callback
             */
            GenerateSinkClass(pClass->mName, pModule, pClass);
        }
    }

    return 0;
}


int ImportCAR(const char *pszFile)
{
    FILE *pFile;
    CLSModule *pModule;
    void *pvCtxToken, *pvCtxError;
    CLSModule *pImportCLS;
    char *pszCtxCurrentPath;

    pImportCLS = CreateCLS();
    if (!pImportCLS) {
        ErrorReport(CAR_E_OutOfMemory);
        return Ret_AbortOnError;
    }

    // Save compiling context of current file.
    //
    pvCtxToken = SaveTokenContext();
    pvCtxError = SaveErrorContext();
    pszCtxCurrentPath = new char[strlen(s_pszCurrentPath) + 1];
    strcpy(pszCtxCurrentPath, s_pszCurrentPath);
    pFile = s_pFile;
    pModule = s_pModule;

    DisableErrorCounting();
    ErrorStringFix("import ", "  -> import error ignored.");

    // Compiling imported car file and ignore any errors.
    //
    if (GetCurrentCarPath(s_pszCarFullName, &s_pszCurrentPath) == Ret_AbortOnError) {
        delete[] pszCtxCurrentPath;
        return Ret_AbortOnError;
    }

    if (DoCompiling(pszFile, pImportCLS) == Ret_AbortOnError) {
        DestroyCLS(pImportCLS);
    }
    else {
        AddCLSLibrary(pImportCLS);
    }

    ErrorStringFix(NULL, NULL);
    EnableErrorCounting();

    // Restore compiling context of current file.
    //
    s_pModule = pModule;
    s_pFile = pFile;
    assert(s_pszCurrentPath);
    delete[] s_pszCurrentPath;
    s_pszCurrentPath = pszCtxCurrentPath;
    RestoreErrorContext(pvCtxError);
    RestoreTokenContext(pvCtxToken);

    return Ret_Continue;
}

int ImportProc(char *pszName)
{
    int n, nNameLen;

    nNameLen = strlen(pszName);

    // check suffix and replace .car with .cls
    //
    if (_stricmp(pszName + nNameLen - 4, ".car")) {
        ErrorReport(CAR_W_UnexpectFileType, pszName);
        return Ret_ContinueOnError;
    }
    strcpy(pszName + nNameLen - 4, ".cls");

    // check same entry
    //
    for (n = 0; n < s_pModule->mLibraryCount; n++) {
        if (!_stricmp(pszName, s_pModule->mLibraryNames[n]))
            return Ret_Continue;
    }

    // add library name to module
    //
    if (MAX_LIBRARY_NUMBER == n) {
        ErrorReport(CAR_W_TooManyLibraries);
        return Ret_Continue;
    }
    s_pModule->mLibraryNames[n] = new char[nNameLen + 1];
    if (!s_pModule->mLibraryNames[n]) {
        ErrorReport(CAR_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    strcpy(s_pModule->mLibraryNames[n], pszName);
    s_pModule->mLibraryCount++;

    strcpy(pszName + nNameLen - 4, ".car");
    return ImportCAR(pszName);
}

// IMPORT      -> k_import s_lparen string s_rparen s_semicolon
//
int P_Import()
{
    if (GetToken(s_pFile) != Token_S_lparen ||
        GetToken(s_pFile) != Token_string) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    if (ImportProc(g_szCurrentToken) == Ret_AbortOnError)
        return Ret_AbortOnError;

    if (GetToken(s_pFile) != Token_S_rparen ||
        GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int ImportLibrary(const char *pszName)
{
    int n;
    CLSModule *pModule;

    // skip Elastos.Runtime.eco
    //
    if (!_stricmp(pszName, "Elastos.Runtime.eco")) return Ret_Continue;

    // check same entry
    //
    for (n = 0; n < s_pModule->mLibraryCount; n++) {
        if (!_stricmp(pszName, s_pModule->mLibraryNames[n]))
            return Ret_Continue;
    }

    // add library name to module
    //
    if (MAX_LIBRARY_NUMBER == n) {
        ErrorReport(CAR_W_TooManyLibraries);
        return Ret_Continue;
    }
    s_pModule->mLibraryNames[n] = new char[strlen(pszName) + 1];
    if (!s_pModule->mLibraryNames[n]) {
        ErrorReport(CAR_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    strcpy(s_pModule->mLibraryNames[n], pszName);
    s_pModule->mLibraryCount++;

    // load it from file and add it to library
    //
    if (LoadCLS(pszName, &pModule) < 0) {
        ErrorReport(CAR_W_LoadLibrary, pszName);
        return Ret_Continue;
    }
    AddCLSLibrary(pModule);

    return Ret_Continue;
}

typedef struct ModuleInfo {
    char        *pszUunm;
    GUID        uuid;
    unsigned char       mMajorVersion;
    unsigned char       mMinorVersion;
    DWORD       mAttribs;
    char        *mName;
}   ModuleInfo;

int MergeCAR(const char *pszFile)
{
    int nRet;
    FILE *pFile;
    ModuleInfo *pInfo, info;
    void *pvCtxToken, *pvCtxError;
    char *pszCtxCurrentPath;

    if (_stricmp(pszFile + strlen(pszFile) - 4, ".car")) {
        ErrorReport(CAR_W_UnexpectFileType, pszFile);
        return Ret_ContinueOnError;
    }

    // Save compiling context of current file.
    //
    pvCtxToken = SaveTokenContext();
    pvCtxError = SaveErrorContext();
    pszCtxCurrentPath = new char[strlen(s_pszCurrentPath) + 1];
    strcpy(pszCtxCurrentPath, s_pszCurrentPath);
    pFile = s_pFile;

    // Save module info of current file
    //
    pInfo = (ModuleInfo *)&s_pModule->mUunm;
    memcpy(&info, pInfo, sizeof(ModuleInfo));
    memset(pInfo, 0, sizeof(ModuleInfo));

    ErrorStringFix("merge ", NULL);
    nRet = DoCompiling(pszFile, s_pModule);
    ErrorStringFix(NULL, NULL);

    // Free allocated resource of merged module, and restore module
    // info of current file
    //
    if (pInfo->pszUunm) delete [] pInfo->pszUunm;
    if (pInfo->mName) delete [] pInfo->mName;
    memcpy(pInfo, &info, sizeof(ModuleInfo));

    // Restore compiling context of current file.
    //
    s_pFile = pFile;

    assert(s_pszCurrentPath);
    delete[] s_pszCurrentPath;
    s_pszCurrentPath = pszCtxCurrentPath;
    RestoreErrorContext(pvCtxError);
    RestoreTokenContext(pvCtxToken);

    return nRet;
}

// MERGE       -> k_merge s_lparen string s_rparen s_semicolon
//
int P_Merge()
{
    if (GetToken(s_pFile) != Token_S_lparen ||
        GetToken(s_pFile) != Token_string) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    if (MergeCAR(g_szCurrentToken) == Ret_AbortOnError)
        return Ret_AbortOnError;

    GenCarDependences(s_pszCarFullName, 1);

    if (GetToken(s_pFile) != Token_S_rparen ||
        GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// IMPORTLIB   -> k_importlib s_lparen string s_rparen s_semicolon
//
int P_ImportLibrary()
{
    if (GetToken(s_pFile) != Token_S_lparen ||
        GetToken(s_pFile) != Token_string) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    if (ImportLibrary(g_szCurrentToken) == Ret_AbortOnError)
        return Ret_AbortOnError;

    if (GetToken(s_pFile) != Token_S_rparen ||
        GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int MergeLibrary(const char *pszName)
{
    CLSModule *pModule;

    if (LoadCLS(pszName, &pModule) < 0) {
        ErrorReport(CAR_E_LoadLibrary, pszName);
        return Ret_ContinueOnError;
    }

    if (MergeCLS(pModule, s_pModule) < 0) {
        ErrorReport(CAR_E_MergeCLS, pszName);
        // continue on error
    }

    char *pszPath = getenv("PATH");
    char szLibFullName[_MAX_PATH];
    SearchFileFromPath(pszPath, pszName, szLibFullName);
    GenCarDependences(szLibFullName, 1);

    return Ret_Continue;
}

// MERGELIB    -> k_merge s_lparen string s_rparen s_semicolon
//
int P_MergeLibrary()
{
    if (GetToken(s_pFile) != Token_S_lparen ||
        GetToken(s_pFile) != Token_string) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    if (MergeLibrary(g_szCurrentToken) == Ret_AbortOnError)
        return Ret_AbortOnError;

    if (GetToken(s_pFile) != Token_S_rparen ||
        GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// @ident(ident = "xxxx", ident = "xxxx")
// @ident
int P_Annotation(AnnotationDescriptor** pAnnotationDescs, int index)
{
    CARToken token = GetToken(s_pFile);

    if (token != Token_ident) {
        ErrorReport(CAR_E_ExpectAnnotationName, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    char *tokenStr = g_szCurrentToken;
    char *pszName = NULL;
    char *pszNamespace = NULL;
    char *dot = strrchr(tokenStr, '.');
    if (dot != NULL) {
        pszNamespace = (char *)malloc(dot - tokenStr + 1);
        memcpy(pszNamespace, g_szCurrentToken, dot - tokenStr);
        pszNamespace[dot - tokenStr] = '\0';
        tokenStr = dot + 1;
    }
    pszName = (char*)malloc(strlen(tokenStr) + 1);
    strcpy(pszName, tokenStr);

    for (int i = 0; i < index; i++) {
        if (!strcmp(pAnnotationDescs[i]->mName, pszName) &&
            ((pAnnotationDescs[i]->mNameSpace == NULL && pszNamespace == NULL) ||
            (pAnnotationDescs[i]->mNameSpace != NULL && pszNamespace != NULL &&
                !strcmp(pAnnotationDescs[i]->mNameSpace, pszNamespace)))) {
            ErrorReport(CAR_E_DupAnnotationName, pszName);
            free(pszName);
            if (pszNamespace != NULL) free(pszNamespace);
            return Ret_AbortOnError;
        }
    }

    int keyPairCount = 0;
    KeyValuePair* pKeyValuePairs[MAX_ANNOTATION_KEY_VALUE_PAIR_NUMBER];

    if ((token = PeekToken(s_pFile)) == Token_S_lparen) {
        while (token != Token_S_rparen) {
            GetToken(s_pFile);

            token = GetToken(s_pFile);
            if (token != Token_ident) {
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            KeyValuePair* pKeyValuePair = new KeyValuePair();
            pKeyValuePair->mKey = (char*)malloc(strlen(g_szCurrentToken) + 1);
            strcpy(pKeyValuePair->mKey, g_szCurrentToken);

            if (GetToken(s_pFile) != Token_S_assign) {
                free(pKeyValuePair->mKey);
                delete pKeyValuePair;
                ErrorReport(CAR_E_ExpectSymbol, "=");
                return Ret_AbortOnError;
            }

            if (GetToken(s_pFile) != Token_string) {
                free(pKeyValuePair->mKey);
                delete pKeyValuePair;
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            int strLen = strlen(g_szCurrentToken);
            pKeyValuePair->mValue = (char*)malloc(strLen - 1);
            memcpy(pKeyValuePair->mValue, g_szCurrentToken + 1, strLen - 2);
            pKeyValuePair->mValue[strLen - 2] = '\0';

            token = PeekToken(s_pFile);
            if (token != Token_S_comma && token != Token_S_rparen) {
                free(pKeyValuePair->mKey);
                free(pKeyValuePair->mValue);
                delete pKeyValuePair;
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            pKeyValuePairs[keyPairCount++] = pKeyValuePair;
        }

        GetToken(s_pFile);
    }

    pAnnotationDescs[index] = new AnnotationDescriptor();
    pAnnotationDescs[index]->mName = pszName;
    pAnnotationDescs[index]->mNameSpace = pszNamespace;
    pAnnotationDescs[index]->mKeyValuePairCount = keyPairCount;
    pAnnotationDescs[index]->mKeyValuePairs = (KeyValuePair**)malloc(sizeof(KeyValuePair*) * keyPairCount);
    for (int i = 0; i < keyPairCount; i++) {
        pAnnotationDescs[index]->mKeyValuePairs[i] = pKeyValuePairs[i];
    }

    return Ret_Continue;
}

#define OPKIND_NONE 0
#define OPKIND_AND 1
#define OPKIND_EOR 2
#define OPKIND_IOR 3
#define OPKIND_LSHIFT 4
#define OPKIND_RSHIFT 5
#define OPKIND_MULTIP 6
#define OPKIND_ADD 7

int P_Boolean(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;
    BOOL doNot = FALSE;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
        doNot = TRUE;
    }

    switch (token = GetToken(s_pFile)) {
        case Token_K_false:
        case Token_K_true:
            {
                BOOL b = token == Token_K_true ? TRUE : FALSE;
                if (opKind == OPKIND_NONE) {
                    pDesc->mType = TYPE_BOOLEAN;
                    pDesc->mV.mBoolValue = doNot ? b ^ TRUE : b;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mBoolValue &= doNot ? b ^ TRUE : b;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mBoolValue ^= doNot ? b ^ TRUE : b;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mBoolValue |= doNot ? b ^ TRUE : b;
                }
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                BOOL b;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_BOOLEAN:
                        b = pSrcDesc->mV.mBoolValue;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mType = TYPE_BOOLEAN;
                pDesc->mV.mBoolValue = doNot ? b ^ TRUE : b;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int ToCharacter(const char* token)
{
    if (token[0] != '\\') return token[0];
    else {
        switch (token[1]) {
            case 'a' : return '\a';
            case 'b' : return '\b';
            case 'f' : return '\f';
            case 'n' : return '\n';
            case 'r' : return '\r';
            case 't' : return '\t';
            case 'v' : return '\v';
            case '\\': return '\\';
            case '\'': return '\'';
            case '\"': return '\"';
            case '0' : return '\0';
            default:
            assert(0);
            return -1;
        }
    }
}

int P_Char32(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
    }

    switch (token = GetToken(s_pFile)) {
        case Token_character:
            pDesc->mType = TYPE_CHAR32;
            pDesc->mV.mInt32Value.mValue = ToCharacter(g_szCurrentToken);
            pDesc->mV.mInt32Value.mFormat = 0;
            break;

        case Token_integer:
        case Token_hinteger:
            {
                int v = token == Token_integer ?
                        atoi(g_szCurrentToken) : Hexstr2Int(g_szCurrentToken);
                pDesc->mType = TYPE_CHAR32;
                pDesc->mV.mInt32Value.mValue = v;
                pDesc->mV.mInt32Value.mFormat =
                        token == Token_integer ? FORMAT_DECIMAL : FORMAT_HEX;
            }
            break;

        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_Byte(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;
    BOOL doNot = FALSE;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
        doNot = TRUE;
    }

    switch (token = GetToken(s_pFile)) {
        case Token_integer:
        case Token_hinteger:
            {
                int v = token == Token_integer ?
                        atoi(g_szCurrentToken) : Hexstr2Int(g_szCurrentToken);

                if (opKind == OPKIND_NONE) {
                    pDesc->mType = TYPE_BYTE;
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                    pDesc->mV.mInt32Value.mFormat =
                            token == Token_integer ? FORMAT_DECIMAL : FORMAT_HEX;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                int v;
                unsigned char format = FORMAT_DECIMAL;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_INTEGER32:
                        v = pSrcDesc->mV.mInt32Value.mValue;
                        format = pSrcDesc->mV.mInt32Value.mFormat;
                        break;
                    case TYPE_INTEGER64:
                        v = (int)pSrcDesc->mV.mInt64Value.mValue;
                        format = pSrcDesc->mV.mInt64Value.mFormat;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mType = TYPE_BYTE;
                if (opKind == OPKIND_NONE) {
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                pDesc->mV.mInt32Value.mFormat = format;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_Integer16(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;
    BOOL doNot = FALSE;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
        doNot = TRUE;
    }

    switch (token = GetToken(s_pFile)) {
        case Token_integer:
        case Token_hinteger:
            {
                int v = token == Token_integer ?
                        atoi(g_szCurrentToken) : Hexstr2Int(g_szCurrentToken);

                if (opKind == OPKIND_NONE) {
                    pDesc->mType = TYPE_INTEGER16;
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                    pDesc->mV.mInt32Value.mFormat =
                            token == Token_integer ? FORMAT_DECIMAL : FORMAT_HEX;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                int v;
                unsigned char format = FORMAT_DECIMAL;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_INTEGER16:
                    case TYPE_INTEGER32:
                        v = pSrcDesc->mV.mInt32Value.mValue;
                        format = pSrcDesc->mV.mInt32Value.mFormat;
                        break;
                    case TYPE_INTEGER64:
                        v = (int)pSrcDesc->mV.mInt64Value.mValue;
                        format = pSrcDesc->mV.mInt64Value.mFormat;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mType = TYPE_INTEGER16;
                if (opKind == OPKIND_NONE) {
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                pDesc->mV.mInt32Value.mFormat = format;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_Integer32(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;
    BOOL doNot = FALSE;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
        doNot = TRUE;
    }

    switch (token = GetToken(s_pFile)) {
        case Token_integer:
        case Token_hinteger:
            {
                int v = token == Token_integer ?
                        atoi(g_szCurrentToken) : Hexstr2Int(g_szCurrentToken);

                if (opKind == OPKIND_NONE) {
                    pDesc->mType = TYPE_INTEGER32;
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                    pDesc->mV.mInt32Value.mFormat =
                            token == Token_integer ? FORMAT_DECIMAL : FORMAT_HEX;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                int v;
                unsigned char format = FORMAT_DECIMAL;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_INTEGER32:
                        v = pSrcDesc->mV.mInt32Value.mValue;
                        format = pSrcDesc->mV.mInt32Value.mFormat;
                        break;
                    case TYPE_INTEGER64:
                        v = (int)pSrcDesc->mV.mInt64Value.mValue;
                        format = pSrcDesc->mV.mInt64Value.mFormat;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mType = TYPE_INTEGER32;
                if (opKind == OPKIND_NONE) {
                    pDesc->mV.mInt32Value.mValue = doNot ? ~v : v;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt32Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt32Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt32Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt32Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt32Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt32Value.mValue += doNot ? ~v : v;
                }
                pDesc->mV.mInt32Value.mFormat = format;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_Integer64(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc, int opKind)
{
    CARToken token;
    BOOL doNot = FALSE;

    if (PeekToken(s_pFile) == Token_S_not) {
        GetToken(s_pFile);
        doNot = TRUE;
    }

    switch (token = GetToken(s_pFile)) {
        case Token_integer:
        case Token_hinteger:
            {
                long long int v = token == Token_integer ?
                        atoll(g_szCurrentToken) : Hexstr2Int(g_szCurrentToken);

                if (opKind == OPKIND_NONE) {
                    pDesc->mType = TYPE_INTEGER64;
                    pDesc->mV.mInt64Value.mValue = doNot ? ~v : v;
                    pDesc->mV.mInt64Value.mFormat =
                            token == Token_integer ? FORMAT_DECIMAL : FORMAT_HEX;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt64Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt64Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt64Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt64Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt64Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt64Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt64Value.mValue += doNot ? ~v : v;
                }
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                long long int v;
                unsigned char format = FORMAT_DECIMAL;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_INTEGER32:
                        v = pSrcDesc->mV.mInt32Value.mValue;
                        format = pSrcDesc->mV.mInt32Value.mFormat;
                        break;
                    case TYPE_INTEGER64:
                        v = pSrcDesc->mV.mInt64Value.mValue;
                        format = pSrcDesc->mV.mInt64Value.mFormat;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mType = TYPE_INTEGER64;
                if (opKind == OPKIND_NONE) {
                    pDesc->mV.mInt64Value.mValue = doNot ? ~v : v;
                }
                else if (opKind == OPKIND_AND) {
                    pDesc->mV.mInt64Value.mValue &= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_EOR) {
                    pDesc->mV.mInt64Value.mValue ^= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_IOR) {
                    pDesc->mV.mInt64Value.mValue |= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_LSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt64Value.mValue <<= v;
                }
                else if (opKind == OPKIND_RSHIFT) {
                    assert(doNot == FALSE);
                    pDesc->mV.mInt64Value.mValue >>= v;
                }
                else if (opKind == OPKIND_MULTIP) {
                    pDesc->mV.mInt64Value.mValue *= doNot ? ~v : v;
                }
                else if (opKind == OPKIND_ADD) {
                    pDesc->mV.mInt64Value.mValue += doNot ? ~v : v;
                }
                pDesc->mV.mInt64Value.mFormat = format;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_Double(InterfaceDescriptor* pInterface, InterfaceConstDescriptor *pDesc)
{
    CARToken token;

    switch (token = GetToken(s_pFile)) {
        case Token_decimal:
        case Token_integer:
        case Token_hinteger:
            {
                double v = atof(g_szCurrentToken);
                pDesc->mV.mDoubleValue = v;
                break;
            }

        case Token_ident:
            {
                int n = SelectInterfaceConstDirEntry(g_szCurrentToken, pInterface);
                if (n < 0) {
                    ErrorReport(CAR_E_NotFound);
                    return Ret_AbortOnError;
                }
                double v;
                InterfaceConstDescriptor *pSrcDesc = pInterface->mConsts[n];
                switch (pSrcDesc->mType) {
                    case TYPE_INTEGER32:
                        v = pSrcDesc->mV.mInt32Value.mValue;
                        break;
                    case TYPE_INTEGER64:
                        v = pSrcDesc->mV.mInt64Value.mValue;
                        break;
                    default:
                        ErrorReport(CAR_E_IllegalValue);
                        return Ret_AbortOnError;
                }
                pDesc->mV.mDoubleValue = v;
                break;
            }
        default:
            ErrorReport(CAR_E_IllegalValue);
        case Token_Error:
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_InterfaceConstChar32(InterfaceDescriptor *pDesc)
{
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Char32(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int P_InterfaceConstBoolean(InterfaceDescriptor *pDesc)
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Boolean(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    while (token = PeekToken(s_pFile), token != Token_S_semicolon) {
        if (token == Token_S_and) {
            GetToken(s_pFile);
            if (P_Boolean(pDesc, pDesc->mConsts[r], OPKIND_AND) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_eor) {
            GetToken(s_pFile);
            if (P_Boolean(pDesc, pDesc->mConsts[r], OPKIND_EOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_ior) {
            GetToken(s_pFile);
            if (P_Boolean(pDesc, pDesc->mConsts[r], OPKIND_IOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else {
            ErrorReport(CAR_E_ExpectSymbol, ";");
            return Ret_AbortOnError;
        }

    }
    return Ret_Continue;
}

int P_InterfaceConstByte(InterfaceDescriptor *pDesc)
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    while (token = PeekToken(s_pFile), token != Token_S_semicolon) {
        if (token == Token_S_and) {
            GetToken(s_pFile);
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_AND) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_eor) {
            GetToken(s_pFile);
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_EOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_ior) {
            GetToken(s_pFile);
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_IOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_langle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_langle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_LSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_rangle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_rangle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_RSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_star) {
            GetToken(s_pFile);
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_MULTIP) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_add) {
            GetToken(s_pFile);
            if (P_Byte(pDesc, pDesc->mConsts[r], OPKIND_ADD) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else {
            ErrorReport(CAR_E_ExpectSymbol, ";");
            return Ret_AbortOnError;
        }

    }
    return Ret_Continue;
}

int P_InterfaceConstInt16(InterfaceDescriptor *pDesc)
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    while (token = PeekToken(s_pFile), token != Token_S_semicolon) {
        if (token == Token_S_and) {
            GetToken(s_pFile);
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_AND) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_eor) {
            GetToken(s_pFile);
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_EOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_ior) {
            GetToken(s_pFile);
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_IOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_langle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_langle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_LSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_rangle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_rangle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_RSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_star) {
            GetToken(s_pFile);
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_MULTIP) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_add) {
            GetToken(s_pFile);
            if (P_Integer16(pDesc, pDesc->mConsts[r], OPKIND_ADD) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else {
            ErrorReport(CAR_E_ExpectSymbol, ";");
            return Ret_AbortOnError;
        }

    }
    return Ret_Continue;
}

int P_InterfaceConstInt32(InterfaceDescriptor *pDesc)
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    while (token = PeekToken(s_pFile), token != Token_S_semicolon) {
        if (token == Token_S_and) {
            GetToken(s_pFile);
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_AND) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_eor) {
            GetToken(s_pFile);
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_EOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_ior) {
            GetToken(s_pFile);
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_IOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_langle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_langle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_LSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_rangle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_rangle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_RSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_star) {
            GetToken(s_pFile);
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_MULTIP) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_add) {
            GetToken(s_pFile);
            if (P_Integer32(pDesc, pDesc->mConsts[r], OPKIND_ADD) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else {
            ErrorReport(CAR_E_ExpectSymbol, ";");
            return Ret_AbortOnError;
        }

    }
    return Ret_Continue;
}

int P_InterfaceConstInt64(InterfaceDescriptor *pDesc)
{
    CARToken token;
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_NONE) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    while (token = PeekToken(s_pFile), token != Token_S_semicolon) {
        if (token == Token_S_and) {
            GetToken(s_pFile);
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_AND) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_eor) {
            GetToken(s_pFile);
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_EOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_ior) {
            GetToken(s_pFile);
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_IOR) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_star) {
            GetToken(s_pFile);
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_MULTIP) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_add) {
            GetToken(s_pFile);
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_ADD) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_langle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_langle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_LSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_S_rangle) {
            GetToken(s_pFile);
            if (GetToken(s_pFile) != Token_S_rangle) {
                ErrorReport(CAR_E_ExpectSymbol, ";");
                return Ret_AbortOnError;
            }
            if (P_Integer64(pDesc, pDesc->mConsts[r], OPKIND_RSHIFT) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else {
            ErrorReport(CAR_E_ExpectSymbol, ";");
            return Ret_AbortOnError;
        }

    }
    return Ret_Continue;
}

int P_InterfaceConstFloat(InterfaceDescriptor *pDesc)
{
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Double(pDesc, pDesc->mConsts[r]) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }
    pDesc->mConsts[r]->mType = TYPE_FLOAT;

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int P_InterfaceConstDouble(InterfaceDescriptor *pDesc)
{
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    if (P_Double(pDesc, pDesc->mConsts[r]) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }
    pDesc->mConsts[r]->mType = TYPE_DOUBLE;

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int P_InterfaceConstString(InterfaceDescriptor *pDesc)
{
    int r;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectConstName);
        return Ret_AbortOnError;
    }

    r = CreateInterfaceConstDirEntry(g_szCurrentToken, pDesc);
    if (r < 0) {
        if (r == CLSError_DupEntry) {
            ErrorReport(CAR_E_DupEntry, "interface const", g_szCurrentToken);
            return Ret_AbortOnError;
        }
        else {
            CreateError(r, "interface const", g_szCurrentToken);
            // continue on error
        }
    }

    if (GetToken(s_pFile) != Token_S_assign) {
        ErrorReport(CAR_E_ExpectSymbol, "=");
        return Ret_AbortOnError;
    }

    CARToken token =  GetToken(s_pFile);
    if (token != Token_string && token != Token_K_null) {
        ErrorReport(CAR_E_UnexpectSymbol);
        return Ret_AbortOnError;
    }

    pDesc->mConsts[r]->mType = TYPE_STRING;
    if (token != Token_K_null) {
        char *str = (char*)malloc(strlen(g_szCurrentToken) + 1);
        strcpy(str, g_szCurrentToken);
        pDesc->mConsts[r]->mV.mStrValue = str;
    }
    else {
        pDesc->mConsts[r]->mV.mStrValue = NULL;
    }

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int P_InterfaceConst(InterfaceDirEntry *pItfDirEntry)
{
    InterfaceDescriptor *pDesc = pItfDirEntry->mDesc;

    if (GetToken(s_pFile) != Token_K_const) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    switch (GetToken(s_pFile)) {
        case Token_K_Boolean:
            return P_InterfaceConstBoolean(pDesc);

        case Token_K_Char32:
            return P_InterfaceConstChar32(pDesc);

        case Token_K_Byte:
            return P_InterfaceConstByte(pDesc);

        case Token_K_Int16:
            return P_InterfaceConstInt16(pDesc);

        case Token_K_Int32:
            return P_InterfaceConstInt32(pDesc);

        case Token_K_Int64:
            return P_InterfaceConstInt64(pDesc);

        case Token_K_Float:
            return P_InterfaceConstFloat(pDesc);

        case Token_K_Double:
            return P_InterfaceConstDouble(pDesc);

        case Token_K_String:
            return P_InterfaceConstString(pDesc);

        case Token_Error:
        default:
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
    }
}

int TypeSignature(TypeDescriptor* pType, StringBuilder& sb)
{
    CARDataType type = pType->mType;
    int pointer = pType->mPointer;
    switch(type) {
        case Type_Char16:
            sb.Append("C16");
            break;
        case Type_Char32:
            sb.Append("C32");
            break;
        case Type_Boolean:
            sb.Append("Z");
            break;
        case Type_Byte:
            sb.Append("B");
            break;
        case Type_Int8:
            sb.Append("I8");
            break;
        case Type_Int16:
            sb.Append("I16");
            break;
        case Type_Int32:
            sb.Append("I32");
            break;
        case Type_Int64:
            sb.Append("I64");
            break;
        case Type_UInt16:
            sb.Append("U16");
            break;
        case Type_UInt32:
            sb.Append("U32");
            break;
        case Type_UInt64:
            sb.Append("U64");
            break;
        case Type_Float:
            sb.Append("F");
            break;
        case Type_Double:
            sb.Append("D");
            break;
        case Type_ECode:
            sb.Append("E");
            break;
        case Type_EMuid:
            sb.Append("IID");
            break;
        case Type_EGuid:
            sb.Append("CID");
            break;
        case Type_String:
            sb.Append("LElastos/String;");
            break;
        case Type_enum: {
            sb.Append("L");
            if (s_pModule->mEnumDirs[pType->mIndex]->mNameSpace != NULL) {
                char* ns = (char*)malloc(strlen(s_pModule->mEnumDirs[pType->mIndex]->mNameSpace) + 1);
                strcpy(ns, s_pModule->mEnumDirs[pType->mIndex]->mNameSpace);

                char* c;
                while ((c = strchr(ns, '.')) != NULL) {
                    *c = '/';
                }

                sb.Append(ns);
                sb.Append("/");
                free(ns);
            }
            sb.Append(s_pModule->mEnumDirs[pType->mIndex]->mName);
            sb.Append(";");
            break;
        }
        case Type_ArrayOf: {
            sb.Append("[");
            if (TypeSignature(pType->mNestedType, sb) == Ret_AbortOnError) return Ret_AbortOnError;
            break;
        }
        case Type_interface: {
            sb.Append("L");
            if (s_pModule->mInterfaceDirs[pType->mIndex]->mNameSpace != NULL) {
                char* ns = (char*)malloc(strlen(s_pModule->mInterfaceDirs[pType->mIndex]->mNameSpace) + 1);
                strcpy(ns, s_pModule->mInterfaceDirs[pType->mIndex]->mNameSpace);

                char* c;
                while ((c = strchr(ns, '.')) != NULL) {
                    *c = '/';
                }

                sb.Append(ns);
                sb.Append("/");
                free(ns);
            }
            sb.Append(s_pModule->mInterfaceDirs[pType->mIndex]->mName);
            sb.Append(";");
            break;
        }
        case Type_alias:
            if (TypeSignature(&s_pModule->mAliasDirs[pType->mIndex]->mType, sb) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
            break;
        default:
            assert(0);
            return Ret_AbortOnError;
    }
    switch (pointer) {
        case 0:
            break;
        case 1:
            sb.Append("*");
            break;
        case 2:
            sb.Append("**");
            break;
        default:
            assert(0);
            return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int BuildArgumentSignature(ParamDescriptor* pParam, StringBuilder& sb)
{
    return TypeSignature(&pParam->mType, sb);
}

// ARG_ATTRIBS -> s_lbracket ARG_ATTRIB { s_comma ARG_ATTRIB } s_rbracket
// ARG_ATTRIB  -> k_in | k_out | k_retval
//
int P_ArgAttribs(DWORD *pAttribs)
{
    CARToken token;

    //eat "["
    GetToken(s_pFile);

    do {
        switch (GetToken(s_pFile)) {
            case Token_K_in:
                *pAttribs |= ParamAttrib_in;
                break;
            case Token_K_out:
                *pAttribs |= ParamAttrib_out;
                break;
            case Token_K_callee:
                if (!(*pAttribs & ParamAttrib_out)) {
                    ErrorReport(CAR_E_CalleeDecl);
                    return Ret_AbortOnError;
                }

                *pAttribs |= ParamAttrib_callee;
                break;
            default:
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            case Token_Error:
                return Ret_AbortOnError;
        }
        token = GetToken(s_pFile);
    } while (Token_S_comma == token);

    if (token != Token_S_rbracket) {
        ErrorReport(CAR_E_ExpectSymbol, "]");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// METHOD_ARG  -> [ ARG_ATTRIBS ] TYPE ident
//
int P_MethodArg(MethodDescriptor *pMethod, BOOL isLocal, BOOL isDeprecated)
{
    int n;
    DWORD attribs;
    TypeDescriptor type;

    attribs = 0;
    memset(&type, 0, sizeof(type));

    if (PeekToken(s_pFile) != Token_S_lbracket) {
        ErrorReport(CAR_E_ExpectParamAttrib);
    }

    if (P_ArgAttribs(&attribs) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    if (P_Type(&type) == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectParamName);
        return Ret_AbortOnError;
    }
    n = CreateMethodParam(g_szCurrentToken, pMethod);
    if (n < 0) {
        CreateError(n, "method parameter", g_szCurrentToken);
        return Ret_Continue;
    }

    //attributes conflict checking
    if (!(attribs & (ParamAttrib_in | ParamAttrib_out))) {
        ErrorReport(CAR_E_ParamNoAttrib, g_szCurrentToken, pMethod->mName);
        return Ret_AbortOnError;
    }

    if ((attribs & ParamAttrib_in) &&
            ((attribs & ParamAttrib_out) || (attribs & ParamAttrib_callee))) {
        ErrorReport(CAR_E_IllegalParamAttrib, g_szCurrentToken, pMethod->mName);
        return Ret_AbortOnError;
    }

    if ((attribs & ParamAttrib_callee) && !isLocal &&
        (type.mType != Type_ArrayOf)) {
        ErrorReport(CAR_E_CalleeParam, g_szCurrentToken, pMethod->mName);
        return Ret_AbortOnError;
    }

    if (!IsValidParamTypeAttrib(s_pModule, &type, attribs, isLocal, isDeprecated)) {
        ErrorReport(CAR_E_IllegalParamType, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    pMethod->mParams[n]->mAttribs = attribs;
    memcpy(&pMethod->mParams[n]->mType, &type, sizeof(type));

    return Ret_Continue;
}

int BuildMethodSignature(MethodDescriptor *pMethod)
{
    StringBuilder sb;
    sb.Append("(");
    for (int i = 0; i < pMethod->mParamCount; i++) {
        BuildArgumentSignature(pMethod->mParams[i], sb);
    }
    sb.Append(")");
    sb.Append("E");
    char* signature = sb.ToString();
    pMethod->mSignature = new char[strlen(signature) + 1];
    if (!pMethod->mSignature) return Ret_AbortOnError;
    strcpy(pMethod->mSignature, signature);
    return Ret_Continue;
}

BOOL IsMethodDuplicated(InterfaceDescriptor* pIntfDesc, MethodDescriptor* pMethod)
{
    int n;

    if (0 != pIntfDesc->mParentIndex) {
        if (IsMethodDuplicated(s_pModule->mInterfaceDirs[pIntfDesc->mParentIndex]->mDesc, pMethod)) {
            return true;
        }
    }

    for (n = 0; n < pIntfDesc->mMethodCount; n++) {
        if (!strcmp(pIntfDesc->mMethods[n]->mName, pMethod->mName)
            && !strcmp(pIntfDesc->mMethods[n]->mSignature, pMethod->mSignature)) {
            ErrorReport(CAR_E_DupMethodName, pIntfDesc->mMethods[n]->mName);
            return true;
        }
    }

    return false;
}

// METHOD      -> [ oneway ] ident METHOD_ARGS s_semicolon
// METHOD_ARGS -> s_lparen [ METHOD_ARG { s_comma METHOD_ARG } ] s_rparen
//
int P_Method(InterfaceDirEntry *pItfDirEntry, BOOL isDeprecated)
{
    int n;
    CARToken token;
    DWORD attribs = 0;
    int annotationCount = 0;
    AnnotationDescriptor *pAnnotationDescs[MAX_ANNOTATION_NUMBER];
    InterfaceDescriptor *pDesc = pItfDirEntry->mDesc;

    token = GetToken(s_pFile);
    if (token == Token_S_lbracket) {
        token = GetToken(s_pFile);
        while (token != Token_S_rbracket) {
            if (token == Token_K_oneway) {
                attribs |= MethodAttrib_Oneway;
                token = GetToken(s_pFile);
            }
            else if (token == Token_S_at) {
                if (annotationCount >= MAX_ANNOTATION_NUMBER) {
                    ErrorReport(CAR_E_TooManyAnnotation);
                    return Ret_AbortOnError;
                }
                if (P_Annotation(pAnnotationDescs, annotationCount) == Ret_AbortOnError) {
                    ErrorReport(CAR_E_IllegalMethodAnnotation, g_szCurrentToken);
                    return Ret_AbortOnError;
                }
                annotationCount++;
                token = GetToken(s_pFile);
            }
            else {
                ErrorReport(CAR_E_IllegalMethodProperties, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            if (token == Token_S_comma) token = GetToken(s_pFile);
        }
        token = GetToken(s_pFile);
    }
    if (token != Token_ident) {
        ErrorReport(CAR_E_ExpectMethodName);
        return Ret_AbortOnError;
    }
    n = CreateInterfaceMethod(g_szCurrentToken, pDesc);
    if (n < 0) {
        CreateError(n, "interface method", g_szCurrentToken);
        return Ret_AbortOnError;
    }
    pDesc->mMethods[n]->mType.mType = Type_ECode;
    if (pDesc->mAttribs & InterfaceAttrib_oneway) attribs |= MethodAttrib_Oneway;
    pDesc->mMethods[n]->mAnnotationCount = annotationCount;
    pDesc->mMethods[n]->mAnnotations = (AnnotationDescriptor**)malloc(sizeof(AnnotationDescriptor*) * annotationCount);
    for (int i = 0; i < annotationCount; i++) {
        pDesc->mMethods[n]->mAnnotations[i] = pAnnotationDescs[i];
    }
    pDesc->mMethods[n]->mAttribs = attribs;

    if (GetToken(s_pFile) != Token_S_lparen) {
        ErrorReport(CAR_E_ExpectSymbol, "(");
        return Ret_AbortOnError;
    }
    if (PeekToken(s_pFile) != Token_S_rparen) {
        BOOL isLocal = (pDesc->mAttribs & InterfaceAttrib_local);
        do {
            if (P_MethodArg(pDesc->mMethods[n], isLocal, isDeprecated) == \
                Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
            token = GetToken(s_pFile);
        } while (Token_S_comma == token);

        if (Token_S_rparen != token) {
            ErrorReport(CAR_E_ExpectSymbol, ")");
            return Ret_AbortOnError;
        }
    }
    else {
        GetToken(s_pFile); // skip ")"
    }

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    BuildMethodSignature(pDesc->mMethods[n]);

    pDesc->mMethodCount -= 1;
    if (IsMethodDuplicated(pDesc, pDesc->mMethods[n])) {
        return Ret_AbortOnError;
    }
    pDesc->mMethodCount += 1;
    return Ret_Continue;
}

void AddInterfaceParent(const char *pszName, InterfaceDescriptor *pDesc)
{
    int n;
    InterfaceDescriptor *pParent;

    char *pszNamespaces = NULL;
    const char *dot = strrchr(pszName, '.');
    if (dot != NULL) {
        pszNamespaces = (char *)malloc(dot - pszName + 1);
        memset(pszNamespaces, 0, dot - pszName + 1);
        memcpy(pszNamespaces, pszName, dot - pszName);
        pszName = dot + 1;
    }
    else {
        int nsLen = strlen(GetNamespace());
        int usingLen = strlen(s_pszUsingNS);
        if (nsLen + usingLen != 0) {
            int len = nsLen + 1 + usingLen + 1;
            pszNamespaces = (char *)malloc(len);
            memset(pszNamespaces, 0, len);
            if (nsLen != 0) strcpy(pszNamespaces, GetNamespace());
            if (usingLen != 0) {
                if (pszNamespaces[0] != '\0') strcat(pszNamespaces, ";");
                strcat(pszNamespaces, s_pszUsingNS);
            }
        }
    }
    n = RetrieveInterface(pszName, pszNamespaces, s_pModule, FALSE);
    if (pszNamespaces != NULL) free(pszNamespaces);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "interface", pszName);
        return;
    }
    pParent = s_pModule->mInterfaceDirs[n]->mDesc;

    if (pParent == pDesc) {
        ErrorReport(CAR_E_NestedInherit, "interface", pszName);
        return;
    }
    if (!(pParent->mAttribs & InterfaceAttrib_defined)) {
        ErrorReport(CAR_E_NotFound, "interface",
                s_pModule->mInterfaceDirs[n]->mName);
        return;
    }
    pDesc->mParentIndex = n;

    if (!(pDesc->mAttribs & InterfaceAttrib_local)) {
        if (pParent->mAttribs & InterfaceAttrib_local) {
            ErrorReport(CAR_W_LocalParent, pszName);
            fputs("Treated as local automatically.\n", stderr);
            pDesc->mAttribs |= InterfaceAttrib_local;
        }
    }
}

// ITF_BODY    -> [ k_extends ident ]
//                s_lbrace { METHOD { s_semicolon } } s_rbrace
//
int P_InterfaceBody(InterfaceDirEntry *pDirEntry, BOOL isDeprecated)
{
    CARToken token;
    InterfaceDescriptor *pDesc = pDirEntry->mDesc;

    token = GetToken(s_pFile);
    if (Token_K_extends == token) {
        if (GetToken(s_pFile) != Token_ident) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        AddInterfaceParent(g_szCurrentToken, pDesc);

        token = GetToken(s_pFile);
    }

    if (Token_S_lbrace != token) {
        ErrorReport(CAR_E_ExpectSymbol, "{ or ;");
        return Ret_AbortOnError;
    }
    while (token = PeekToken(s_pFile), token != Token_S_rbrace) {
        if (token == Token_K_const) {
            if (P_InterfaceConst(pDirEntry) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (P_Method(pDirEntry, isDeprecated) == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
        while (PeekToken(s_pFile) == Token_S_semicolon) GetToken(s_pFile);
    }
    GetToken(s_pFile); // skip "}"

    pDesc->mAttribs |= InterfaceAttrib_defined;

    if (0 == pDesc->mMethodCount) ErrorReport(CAR_W_NoMethods);
    return Ret_Continue;
}

// INTERFACE   -> [ ITF_PROPERTIES ] ITF_ID [ ITF_BODY | s_semicolon ]
// ITF_ID      -> interface ident
//
int P_Interface(CARToken token, DWORD properties, int annotationCount, AnnotationDescriptor **pAnnotationDescs)
{
    int r, ret;
    DWORD attribs = 0;
    InterfaceDescriptor *pDesc;
    BOOL isDeprecated = FALSE;

    //set properties
    if (properties & Prop_deprecated) isDeprecated = TRUE;
    if (properties & Prop_local) attribs |= InterfaceAttrib_local;
    if (properties & Prop_oneway) attribs |= InterfaceAttrib_oneway;
    if (properties & Prop_parcelable) attribs |= InterfaceAttrib_parcelable;
    if ((properties & Prop_private)) {
        //error
        ErrorReport(CAR_E_IllegalInterfaceProperties);
    }

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectInterfaceName);
        return Ret_AbortOnError;
    }

    if (Token_K_interface == token) {
        const char *pszName = g_szCurrentToken;
        const char *dot = strrchr(pszName, '.');
        if (dot != NULL) {
            pszName = dot + 1;
        }
        if (*pszName != 'I') {
            ErrorReport(CAR_E_IllegalInterfaceName, pszName);
            return Ret_AbortOnError;
        }
        attribs |= InterfaceAttrib_t_normal;
    }
    else {
        const char *pszName = g_szCurrentToken;
        const char *dot = strrchr(pszName, '.');
        if (dot != NULL) {
            pszName = dot + 1;
        }
        if (*pszName != 'J') {
            ErrorReport(CAR_E_IllegalCallbacksOrDelegatesName, pszName);
            return Ret_AbortOnError;
        }
        if (Token_K_callbacks == token) attribs |= InterfaceAttrib_t_callbacks;
        if (Token_K_delegates == token) attribs |= InterfaceAttrib_t_delegates;
    }

    if (((attribs & InterfaceAttrib_t_callbacks) || (attribs & InterfaceAttrib_t_delegates))
            && (attribs & InterfaceAttrib_local)) {
        ErrorReport(CAR_E_LocalConflict);
    }

    int len = strlen(GetNamespace()) + strlen(g_szCurrentToken);
    char *pszFullName = (char*)malloc(len + 2);
    memset(pszFullName, 0, len + 2);
    if (GetNamespace()[0] != 0) {
        strcpy(pszFullName, GetNamespace());
        strcat(pszFullName, ".");
    }
    strcat(pszFullName, g_szCurrentToken);
    r = CreateInterfaceDirEntry(pszFullName, s_pModule, attribs);

    if (Token_S_semicolon == PeekToken(s_pFile)) {
        AddFrontDeclaration(pszFullName, Type_interface);
        free(pszFullName);
        GetToken(s_pFile); // ignore ";"
        if (INTERFACE_ATTR(attribs) != 0) {
            ErrorReport(CAR_E_DeclaredInterfaceProp);
            return Ret_ContinueOnError;
        }
        if (r < 0) {
            if (r != CLSError_DupEntry)
                CreateError(r, "interface", g_szCurrentToken);
        }
        else {
            s_pModule->mInterfaceDirs[r]->mDesc->mAttribs |= attribs;
        }
        return Ret_Continue;
    }
    free(pszFullName);
    if (r < 0) {
        CreateError(r, "interface", g_szCurrentToken);
        return Ret_AbortOnError;
    }
    s_pModule->mInterfaceDirs[r]->mFileIndex = CreateFileDirEntry(GetSubmodule(), s_pModule);
    pDesc = s_pModule->mInterfaceDirs[r]->mDesc;
    pDesc->mAttribs = attribs;
    s_pModule->mDefinedInterfaceIndexes[s_pModule->mDefinedInterfaceCount++] = r;
    if (annotationCount > 0) {
        pDesc->mAnnotationCount = annotationCount;
        pDesc->mAnnotations = (AnnotationDescriptor**)malloc(sizeof(AnnotationDescriptor*) * annotationCount);
        for (int i = 0; i < annotationCount; i++) {
            pDesc->mAnnotations[i] = pAnnotationDescs[i];
        }
    }

    ret = P_InterfaceBody(s_pModule->mInterfaceDirs[r], isDeprecated);
    if (ret != Ret_Continue) return ret;

    if ((attribs & InterfaceAttrib_t_callbacks)
            || (attribs & InterfaceAttrib_t_delegates)) {
        GenerateHandlerInterface(
            s_pModule->mInterfaceDirs[r]->mName,
            s_pModule,
            s_pModule->mInterfaceDirs[r]);
    }

    return Ret_Continue;
}

/*
void GenerateDispatchInterface(ClassDescriptor *pDesc)
{
    int n, nDispIndex;
    const char *pszDispName;
    InterfaceDescriptor *pDisp, *pIntf;
    ClassInterface *pClsIntf;

    n = RetrieveInterface("IDispatch", s_pModule, FALSE);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "interface", "IDispatch");
        return;
    }

    pszDispName = GenerateInternalName("Dispatch");
    nDispIndex = CreateInterfaceDirEntry(pszDispName, s_pModule, 0);
    if (nDispIndex < 0) {
        CreateError(nDispIndex, "interface", pszDispName);
        return;
    }

    pDisp = s_pModule->mInterfaceDirs[nDispIndex]->mDesc;
    pDisp->mParentIndex = n;
    pDisp->mAttribs = InterfaceAttrib_dual;

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        pIntf = s_pModule-> \
            mInterfaceDirs[pDesc->mInterfaces[n]->mIndex]->mDesc;
        if (!(pDesc->mInterfaces[n]->\
            mAttribs & ClassInterfaceAttrib_callback)) {
            if (InterfaceMethodsAppend(s_pModule, pIntf, pDisp) < 0) {
                ErrorReport(CAR_E_GenDisp);
                return;
            }
        }
    }

    n = CreateClassInterface(nDispIndex, pDesc);
    if (n < 0) {
        CreateError(n, "class dispatch interface", pszDispName);
        return;
    }
    pDesc->mInterfaces[n]->mAttribs |= ClassInterfaceAttrib_autoadded;

    // Interchange the dispatch interface to first position.
    //
    if (0 != n) {
        pClsIntf = pDesc->mInterfaces[0];
        pDesc->mInterfaces[0] = pDesc->mInterfaces[n];
        pDesc->mInterfaces[n] = pClsIntf;
    }
}
*/

typedef struct TempClassAttribs {
    DWORD       mAttribs;
    USHORT      mAggregateCount;
    USHORT      mAspectCount;
    USHORT      mClassCount;
    USHORT      aggrIndexs[MAX_CLASS_ASPECT_NUMBER];
    USHORT      aspectIndexs[MAX_CLASS_ASPECT_NUMBER];
    USHORT      classIndexs[MAX_AGGRCLASSE_OF_ASPECT_NUMBER];
}   TempClassAttribs;

void CheckInterfaceDupMethodNameEx(InterfaceDirEntry *pientry)
{
    if (!strcmp(pientry->mName, "IInterface")) return;

    int n, m;
    InterfaceDescriptor *pDesc = pientry->mDesc;

    //Check dup method name with the interface "IInterface"
    for (n = 0; n < pDesc->mMethodCount; n++) {
        if (!strcmp(pDesc->mMethods[n]->mName, "Probe")
            || !strcmp(pDesc->mMethods[n]->mName, "AddRef")
            || !strcmp(pDesc->mMethods[n]->mName, "Release")
            || !strcmp(pDesc->mMethods[n]->mName, "GetInterfaceID")) {
            ErrorReport(CAR_E_DupMethodName, pDesc->mMethods[n]->mName);
        }
    }

    //Check dup method name with the parent interfaces without "IInterface"
    if (0 != pDesc->mParentIndex) {
        int nParentIndex = pDesc->mParentIndex;
        do {
            for (n = 0; n < pDesc->mMethodCount; n++) {
                for (m = 0; m < s_pModule->mInterfaceDirs[nParentIndex]->mDesc->mMethodCount; m++) {
                    if (!strcmp(pDesc->mMethods[n]->mName,
                        s_pModule->mInterfaceDirs[nParentIndex]->mDesc->mMethods[m]->mName)
                        && !strcmp(pDesc->mMethods[n]->mSignature,
                            s_pModule->mInterfaceDirs[nParentIndex]->mDesc->mMethods[m]->mSignature)) {
                        ErrorReport(CAR_E_DupMethodName,
                                    pDesc->mMethods[n]->mName);
                    }
                }
            }
            nParentIndex = s_pModule->mInterfaceDirs[nParentIndex]->mDesc->mParentIndex;
        } while (nParentIndex != 0);
    }
}

void CheckInterfaceDupMethodName(
    InterfaceDescriptor *pSrc, InterfaceDescriptor *pDest)
{
    int n, m;

    //same parent
    if (pSrc == pDest)
        return;

    if (0 != pSrc->mParentIndex) {
        CheckInterfaceDupMethodName(
            s_pModule->mInterfaceDirs[pSrc->mParentIndex]->mDesc, pDest);
    }
    if (0 != pDest->mParentIndex) {
        CheckInterfaceDupMethodName(
            pSrc, s_pModule->mInterfaceDirs[pDest->mParentIndex]->mDesc);
    }

    for (n = 0; n < pSrc->mMethodCount; n++)
        for (m = 0; m < pDest->mMethodCount; m++) {
            if (!strcmp(pSrc->mMethods[n]->mName,
                pDest->mMethods[m]->mName)) {
                ErrorReport(CAR_E_DupMethodName, pSrc->mMethods[n]->mName);
            }
        }
}

void CheckClassDupCtorSignature(ClassDescriptor *pClsDesc)
{
    int n, m;
    InterfaceDescriptor *pIntfDesc =
            s_pModule->mInterfaceDirs[pClsDesc->mCtorIndex]->mDesc;

    for (n = 0; n < pIntfDesc->mMethodCount; n++) {
        for (m = n + 1; m < pIntfDesc->mMethodCount; m++) {
            if (!strcmp(pIntfDesc->mMethods[n]->mName,
                pIntfDesc->mMethods[m]->mName)) {
                ErrorReport(CAR_E_DupMethodName, pIntfDesc->mMethods[n]->mName);
            }
        }
    }
}

void CheckClassDupMethodName(ClassDescriptor *pDesc)
{
    int n, m;

    for (n = 0; n < pDesc->mInterfaceCount; n++)
        for (m = n + 1; m < pDesc->mInterfaceCount; m++)
            CheckInterfaceDupMethodName(
                s_pModule->mInterfaceDirs \
                    [pDesc->mInterfaces[n]->mIndex]->mDesc,
                s_pModule->mInterfaceDirs \
                    [pDesc->mInterfaces[m]->mIndex]->mDesc);
}

void CheckClassAttribs(ClassDescriptor *pDesc)
{
    int j=0;
    char szAttribName[2][20];

    if (pDesc->mAttribs & ClassAttrib_t_aspect) {
        if (pDesc->mAttribs & ClassAttrib_singleton) {
            ErrorReport(CAR_E_AttribConflict, "singleton", "aspect type");
        }
    }

    if (j >= 2) {
        ErrorReport(CAR_E_AttribConflict, szAttribName[0], szAttribName[1]);
    }

    if (pDesc->mAttribs & ClassAttrib_aspect) {
        if (!(pDesc->mAttribs & ClassAttrib_t_regime)) {
            ErrorReport(CAR_E_AspectUse);
        }
    }

    if (!(pDesc->mAttribs & ClassAttrib_t_aspect)) {
        AddConstClassInterface("IObject", NULL, pDesc);
        AddConstClassInterface("ISynchronize", "Elastos.Core", pDesc);
        AddConstClassInterface("IWeakReferenceSource", NULL, pDesc);
    }
    else {
        AddConstClassInterface("IAspect", NULL, pDesc);
    }

    if (pDesc->mAttribs & ClassAttrib_t_regime) {
        AddConstClassInterface("IRegime", NULL, pDesc);
    }

    // if (s_bSupportWeakRef && !(pDesc->mAttribs & ClassAttrib_singleton)) {
    //     AddConstClassInterface("IWeakReferenceSource", NULL, pDesc);
    // }

    CheckClassDupCtorSignature(pDesc);
    // CheckClassDupMethodName(pDesc);
}

// CLASS     -> k_class s_lparen ident { s_comma ident } s_rparen
//
int GetClasses(USHORT* classIndexs)
{
    int n, classCount;
    CARToken token;

    classCount = 0;
    do {
        if (GetToken(s_pFile) != Token_ident) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        n = RetrieveClass(g_szCurrentToken, NULL, s_pModule, FALSE);
        if (n < 0) {
            ErrorReport(CAR_E_NotFound, "class", g_szCurrentToken);
            goto ErrorSkip;
        }

        if (s_pModule->mClassDirs[n]->mDesc->mAttribs & \
            (ClassAttrib_t_aspect | ClassAttrib_t_generic
             | ClassAttrib_t_sink | ClassAttrib_t_clsobj)) {
            ErrorReport(CAR_E_NotClass, g_szCurrentToken);
            goto ErrorSkip;
        }
        if (classCount >= MAX_AGGRCLASSE_OF_ASPECT_NUMBER) {
            ErrorReport(CAR_E_FullEntry, "classes");
            goto ErrorSkip;
        }

        classIndexs[classCount++] = n;

ErrorSkip:
        token = PeekToken(s_pFile);
        if (Token_S_comma == token) token = GetToken(s_pFile);

    } while (Token_S_comma == token);

    return classCount;
}

inline int P_ClassForAspect(TempClassAttribs *pAttr)
{
    int n;

    n = GetClasses(pAttr->classIndexs);
    if (n <= 0) return n;

    pAttr->mClassCount= n;
    return Ret_Continue;
}

// AGGREGATE  -> k_aggregate s_lparen ident { s_comma ident } s_rparen
// ASPECT     -> k_aspect s_lparen ident { s_comma ident } s_rparen
//
int GetAspects(USHORT* aspectIndexs)
{
    int n, aspectCount;
    CARToken token;

    aspectCount = 0;
    do {
        token = PeekToken(s_pFile);

        if (Token_ident != token) {
            if ((0 == aspectCount)
                    || ((Token_K_pertainsto != token)
                    && (Token_K_substitutes != token))){
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            else break;
        }
        else token = GetToken(s_pFile);

        n = RetrieveClass(g_szCurrentToken, NULL, s_pModule, FALSE);
        if (n < 0) {
            ErrorReport(CAR_E_NotFound, "aspect", g_szCurrentToken);
            goto ErrorSkip;
        }

        if (!(s_pModule->mClassDirs[n]->mDesc->mAttribs & \
            ClassAttrib_t_aspect)) {
            ErrorReport(CAR_E_NotAspect, g_szCurrentToken);
            goto ErrorSkip;
        }
        if (aspectCount >= MAX_CLASS_ASPECT_NUMBER) {
            ErrorReport(CAR_E_FullEntry, "aspects");
            goto ErrorSkip;
        }

        aspectIndexs[aspectCount++] = n;

ErrorSkip:
        token = PeekToken(s_pFile);
        if (Token_S_comma == token) token = GetToken(s_pFile);

    } while (Token_S_comma == token);

    return aspectCount;
}

inline int P_Aggregate(TempClassAttribs *pAttr)
{
    int n;

    n = GetAspects(pAttr->aggrIndexs);
    if (n <= 0) return n;

    pAttr->mAggregateCount = n;
    return Ret_Continue;
}

inline int P_Aspect(TempClassAttribs *pAttr)
{
    int n;

    n = GetAspects(pAttr->aspectIndexs);
    if (n <= 0) return n;

    pAttr->mAspectCount = n;
    return Ret_Continue;
}

BOOL IsInterfaceParent(InterfaceDescriptor *pDesc, int nIndex)
{
    while (0 != pDesc->mParentIndex) {
        if (nIndex == pDesc->mParentIndex) return TRUE;
        pDesc = s_pModule->mInterfaceDirs[pDesc->mParentIndex]->mDesc;
    }
    return FALSE;
}

BOOL IsClsIntfParent(ClassDescriptor *pDesc, int nIndex)
{
    int n;

    if (0 == nIndex) return TRUE;

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (IsInterfaceParent(s_pModule->mInterfaceDirs
            [pDesc->mInterfaces[n]->mIndex]->mDesc, nIndex))
            return TRUE;
    }
    return FALSE;
}

int TryGetParentClsIntf(ClassDescriptor *pDesc, InterfaceDescriptor *pIntf)
{
    int n;

    if (0 == pIntf->mParentIndex) return -1;

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (IsInterfaceParent(pIntf, pDesc->mInterfaces[n]->mIndex))
            return n;
    }
    return -1;
}

BOOL IsClassInterfaceVirtuallyImplementedByAnyone(
    ClassInterface* pClsIntf, int clsIndex)
{
    assert(pClsIntf && s_pModule->mClassDirs[clsIndex]);
    ClassDescriptor* pDesc = s_pModule->mClassDirs[clsIndex]->mDesc;

    int index = pClsIntf->mIndex;

    for (int n = 0; n < pDesc->mInterfaceCount; n++) {
        ClassInterface* pClsIntfn = pDesc->mInterfaces[n];
        if (index == pClsIntfn->mIndex) {
            return (pClsIntfn->mAttribs & ClassInterfaceAttrib_virtual);
        }
    }

    return FALSE;
}

// CLS_ITF     -> [ k_virtual | k_hidden | k_callback ]
//                k_interface ident s_semicolon
//
int P_ClassInterface(ClassDirEntry *pClass)
{
    int n, m, x, nIndexOfInterface;
    CARToken token;
    WORD attribs = 0;
    ClassDescriptor *pDesc;
    InterfaceDescriptor *pIntf;
    char szInterfaceName[c_nMaxTokenSize + 1];

    pDesc = pClass->mDesc;

    // [ k_virtual | k_hidden | k_privileged | k_asynchronous | k_filtering ] k_interface
    //
    token = GetToken(s_pFile);
    switch (token) {
        case Token_K_virtual:
            pDesc->mAttribs |= ClassAttrib_hasvirtual;
            attribs |= ClassInterfaceAttrib_virtual;
            token = GetToken(s_pFile);
            break;
        case Token_K_hidden:
            attribs |= ClassInterfaceAttrib_hidden;
            token = GetToken(s_pFile);
            break;
        case Token_K_privileged:
            attribs |= ClassInterfaceAttrib_privileged;
            token = GetToken(s_pFile);
            break;
        case Token_K_asynchronous:
            pDesc->mAttribs |= ClassAttrib_hasasynchronous;
            attribs |= ClassInterfaceAttrib_async;
            token = GetToken(s_pFile);
            break;
        case Token_K_filtering:
            pDesc->mAttribs |= ClassAttrib_hasfilter;
            attribs |= ClassInterfaceAttrib_filter;
            token = GetToken(s_pFile);
            break;
        default:
            break;
    }

    if (Token_K_callbacks == token) {
        if (!(attribs & ClassInterfaceAttrib_filter)) {
            attribs |= ClassInterfaceAttrib_callback;
            pDesc->mAttribs |= ClassAttrib_hascallback;
        }
    }
    else if (Token_K_callbacksink == token) {
        attribs |= ClassInterfaceAttrib_callbacksink;
    }
    else if (Token_K_delegates == token) {
        if (!(attribs & ClassInterfaceAttrib_filter)) {
            attribs |= ClassInterfaceAttrib_delegate;
            attribs |= ClassInterfaceAttrib_callback;
            pDesc->mAttribs |= ClassAttrib_hascallback;
        }
    }
    else if (Token_K_delegatesink == token) {
        attribs |= ClassInterfaceAttrib_delegatesink;
    }
    else if (Token_K_interface != token) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    // ident
    //
    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_ExpectInterfaceName);
        return Ret_AbortOnError;
    }

    const char *pszName = g_szCurrentToken;
    char *pszNamespaces = NULL;
    const char *dot = strrchr(pszName, '.');
    if (dot != NULL) {
        pszNamespaces = (char *)malloc(dot - pszName + 1);
        memset(pszNamespaces, 0, dot - pszName + 1);
        memcpy(pszNamespaces, pszName, dot - pszName);
        pszName = dot + 1;
    }
    else {
        FrontDeclaration* pDecl;
        int count = FindFrontDeclaration(pszName, Type_interface, &pDecl);
        if (count > 1) {
            ErrorReport(CAR_E_TypeAmbiguous, g_szCurrentToken);
            return Ret_AbortOnError;
        }
        if (count == 1) {
            pszName = pDecl->pszFullName;
            dot = strrchr(pszName, '.');
            if (dot != NULL) {
                pszNamespaces = (char *)malloc(dot - pszName + 1);
                memset(pszNamespaces, 0, dot - pszName + 1);
                memcpy(pszNamespaces, pszName, dot - pszName);
                pszName = dot + 1;
            }
        }
        else {
            int nsLen = strlen(GetNamespace());
            int usingLen = strlen(s_pszUsingNS);
            if (nsLen + usingLen != 0) {
                int len = nsLen + 1 + usingLen + 1;
                pszNamespaces = (char *)malloc(len);
                memset(pszNamespaces, 0, len);
                if (nsLen != 0) strcpy(pszNamespaces, GetNamespace());
                if (usingLen != 0) {
                    if (pszNamespaces[0] != '\0') strcat(pszNamespaces, ";");
                    strcat(pszNamespaces, s_pszUsingNS);
                }
            }
        }
    }
    nIndexOfInterface = n = RetrieveInterface(pszName, pszNamespaces, s_pModule, FALSE);
    if (pszNamespaces != NULL) free(pszNamespaces);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "interface", g_szCurrentToken);
        goto ErrorSkip;
    }

    strcpy(szInterfaceName, s_pModule->mInterfaceDirs[n]->mName);
    pIntf = s_pModule->mInterfaceDirs[n]->mDesc;

    //checking whether the interfaces declared in class and the interfaces
    //defined in module match.
    if (pIntf->mAttribs & InterfaceAttrib_t_callbacks) {
        if (!(attribs & ClassInterfaceAttrib_callback)
                && !(attribs & ClassInterfaceAttrib_callbacksink)
                && !(attribs & ClassInterfaceAttrib_filter))
            ErrorReport(CAR_E_UnmatchedInterface, g_szCurrentToken);
    }
    else if (pIntf->mAttribs & InterfaceAttrib_t_delegates) {
        if (!(attribs & ClassInterfaceAttrib_delegate)
                && !(attribs & ClassInterfaceAttrib_delegatesink)
                && !(attribs & ClassInterfaceAttrib_filter))
            ErrorReport(CAR_E_UnmatchedInterface, g_szCurrentToken);
    }
    else {
        if ((attribs & ClassInterfaceAttrib_callback)
                || (attribs & ClassInterfaceAttrib_callbacksink)
                || (attribs & ClassInterfaceAttrib_delegate)
                || (attribs & ClassInterfaceAttrib_delegatesink))
            ErrorReport(CAR_E_UnmatchedInterface, g_szCurrentToken);
    }

    //checking only aspect can have privileged interface;
    if (!(pDesc->mAttribs & ClassAttrib_t_aspect) &&
            (attribs & ClassInterfaceAttrib_privileged)) {
        ErrorReport(CAR_E_IllegalPrivilegedInterface,
                pClass->mName, s_pModule->mInterfaceDirs[n]->mName);
        return Ret_AbortOnError;
    }

    if ((pDesc->mAttribs & ClassAttrib_t_generic) &&
            (pDesc->mAttribs & ClassAttrib_hasvirtual)) {
        ErrorReport(CAR_E_GenericCouldntHaveVirtualInterface,
                pClass->mName, s_pModule->mInterfaceDirs[n]->mName);
        return Ret_AbortOnError;
    }

    if (!(pIntf->mAttribs & InterfaceAttrib_defined)) {
        ErrorReport(CAR_E_NotFound, "interface", g_szCurrentToken);
        goto ErrorSkip;
    }

    // Eliminate parent-son relation of class interfaces.
    //
    m = TryGetParentClsIntf(pDesc, pIntf);
    if (m >= 0) {
        ErrorReport(CAR_E_InheritanceConflict, g_szCurrentToken, pClass->mName);
        goto ErrorSkip; // parent interface is replaced
    }
    else if (IsClsIntfParent(pDesc, n)) {
        ErrorReport(CAR_E_InheritanceConflict, g_szCurrentToken, pClass->mName);
        goto ErrorSkip; // ignore parent interface of others
    }

    n = CreateClassInterface(n, pDesc);
    if (n < 0) {
        ErrorReport(CAR_E_DupEntry, "class interface", g_szCurrentToken);
        goto ErrorSkip;
    }
    if (!(attribs & ClassInterfaceAttrib_callback)) {
        if (!(pIntf->mAttribs & InterfaceAttrib_local)
            || (pIntf->mAttribs & InterfaceAttrib_parcelable))
            pDesc->mAttribs &= ~ClassAttrib_classlocal;
        pDesc->mAttribs |= ClassAttrib_defined;
    }

    if (pDesc->mAttribs & ClassAttrib_hasparent) {
        BOOL inherit = IsClassInterfaceVirtuallyImplementedByAnyone(
            pDesc->mInterfaces[n], pDesc->mParentIndex);
        if (inherit) attribs |= ClassInterfaceAttrib_inherited;
    }

    pDesc->mInterfaces[n]->mAttribs = attribs;

    if ((attribs & ClassInterfaceAttrib_callback)
        && !(attribs & ClassInterfaceAttrib_delegate)) {
        if (s_pModule->mInterfaceDirs[nIndexOfInterface]->mDesc->mAttribs & InterfaceAttrib_local) {
            ErrorReport(CAR_E_LocalConflict);
            goto ErrorSkip;
        }
        GenerateHandlerInterface(szInterfaceName, s_pModule, s_pModule->mInterfaceDirs[nIndexOfInterface]);
    }
    else if (attribs & ClassInterfaceAttrib_async) {
        if (s_pModule->mInterfaceDirs[nIndexOfInterface]->mDesc->mAttribs & InterfaceAttrib_local) {
            ErrorReport(CAR_E_LocalConflict);
            goto ErrorSkip;
        }
        x = GenerateAsyncCallbackInterface(
                szInterfaceName,
                s_pModule,
                s_pModule->mInterfaceDirs[nIndexOfInterface]);

        if (x > 0) s_pModule->mInterfaceDirs[x]->mDesc->mAttribs |= InterfaceAttrib_defined;
        strcat(szInterfaceName, "Async");
        x = AddConstClassInterface(szInterfaceName, NULL, pDesc);
        if (x > 0) {
            pDesc->mInterfaces[x]->mAttribs |= ClassInterfaceAttrib_callback;
            pDesc->mInterfaces[x]->mAttribs |= ClassInterfaceAttrib_async;
            pDesc->mAttribs |= ClassAttrib_hascallback;
        }
    }

    if (s_bInNakedMode) {
        pClass->mDesc->mAttribs |= ClassAttrib_naked;
    }

ErrorSkip:
    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }
    return Ret_Continue;
}

int GenerateClassObject(ClassDirEntry *pClass)
{
    ClassDescriptor *pDesc;

    pDesc = pClass->mDesc;

    assert(0 == pDesc->mCtorIndex);

    int r, n;
    char szName[c_nMaxTokenSize + 1];
    int attr = InterfaceAttrib_defined | InterfaceAttrib_clsobj | InterfaceAttrib_t_normal;
    int classAttr = ClassAttrib_defined | ClassAttrib_t_clsobj;
    if (pDesc->mAttribs & ClassAttrib_t_generic) {
        attr |= InterfaceAttrib_generic;
        classAttr |= ClassAttrib_t_generic;
    }
    if (pDesc->mAttribs & ClassAttrib_naked) {
        classAttr |= ClassAttrib_naked;
    }

    pDesc->mAttribs |= ClassAttrib_hasctor;

    //need to add namespace
    if (pClass->mNameSpace != NULL) {
        sprintf(szName, "%s.I%sClassObject", pClass->mNameSpace, pClass->mName);
    }
    else {
        sprintf(szName, "I%sClassObject", pClass->mName);
    }
    r = CreateInterfaceDirEntry(szName, s_pModule, 0);
    if (r < 0) {
        CreateError(r, "interface", szName);
        return Ret_AbortOnError;
    }
    pDesc->mCtorIndex = r;

    s_pModule->mInterfaceDirs[pDesc->mCtorIndex]->mDesc->mAttribs  = attr;
    s_pModule->mInterfaceDirs[pDesc->mCtorIndex]->mFileIndex = pClass->mFileIndex;

    n = RetrieveInterface("IClassObject", NULL, s_pModule, TRUE);
    if (n < 0) {
        return Ret_AbortOnError;
    }
    s_pModule->mInterfaceDirs[pDesc->mCtorIndex]->mDesc->mParentIndex = n;

    s_pModule->mDefinedInterfaceIndexes[s_pModule->mDefinedInterfaceCount++] = r;

    //need to add namespace
    if (pClass->mNameSpace != NULL) {
        sprintf(szName, "%s.%sClassObject", pClass->mNameSpace, pClass->mName);
    }
    else {
        sprintf(szName, "%sClassObject", pClass->mName);
    }
    r = CreateClassDirEntry(szName, s_pModule, 0);

    n = CreateClassInterface(pDesc->mCtorIndex, s_pModule->mClassDirs[r]->mDesc);
    if (n < 0) {
        if (n != CLSError_DupEntry)
            CreateError(n, "class interface",
                s_pModule->mInterfaceDirs[pDesc->mCtorIndex]->mName);
        return Ret_AbortOnError;
    }

    if (r < 0) {
        if (r != CLSError_DupEntry)
            CreateError(r, "class", szName);
    }
    else {
        s_pModule->mClassDirs[r]->mDesc->mAttribs = classAttr;
        s_pModule->mClassDirs[r]->mFileIndex = pClass->mFileIndex;
    }
    return Ret_Continue;
}

void AddClassParent(
    const char *pszName, ClassDirEntry* pClass)
{
    ClassDescriptor *pDesc = pClass->mDesc;
    int n;
    ClassDescriptor *pParent;

    n = RetrieveClass(pszName, NULL, s_pModule, FALSE);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "class", pszName);
        return;
    }
    pParent = s_pModule->mClassDirs[n]->mDesc;

    if (pDesc == pParent) {
        ErrorReport(CAR_E_NestedInherit,
                "class", s_pModule->mClassDirs[n]->mName);
        return;
    }
    if (!(pParent->mAttribs & ClassAttrib_defined)) {
        ErrorReport(CAR_E_NotFound, "class", pszName);
        return;
    }
    if (pParent->mAttribs & ClassAttrib_t_generic) {
        ErrorReport(CAR_E_CouldNotInheritGeneric, pszName);
        return;
    }
    if (pParent->mAttribs & ClassAttrib_final) {
        ErrorReport(CAR_E_CouldNotInheritFinalClass, pszName);
        return;
    }

    pDesc->mParentIndex = n;
    pDesc->mAttribs |= ClassAttrib_hasparent;
}

void AddGenericParent(
    const char *pszName, ClassDirEntry* pClass)
{
    int n;
    ClassDescriptor *pParent;
    ClassDescriptor *pDesc = pClass->mDesc;

    n = RetrieveClass(pszName, NULL, s_pModule, FALSE);
    if (n < 0) {
        ErrorReport(CAR_E_NotFound, "Generic", pszName);
        return;
    }
    pParent = s_pModule->mClassDirs[n]->mDesc;

    if (pDesc == pParent) {
        ErrorReport(CAR_E_NestedInherit,
                "Generic", s_pModule->mClassDirs[n]->mName);
        return;
    }
    if (!(pParent->mAttribs & ClassAttrib_defined)) {
        ErrorReport(CAR_E_NotFound, "Generic", pszName);
        return;
    }
    if (!(pParent->mAttribs & ClassAttrib_t_generic)) {
        ErrorReport(CAR_E_NotGeneric, pszName);
        return;
    }
    pDesc->mParentIndex = n;
    pDesc->mAttribs |= ClassAttrib_hasparent;
}

// CLS_CTOR    -> k_constructor s_lparen [ METHOD_ARG { s_comma METHOD_ARG } ] s_rparen s_semicolon
// METHOD_ARG  -> [ ARG_ATTRIBS ] TYPE ident
//
int P_ClassCtorMethod(ClassDirEntry *pClass, BOOL isDeprecated)
{
    InterfaceDescriptor *pIntfDesc = NULL;
    int annotationCount = 0;
    AnnotationDescriptor *pAnnotationDescs[MAX_ANNOTATION_NUMBER];
    ClassDescriptor *pClsDesc;
    pClsDesc = pClass->mDesc;

    if (0 == pClsDesc->mCtorIndex) {
        if (GenerateClassObject(pClass) == Ret_AbortOnError)
            return Ret_AbortOnError;
    }

    pIntfDesc = s_pModule->mInterfaceDirs[pClsDesc->mCtorIndex]->mDesc;

    CARToken token = GetToken(s_pFile);
    if (token == Token_S_lbracket) {
        token = GetToken(s_pFile);
        while (token != Token_S_rbracket) {
            if (token == Token_S_at) {
                if (annotationCount >= MAX_ANNOTATION_NUMBER) {
                    ErrorReport(CAR_E_TooManyAnnotation);
                    return Ret_AbortOnError;
                }
                if (P_Annotation(pAnnotationDescs, annotationCount) == Ret_AbortOnError) {
                    ErrorReport(CAR_E_IllegalMethodAnnotation, g_szCurrentToken);
                    return Ret_AbortOnError;
                }
                annotationCount++;
            }
            else {
                ErrorReport(CAR_E_IllegalMethodProperties, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            token = GetToken(s_pFile);
        }
        token = GetToken(s_pFile);
    }
    if (token != Token_K_constructor) {
        ErrorReport(CAR_E_ExpectSymbol, "constructor");
        return Ret_AbortOnError;
    }

    int n = CreateInterfaceMethod("Method", pIntfDesc);
    if (n < 0) {
        CreateError(n, "interface method", g_szCurrentToken);
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethods[n]->mType.mType = Type_ECode;
    pIntfDesc->mMethods[n]->mAnnotationCount = annotationCount;
    pIntfDesc->mMethods[n]->mAnnotations = (AnnotationDescriptor**)malloc(sizeof(AnnotationDescriptor*) * annotationCount);
    for (int i = 0; i < annotationCount; i++) {
        pIntfDesc->mMethods[n]->mAnnotations[i] = pAnnotationDescs[i];
    }

    if (GetToken(s_pFile) != Token_S_lparen) {
        ErrorReport(CAR_E_ExpectSymbol, "(");
        return Ret_AbortOnError;
    }
    if (pClass->mDesc->mAttribs & ClassAttrib_local) {
        pIntfDesc->mAttribs |= InterfaceAttrib_local;
    }
    if (PeekToken(s_pFile) != Token_S_rparen) {
        if (pClass->mDesc->mAttribs & ClassAttrib_singleton) {
            ErrorReport(CAR_E_ParameterInSingletonCtor);
            return Ret_AbortOnError;
        }
        if (pClass->mDesc->mAttribs & ClassAttrib_t_aspect) {
            ErrorReport(CAR_E_ContructorWithAspect);
            return Ret_AbortOnError;
        }
        BOOL isLocal = (pClsDesc->mAttribs & ClassAttrib_local);
        do {
            if (P_MethodArg(pIntfDesc->mMethods[n], isLocal, isDeprecated) == \
                Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
            token = GetToken(s_pFile);
        } while (Token_S_comma == token);

        if (Token_S_rparen != token) {
            ErrorReport(CAR_E_ExpectSymbol, ")");
            return Ret_AbortOnError;
        }

        pIntfDesc->mMethods[n]->mAttribs |= MethodAttrib_NonDefaultCtor;
    }
    else {
        GetToken(s_pFile); // skip ")"
        pIntfDesc->mMethods[n]->mAttribs |= MethodAttrib_DefaultCtor;
        pClass->mDesc->mAttribs |= ClassAttrib_hasDefaultCtor;
    }

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_ExpectSymbol, ";");
        return Ret_AbortOnError;
    }

    //Create the method name combined with paramters' name
    char szMethodName[c_nMaxTokenSize + 1] = {'\0'};
    strcpy(szMethodName, "CreateObjectWith");
    int nCurLength = strlen("CreateObjectWith");
    if (pIntfDesc->mMethods[n]->mParamCount != 0) {
        int nMeth, tokenSize = 0;
        for (nMeth = 0; nMeth < pIntfDesc->mMethods[n]->mParamCount; nMeth++) {
            if (pIntfDesc->mMethods[n]->mParams[nMeth]->mAttribs & ParamAttrib_out) {
                ErrorReport(CAR_E_OutParameterInCtor);
                return Ret_AbortOnError;
            }
            tokenSize += strlen(pIntfDesc->mMethods[n]->mParams[nMeth]->mName);
            if (tokenSize > c_nMaxTokenSize) {
                ErrorReport(CAR_E_SymbolTooLong);
                return Ret_AbortOnError;
            }
            strcat(szMethodName, pIntfDesc->mMethods[n]->mParams[nMeth]->mName);
            if (isalpha(szMethodName[nCurLength]) && islower(szMethodName[nCurLength])) {
                szMethodName[nCurLength] = toupper(szMethodName[nCurLength]);
            }
            nCurLength = strlen(szMethodName);
        }
    }
    else {
        strcat(szMethodName, "DefaultCtor");
        nCurLength = strlen(szMethodName);
    }
    delete pIntfDesc->mMethods[n]->mName;
    pIntfDesc->mMethods[n]->mName = NULL;
    pIntfDesc->mMethods[n]->mName = new char[nCurLength + 1];
    if (pIntfDesc->mMethods[n]->mName == NULL) {
        return Ret_AbortOnError;
    }
    strcpy(pIntfDesc->mMethods[n]->mName, szMethodName);

    // Append "[out] IInterface **ppNewObj" Param
    TypeDescriptor type;
    int i;

    memset(&type, 0, sizeof(type));
    type.mType = Type_interface;
    type.mIndex = SelectInterfaceDirEntry("IInterface", NULL, s_pModule);
    type.mPointer = 2;

    i = CreateMethodParam("newObj", pIntfDesc->mMethods[n]);
    if (i < 0) {
        CreateError(n, "method parameter", "newObj");
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethods[n]->mParams[i]->mAttribs = ParamAttrib_out;
    memcpy(&pIntfDesc->mMethods[n]->mParams[i]->mType, &type, sizeof(type));

    BuildMethodSignature(pIntfDesc->mMethods[n]);
    pIntfDesc->mMethodCount -= 1;
    if (IsMethodDuplicated(pIntfDesc, pIntfDesc->mMethods[n])) {
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethodCount += 1;

    return Ret_Continue;
}

int AddTrivialClassCtorMethod(ClassDirEntry *pClass)
{
    InterfaceDescriptor *pIntfDesc = NULL;
    ClassDescriptor *pClsDesc;
    pClsDesc = pClass->mDesc;

    if (0 == pClsDesc->mCtorIndex) {
        if (GenerateClassObject(pClass) == Ret_AbortOnError)
            return Ret_AbortOnError;
    }

    pIntfDesc = s_pModule->mInterfaceDirs[pClsDesc->mCtorIndex]->mDesc;

    int n;

    n = CreateInterfaceMethod("Method", pIntfDesc);
    if (n < 0) {
        CreateError(n, "interface method", g_szCurrentToken);
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethods[n]->mType.mType = Type_ECode;

    pIntfDesc->mMethods[n]->mAttribs |= MethodAttrib_TrivialCtor | MethodAttrib_DefaultCtor;
    pClass->mDesc->mAttribs |= ClassAttrib_hasTrivialCtor | ClassAttrib_hasDefaultCtor;

    //Create the method name combined with "DefaultCtor"
    char szMethodName[c_nMaxTokenSize + 1] = {'\0'};
    strcpy(szMethodName, "CreateObjectWith");
    int nCurLength = strlen("CreateObjectWith");
    strcat(szMethodName, "DefaultCtor");
    nCurLength = strlen(szMethodName);
    delete pIntfDesc->mMethods[n]->mName;
    pIntfDesc->mMethods[n]->mName = NULL;
    pIntfDesc->mMethods[n]->mName = new char[nCurLength + 1];
    if (pIntfDesc->mMethods[n]->mName == NULL) {
        return Ret_AbortOnError;
    }
    strcpy(pIntfDesc->mMethods[n]->mName, szMethodName);

    // Append "[out] IInterface **ppNewObj" Param
    TypeDescriptor type;
    int i;

    memset(&type, 0, sizeof(type));
    type.mType = Type_interface;
    type.mIndex = SelectInterfaceDirEntry("IInterface", NULL, s_pModule);
    type.mPointer = 2;

    i = CreateMethodParam("newObj", pIntfDesc->mMethods[n]);
    if (i < 0) {
        CreateError(n, "method parameter", "newObj");
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethods[n]->mParams[i]->mAttribs = ParamAttrib_out;
    memcpy(&pIntfDesc->mMethods[n]->mParams[i]->mType, &type, sizeof(type));

    BuildMethodSignature(pIntfDesc->mMethods[n]);
    pIntfDesc->mMethodCount -= 1;
    if (IsMethodDuplicated(pIntfDesc, pIntfDesc->mMethods[n])) {
        return Ret_AbortOnError;
    }
    pIntfDesc->mMethodCount += 1;

    return Ret_Continue;
}

int AddCallbackCoalesceMethod(char* pszMethodName, ClassDirEntry *pClass)
{
    InterfaceDescriptor *pIntfDesc = NULL;
    ClassDescriptor *pClsDesc;
    MethodDescriptor *pMethod;
    pClsDesc = pClass->mDesc;
    int n, x;

    for (n = 0; n < pClsDesc->mInterfaceCount; n++) {
        if (pClsDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback) {
            x = RetrieveMethod(pszMethodName, s_pModule,
                                                    pClsDesc->mInterfaces[n]);
            if (x < 0) continue;

            pIntfDesc = s_pModule->mInterfaceDirs[pClsDesc->mInterfaces[n]->mIndex]->mDesc;
            pMethod = pIntfDesc->mMethods[x];

            if (0 == pMethod->mParamCount) {
                ErrorReport(CAR_E_NoParameters, pMethod->mName);
                return Ret_AbortOnError;
            }

            pMethod->mAttribs |= MethodAttrib_WithCoalescence;
            return x;
        }
    }

    return Ret_AbortOnError;
}

// CLS_BODY    -> [ k_inherits ident] [ k_aggregates ident [, ident]] [k_substitutes ident]
//                s_lbrace { < CLS_CTOR | CLS_ITF > { s_semicolon } } s_rbrace
//
int P_ClassBody(ClassDirEntry *pClass, BOOL isDeprecated)
{
    CARToken token;
    ClassDescriptor *pDesc;
    int nCtorCount = 0;

    pDesc = pClass->mDesc;
    pDesc->mAttribs |= ClassAttrib_classlocal;

    token = GetToken(s_pFile);

    if (pDesc->mAttribs & ClassAttrib_t_normalClass) {
        if (Token_K_inherits == token) {
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            AddClassParent(g_szCurrentToken, pClass);

            token = GetToken(s_pFile);
            if (Token_S_lbrace != token) {
                //should be ','
                if (Token_S_comma != token) {
                    ErrorReport(CAR_E_ExpectSymbol, ",");
                    return Ret_AbortOnError;
                }
                token = GetToken(s_pFile);
            }
        }

        if (Token_K_aggregates == token) {
            //some class can't aggregate aspect;
            if ((pDesc->mAttribs & ClassAttrib_t_aspect)
                    || (pDesc->mAttribs & ClassAttrib_t_generic)) {
                ErrorReport(CAR_E_AggregateUse);
                return Ret_AbortOnError;
            }

            TempClassAttribs attr;
            if (P_Aggregate(&attr) == Ret_AbortOnError)
                return Ret_AbortOnError;
            pDesc->mAttribs |= ClassAttrib_aggregate;

            memcpy(pDesc->mAggrIndexes, attr.aggrIndexs,
                    attr.mAggregateCount * sizeof(USHORT));
            pDesc->mAggregateCount = attr.mAggregateCount;

            token = GetToken(s_pFile);
        }

        if (Token_K_substitutes == token) {
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            AddGenericParent(g_szCurrentToken, pClass);

            token = GetToken(s_pFile);
        }

    }
    else if (pDesc->mAttribs & ClassAttrib_t_regime) {
        if (Token_K_aggregates == token) {
            //some class can't aggregate aspect;
            if ((pDesc->mAttribs & ClassAttrib_t_aspect)
                    || (pDesc->mAttribs & ClassAttrib_t_generic)) {
                ErrorReport(CAR_E_AggregateUse);
                return Ret_AbortOnError;
            }

            TempClassAttribs attr;
            if (P_Aggregate(&attr) == Ret_AbortOnError)
                return Ret_AbortOnError;
            pDesc->mAttribs |= ClassAttrib_aggregate;

            memcpy(pDesc->mAggrIndexes, attr.aggrIndexs,
                    attr.mAggregateCount * sizeof(USHORT));
            pDesc->mAggregateCount = attr.mAggregateCount;

            token = GetToken(s_pFile);
        }

        if (Token_K_pertainsto == token) {
            TempClassAttribs attr;
            if (P_Aspect(&attr) == Ret_AbortOnError)
                return Ret_AbortOnError;
            pDesc->mAttribs |= ClassAttrib_aspect;

            memcpy(pDesc->mAspectIndexes, attr.aspectIndexs,
                    attr.mAspectCount * sizeof(USHORT));
            pDesc->mAspectCount = attr.mAspectCount;

            token = GetToken(s_pFile);
        }
    }
    else if (pDesc->mAttribs & ClassAttrib_t_aspect) {
        if (Token_K_affiliates == token) {
            TempClassAttribs attr;
            if (P_ClassForAspect(&attr) == Ret_AbortOnError)
                return Ret_AbortOnError;
            pDesc->mAttribs |= ClassAttrib_specialAspect;

            memcpy(pDesc->mClassIndexes, attr.classIndexs,
                    attr.mClassCount* sizeof(USHORT));
            pDesc->mClassCount = attr.mClassCount;

            token = GetToken(s_pFile);
        }
    }

    if (Token_S_lbrace != token) {
        ErrorReport(CAR_E_ExpectSymbol, "{ or ;");
        return Ret_AbortOnError;
    }

    while (PeekToken(s_pFile) != Token_S_rbrace) {
        token = PeekToken(s_pFile);
        if (token == Token_K_constructor || token == Token_S_lbracket) {
            if (pDesc->mAttribs & ClassAttrib_t_generic) {
                ErrorReport(CAR_E_CouldntHasCtor, "generic");
                return Ret_AbortOnError;
            }

            nCtorCount++;
            if (P_ClassCtorMethod(pClass, isDeprecated) == Ret_AbortOnError) {
                return Ret_AbortOnError;
            }
        }
        else if (token == Token_K_coalesce) {
            GetToken(s_pFile); // peek 'coalesce'
            GetToken(s_pFile); // peek 'callback'
            if (strcmp(g_szCurrentToken, "callback")) {
                ErrorReport(CAR_E_UndefinedSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

            GetToken(s_pFile); // peek method name

            if (AddCallbackCoalesceMethod(g_szCurrentToken, pClass)
                               == Ret_AbortOnError) {
                ErrorReport(CAR_E_InvalidMemberName, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            pDesc->mAttribs |= ClassAttrib_hascoalescence;
        }
        else
        {
            if (P_ClassInterface(pClass) == Ret_AbortOnError)
                return Ret_AbortOnError;
        }

        while (PeekToken(s_pFile) == Token_S_semicolon) GetToken(s_pFile);
    }
    GetToken(s_pFile); // skip "}"

    if (nCtorCount == 0 && !s_bInKernel) {
        if (AddTrivialClassCtorMethod(pClass) == Ret_AbortOnError)
            return Ret_AbortOnError;
    }

    if (pDesc->mAttribs & ClassAttrib_hasparent) {
        ClassDirEntry* pParent = s_pModule->mClassDirs[pDesc->mParentIndex];
        if (!(pParent->mDesc->mAttribs & ClassAttrib_t_generic)) {
            if ((pParent->mDesc->mAttribs & ClassAttrib_hasctor) &&
                !(pParent->mDesc->mAttribs & ClassAttrib_hasDefaultCtor)) {
                if (!(pDesc->mAttribs & ClassAttrib_hasctor) ||
                     (pDesc->mAttribs & ClassAttrib_hasTrivialCtor)) {
                    ErrorReport(CAR_E_NoConstructor, pParent->mName, pClass->mName);
                    return Ret_AbortOnError;
                }
            }
            if (!(pParent->mDesc->mAttribs & ClassAttrib_hasvirtual)) {
                ErrorReport(CAR_E_NoVirtualInterface, pParent->mName);
                return Ret_AbortOnError;
            }
        }
    }

    CheckClassAttribs(pDesc);
    if (!(pDesc->mAttribs & ClassAttrib_defined)) {
        ErrorReport(CAR_E_NoClassInterfaces);
    }
    return Ret_Continue;
}

// CLASS       -> [ CLS_PROPERTIES ] [CLS_ATTRIBS] [k_singleton] [k_final] CLS_ID [ CLS_BODY | s_semicolon ]
// CLS_ID      -> CLS_TYPE ident
// CLS_TYPE    -> k_class | k_category | k_aspect | k_regime | k_domain
//
int P_Class(CARToken token, DWORD properties, int annotationCount, AnnotationDescriptor** pAnnotationDescs)
{
    int r;
    //TempClassAttribs attr;
    DWORD attribs = 0;
    ClassDirEntry * pClass;
    ClassDescriptor *pDesc;
    BOOL isDeprecated = FALSE;

    if (properties & Prop_deprecated) isDeprecated = TRUE;
    if (properties & Prop_local) attribs |= ClassAttrib_local;
    if (properties & Prop_oneway) {
        ErrorReport(CAR_E_IllegalClassProperties);
        return Ret_AbortOnError;
    }
    if (properties & Prop_private) attribs |= ClassAttrib_private;

    //check properties
    attribs |= s_DefaultThreadModel;

    //[k_singleton]
    if (Token_K_singleton == token) {
        attribs |= ClassAttrib_singleton;

        token = GetToken(s_pFile);
    }

    //[k_final]
    if (Token_K_final == token) {
        attribs |= ClassAttrib_final;

        token = GetToken(s_pFile);
    }

    switch (token) {
        case Token_K_generic:
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectClassName);
                return Ret_AbortOnError;
            }
            if (g_szCurrentToken[0] != 'G' && !s_bLenientNaming) {
                ErrorReport(CAR_E_IllegalGenericName, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            if (attribs & ClassAttrib_final) {
                ErrorReport(CAR_E_IllegalFinalUsage);
                return Ret_AbortOnError;
            }

            attribs |= ClassAttrib_t_generic;
            break;

        case Token_K_aspect:
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectClassName);
                return Ret_AbortOnError;
            }
            if (g_szCurrentToken[0] != 'A' && !s_bLenientNaming) {
                ErrorReport(CAR_E_IllegalAspectName, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            if (attribs & ClassAttrib_local) {
                ErrorReport(CAR_E_LocalUse);
                return Ret_AbortOnError;
            }
            if (attribs & ClassAttrib_aggregate) {
                ErrorReport(CAR_E_AggregateUse);
                return Ret_AbortOnError;
            }
            if (attribs & ClassAttrib_final) {
                ErrorReport(CAR_E_IllegalFinalUsage);
                return Ret_AbortOnError;
            }

            attribs |= ClassAttrib_t_aspect;
            break;

        case Token_K_regime:
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectClassName);
                return Ret_AbortOnError;
            }
            if (g_szCurrentToken[0] != 'R' && !s_bLenientNaming) {
                ErrorReport(CAR_E_IllegalRegimeName, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            if (attribs & ClassAttrib_final) {
                ErrorReport(CAR_E_IllegalFinalUsage);
                return Ret_AbortOnError;
            }

            attribs |= ClassAttrib_t_regime;
            break;

        case Token_K_class:
            if (GetToken(s_pFile) != Token_ident) {
                ErrorReport(CAR_E_ExpectClassName);
                return Ret_AbortOnError;
            }
            if (g_szCurrentToken[0] != 'C' && !s_bLenientNaming) {
                ErrorReport(CAR_E_IllegalClassName, g_szCurrentToken);
                return Ret_AbortOnError;
            }
            attribs |= ClassAttrib_t_normalClass;
            break;
        default:
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
    }

    int len = strlen(GetNamespace()) + strlen(g_szCurrentToken);
    char *pszFullName = (char*)malloc(len + 2);
    memset(pszFullName, 0, len + 2);
    if (GetNamespace()[0] != 0) {
        strcpy(pszFullName, GetNamespace());
        strcat(pszFullName, ".");
    }
    strcat(pszFullName, g_szCurrentToken);
    r = CreateClassDirEntry(pszFullName, s_pModule, attribs);
    free(pszFullName);

    if (Token_S_semicolon == PeekToken(s_pFile)) {
        GetToken(s_pFile); // ignore ";"
        if (CLASS_ATTR(attribs) != 0) {
            ErrorReport(CAR_E_DeclaredClassProp);
            return Ret_ContinueOnError;
        }
        if (r < 0) {
            if (r != CLSError_DupEntry)
                CreateError(r, "class", g_szCurrentToken);
        }
        else {
            s_pModule->mClassDirs[r]->mDesc->mAttribs |= attribs;
        }
        return Ret_Continue;
    }
    if (r < 0) {
        CreateError(r, "class", g_szCurrentToken);
        return Ret_AbortOnError;
    }

    pClass = s_pModule->mClassDirs[r];
    pClass->mFileIndex = CreateFileDirEntry(GetSubmodule(), s_pModule);
    pDesc = pClass->mDesc;
    pDesc->mAttribs = attribs;

    if (annotationCount > 0) {
        pDesc->mAnnotationCount = annotationCount;
        pDesc->mAnnotations = (AnnotationDescriptor**)malloc(sizeof(AnnotationDescriptor*) * annotationCount);
        for (int i = 0; i < annotationCount; i++) {
            pDesc->mAnnotations[i] = pAnnotationDescs[i];
        }
    }

    return P_ClassBody(s_pModule->mClassDirs[r], isDeprecated);
}

// PROPERTY  -> k_scriptable | k_monitor | k_singleton
//                k_sequenced | k_synchronized
//                k_local
int P_InterfaceAndClass(CARToken token)
{
    DWORD props = 0;
    int annotationCount = 0;
    AnnotationDescriptor *pAnnotationDescs[MAX_ANNOTATION_NUMBER];

    if (Token_S_lbracket == token) {
        //parse properties
        if (Token_S_rbracket == PeekToken(s_pFile)) {
            //warning
            ErrorReport(CAR_W_NoProperties);
        }
        else {
            do {
                token = GetToken(s_pFile);
                switch(token) {
                    case Token_K_deprecated:
                        props |= Prop_deprecated;
                        break;

                    case Token_K_local:
                        props |= Prop_local;
                        break;

                    case Token_K_oneway:
                        props |= Prop_oneway;
                        break;

                    case Token_K_private:
                        props |= Prop_private;
                        break;

                    case Token_K_parcelable:
                        props |= Prop_parcelable;
                        break;

                    case Token_S_at: {
                        if (annotationCount >= MAX_ANNOTATION_NUMBER) {
                            ErrorReport(CAR_E_TooManyAnnotation);
                            return Ret_AbortOnError;
                        }
                        if (P_Annotation(pAnnotationDescs, annotationCount) == Ret_AbortOnError) {
                            ErrorReport(CAR_E_IllegalClassOrInterfaceAnnotation);
                            return Ret_AbortOnError;
                        }
                        annotationCount++;
                        break;
                    }

                    default:
                        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                }

                token = GetToken(s_pFile);
            } while(Token_S_comma == token);

            if (Token_S_rbracket != token) {
                ErrorReport(CAR_E_ExpectSymbol, ", or ]");
            }
        }

        token = GetToken(s_pFile);
    }

    switch(token) {
        case Token_K_interface:
        case Token_K_callbacks:
        case Token_K_delegates:
            return P_Interface(token, props, annotationCount, pAnnotationDescs);

        default:
            return P_Class(token, props, annotationCount, pAnnotationDescs);
    }
}

int P_UsingInterface()
{
    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    AddFrontDeclaration(g_szCurrentToken, Type_interface);

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int P_UsingNamespace()
{
    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    AddUsingNS(g_szCurrentToken);

    if (GetToken(s_pFile) != Token_S_semicolon) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

int P_DeclNamespace();
int P_Submodule();

// CAR_ELEM    -> INTERFACE | CLASS | ENUM | STRUCT | TYPEDEF |
//                PRAGMA | IMPORT | IMPORTLIB | MERGE
//
int P_CARElement()
{
    CARToken token;

    token = GetToken(s_pFile);
    switch (token) {
        case Token_K_enum:
            return P_Enum();

        case Token_K_struct:
            return P_Struct();

        case Token_K_typedef:
            return P_Typedef();

        case Token_K_const:
            return P_Const();

        case Token_K_pragma:
            return P_Pragma();

        case Token_K_import:
            return P_Import();

        case Token_K_importlib:
            return P_ImportLibrary();

        case Token_K_merge:
            return P_Merge();

        case Token_K_mergelib:
            return P_MergeLibrary();

        case Token_K_namespace:
            return P_DeclNamespace();

        case Token_K_using:
            if (PeekToken(s_pFile) == Token_K_namespace) {
                GetToken(s_pFile);
                return P_UsingNamespace();
            }
            else if (PeekToken(s_pFile) == Token_K_interface) {
                GetToken(s_pFile);
                return P_UsingInterface();
            }
            else {
                ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
                return Ret_AbortOnError;
            }

        case Token_K_submodule:
            return P_Submodule();

        default:
            return P_InterfaceAndClass(token);

        case Token_Error:
            return Ret_AbortOnError;
    }
}

// NAMESPACE  -> k_namespace string s_lbracket CAR_BODY s-rbracket
int P_DeclNamespace()
{
    CARToken token;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    PushNamespace(g_szCurrentToken);

    if (GetToken(s_pFile) != Token_S_lbrace) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    do {
        if (P_CARElement() == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
        token = PeekToken(s_pFile);
    } while (Token_S_rbrace != token);

    GetToken(s_pFile); // skip "}"
    PopNamespace();
    return Ret_Continue;
}

// FILE -> k_submodule string s_lbracket CAR_BODY s-rbracket
int P_Submodule()
{
    CARToken token;

    if (GetToken(s_pFile) != Token_ident) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    ChangeSubmodule(g_szCurrentToken);

    if (GetToken(s_pFile) != Token_S_lbrace) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }

    do {
        if (P_CARElement() == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
        token = PeekToken(s_pFile);
    } while (Token_S_rbrace != token);

    GetToken(s_pFile); // skip "}"
    ChangeSubmodule(NULL);
    return Ret_Continue;
}

// CAR_BODY    -> s_lbrace { CAR_ELEM { s_semicolon } }+ s_rbrace
//
int P_CARBody()
{
    CARToken token;

    InitFrontDeclaration();

    // import IInterface first and ignore any errors
    //
    RetrieveInterface("IInterface", NULL, s_pModule, TRUE);
    RetrieveInterface("IObject", NULL, s_pModule, TRUE);
    RetrieveInterface("IAspect", NULL, s_pModule, TRUE);
    RetrieveInterface("IClassObject", NULL, s_pModule, TRUE);

    token = GetToken(s_pFile);
    if (Token_S_lbrace == token) {
        do {
            if (P_CARElement() == Ret_AbortOnError)
                return Ret_AbortOnError;
            token = PeekToken(s_pFile);
        } while (Token_S_rbrace != token);

        GetToken(s_pFile); // skip ")"
        return Ret_Continue;
    }
    else {
        ErrorReport(CAR_E_ExpectSymbol, "{");
        return Ret_AbortOnError;
    }
}

int P_CARVersion()
{
    CARToken token;
    int version;

    if (GetToken(s_pFile) != Token_S_lparen) {
        ErrorReport(CAR_E_ExpectSymbol, "(");
        return Ret_AbortOnError;
    }

    token = GetToken(s_pFile);
    if (token != Token_decimal && token != Token_integer) {
        ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        return Ret_AbortOnError;
    }
    if (0 == s_pModule->mMajorVersion && 0 == s_pModule->mMinorVersion) {
        int len = strlen(g_szCurrentToken);
        int dotIdx = -1;
        for (int k = 0; k < len; k++) {
            if (g_szCurrentToken[k] == '.') {
                dotIdx = k;
                break;
            }
        }
        version = atoi(g_szCurrentToken);
        if (version < 0 || version > 255) {
            ErrorReport(CAR_E_IllegalVersion);
            return Ret_AbortOnError;
        }
        if (dotIdx == -1 || dotIdx == len - 1) {
            s_pModule->mMajorVersion = (unsigned char)version;
            s_pModule->mMinorVersion = 0;
        }
        else {
            s_pModule->mMajorVersion = (unsigned char)version;
            char* digits = (g_szCurrentToken + dotIdx + 1);
            s_pModule->mMinorVersion = (unsigned char)atoi(digits);
        }
    }

    if (GetToken(s_pFile) != Token_S_rparen) {
        ErrorReport(CAR_E_ExpectSymbol, ")");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// CAR_ATTRIB  -> [VERSION | k_console | SERVICE | GRAPHICS
//                  | k_project ]
// VERSION     -> k_version s_lparen < integer | decimal > s_rparen
// SERVICE     -> k_service s_lparen string s_rparen
// GRAPHICS    -> k_graphics [ s_lparen k_litegraphics s_rparen ]
//
int P_CARAttrib()
{
    CARToken token;

    token = GetToken(s_pFile);

    switch (token) {
        case Token_K_version:
            if (P_CARVersion() == Ret_AbortOnError) return Ret_AbortOnError;
            break;

        case Token_EOF:
            ErrorReport(CAR_E_UnexpectEOF);
        case Token_Error:
            return Ret_AbortOnError;

        default:
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
            return Ret_AbortOnError;
    }

    return Ret_Continue;
}

// CAR_ATTRIBS -> s_lbracket CAR_ATTRIB { s_comma CAR_ATTRIB } s_rbracket
//
int P_CARAttribs()
{
    CARToken token;

    do {
        if (P_CARAttrib() == Ret_AbortOnError)
            return Ret_AbortOnError;
        token = GetToken(s_pFile);
    } while (token == Token_S_comma);

    if (token != Token_S_rbracket) {
        ErrorReport(CAR_E_ExpectSymbol, "]");
        return Ret_AbortOnError;
    }

    return Ret_Continue;
}

void EnumLastCheck(EnumDirEntry *pEnum)
{
    // Try to retrieve declared but undefined enum from imported libraries.
    //
    if (0 == pEnum->mDesc->mElementCount) {
        if (RetrieveEnum(pEnum->mName, NULL, s_pModule, TRUE) < 0) {
            ErrorReport(CAR_E_NotFound, "enum", pEnum->mName);
        }
    }
}

void StructLastCheck(StructDirEntry *pStruct)
{
    // Try to retrieve declared but undefined struct from imported libraries.
    //
    if (0 == pStruct->mDesc->mElementCount) {
        if (RetrieveStruct(pStruct->mName, s_pModule, TRUE) < 0) {
            ErrorReport(CAR_E_NotFound, "struct", pStruct->mName);
        }
    }
}

//
// Generate an IID seed string from interface info.
// The seed string is compose of alphabet, number or '_', we get them
// from name string, the selective order is:
//
//      (interface name) -> (method names) ->
//      (parent name) -> (parent method names) ->
//      (parent's parent name) -> ... and so on,
//
void GenIIDSeedString(InterfaceDirEntry *pInterface, char *pszBuf)
{
    int n, nSize, nTotalSize = 0;
    InterfaceDescriptor *pDesc = pInterface->mDesc;

    while (1) {
        nSize = strlen(pInterface->mName);
        if (nSize + nTotalSize + 1 > MAX_SEED_SIZE) break;
        memcpy(pszBuf, pInterface->mName, nSize);
        pszBuf += nSize;
        *pszBuf++ = '/';
        nTotalSize += nSize + 1;

        if (pInterface->mNameSpace != NULL) {
            nSize = strlen(pInterface->mNameSpace);
            if (nSize + nTotalSize + 1 > MAX_SEED_SIZE) break;
            memcpy(pszBuf, pInterface->mNameSpace, nSize);
            pszBuf += nSize;
            *pszBuf++ = '/';
            nTotalSize += nSize + 1;
        }

        for (n = 0; n < pDesc->mMethodCount; n++) {
            nSize = strlen(pDesc->mMethods[n]->mName);
            if (nSize + nTotalSize > MAX_SEED_SIZE) goto ExitEntry;
            memcpy(pszBuf, pDesc->mMethods[n]->mName, nSize);
            pszBuf += nSize;
            nTotalSize += nSize;
        }
        if (0 == pDesc->mParentIndex) break;

        pInterface = s_pModule->mInterfaceDirs[pDesc->mParentIndex];
        pDesc = pInterface->mDesc;
    }

ExitEntry:
    *pszBuf = 0;
}

void InterfaceLastCheck(InterfaceDirEntry *pInterface)
{
    char szSeedBuf[MAX_SEED_SIZE + 1];

    if (NULL != pInterface->mNameSpace && !strcmp(pInterface->mNameSpace, "systypes")) return;

    CheckInterfaceDupMethodNameEx(pInterface);

    // Try to retrieve declared but undefined interface from imported
    // libraries.
    //
    if (!(pInterface->mDesc->mAttribs & InterfaceAttrib_defined)) {
        if (RetrieveInterface(pInterface->mName, NULL, s_pModule, TRUE) < 0) {
            ErrorReport(CAR_E_NotFound, "interface", pInterface->mName);
            // continue on error
        }
    }

    // Auto generate iid, the seed string of iid is generated base on
    // interface name, methods' name, parent name, parent methods' name, etc.
    //

    if (0 == strcmp(pInterface->mName, "IInterface")) {
        pInterface->mDesc->mIID = EIID_IInterface;
    }
    else if (0 == strcmp(pInterface->mName, "IObject")) {
        pInterface->mDesc->mIID = EIID_IObject;
    }
    else if (0 == strcmp(pInterface->mName, "IAspect")) {
        pInterface->mDesc->mIID = EIID_IAspect;
    }
    else if (0 == strcmp(pInterface->mName, "IClassObject")) {
        pInterface->mDesc->mIID = EIID_IClassObject;
    }
    else if (0 == strcmp(pInterface->mName, "IProxyDeathRecipient")) {
        pInterface->mDesc->mIID = EIID_IProxyDeathRecipient;
    }
    else if (0 == strcmp(pInterface->mName, "IWeakReference")) {
        pInterface->mDesc->mIID = EIID_IWeakReference;
    }
    else if (0 == strcmp(pInterface->mName, "IWeakReferenceSource")) {
        pInterface->mDesc->mIID = EIID_IWeakReferenceSource;
    }
    else {
        GenIIDSeedString(pInterface, szSeedBuf);
        GuidFromSeedString(szSeedBuf, &pInterface->mDesc->mIID);
    }
}

void ClassLastCheck(ClassDirEntry *pClass)
{
    int n,m,i;
    ClassDescriptor *pDesc = pClass->mDesc;
    ClassInterface *pClsIntf;
    InterfaceDescriptor *pIntfDesc;
    MethodDescriptor *pMethod;
    char szSeedBuf[MAX_SEED_SIZE + 1];

    if (NULL != pClass->mNameSpace && !strcmp(pClass->mNameSpace, "systypes")) return;

    //Void class name being duplicated with module name.
    if (strcmp(pClass->mName, s_pModule->mName) == 0) {
        ErrorReport(CAR_E_DupNameWithModule);
        return ;
    }

    // Try to retrieve declared but undefined class.
    //
    if (!(pDesc->mAttribs & ClassAttrib_defined)) {
        if (RetrieveClass(pClass->mName, NULL, s_pModule, TRUE) < 0) {
            ErrorReport(CAR_E_NotFound, "class", pClass->mName);
            // continue on error
            return;
        }
    }

    // Auto generate clsid base on class name and module name
    //
    if (!(pDesc->mAttribs & ClassAttrib_t_sink)) {
        strcpy(szSeedBuf, pClass->mName);
        strcat(szSeedBuf, "/");
        if (pClass->mNameSpace != NULL) {
            strcat(szSeedBuf, pClass->mNameSpace);
            strcat(szSeedBuf, "/");
        }
        strcat(szSeedBuf, s_pModule->mName);
        GuidFromSeedString(szSeedBuf, &pDesc->mClsid);
    }

    // check if all interfaces of generic have implemented in 'child'.
    //
    if (pDesc->mAttribs & ClassAttrib_hasparent) {
        ClassDirEntry* pParent = s_pModule->mClassDirs[pDesc->mParentIndex];
        if (pParent->mDesc->mAttribs & ClassAttrib_t_generic) {
            // the 'parent' is generic
            // the interfaces of generic all must have implemented in 'child'
            for (int i = 0; i < pParent->mDesc->mInterfaceCount; ++i) {
                bool bFound = FALSE;
                for (int j = 0; j < pDesc->mInterfaceCount; ++j) {
                    if (pDesc->mInterfaces[j]->mIndex ==
                            pParent->mDesc->mInterfaces[i]->mIndex) {
                        bFound = TRUE;
                        break;
                    }
                }
                if (!bFound) {
                    ErrorReport(CAR_E_NoImplIntfOfGeneric,
                        s_pModule->mInterfaceDirs[pParent->mDesc->
                        mInterfaces[i]->mIndex]->mName, pClass->mName);
                    return;
                }
            }
        }
    }

    // Generate sink class if current class has callback interface.
    // Callback interface can't support method with [out] parameter.
    //
    if (pDesc->mAttribs & ClassAttrib_hascallback) {
        for (n = 0; n < pDesc->mInterfaceCount; n++) {
            if ((pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback)
                && !(pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_delegate)) {
                pIntfDesc = s_pModule->mInterfaceDirs\
                            [pDesc->mInterfaces[n]->mIndex]->mDesc;
                for (m = 0; m < pIntfDesc->mMethodCount; m++) {
                    pMethod = pIntfDesc->mMethods[m];
                    for (i = 0; i < pMethod->mParamCount; i++) {
                        if (pMethod->mParams[i]->mAttribs & ParamAttrib_out) {
                            ErrorReport(CAR_E_OutParameterWithCallback,
                                        pMethod->mName);
                            return;
                        }
                    }
                }
            }
        }
    }

    if ((pDesc->mAttribs & ClassAttrib_classlocal)
        && (pDesc->mAttribs & ClassAttrib_hasctor)) {
        pDesc->mAttribs |= ClassAttrib_local;
        s_pModule->mInterfaceDirs[pDesc->mCtorIndex]->mDesc->mAttribs |= InterfaceAttrib_local;
    }

    if (pDesc->mAttribs & ClassAttrib_t_clsobj) {
        if (s_pModule->mInterfaceDirs[pDesc->mInterfaces[0]->mIndex]->mDesc->mAttribs
            & InterfaceAttrib_local) {
            pDesc->mAttribs |= ClassAttrib_classlocal;
        }
    }

    // If class is not local, assure the first class interface is not local,
    //
    if (!(pDesc->mAttribs & ClassAttrib_classlocal)) {
        InterfaceDirEntry *pIntf;

        pClsIntf = pDesc->mInterfaces[0];
        pIntf = s_pModule->mInterfaceDirs[pClsIntf->mIndex];

        if ((pIntf->mDesc->mAttribs & InterfaceAttrib_local)
            || (pClsIntf->mAttribs & ClassInterfaceAttrib_callback)) {
            for (n = 1; n < pDesc->mInterfaceCount; n++) {
                if (!(s_pModule->mInterfaceDirs
                    [pDesc->mInterfaces[n]->mIndex]->mDesc->\
                    mAttribs & InterfaceAttrib_local)
                    && !(pDesc->mInterfaces[n]->mAttribs &
                    ClassInterfaceAttrib_callback)) {
                    pDesc->mInterfaces[0] = pDesc->mInterfaces[n];
                    pDesc->mInterfaces[n] = pClsIntf;
                    break;
                }
            }
            assert(n != pDesc->mInterfaceCount);
        }
    }
}
void CalcStructAlignedSize(const CLSModule *pModule, StructDescriptor *pDesc);
void LastCheck()
{
    int n;
    DWORD attribs = s_pModule->mAttribs;

    // Auto generate uuid base on uunm or name.
    //
    if (s_pModule->mUunm) {
        GuidFromSeedString(s_pModule->mUunm, &s_pModule->mUuid);
    }
    else {
        GuidFromSeedString(s_pModule->mName, &s_pModule->mUuid);
    }

    if (0 == s_pModule->mClassCount && !(attribs & CARAttrib_library)) {
        ErrorReport(CAR_E_NoClasses);
    }

    for (n = 0; n < s_pModule->mEnumCount; n++) {
        EnumLastCheck(s_pModule->mEnumDirs[n]);
    }

    for (n = 0; n < s_pModule->mStructCount; n++) {
        StructLastCheck(s_pModule->mStructDirs[n]);
        CalcStructAlignedSize(s_pModule, s_pModule->mStructDirs[n]->mDesc);
    }

    for (n = 0; n < s_pModule->mInterfaceCount; n++) {
        InterfaceLastCheck(s_pModule->mInterfaceDirs[n]);
    }

    for (n = 0; n < s_pModule->mClassCount; n++) {
        ClassLastCheck(s_pModule->mClassDirs[n]);
    }

    GenerateEnums(s_pModule);
}

int LoadSystemLibrary(BOOL bNoElastos)
{
    CLSModule *pModule;

    // Load CLS from current module(carc.exe), system types defined here.
    //
    if (LoadCLSFromDll(NULL, &pModule) < 0) {
        ErrorReport(CAR_E_LoadSystemLib);
        return -1;
    }
    AddCLSLibrary(pModule);

    // Load Elastos.Runtime.eco.
    //
    if (!bNoElastos) {
        if ((LoadCLSFromDll("Elastos.Runtime.eco", &pModule) == CLS_NoError)
            || (LoadCLS("ElastosCore.cls", &pModule) == CLS_NoError)) {
            AddCLSLibrary(pModule);
        }
        else {
            fprintf(stderr, "[ERROR] carc (0x1005) : %s\n", CLSLastErrorMessage());
            DestroyAllLibraries();
            return -1;
        }
    }

    return 0;
}

int GenerateModuleName(const char *pszModuleName)
{
    s_pModule->mName = new char[strlen(pszModuleName) + 1];
    if (!s_pModule->mName) goto OutOfMemoryError;
    strcpy(s_pModule->mName, pszModuleName);

    if (NULL != s_pModule->mUunm) return Ret_Continue;
    if (s_pModule->mAttribs & CARAttrib_library) return Ret_Continue;

    s_pModule->mUunm = new char
            [strlen(s_pszFactoryUrl) + strlen(pszModuleName) + 6];
    if (!s_pModule->mUunm) goto OutOfMemoryError;
    if (0 != *s_pszFactoryUrl) {
        strcpy(s_pModule->mUunm, s_pszFactoryUrl);
        strcat(s_pModule->mUunm, "/");
    }
    else {
        *s_pModule->mUunm = 0;
    }

    strcat(s_pModule->mUunm, pszModuleName);
    strcat(s_pModule->mUunm, ".eco");
    return Ret_Continue;

OutOfMemoryError:
    ErrorReport(CAR_E_OutOfMemory);
    return Ret_AbortOnError;
}

// CAR         -> [ CAR_ATTRIBS ] CAR_ID CAR_BODY
// CAR_ID      -> < k_module | k_library > [ uunm ]
//
int P_CAR(const char *pszModuleName, BOOL bNested)
{
    CARToken token;
    char szNakedUunm[c_nMaxTokenSize];
    const char* pszUunm;
    int nUunmlen;

    token = GetToken(s_pFile);

    // CAR_ATTRIBS
    //
    if (Token_S_lbracket == token) {
        if (P_CARAttribs() == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
        token = GetToken(s_pFile);
    }

    // CAR_ID
    //
    if (Token_K_library == token) {
        s_pModule->mAttribs |= CARAttrib_library;
    }
    else if (Token_K_module != token) {
        if (token != Token_Error) {
            ErrorReport(CAR_E_UnexpectSymbol, g_szCurrentToken);
        }
        return Ret_AbortOnError;
    }

    token = PeekToken(s_pFile);
    /*
     * If there are uunm, token will be Token_ident else token will be
     * Token_S_lbrace (grammar correct) or Token_Nothing (grammar error)
     */
    if (Token_S_lbrace != token && Token_Nothing != token) {
        token = GetUunm(s_pFile); // must be uunm in the next!
        if (Token_uunm == token) {
            if (!s_pModule->mUunm) {
                GetNakedFileName(g_szCurrentToken, szNakedUunm);
                if (strcmp("elastos", szNakedUunm)) {
                    if (strcmp(pszModuleName, szNakedUunm)) {
                        ErrorReport(CAR_E_UunmDifferFromCarName);
                        return Ret_AbortOnError;
                    }
                    else pszUunm = g_szCurrentToken;
                }
                else {
                    //if g_szCurrentToken is "Elastos:System:Elastos.Runtime.eco",
                    //we delete prefix "Elastos:System:" in order to
                    //make function IsElastosUunm in locmod.cpp work correctly.
                    pszUunm = "Elastos.Runtime.eco";
                }

                nUunmlen = strlen(pszUunm);
                s_pModule->mUunm = new char[nUunmlen + 1];
                if (!s_pModule->mUunm) {
                    ErrorReport(CAR_E_OutOfMemory);
                    return Ret_AbortOnError;
                }
                strcpy(s_pModule->mUunm, pszUunm);
            }
        }
        else { // Token_Error == token
            return Ret_AbortOnError;
        }
    }
//    else ErrorReport(CAR_W_CARUuidUndef);

    if (!bNested) {
        if (GenerateModuleName(pszModuleName) == Ret_AbortOnError) {
            return Ret_AbortOnError;
        }
    }

    // CAR_BODY
    //
    if (P_CARBody() == Ret_AbortOnError) {
        return Ret_AbortOnError;
    }

    if (!bNested) LastCheck();
    return Ret_Continue;
}

//
// Note: the DoCompiling will be nested invoked if the .car source file
// include merge() or import() statements.
//
int DoCompiling(const char *pszName, CLSModule *pModule)
{
    char *pszPath;
    BOOL bNested = FALSE;
    int n1, n2, n3, n4, nRet;
    char szModuleName[c_nMaxTokenSize];

    n1 = strlen(s_pszCurrentPath);
    n2 = strlen(s_pszSourcePath);
    n3 = strlen(pszName);
    n4 = n1 > n2? n1 + n3: n2 + n3;
    pszPath = new char[n4 + 2];
    if (NULL == pszPath) {
        ErrorReport(CAR_E_OutOfMemory);
        return Ret_AbortOnError;
    }

    if (SearchFileFromPath(s_pszCurrentPath, pszName, pszPath) < 0) {
        if (SearchFileFromPath(s_pszSourcePath, pszName, pszPath) < 0) {
            ErrorReport(CAR_E_OpenFile, pszName);
            return Ret_ContinueOnError;
        }
    }

    if (GetCurrentCarPath(pszPath, &s_pszCurrentPath) == Ret_AbortOnError) {
        delete [] pszPath;
        return Ret_AbortOnError;
    }

    GetNakedFileName(pszName, szModuleName);

    s_pFile = fopen(pszPath, "rt");
    if (!s_pFile) {
        ErrorReport(CAR_E_OpenFile, pszName);
        return Ret_ContinueOnError;
    }
    if (s_pModule == pModule) {
        bNested = TRUE;
    }
    else {
        s_pModule = pModule;
    }

    TokenInit();
    ErrorReporterInit(pszName);

    nRet = P_CAR(szModuleName, bNested);

    if ((s_pszCarFullName) && ('\0' != *s_pszCarFullName))
        delete[] s_pszCarFullName;
    s_pszCarFullName = new char[strlen(pszPath) + 1];
    if (NULL == s_pszCarFullName) {
        ErrorReport(CAR_E_OutOfMemory);
        return Ret_AbortOnError;
    }
    strcpy(s_pszCarFullName, pszPath);
    delete[] pszPath;

    fclose(s_pFile);
    return nRet;
}
