//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <lube.h>
#include <clsutil.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <ctype.h>


#define HTIME_GT_DLLTIME -2
#define HTIME_LT_DLLTIME 2

#ifdef _linux
#define _MAX_PATH 256

#define _strdup strdup
#endif

extern const char *g_pszOutputPath;
extern char * ImplNamespaceType(const char * str);

DECL_USERFUNC(PrintMessage);
DECL_USERFUNC(ErrorMessage);
DECL_USERFUNC(Upper);
DECL_USERFUNC(Lower);
DECL_USERFUNC(Embed);
DECL_USERFUNC(Rewrite);
DECL_USERFUNC(MacroRewrite);
DECL_USERFUNC(FormatUuid);
DECL_USERFUNC(CLS2CAR);
DECL_USERFUNC(CLS2Abrg);
DECL_USERFUNC(CStyleParamType);
DECL_USERFUNC(TypeClass);
DECL_USERFUNC(ClassNameOfSink);
DECL_USERFUNC(ClassNameOfClassObj);
DECL_USERFUNC(NewOfGeneric);
DECL_USERFUNC(UsageNewOfCtor);
DECL_USERFUNC(UsageNewOfGeneric);
DECL_USERFUNC(NewHeaderOfGeneric);
DECL_USERFUNC(EmptyImplOfGeneric);
DECL_USERFUNC(NewHeaderOfGenericClassObjects);
DECL_USERFUNC(NewHeaderOfGenericInterfaces);
DECL_USERFUNC(ImplOfGenericClassObjects);
DECL_USERFUNC(GenericInfoQI);
DECL_USERFUNC(CreateHFiles);
DECL_USERFUNC(CreateCPPFiles);
DECL_USERFUNC(CStyleName);
DECL_USERFUNC(DefaultInterface);
DECL_USERFUNC(PrefixingName);
DECL_USERFUNC(PrefixingNameByName);
DECL_USERFUNC(InterfaceNameOfSink);
DECL_USERFUNC(ParamOrigType);
DECL_USERFUNC(ParamAddRef);
DECL_USERFUNC(ParamRelease);
DECL_USERFUNC(NamespaceType);
DECL_USERFUNC(ParamNamespaceType);
DECL_USERFUNC(MemberNamespaceType);
DECL_USERFUNC(ParamType2String);
DECL_USERFUNC(IsFiltered);
DECL_USERFUNC(HasTrivialConstructor);
DECL_USERFUNC(HasDefaultConstructor);
DECL_USERFUNC(ParcelParameter);
DECL_USERFUNC(HasParameters);
DECL_USERFUNC(OrgClassIsAspect);
DECL_USERFUNC(ClassFullPath);
DECL_USERFUNC(ClassNamespaceBegin);
DECL_USERFUNC(ClassNamespaceEnd);
DECL_USERFUNC(ClassFullNameOfClassObj);
DECL_USERFUNC(ClassFullPathOfClassObject);
DECL_USERFUNC(InterfaceNamespaceBegin);
DECL_USERFUNC(InterfaceNamespaceEnd);
DECL_USERFUNC(EnumNamespaceBegin);
DECL_USERFUNC(EnumNamespaceEnd);
DECL_USERFUNC(GenerateModuleDeclaration);
DECL_USERFUNC(GenerateModuleImplementation);
DECL_USERFUNC(GenerateModuleImplementation2);
DECL_USERFUNC(GenerateDependHeaderForClass);
DECL_USERFUNC(GenerateDependHeaderForModule);

const UserFuncEntry g_userFuncs[] = {
    USERFUNC_(Embed, ARGTYPE_STRING, \
            "Embed string to target file"),
    USERFUNC_(PrintMessage, ARGTYPE_STRING, \
            "Put string to stdout"),
    USERFUNC_(ErrorMessage, ARGTYPE_STRING, \
            "Put string to stderr and stop lube"),
    USERFUNC_(Upper, ARGTYPE_STRING, \
            "Convert string to uppercase"),
    USERFUNC_(Lower, ARGTYPE_STRING, \
            "Convert string to lowercase"),
    USERFUNC_(FormatUuid, ARGTYPE_(Object_None, Member_Uuid), \
            "Generate an idl format uuid string"),
    USERFUNC_(Rewrite, ARGTYPE_STRING, \
            "Overwrite previous writed string"),
    USERFUNC_(MacroRewrite, ARGTYPE_STRING, \
            "Rewrite macro string"),
    USERFUNC_(CStyleParamType, ARGTYPE_(Object_None, Member_Type), \
            "Generate C style type string of method's parameters"),
    USERFUNC_(TypeClass, ARGTYPE_(Object_None, Member_Type), \
            "Generate type class string"),
    USERFUNC_(CLS2CAR, ARGTYPE_(Object_Module, Member_None), \
            "Generate CAR source code"),
    USERFUNC_(CLS2Abrg, ARGTYPE_(Object_Module, Member_None), \
            "Generate abridged class info C code"),
    USERFUNC_(ClassNameOfSink, ARGTYPE_STRING, \
            "Get class name from sink name"),
    USERFUNC_(ClassNameOfClassObj, ARGTYPE_STRING, \
            "Get class name from class object"),
    USERFUNC_(NewOfGeneric, ARGTYPE_(Object_Class, Member_None), \
            "Generate the function New source code for the generic with constructors"),
    USERFUNC_(UsageNewOfCtor, ARGTYPE_(Object_Class, Member_None), \
            "Generate usage for the classes with New methods"),
    USERFUNC_(UsageNewOfGeneric, ARGTYPE_(Object_Class, Member_None), \
            "Generate usage for the generics with New methods"),
    USERFUNC_(NewHeaderOfGeneric, ARGTYPE_(Object_Class, Member_None), \
            "Generate New hader for the generic with constructors"),
    USERFUNC_(EmptyImplOfGeneric, ARGTYPE_(Object_Class, Member_None), \
            "Generate empty New implementations New for generics with constructors"),
    USERFUNC_(NewHeaderOfGenericClassObjects, ARGTYPE_(Object_Class, Member_None), \
            "Generate GenericCreateClassObjectWith source code in _*ClassObject_.cpp for generics with constructors"),
    USERFUNC_(NewHeaderOfGenericInterfaces, ARGTYPE_(Object_Interface, Member_None), \
            "Generate GenericCreateClassObjectWith source code in _*.h for generics with constructors"),
    USERFUNC_(ImplOfGenericClassObjects, ARGTYPE_(Object_Class, Member_None), \
            "Generate GenericCreateClassObjectWith implementation for the generic with constructors"),
    USERFUNC_(GenericInfoQI, ARGTYPE_(Object_Class, Member_None), \
            "Insert EIID_GENERIC_INFO Probe"),
    USERFUNC_(CreateHFiles, ARGTYPE_STRING, \
            "Create .h files from all libraries"),
    USERFUNC_(CreateCPPFiles, ARGTYPE_STRING, \
            "Create .cpp files from all libraries"),
    USERFUNC_(CStyleName, ARGTYPE_(Object_Param, Member_None), \
            "Generate C style name string"),
    USERFUNC_(DefaultInterface, ARGTYPE_(Object_Class, Member_None), \
            "Get default interface name from class"),
    USERFUNC_(PrefixingName, ARGTYPE_(Object_Param, Member_None), \
            "Generate prefixing variable name string"),
    USERFUNC_(PrefixingNameByName, ARGTYPE_(Object_Param, Member_None), \
            "Generate prefixing variable name string"),
    USERFUNC_(InterfaceNameOfSink, ARGTYPE_STRING, \
            "Convert string to callback format"),
    USERFUNC_(ParamOrigType, ARGTYPE_(Object_None, Member_Type), \
            "Generate original C style parameter type string"),
    USERFUNC_(ParamAddRef, ARGTYPE_(Object_Param, Member_None), \
            "If the type of parameter is Interface then call its AddRef()."),
    USERFUNC_(ParamRelease, ARGTYPE_(Object_Param, Member_None), \
            "If the type of parameter is Interface then call its Release()."),
    USERFUNC_(NamespaceType, ARGTYPE_(Object_None, Member_Type), \
            "Generate elastos namepasce prefixing"),
    USERFUNC_(ParamNamespaceType, ARGTYPE_(Object_None, Member_Type), \
            "Generate elastos namepasce prefixing for method parameters"),
    USERFUNC_(MemberNamespaceType, ARGTYPE_(Object_None, Member_Type), \
            "Generate elastos namepasce prefixing"),
    USERFUNC_(ParamType2String, ARGTYPE_(Object_None, Member_None), \
            "Generate paramters type string"),
    USERFUNC_(IsFiltered, ARGTYPE_(Object_ClsIntf, Member_None), \
            "Judge whether the interface is filtered"),
    USERFUNC_(HasTrivialConstructor, ARGTYPE_(Object_Class, Member_None), \
            "Judge whether the contructor method in the class object is a constructor generated by car compiler automatically"),
    USERFUNC_(HasDefaultConstructor, ARGTYPE_(Object_Class, Member_None), \
            "Judge whether the contructor method in the class object is a default constructor"),
    USERFUNC_(ParcelParameter, ARGTYPE_(Object_Param, Member_None), \
            "Parcel parameters"),
    USERFUNC_(HasParameters, ARGTYPE_(Object_IntfMethod, Member_None), \
            "Judge if the method has some parameters"),
    USERFUNC_(OrgClassIsAspect, ARGTYPE_(Object_Class, Member_None), \
            "Judge whether the original class of this sink class is aspect"),
    USERFUNC_(ClassFullPath, ARGTYPE_(Object_Class, Member_None), \
            "Generate the full path of the class"),
    USERFUNC_(ClassNamespaceBegin, ARGTYPE_(Object_Class, Member_None), \
            "Generate the namespace beginning of the class"),
    USERFUNC_(ClassNamespaceEnd, ARGTYPE_(Object_Class, Member_None), \
            "Generate the namespace end of the class"),
    USERFUNC_(ClassFullNameOfClassObj, ARGTYPE_(Object_Class, Member_None), \
            "Get class name from class object"),
    USERFUNC_(ClassFullPathOfClassObject, ARGTYPE_(Object_Class, Member_None), \
            "Generate the full path of the class in it's class object"),
    USERFUNC_(InterfaceNamespaceBegin, ARGTYPE_(Object_Interface, Member_None), \
            "Generate the namespace beginning of the interface"),
    USERFUNC_(InterfaceNamespaceEnd, ARGTYPE_(Object_Interface, Member_None), \
            "Generate the namespace end of the interface"),
    USERFUNC_(EnumNamespaceBegin, ARGTYPE_(Object_Enum, Member_None), \
            "Generate the namespace beginning of the enum"),
    USERFUNC_(EnumNamespaceEnd, ARGTYPE_(Object_Enum, Member_None), \
            "Generate the namespace end of the enum"),
    USERFUNC_(GenerateModuleDeclaration, ARGTYPE_(Object_Module, Member_None), \
            "Generate the declaration of the module"),
    USERFUNC_(GenerateModuleImplementation, ARGTYPE_(Object_Module, Member_None), \
            "Generate the implementation of the module"),
    USERFUNC_(GenerateModuleImplementation2, ARGTYPE_(Object_Module, Member_None), \
            "Generate the implementation of the module"),
    USERFUNC_(GenerateDependHeaderForClass, ARGTYPE_(Object_Class, Member_None), \
            "Generate the depend header files for class"),
    USERFUNC_(GenerateDependHeaderForModule, ARGTYPE_(Object_Module, Member_None), \
            "Generate the depend header files for class"),
};
const int c_cUserFuncs = sizeof(g_userFuncs) / sizeof(UserFuncEntry);

IMPL_USERFUNC(PrintMessage)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    fputs((char *)pvArg, stdout);
    return LUBE_OK;
}

IMPL_USERFUNC(ErrorMessage)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    fputs((char *)pvArg, stderr);
    return LUBE_FAIL;
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

static void _strlwr(char* str)
{
    int i = 0;
    while(str[i] != '\0') {
        str[i] = tolower(str[i]);
        i++;
    }
}
#endif

IMPL_USERFUNC(Upper)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    char *pString = _strdup((char *)pvArg);

    _strupr(pString);
    int n = strlen(pString);
    for(int i = 0; i < n; i++) {
        if (*(pString + i) == '.') {
            *(pString + i) = '_';
        }
    }
    pCtx->PutString(pString);
    free(pString);

    return LUBE_OK;
}

IMPL_USERFUNC(Lower)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    char *pString = _strdup((char *)pvArg);

    _strlwr(pString);
    pCtx->PutString(pString);
    free(pString);

    return LUBE_OK;
}

IMPL_USERFUNC(InterfaceNameOfSink)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    char *pString = _strdup((char *)pvArg);
    int len = strlen(pString);
    if (len > 8) len -= 8;
    pString[len] = '\0'; // cut "Callback"
    pCtx->PutString(pString);
    free(pString);

    return LUBE_OK;
}

IMPL_USERFUNC(Embed)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    pCtx->PutString((char *)pvArg);
    return LUBE_OK;
}

IMPL_USERFUNC(Rewrite)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    if (pCtx->m_pFile) {
        int n = strlen((char *)pvArg);
        fseek(pCtx->m_pFile, -n, SEEK_CUR);
        fputs((char *)pvArg, pCtx->m_pFile);
    }
    return LUBE_OK;
}

IMPL_USERFUNC(MacroRewrite)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    char *pString = _strdup((char *)pvArg);

    int n = strlen(pString);
    for(int i = 0; i < n; i++) {
        if (*(pString + i) == '.') {
            *(pString + i) = '_';
        }
    }
    pCtx->PutString(pString);
    free(pString);

    return LUBE_OK;
}

IMPL_USERFUNC(FormatUuid)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    pCtx->PutString(Uuid2IString((GUID *)pvArg, TRUE));
    return LUBE_OK;
}

IMPL_USERFUNC(CLS2CAR)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    if (pCtx->m_pFile) {
        return CLS2CAR_(pCtx->m_pFile, (CLSModule *)pvArg);
    }
    return LUBE_OK;
}

IMPL_USERFUNC(CLS2Abrg)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    if (pCtx->m_pFile) {
        return CLS2AbrgCpp_(pCtx->m_pFile, (CLSModule *)pvArg);
    }
    return LUBE_OK;
}

IMPL_USERFUNC(CStyleParamType)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    assert(NULL != pCtx->m_pParam);

    const char *pszType = NULL;
    char szType[128];
    TypeDescriptor type, *pType = (TypeDescriptor *)pvArg;
    DWORD attribs = pCtx->m_pParam->mAttribs;

    switch (pType->mType) {
        case Type_alias:
            GetOriginalType(pCtx->m_pModule, pType, &type);
            if ((Type_EMuid == type.mType)
                    || (Type_EGuid == type.mType)
                    || (Type_struct == type.mType)){
                if (attribs & ParamAttrib_in) {
                    if (0 == type.mPointer) {
                        assert(0 == pType->mPointer);
                        sprintf(szType, "const %s *", Type2CString(pCtx->m_pModule, pType));
                        pszType = szType;
                    }
                    else pszType = Type2CString(pCtx->m_pModule, pType);
                }
                else pszType = Type2CString(pCtx->m_pModule, pType);
            }
            else if (Type_ArrayOf == type.mType) {
                if (attribs & ParamAttrib_in) {
                    if (0 == type.mPointer) {
                        assert(0 == pType->mPointer);
                        sprintf(szType, "const %s *", Type2CString(pCtx->m_pModule, pType));
                        pszType = szType;
                    }
                    else pszType = Type2CString(pCtx->m_pModule, pType);
                }
                else {
                    if (attribs & ParamAttrib_callee) {
                        if (0 == pType->mPointer) {
                            assert(1 == type.mPointer);
                            sprintf(szType, "%s *", Type2CString(pCtx->m_pModule, pType));
                        }
                        else if (1 == pType->mPointer) {
                            assert(0 == type.mPointer);
                            sprintf(szType, "%s **", Type2CString(pCtx->m_pModule, pType));
                        }
                        pszType = szType;
                    }
                    else {
                        assert(0 == type.mPointer);
                        assert(0 == pType->mPointer);
                        sprintf(szType, "%s *", Type2CString(pCtx->m_pModule, pType));
                        pszType = szType;
                    }
                }
            }
            else pszType = Type2CString(pCtx->m_pModule, pType);
            break;

        case Type_EMuid:
        case Type_EGuid:
        case Type_struct:
            if (attribs & ParamAttrib_in) {
                if (0 == pType->mPointer) {
                    sprintf(szType, "const %s *", Type2CString(pCtx->m_pModule, pType));
                    pszType = szType;
                }
                else pszType = Type2CString(pCtx->m_pModule, pType);
            }
            else pszType = Type2CString(pCtx->m_pModule, pType);
            break;

        case Type_ArrayOf:
            if (attribs & ParamAttrib_in) {
                pszType = "PCarQuintet";    //const
            }
            if (attribs & ParamAttrib_out) {
                if (attribs & ParamAttrib_callee) pszType = "PCarQuintet *";
                else pszType = "PCarQuintet";
            }
            break;

        case Type_String:
            assert(pType->mPointer <= 1);
            if (1 == pType->mPointer) {
                pszType = "String*";
            }
            else if (0 == pType->mPointer) {
                pszType = "String";
            }
            break;

        default:
            pszType = Type2CString(pCtx->m_pModule, pType);
            break;
    }

    pCtx->PutString(pszType);

    return LUBE_OK;
}

IMPL_USERFUNC(TypeClass)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    const char *pszType;
    TypeDescriptor *pType = (TypeDescriptor *)pvArg;

    // TODO: need to improve
    TypeDescriptor orgType;

Restart:
    switch (pType->mType) {
        case Type_Char16:
        case Type_Char32:
        case Type_Int8:
        case Type_Int16:
        case Type_Int32:
        case Type_Int64:
        case Type_UInt16:
        case Type_UInt32:
        case Type_UInt64:
        case Type_Byte:
        case Type_Boolean:
        case Type_Float:
        case Type_Double:
        case Type_PVoid:
        case Type_ECode:
        case Type_EventHandler:
        case Type_enum:
            pszType = "Intrinsic";
            break;

        case Type_EMuid:
            pszType = "EMuid";
            break;

        case Type_EGuid:
            pszType = "EGuid";
            break;

        case Type_ArrayOf:
            if (Type_String == pType->mNestedType->mType) {
                pszType = "ArrayOfString";
            }
            else {
                pszType = "CarArray";
            }
            break;

        case Type_Array:
            pszType = "CarArray";
            break;

        case Type_interface:
            pszType = "Interface";
            break;

        case Type_struct:
            pszType = "Struct";
            break;

        case Type_String:
            pszType = "String";
            break;

        case Type_alias:
            GetOriginalType(pCtx->m_pModule, pType, &orgType);
            pType = &orgType;
            goto Restart;

        case Type_const:
        default:
            pszType = "Unknown";
            break;
    }

    pCtx->PutString(pszType);

    return LUBE_OK;
}

IMPL_USERFUNC(ClassNameOfSink)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert (NULL != pvArg);

    char szName[c_nStrBufSize];

    strcpy(szName, (char *)pvArg);
    int len = strlen(szName);
    if (len > 4) len -= 4;
    szName[len] = 0; // cut "Sink"
    pCtx->PutString(szName);

    return LUBE_OK;
}

IMPL_USERFUNC(ClassNameOfClassObj)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert (NULL != pvArg);

    char szName[c_nStrBufSize];

    strcpy(szName, (char *)pvArg);
    szName[strlen(szName) - 11] = 0;
    pCtx->PutString(szName);

    return LUBE_OK;
}

#define CTOR_LOOP_BEGIN() \
    assert(pCtx->m_pClass->mDesc->mCtorIndex);\
    InterfaceDirEntry *pCtorInterface = \
                pCtx->m_pModule->mInterfaceDirs[pCtx->m_pClass->mDesc->mCtorIndex];\
    int m, p, i;\
    char szBuf[c_nStrBufSize];\
    int nLoopNum = 0;\
    int nLoopEnd = 3;\
    TypeDescriptor type, OrigType;\
    const char *string;\
    for (i = 0; i < pCtx->m_pClass->mDesc->mInterfaceCount; i++) {\
        if (pCtx->m_pClass->mDesc->mInterfaces[i]->mAttribs & ClassInterfaceAttrib_callback) {\
            continue;\
        }\
        /*For all constructor methods*/ \
        for (m = 0; m < pCtorInterface->mDesc->mMethodCount; m++) {\
            nLoopNum = 0;\
            do {\
                ++nLoopNum;

#define CTOR_PARAMS() \
                pCtx->PutString("        ");\
                for (p = 0; p < pCtorInterface->mDesc->mMethods[m]->mParamCount - 1; p++) {\
                    memset(szBuf, 0, c_nStrBufSize);\
                    type = pCtorInterface->mDesc->mMethods[m]->mParams[p]->mType;\
                    if (Type_struct == type.mType || Type_EMuid == type.mType ||\
                        Type_EGuid == type.mType || Type_ArrayOf == type.mType) {\
                        string = StructType2CString(pCtx->m_pModule, &type);\
                    }\
                    else if (Type_alias == type.mType) {\
                        GetOriginalType(pCtx->m_pModule, &type, &OrigType);\
                        if ((Type_struct == OrigType.mType || Type_EMuid == OrigType.mType ||\
                             Type_EGuid == OrigType.mType || Type_ArrayOf == type.mType)\
                             && OrigType.mPointer == 0) {\
                            string = StructType2CString(pCtx->m_pModule, &type);\
                        }\
                        else {\
                            string = Type2CString(pCtx->m_pModule, &type);\
                        }\
                    }\
                    else {\
                        string = Type2CString(pCtx->m_pModule, &type);\
                    }\
                    sprintf(szBuf, "/*[in]*/  %s %s,\n", string, \
                            pCtorInterface->mDesc->mMethods[m]->mParams[p]->mName);\
                    pCtx->PutString(ImplNamespaceType(szBuf));\
                    pCtx->PutString("        ");\
                }\
                memset(szBuf, 0, c_nStrBufSize);\
                sprintf(szBuf, "/*[out]*/ %s **pp%s", \
                  pCtx->m_pModule->mInterfaceDirs[pCtx->m_pClass->mDesc->mInterfaces[i]->mIndex]->mName, \
                  pCtx->m_pModule->mInterfaceDirs[pCtx->m_pClass->mDesc->mInterfaces[i]->mIndex]->mName);\
                pCtx->PutString(ImplNamespaceType(szBuf));\

#define CTOR_LOOP_END() \
            } while (nLoopNum < nLoopEnd);\
        }\
    }\

IMPL_USERFUNC(UsageNewOfCtor)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    CTOR_LOOP_BEGIN()
                if (pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_singleton) {
                    if (1 == nLoopNum) {
                        //pCtx->PutString("static _ELASTOS ECode ");
                        pCtx->PutString(pCtx->m_pClass->mName);
                        pCtx->PutString("::AcquireSingletonInDomain(\n");
                        pCtx->PutString("        /*[in]*/  PRegime pRegime,\n");
                    }
                    else {
                        //pCtx->PutString("static _ELASTOS ECode ");
                        pCtx->PutString(pCtx->m_pClass->mName);
                        pCtx->PutString("::AcquireSingleton(\n");
                    }
                }
                else {
                    if (1 == nLoopNum) {
                        //pCtx->PutString("static _ELASTOS ECode ");
                        pCtx->PutString(pCtx->m_pClass->mName);
                        pCtx->PutString("::NewInRegime(\n");
                        pCtx->PutString("        /*[in]*/  PRegime pRegime,\n");
                    }
                    else {
                        //pCtx->PutString("static _ELASTOS ECode ");
                        pCtx->PutString(pCtx->m_pClass->mName);
                        pCtx->PutString("::New(\n");
                    }
                }
                CTOR_PARAMS()
                pCtx->PutString(");\n");
    CTOR_LOOP_END()
    return LUBE_OK;
}

/*
 * This template will prepare class from given class object interface!
 */
#define PREPARE_CLASS(pszName, before, after) \
    ClassDirEntry *pClass = NULL;\
    char szName[c_nStrBufSize];\
    strcpy(szName, (char *)pszName + before);\
    szName[strlen(szName) - after] = 0;\
    int r = SelectClassDirEntry(szName, NULL, pCtx->m_pModule);\
    if (r < 0) { /* Should never be in here! */ \
        /* ignore non existed generic class silently! */ \
        return LUBE_OK;\
    }\
    pClass = pCtx->m_pModule->mClassDirs[r];\
    if (0 == (pClass->mDesc->mAttribs & ClassAttrib_t_generic)) {\
        return LUBE_OK;\
    }\

#define START_FOR_LOOP(pClass) \
    int m, p, i;\
    int length;\
    char szBuf[c_nStrBufSize];\
    char *pszName;\
    InterfaceDirEntry *pCtorInterface = NULL;\
    pCtorInterface = pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mCtorIndex];\
    for (i = 0; i < pClass->mDesc->mInterfaceCount; i++) {\
        if (pClass->mDesc->mInterfaces[i]->mAttribs & ClassInterfaceAttrib_callback) {\
            continue;\
        }\
        pszName = pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName;\
        if (!strcmp(pszName, "IObject")) {\
            continue;\
        }\
        /*For all constructor methods*/ \
        length = pCtorInterface->mDesc->mMethodCount;\
        for (m = 0; m < length; m++) {\

/* generate method declaration sources */

#define DECLARE_PARAMS(pClass, spaces) \
            pCtx->PutString("\n");\
            pCtx->PutString(spaces);\
            for (p = 0; p < pCtorInterface->mDesc->mMethods[m]->mParamCount - 1; p++) {\
                memset(szBuf, 0, c_nStrBufSize);\
                sprintf(szBuf, "/*[in]*/  %s %s,\n", \
                        Type2CString(pCtx->m_pModule, \
                        &(pCtorInterface->mDesc->mMethods[m]->mParams[p]->mType)), \
                        pCtorInterface->mDesc->mMethods[m]->mParams[p]->mName);\
                pCtx->PutString(ImplNamespaceType(szBuf));\
                pCtx->PutString(spaces);\
            }\
            memset(szBuf, 0, c_nStrBufSize);\
            sprintf(szBuf, "/*[out]*/ %s **pp%s", \
                pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName, \
                pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName);\
            pCtx->PutString(ImplNamespaceType(szBuf));\

/* Here will be main method body codes or nothing */

#define END_FOR_LOOP \
        }\
    }\

#define INVOKE_PARAMS(pClass, spaces) \
            pCtx->PutString("\n");\
            pCtx->PutString(spaces);\
            for (p = 0; p < pCtorInterface->mDesc->mMethods[m]->mParamCount - 1; p++) {\
                pCtx->PutString(pCtorInterface->mDesc->mMethods[m]->mParams[p]->mName);\
                pCtx->PutString(",\n");\
                pCtx->PutString(spaces);\
            }\
            pCtx->PutString("pp");\
            pCtx->PutString(pCtx->m_pModule->\
                    mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName);\

//Create the method name combined with paramter names
#define GENERIC_UPPER_CASE_PARAMS(pCtorInterface) \
    char szMethodName[c_nStrBufSize];\
    pCtx->PutString("GenericCreateClassObjectWith");\
    int nMeth;\
    for (nMeth = 0; nMeth < pCtorInterface->mDesc->mMethods[m]->mParamCount - 1; nMeth++) {\
        if (pCtorInterface->mDesc->mMethods[m]->mParams[nMeth]->mAttribs & ParamAttrib_out) {\
            continue;\
        }\
        memset(szMethodName, 0, c_nStrBufSize);\
        strcpy(szMethodName, pCtorInterface->mDesc->mMethods[m]->mParams[nMeth]->mName);\
        if (szMethodName[0] >= 'a' && szMethodName[0] <= 'z') {\
            szMethodName[0] -= 'a' - 'A';\
        }\
        pCtx->PutString(szMethodName);\
    }\

IMPL_USERFUNC(ImplOfGenericClassObjects)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    PREPARE_CLASS(pCtx->m_pClass->mName, 0, 11)
    START_FOR_LOOP(pClass)
            sprintf(szBuf, "ECode %s::", pCtx->m_pClass->mName);
            pCtx->PutString(szBuf);
            GENERIC_UPPER_CASE_PARAMS(pCtorInterface)
            pCtx->PutString("(");
            DECLARE_PARAMS(pClass, "    ");
            pCtx->PutString(")\n{\n");

            pCtx->PutString("    EMuid *pClsid = NULL;\n");
            sprintf(szBuf, "    _ELASTOS ECode ec = %s::New(", szName);
            pCtx->PutString(szBuf);
            INVOKE_PARAMS(pClass, "        ")
            pCtx->PutString("\n    );\n");
            pCtx->PutString("    if (FAILED(ec)) return ec;\n");

            sprintf(szBuf, "    pClsid = (EMuid*)(*pp%s)->Probe(EIID_GENERIC_INFO);\n",
                    pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName);
            pCtx->PutString(szBuf);
            pCtx->PutString("    if (!pClsid) {\n");
            sprintf(szBuf, "        (*pp%s)->Release();\n",
                    pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName);
            pCtx->PutString(szBuf);
            pCtx->PutString("        return E_WRONG_GENERIC;\n");
            pCtx->PutString("    }\n");
            sprintf(szBuf, "    if (*pClsid != ECLSID_%s) {\n", szName);
            pCtx->PutString(szBuf);
            sprintf(szBuf, "        (*pp%s)->Release();\n",
                    pCtx->m_pModule->mInterfaceDirs[pClass->mDesc->mInterfaces[i]->mIndex]->mName);
            pCtx->PutString(szBuf);
            pCtx->PutString("        return E_WRONG_GENERIC;\n");
            pCtx->PutString("    }\n");
            pCtx->PutString("    return ec;\n");
            pCtx->PutString("}\n\n");
    END_FOR_LOOP

    return LUBE_OK;
}

IMPL_USERFUNC(NewHeaderOfGenericClassObjects)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    PREPARE_CLASS(pCtx->m_pClass->mName, 0, 11)
    START_FOR_LOOP(pClass)
            pCtx->PutString("    CARAPI ");
            GENERIC_UPPER_CASE_PARAMS(pCtorInterface)
            pCtx->PutString("(");
            DECLARE_PARAMS(pClass, "        ")
            pCtx->PutString(");\n\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(NewHeaderOfGenericInterfaces)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    PREPARE_CLASS(pCtx->m_pInterface->mName, 1, 11)
    START_FOR_LOOP(pClass)
            pCtx->PutString("    virtual CARAPI ");
            GENERIC_UPPER_CASE_PARAMS(pCtorInterface)
            pCtx->PutString("(");
            DECLARE_PARAMS(pClass, "        ")
            pCtx->PutString(") = 0;\n\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(NewHeaderOfGeneric)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    START_FOR_LOOP(pCtx->m_pClass)
            pCtx->PutString("    static CARAPI New(");
            DECLARE_PARAMS(pCtx->m_pClass, "        ")
            pCtx->PutString(");\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(EmptyImplOfGeneric)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    START_FOR_LOOP(pCtx->m_pClass)
            sprintf(szBuf, "ECode %s::New(", pCtx->m_pClass->mName);
            pCtx->PutString(szBuf);
            DECLARE_PARAMS(pCtx->m_pClass, "    ")
            pCtx->PutString(")\n{\n");
            pCtx->PutString("    // TODO: Add your code here\n");
            pCtx->PutString("    return E_NOT_IMPLEMENTED;\n");
            pCtx->PutString("}\n\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(NewOfGeneric)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    START_FOR_LOOP(pCtx->m_pClass)
            pCtx->PutString("    static CARAPI New(");
            DECLARE_PARAMS(pCtx->m_pClass, "        ")
            pCtx->PutString(")\n    {\n");
            pCtx->PutString("        _ELASTOS ECode _ecode = NOERROR;\n");
            memset(szBuf, 0, c_nStrBufSize);
            sprintf(szBuf, "        %s *_p%s = NULL;\n\n",
                    pCtorInterface->mName, pCtorInterface->mName);
            pCtx->PutString(szBuf);
            pCtx->PutString("        _ecode = _CObject_AcquireClassFactory(ECLSID_");
            pCtx->PutString(pCtx->m_pClass->mName);
//            pCtx->PutString("ClassObject,\n");
            pCtx->PutString(",\n");
            pCtx->PutString("                ");
            pCtx->PutString("RGM_SAME_DOMAIN, ");
            pCtx->PutString("EIID_I");
            pCtx->PutString(pCtx->m_pClass->mName);
            pCtx->PutString("ClassObject, ");
            pCtx->PutString("(IInterface**)&_p");
            pCtx->PutString(pCtorInterface->mName);
            pCtx->PutString(");\n");
            pCtx->PutString("        if (FAILED(_ecode)) goto Exit;\n\n");
            pCtx->PutString("        _ecode = _p");
            pCtx->PutString(pCtorInterface->mName);
            pCtx->PutString("->");
            GENERIC_UPPER_CASE_PARAMS(pCtorInterface)
            pCtx->PutString("(");
            INVOKE_PARAMS(pCtx->m_pClass, "            ")
            pCtx->PutString("\n        );\n");
            pCtx->PutString("        if (FAILED(_ecode)) goto Exit;\n\n");
            pCtx->PutString("    Exit:\n");
            pCtx->PutString("        if (_p");
            pCtx->PutString(pCtorInterface->mName);
            pCtx->PutString(") _p");
            pCtx->PutString(pCtorInterface->mName);
            pCtx->PutString("->Release();\n");
            pCtx->PutString("        return _ecode;\n");
            pCtx->PutString("    }\n\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(UsageNewOfGeneric)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    START_FOR_LOOP(pCtx->m_pClass)
            //pCtx->PutString("static _ELASTOS ECode ");
            pCtx->PutString(pCtx->m_pClass->mName);
            pCtx->PutString("::New(");
            DECLARE_PARAMS(pCtx->m_pClass, "        ")
            pCtx->PutString(")\n");
    END_FOR_LOOP
    return LUBE_OK;
}

IMPL_USERFUNC(GenericInfoQI)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    ClassDirEntry *pClass = pCtx->m_pClass;

    /* generic class, if existed, will always be the root parent */
    while (pClass->mDesc->mAttribs & ClassAttrib_hasparent) {
        pClass = pCtx->m_pModule->mClassDirs[pClass->mDesc->mParentIndex];
    }
    pCtx->PutString("        return ");
    if (pClass->mDesc->mAttribs & ClassAttrib_t_generic) {
        pCtx->PutString("(IInterface *)&ECLSID_");
        pCtx->PutString(pClass->mName);
        pCtx->PutString(";\n");
    }
    else {
        pCtx->PutString("NULL;\n");
    }
    return LUBE_OK;
}

//
//Compare the last modified time of XXX.h & XXX.dll files to avoid repeated building .h file
//
int CmpHModTime(char* szName)
{
    char szPath[_MAX_PATH];
    struct stat statHFileInfo;
    int nRet;
    char szBuf[_MAX_PATH];

    GetNakedFileName(szName, szBuf);
    char *szHName = (char *)alloca(strlen(szBuf) + 4);
    if (!szHName) return LUBE_FAIL;
    strcpy(szHName, szBuf);
    strcat(szHName, ".h");

    if (!strlen(g_pszOutputPath)) {
        if (getcwd(szPath, _MAX_PATH) == NULL) {
            fprintf(stderr, "Warning: _getcwd error!\n");
            return LUBE_FALSE;
        }
    }
    else
        strcpy(szPath, g_pszOutputPath);

#ifdef _win32
        strcat(szName, "\\");
#else
        strcat(szName, "/");
#endif
    strcat(szPath, szHName);

    if (-1 != access(szPath, 0)) {
        nRet = stat(szPath, &statHFileInfo);
        if (-1 == nRet) {
            return LUBE_FALSE;
        }
    }
    else
        return LUBE_OK;

    //Get DLL / cls file info
    struct stat statFileInfo;

    nRet = SearchFileFromPath(getenv("PATH"), szName, szPath);
    if (0 == nRet) {
        nRet = stat(szPath, &statFileInfo);
        if (-1 == nRet) {
            return LUBE_FALSE;
        }
    }
    else
        return LUBE_FAIL;

    //Compare  last modified time of .h & .dll files
    if (statHFileInfo.st_mtime > statFileInfo.st_mtime) {
        return HTIME_GT_DLLTIME;
    }
    else {
        return HTIME_LT_DLLTIME;
    }
}

class Librarys
{
public:
    Librarys()
        : mCount(0)
        , mCapacity(10)
    {
        mName = (char**)malloc(mCapacity * sizeof(char*));
    }

    ~Librarys()
    {
        for (int i = 0; i < mCount; i++) {
            free(mName[i]);
        }
        free(mName);
    }

    BOOL Contains(char* library)
    {
        for (int i = 0; i < mCount; i++) {
            if (!strcmp(mName[i], library)) return TRUE;
        }
        return FALSE;
    }

    void Add(char* library)
    {
        if (mCount >= mCapacity) Grow(mCapacity * 2);

        mName[mCount] = (char*)malloc(strlen(library) + 1);
        strcpy(mName[mCount], library);
        mCount++;
    }

private:
    void Grow(int capacity)
    {
        char** newLibrarys = (char**)malloc(capacity * sizeof(char*));
        for (int i = 0; i < mCount; i++) {
            newLibrarys[i] = mName[i];
        }
        free(mName);
    }

private:
    int mCount;
    int mCapacity;
    char** mName;
};

static Librarys slibraries;

IMPL_USERFUNC(CreateHFiles)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    char *szName = (char *)alloca(strlen((char *)pvArg) + 4);
    int nRet;
    CLSModule *pModule;

    strcpy(szName, (char *)pvArg);

    nRet = LoadCLS(szName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. Ignored.\n", szName);
        return LUBE_FALSE;
    }
    else if (nRet < 0)
        return LUBE_FAIL;

    pCtx->PutString("#include <");
    pCtx->PutString(pModule->mName);
    pCtx->PutString(".h>\n");

    PLUBEHEADER pLube;
    if (LoadLube(NULL, &pLube) < 0) {
        return LUBE_FAIL;
    }

    LubeContext ctx(pLube, pModule, 2);

    //Compare .h file and .dll/.cls file to avoid repeated building problem
    nRet = CmpHModTime(szName);

    if (nRet >= 0) {
        if (!slibraries.Contains(szName)) {
            slibraries.Add(szName);
            nRet = ctx.ExecTemplate("header2");
        }
    }

    DisposeFlattedCLS(pModule);
    if (nRet < 0 && nRet != HTIME_GT_DLLTIME)
        return nRet;

    return LUBE_OK;
}

IMPL_USERFUNC(CreateCPPFiles)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    char *szName = (char *)alloca(strlen((char *)pvArg) + 4);
    int nRet;
    CLSModule *pModule;

    strcpy(szName, (char *)pvArg);

    nRet = LoadCLS(szName, &pModule);
    if (nRet == CLSError_NotFound) {
        printf("Warning --Cannot find %s. Ignored.\n", szName);
        return LUBE_FALSE;
    }
    else if (nRet < 0)
        return LUBE_FAIL;

    PLUBEHEADER pLube;
    if (LoadLube(NULL, &pLube) < 0) {
        return LUBE_FAIL;
    }

    LubeContext ctx(pLube, pModule, 2);

    //Compare .h file and .dll/.cls file to avoid repeated building problem
    nRet = CmpHModTime(szName);

    if (nRet >= 0) nRet = ctx.ExecTemplate("headercpp");

    DisposeFlattedCLS(pModule);
    if (nRet < 0 && nRet != HTIME_GT_DLLTIME)
        return nRet;

    return LUBE_OK;
}

int GetCStyleStructParamName(PLUBECTX pCtx, const ParamDescriptor *pParamDesc)
{
    assert(Type_struct == pParamDesc->mType.mType);

    char szName[c_nStrBufSize];
    szName[0] = 0;
    // If struct parameter type is't pointer, we have to change its type
    // to a pointer and prefix 'p' to its name.
    if (0 == pParamDesc->mType.mPointer) {
        sprintf(szName, "p%s", pParamDesc->mName);
        pCtx->PutString(szName);
    }
    else {
        pCtx->PutString(pParamDesc->mName);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(CStyleName)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    ParamDescriptor *pParamDesc;

    assert(pvArg);

    pParamDesc = (ParamDescriptor *)pvArg;

    switch (pParamDesc->mType.mType) {
        case Type_struct:
            return GetCStyleStructParamName(pCtx, pParamDesc);
        default:
            pCtx->PutString(pParamDesc->mName);
            break;
    }

    return LUBE_OK;
}

IMPL_USERFUNC(DefaultInterface)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    int i;

    for (i = 0; i < pCtx->m_pClass->mDesc->mInterfaceCount; i++) {
        if (pCtx->m_pClass->mDesc->mInterfaces[i]->mAttribs & ClassInterfaceAttrib_callback) {
            continue;
        }
        pCtx->PutString(pCtx->m_pModule->mInterfaceDirs[pCtx->m_pClass->mDesc->mInterfaces[i]->mIndex]->mName);
        return LUBE_OK;
    }

    return LUBE_FAIL;
}

#define TYPE_CASE(type, p0, p1, p2) \
        case type:\
            pszPrefix0 = p0;\
            pszPrefix1 = p1;\
            pszPrefix2 = p2;\
            break;

IMPL_USERFUNC(PrefixingNameByName)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    ParamDescriptor *pParamDesc = (ParamDescriptor *)pvArg;
    const char *pszPrefix0 = NULL;
    const char *pszPrefix1 = NULL;
    const char *pszPrefix2 = NULL;
    char *pszPrefix = NULL;

    TypeDescriptor paramType = pParamDesc->mType;
    if (Type_alias == pParamDesc->mType.mType) {
        GetOriginalType(pCtx->m_pModule, &pParamDesc->mType, &paramType);
    }

    switch (paramType.mType) {

        TYPE_CASE(Type_Int8, NULL, "p", NULL)
        TYPE_CASE(Type_Byte, NULL, "p", NULL)
        TYPE_CASE(Type_Int16, NULL, "p", NULL)
        TYPE_CASE(Type_UInt16, NULL, "p", NULL)
        TYPE_CASE(Type_Int32, NULL, "p", NULL)
        TYPE_CASE(Type_UInt32, NULL, "p", NULL)
        TYPE_CASE(Type_Int64, NULL, "p", NULL)
        TYPE_CASE(Type_UInt64, NULL, "p", NULL)
        TYPE_CASE(Type_Float, NULL, "p", NULL)
        TYPE_CASE(Type_Double, NULL, "p", NULL)
        TYPE_CASE(Type_Boolean, NULL, "p", NULL)
        TYPE_CASE(Type_Char16, NULL, "p", NULL)
        TYPE_CASE(Type_Char32, NULL, "p", NULL)
        TYPE_CASE(Type_PVoid, "p", "pp", NULL)
        TYPE_CASE(Type_String, NULL, "p", NULL)
        TYPE_CASE(Type_ArrayOf, "p", "pp", NULL)
        case Type_enum:
        case Type_struct:
            pszPrefix1 = "p";
            pszPrefix2 = NULL;
            break;
        default:
            pszPrefix1 = "p";
            pszPrefix2 = "pp";
            break;
    }

    char* pszVarName;

    pszVarName = pParamDesc->mName;
    if (pParamDesc->mAttribs & ParamAttrib_in) {
        if ((Type_PVoid == paramType.mType)
                || (1 == paramType.mPointer)) {
            pszPrefix = GeneratePrefixalVarName("p", pszVarName);
        }
    }
    else if (pParamDesc->mAttribs & ParamAttrib_out) {
        if (1 == paramType.mPointer) { // Considered as caller
            pszPrefix = GeneratePrefixalVarName(pszPrefix1, pszVarName);
        }
        else if (2 == paramType.mPointer) { // Considered as callee
            pszPrefix = GeneratePrefixalVarName(pszPrefix2, pszVarName);
        }
        else {
            pszPrefix = GeneratePrefixalVarName(pszPrefix0, pszVarName);
        }
    }
    // do nothing to those variables with neither [in] nor [out] attributes!
    if (pszPrefix != NULL) {
        pCtx->PutString(pszPrefix);
        delete pszPrefix;
    }
    else {
        pCtx->PutString(pszVarName);
    }
    return LUBE_OK;
}
IMPL_USERFUNC(PrefixingName)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    ParamDescriptor *pParamDesc = (ParamDescriptor *)pvArg;
    const char *pszPrefix0 = NULL;
    const char *pszPrefix1 = NULL;
    const char *pszPrefix2 = NULL;
    char *pszPrefix = NULL;

    TypeDescriptor paramType = pParamDesc->mType;
    if (Type_alias == pParamDesc->mType.mType) {
        GetOriginalType(pCtx->m_pModule, &pParamDesc->mType, &paramType);
    }

    switch (paramType.mType) {

        TYPE_CASE(Type_Int8, NULL, "p", NULL)
        TYPE_CASE(Type_Byte, NULL, "p", NULL)
        TYPE_CASE(Type_Int16, NULL, "p", NULL)
        TYPE_CASE(Type_UInt16, NULL, "p", NULL)
        TYPE_CASE(Type_Int32, NULL, "p", NULL)
        TYPE_CASE(Type_UInt32, NULL, "p", NULL)
        TYPE_CASE(Type_Int64, NULL, "p", NULL)
        TYPE_CASE(Type_UInt64, NULL, "p", NULL)
        TYPE_CASE(Type_Float, NULL, "p", NULL)
        TYPE_CASE(Type_Double, NULL, "p", NULL)
        TYPE_CASE(Type_Boolean, NULL, "p", NULL)
        TYPE_CASE(Type_Char16, NULL, "p", NULL)
        TYPE_CASE(Type_Char32, NULL, "p", NULL)
        TYPE_CASE(Type_PVoid, "p", "pp", NULL)
        TYPE_CASE(Type_String, NULL, "p", NULL)
        TYPE_CASE(Type_ArrayOf, "p", "pp", NULL)
        case Type_enum:
        case Type_struct:
            pszPrefix1 = "p";
            pszPrefix2 = NULL;
            break;
        default:
            pszPrefix1 = "p";
            pszPrefix2 = "pp";
            break;
    }

    char* pszVarName;
    char szValName[12];
    int nParam = 0;

    if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam = 1;

    for (; nParam < pCtx->m_pMethod->mParamCount; ++nParam) {
        if (pParamDesc == pCtx->m_pMethod->mParams[nParam]) {
            if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam -= 1;
            break;
        }
    }
    sprintf(szValName, "Param%d", nParam);

    pszVarName = szValName;

    if (pParamDesc->mAttribs & ParamAttrib_in) {
        if ((Type_PVoid == paramType.mType)
                || (1 == paramType.mPointer)) {
            pszPrefix = GeneratePrefixalVarName("p", pszVarName);
        }
    }
    else if (pParamDesc->mAttribs & ParamAttrib_out) {
        if (1 == paramType.mPointer) {
            pszPrefix = GeneratePrefixalVarName(pszPrefix1, pszVarName);
        }
        else if (2 == paramType.mPointer) {
            pszPrefix = GeneratePrefixalVarName(pszPrefix2, pszVarName);
        }
        else {
            pszPrefix = GeneratePrefixalVarName(pszPrefix0, pszVarName);
        }
    }

    // do nothing to those variables with neither [in] nor [out] attributes!
    if (pszPrefix != NULL) {
        pCtx->PutString(pszPrefix);
        delete pszPrefix;
    }
    else {
        pCtx->PutString(pszVarName);
    }
    return LUBE_OK;
}

IMPL_USERFUNC(ParamAddRef)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    ParamDescriptor *pParamDesc = (ParamDescriptor *)pvArg;
    char szExpression[128];
    char* pszPrefix = NULL;
    TypeDescriptor type, *pType = &pParamDesc->mType;

    if (Type_alias == pParamDesc->mType.mType) {
        GetOriginalType(pCtx->m_pModule, pType, &type);
        pType = &type;
    }

    if (Type_interface == pType->mType &&
        pParamDesc->mAttribs & ParamAttrib_in &&
        1 == pType->mPointer) {

        char* pszVarName;
        char szValName[12];
        int nParam = 0;

        if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam = 1;

        for (; nParam < pCtx->m_pMethod->mParamCount; ++nParam) {
            if (pParamDesc == pCtx->m_pMethod->mParams[nParam]) {
                if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam -= 1;
                break;
            }
        }
        sprintf(szValName, "Param%d", nParam);
        pszVarName = szValName;

        pszPrefix = GeneratePrefixalVarName("p", pszVarName);
        if (NULL == pszPrefix) return LUBE_FALSE;
        sprintf(szExpression, "if (m_pUserParams->m_%s) m_pUserParams->m_%s->AddRef();", pszPrefix, pszPrefix);
        pCtx->PutString(szExpression);
        delete [] pszPrefix;
    }

    return LUBE_OK;
}

IMPL_USERFUNC(ParamRelease)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    ParamDescriptor *pParamDesc = (ParamDescriptor *)pvArg;
    char szExpression[128];
    char* pszPrefix = NULL;
    TypeDescriptor type, *pType = &pParamDesc->mType;

    if (Type_alias == pType->mType) {
        GetOriginalType(pCtx->m_pModule, pType, &type);
        pType = &type;
    }

    if (Type_interface == pType->mType &&
        pParamDesc->mAttribs & ParamAttrib_in &&
        1 == pType->mPointer) {
        char* pszVarName;
        char szValName[12];
        int nParam = 0;

        if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam = 1;

        for (; nParam < pCtx->m_pMethod->mParamCount; ++nParam) {
            if (pParamDesc == pCtx->m_pMethod->mParams[nParam]) {
                if (pCtx->m_pClass && pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) nParam -= 1;
                break;
            }
        }
        sprintf(szValName, "Param%d", nParam);
        pszVarName = szValName;

        pszPrefix = GeneratePrefixalVarName("p", pszVarName);
        if (NULL == pszPrefix) return LUBE_FALSE;
        sprintf(szExpression, "if (m_pUserParams->m_%s) m_pUserParams->m_%s->Release();", pszPrefix, pszPrefix);
        pCtx->PutString(szExpression);
        delete [] pszPrefix;
    }

    return LUBE_OK;
}

IMPL_USERFUNC(ParamOrigType)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    const char *pszType;
    char szType[32];
    TypeDescriptor type;
    TypeDescriptor *pType = (TypeDescriptor *)pvArg;

    assert(NULL != pCtx->m_pParam);
    DWORD attribs = pCtx->m_pParam->mAttribs;

    switch (pType->mType) {
        case Type_ArrayOf:
            assert(pType->mNestedType);
            if (attribs & ParamAttrib_in) {
                sprintf(szType, "ArrayOf<%s>",
                    Type2CString(pCtx->m_pModule, pType->mNestedType));
            }
            else {
                sprintf(szType, "ArrayOf<%s>*",
                    Type2CString(pCtx->m_pModule, pType->mNestedType));
            }
            pszType = szType;
            break;

        case Type_alias:
            GetOriginalType(pCtx->m_pModule, pType, &type);
            if ((type.mType == Type_EGuid ||
                type.mType == Type_EMuid) &&
                1 == type.mPointer) {
                if (type.mType == Type_EGuid) strcpy(szType, "EGuid");
                else if (type.mType == Type_EMuid) strcpy(szType, "EMuid");
                pszType = szType;
                break;
            }
        default:
            pszType = Type2CString(pCtx->m_pModule, pType);
            break;
    }

    pCtx->PutString(pszType);

    return LUBE_OK;
}

IMPL_USERFUNC(MemberNamespaceType)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    TypeDescriptor *pType = (TypeDescriptor *)pvArg;
    const char *pszType;
    TypeDescriptor type;

    memset(&type, 0, sizeof(type));

    if (Type_Array == pType->mType) {
        GetArrayBaseType(pCtx->m_pModule, pType, &type);
        pType = &type;
    }

    pszType = Type2CString(pCtx->m_pModule, pType);

    pCtx->PutString(ImplNamespaceType(pszType));
    return LUBE_OK;
}

IMPL_USERFUNC(ParamNamespaceType)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    TypeDescriptor *pType = (TypeDescriptor *)pvArg;
    const char *pszType;
    char szType[128];
    TypeDescriptor type;

    memset(&type, 0, sizeof(type));

    assert(NULL != pCtx->m_pParam);
    DWORD attribs = pCtx->m_pParam->mAttribs;

    if ((Type_struct == pType->mType)
            || (Type_EMuid == pType->mType)
            || (Type_EGuid == pType->mType)) {
        if (0 == pType->mPointer) {
            sprintf(szType, "const %s &",
                    Type2CString(pCtx->m_pModule, pType));
            pszType = szType;
        }
        else {
            pszType = Type2CString(pCtx->m_pModule, pType);
        }
    }
    else if (Type_alias == pType->mType) {
        GetOriginalType(pCtx->m_pModule, pType, &type);
        if ((Type_EMuid == type.mType)
                || (Type_EGuid == type.mType)
                || (Type_struct == type.mType)) {
            if (attribs & ParamAttrib_in) {
                if (0 == type.mPointer) {
                    assert(0 == pType->mPointer);
                    sprintf(szType, "const %s &",
                            Type2CString(pCtx->m_pModule, pType));
                    pszType = szType;
                }
                else {
                    pszType = Type2CString(pCtx->m_pModule, pType);
                }
            }
            else pszType = Type2CString(pCtx->m_pModule, pType);
        }
        else if (Type_ArrayOf == type.mType) {
            if (attribs & ParamAttrib_in) {
                if (0 == type.mPointer) {
                    assert(0 == pType->mPointer);
                    sprintf(szType, "const %s &",
                            Type2CString(pCtx->m_pModule, pType));
                    pszType = szType;
                }
                else {
                    pszType = Type2CString(pCtx->m_pModule, pType);
                }
            }
            else {
                if (attribs & ParamAttrib_callee) {
                    if (0 == pType->mPointer) {
                        assert(1 == type.mPointer);
                        sprintf(szType, "%s *", Type2CString(pCtx->m_pModule, pType));
                    }
                    else if (1 == pType->mPointer) {
                        assert(0 == type.mPointer);
                        sprintf(szType, "%s **", Type2CString(pCtx->m_pModule, pType));
                    }
                    pszType = szType;
                }
                else {
                    assert(0 == pType->mPointer);
                    assert(0 == type.mPointer);
                    sprintf(szType, "%s *", Type2CString(pCtx->m_pModule, pType));
                    pszType = szType;
                }
            }
        }
        else pszType = Type2CString(pCtx->m_pModule, pType);
    }
    else if (Type_ArrayOf == pType->mType) {
        if (attribs & ParamAttrib_in) {
            if (0 == pType->mPointer) {
                sprintf(szType, "const ArrayOf<%s> &",
                        Type2CString(pCtx->m_pModule, pType->mNestedType));
            }
            else sprintf(szType, "%s", Type2CString(pCtx->m_pModule, pType));
        }
        else {
            //attribs == ParamAttrib_out
            sprintf(szType, "ArrayOf<%s>",
                    Type2CString(pCtx->m_pModule, pType->mNestedType));
            if (0 == pType->mPointer) strcat(szType, " *");
            else strcat(szType, " **");
        }
        pszType = szType;
    }
    else {
        pszType = Type2CString(pCtx->m_pModule, pType);
    }

    pCtx->PutString(ImplNamespaceType(pszType));
    return LUBE_OK;
}

IMPL_USERFUNC(NamespaceType)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    TypeDescriptor *pType = (TypeDescriptor *)pvArg;
    const char *pszType;
    char szType[128];
    TypeDescriptor type;

    memset(&type, 0, sizeof(type));

    switch(pType->mType) {
        case Type_ArrayOf:
            strcpy(szType, "ArrayOf<");
            strcat(szType, Type2CString(pCtx->m_pModule, pType->mNestedType));
            strcat(szType, ">");
            if (1 == pType->mPointer) strcat(szType, " *");
            else if (2 == pType->mPointer) strcat(szType, " **");
            pszType = szType;
            break;

        default:
            pszType = Type2CString(pCtx->m_pModule, pType);
            break;
    }

    pCtx->PutString(ImplNamespaceType(pszType));

    return LUBE_OK;
}

IMPL_USERFUNC(ParamType2String)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    char szOutput[512] = {0};

    MethodDescriptor *pMethod = (MethodDescriptor *)pvArg;

    strcpy(szOutput, "ParamType_");

    int i = 0;
    if (pCtx->m_pClass->mDesc->mAttribs & ClassAttrib_t_sink) i = 1;
    else i = 0;

    for (; i < pMethod->mParamCount; ++i) {
        TypeDescriptor *pType = &pMethod->mParams[i]->mType;
        GenerateTypeStringForParam(pCtx->m_pModule, pType, szOutput);
        strcat(szOutput, "_");
    }

    pCtx->PutString(szOutput);

    return LUBE_OK;
}

IMPL_USERFUNC(IsFiltered)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClsIntf && pvArg == pCtx->m_pClsIntf);
    assert(NULL != pCtx->m_pOrigClass);

    ClassInterface *pClsIntf = pCtx->m_pClsIntf;
    ClassDescriptor *pClsDesc = pCtx->m_pOrigClass->mDesc;

    for(int i = 0; i < pClsDesc->mInterfaceCount; i++) {
        if ((pClsDesc->mInterfaces[i]->mIndex == pClsIntf->mIndex)
                || (pClsDesc->mInterfaces[i]->mAttribs & ClassInterfaceAttrib_filter)) {
            return true;
        }
    }

    return false;
}

IMPL_USERFUNC(HasTrivialConstructor)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDescriptor *pClsDesc = pCtx->m_pClass->mDesc;
    assert(pClsDesc->mAttribs & ClassAttrib_t_clsobj);

    ClassInterface *pClsIntf;
    InterfaceDescriptor *pIntfDesc;

    for(int i = 0; i < pClsDesc->mInterfaceCount; i++) {
        pClsIntf = pClsDesc->mInterfaces[i];
        pIntfDesc = pCtx->m_pModule->mInterfaceDirs[pClsIntf->mIndex]->mDesc;
        for (int j = 0; j < pIntfDesc->mMethodCount; j++) {
            if (pIntfDesc->mMethods[j]->mAttribs & MethodAttrib_TrivialCtor) {
                return true;
            }
        }
    }

    return false;
}

IMPL_USERFUNC(HasDefaultConstructor)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDescriptor *pClsDesc = pCtx->m_pClass->mDesc;
    assert(pClsDesc->mAttribs & ClassAttrib_t_clsobj);

    ClassInterface *pClsIntf;
    InterfaceDescriptor *pIntfDesc;

    for(int i = 0; i < pClsDesc->mInterfaceCount; i++) {
        pClsIntf = pClsDesc->mInterfaces[i];
        pIntfDesc = pCtx->m_pModule->mInterfaceDirs[pClsIntf->mIndex]->mDesc;
        for (int j = 0; j < pIntfDesc->mMethodCount; j++) {
            if (pIntfDesc->mMethods[j]->mAttribs & MethodAttrib_DefaultCtor) {
                return true;
            }
        }
    }

    return false;
}

IMPL_USERFUNC(OrgClassIsAspect)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    assert(pClsDir->mDesc->mAttribs & ClassAttrib_t_sink);

    if (pClsDir->mName[0] == 'A') return true;
    else return false;
}

IMPL_USERFUNC(ParcelParameter)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);

    ParamDescriptor* pParamDesc = (ParamDescriptor*)pvArg;
    TypeDescriptor *pType = &(pParamDesc->mType);
    TypeDescriptor orgType;

Restart:
    switch(pType->mType) {
        case Type_alias:
            GetOriginalType(pCtx->m_pModule, pType, &orgType);
            pType = &orgType;
            goto Restart;

        case Type_Byte:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteByte(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Char16:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteChar16(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Char32:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteChar32(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                pCtx->PutString("pParams->WriteChar32Ptr((Handle32)");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            break;

        case Type_String:
            assert(0 == pType->mPointer);

            pCtx->PutString("pParams->WriteString(");
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            pCtx->PutString(");");
            break;

        case Type_Boolean:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteBoolean(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_ECode:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInt32(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Int16:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInt16(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Int32:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInt32(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Int64:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInt64(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Float:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteFloat(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_Double:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteDouble(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_enum:
            assert(1 >= pType->mPointer);

            if (0 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInt32(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else {
                assert(0);
            }
            break;

        case Type_struct:
            pCtx->PutString("pParams->WriteStruct((Handle32)&");
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            pCtx->PutString(", sizeof(");
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            pCtx->PutString("));");
            break;

        case Type_EMuid:
            assert(0 == pType->mPointer);

            pCtx->PutString("pParams->WriteEMuid(");
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            pCtx->PutString(");");
            break;

        case Type_EGuid:
            assert(0 == pType->mPointer);

            pCtx->PutString("pParams->WriteEGuid(");
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            pCtx->PutString(");");
            break;

        case Type_interface:
            if (1 == pType->mPointer) {
                pCtx->PutString("pParams->WriteInterfacePtr(");
                UserFunc_PrefixingName(pCtx, pDesc, pvArg);
                pCtx->PutString(");");
            }
            else if (2 == pType->mPointer) {
                assert(0);
            }
            else assert(0);

            break;

        case Type_ArrayOf:
            assert(0 == pType->mPointer);

            if (Type_String == pType->mNestedType->mType) {
                pCtx->PutString("pParams->WriteArrayOfString(const_cast<ArrayOf<String>*>(&");
            }
            else {
                pCtx->PutString("pParams->WriteArrayOf((Handle32)&");
            }
            UserFunc_PrefixingName(pCtx, pDesc, pvArg);
            if (Type_String == pType->mNestedType->mType) {
                pCtx->PutString("));");
            }
            else {
                pCtx->PutString(");");
            }
            break;

        default:
            assert(0);
            break;
    }

    return LUBE_OK;
}

IMPL_USERFUNC(HasParameters)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pvArg);
    assert(NULL != pCtx->m_pMethod);

    if (pCtx->m_pMethod->mParamCount > 1) return true;

    return false;
}

IMPL_USERFUNC(ClassFullPath)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pClsDir->mNameSpace) + 1);
        strcpy(pszNamespace, pClsDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            int length = strlen(begin);
            for (int i = 0; i < length; i++) {
                begin[i] = tolower(begin[i]);
            }
            pCtx->PutString(begin);
            pCtx->PutString("/");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }
    pCtx->PutString(pClsDir->mName);

    return LUBE_OK;
}

IMPL_USERFUNC(ClassNamespaceBegin)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pClsDir->mNameSpace) + 1);
        strcpy(pszNamespace, pClsDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("namespace ");
            pCtx->PutString(begin);
            pCtx->PutString(" {\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(ClassNamespaceEnd)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pClsDir->mNameSpace) + 1);
        strcpy(pszNamespace, pClsDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("}\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(ClassFullNameOfClassObj)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pClsDir->mNameSpace) + 1);
        strcpy(pszNamespace, pClsDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString(begin);
            pCtx->PutString("_");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }
    char szName[c_nStrBufSize];
    strcpy(szName, (char *)pClsDir->mName);
    szName[strlen(szName) - 11] = 0;
    pCtx->PutString(szName);

    return LUBE_OK;
}

IMPL_USERFUNC(ClassFullPathOfClassObject)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pClsDir->mNameSpace) + 1);
        strcpy(pszNamespace, pClsDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            int length = strlen(begin);
            for (int i = 0; i < length; i++) {
                begin[i] = tolower(begin[i]);
            }
            pCtx->PutString(begin);
            pCtx->PutString("/");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }
    char szName[c_nStrBufSize];
    strcpy(szName, (char *)pClsDir->mName);
    szName[strlen(szName) - 11] = 0;
    pCtx->PutString(szName);

    return LUBE_OK;
}

IMPL_USERFUNC(InterfaceNamespaceBegin)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pInterface && pvArg == pCtx->m_pInterface);

    InterfaceDirEntry *pItfDir = pCtx->m_pInterface;
    if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pItfDir->mNameSpace) + 1);
        strcpy(pszNamespace, pItfDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("namespace ");
            pCtx->PutString(begin);
            pCtx->PutString(" {\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(InterfaceNamespaceEnd)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pInterface && pvArg == pCtx->m_pInterface);

    InterfaceDirEntry *pItfDir = pCtx->m_pInterface;
    if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pItfDir->mNameSpace) + 1);
        strcpy(pszNamespace, pItfDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("}\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(EnumNamespaceBegin)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pEnum && pvArg == pCtx->m_pEnum);

    EnumDirEntry *pEnumDir = pCtx->m_pEnum;
    if (pEnumDir->mNameSpace != NULL && pEnumDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pEnumDir->mNameSpace) + 1);
        strcpy(pszNamespace, pEnumDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("namespace ");
            pCtx->PutString(begin);
            pCtx->PutString(" {\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

IMPL_USERFUNC(EnumNamespaceEnd)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pEnum && pvArg == pCtx->m_pEnum);

    EnumDirEntry *pEnumDir = pCtx->m_pEnum;
    if (pEnumDir->mNameSpace != NULL && pEnumDir->mNameSpace[0] != '\0') {
        char *pszNamespace = (char*)malloc(strlen(pEnumDir->mNameSpace) + 1);
        strcpy(pszNamespace, pEnumDir->mNameSpace);
        char *begin = pszNamespace;
        while (begin != NULL) {
            char *dot = strchr(begin, '.');
            if (dot != NULL) *dot = '\0';
            pCtx->PutString("}\n");
            if (dot != NULL) begin = dot + 1;
            else begin = NULL;
        }
        free(pszNamespace);
    }

    return LUBE_OK;
}

class ModuleDeclaration
{
private:
    class NamespaceUnit
    {
    private:
        class DeclarationUnit
        {
        public:
            DeclarationUnit()
                : mType(TYPE_NONE)
                , mInterface(NULL)
                , mNext(NULL)
            {}

            ~DeclarationUnit()
            {
                mInterface = NULL;
                mClass = NULL;
                mNext = NULL;
            }

            void Print(PLUBECTX context)
            {
                switch (mType) {
                    case TYPE_INTERFACE:
                    context->PutString("interface ");
                    context->PutString(mInterface->mName);
                    context->PutString(";\n");
                    context->PutString("EXTERN const _ELASTOS InterfaceID EIID_");
                    context->PutString(mInterface->mName);
                    context->PutString(";\n");
                    break;

                    case TYPE_CLASS:
                    context->PutString("EXTERN const _ELASTOS ClassID ECLSID_");
                    context->PutString(mClass->mName);
                    context->PutString(";\n");
                    break;

                    default:
                    break;
                }
            }

        public:
            int mType;
            InterfaceDirEntry* mInterface;
            ClassDirEntry* mClass;
            DeclarationUnit* mNext;
        };

    public:
        NamespaceUnit()
            : mNameSpace(NULL)
            , mNext(NULL)
            , mDeclaration(NULL)
            , mLastDecl(NULL)
            , mDeclCount(0)
        {}

        ~NamespaceUnit()
        {
            mNameSpace = NULL;
            mNext = NULL;
            mLastDecl = NULL;

            if (mDeclaration == NULL) return;
            DeclarationUnit* declaration = mDeclaration;
            while (declaration != NULL) {
                DeclarationUnit* next = declaration->mNext;
                delete declaration;
                declaration = next;
            }

        }

        void AddInterfaceDeclaration(InterfaceDirEntry* itfDir)
        {
            DeclarationUnit* unit = new DeclarationUnit();
            unit->mType = TYPE_INTERFACE;
            unit->mInterface = itfDir;
            mDeclCount++;

            if (mDeclaration == NULL) {
                mDeclaration = unit;
                mLastDecl = mDeclaration;
                return;
            }

            mLastDecl->mNext = unit;
            mLastDecl = unit;
        }

        void AddClassDeclaration(ClassDirEntry* clsDir)
        {
            DeclarationUnit* unit = new DeclarationUnit();
            unit->mType = TYPE_CLASS;
            unit->mClass = clsDir;
            mDeclCount++;

            if (mDeclaration == NULL) {
                mDeclaration = unit;
                mLastDecl = mDeclaration;
                return;
            }

            mLastDecl->mNext = unit;
            mLastDecl = unit;
        }

        void Print(PLUBECTX context)
        {
            PrintHead(context);

            DeclarationUnit* unit = mDeclaration;
            while (unit != NULL) {
                unit->Print(context);
                unit = unit->mNext;
            }

            PrintEnd(context);
            context->PutString("\n");
        }

    private:
        void PrintHead(PLUBECTX context)
        {
            if (mNameSpace[0] == '\0') return;

            char *nameSpace = (char*)malloc(strlen(mNameSpace) + 1);
            strcpy(nameSpace, mNameSpace);
            char *begin = nameSpace;
            while (begin != NULL) {
                char *dot = strchr(begin, '.');
                if (dot != NULL) *dot = '\0';
                context->PutString("namespace ");
                context->PutString(begin);
                context->PutString(" {\n");
                if (dot != NULL) begin = dot + 1;
                else begin = NULL;
            }
            free(nameSpace);
        }

        void PrintEnd(PLUBECTX context)
        {
            if (mNameSpace[0] == '\0') return;

            char *nameSpace = (char*)malloc(strlen(mNameSpace) + 1);
            strcpy(nameSpace, mNameSpace);
            char *begin = nameSpace;
            while (begin != NULL) {
                char *dot = strchr(begin, '.');
                if (dot != NULL) *dot = '\0';
                context->PutString("}\n");
                if (dot != NULL) begin = dot + 1;
                else begin = NULL;
            }
            free(nameSpace);
        }

    public:
        char* mNameSpace;
        NamespaceUnit* mNext;
        DeclarationUnit* mDeclaration;
        DeclarationUnit* mLastDecl;
        int mDeclCount;
    };

public:
    ModuleDeclaration(PLUBECTX ctx)
        : mContext(ctx)
        , mHead(NULL)
    {}

    ~ModuleDeclaration()
    {
        if (mHead == NULL) return;

        NamespaceUnit* current = mHead;
        while (current != NULL) {
            NamespaceUnit* next = current->mNext;
            delete current;
            current = next;
        }
    }

    void AddInterfaceDeclaration(InterfaceDirEntry* itfDir)
    {
        NamespaceUnit* unit = FindUnit(itfDir->mNameSpace);
        unit->AddInterfaceDeclaration(itfDir);
    }

    void AddClassDeclaration(ClassDirEntry* clsDir)
    {
        NamespaceUnit* unit = FindUnit(clsDir->mNameSpace);
        unit->AddClassDeclaration(clsDir);
    }

    void Print()
    {
        NamespaceUnit* unit = mHead;
        while (unit != NULL) {
            unit->Print(mContext);
            unit = unit->mNext;
        }
    }

private:
    NamespaceUnit* FindUnit(char* nameSpace)
    {
        if (nameSpace == NULL) nameSpace = "";
        if (mHead == NULL) {
            mHead = new NamespaceUnit();
            mHead->mNameSpace = nameSpace;
            return mHead;
        }
        else {
            NamespaceUnit* prev = mHead;
            while (prev != NULL) {
                if (!strcmp(prev->mNameSpace, nameSpace)) return prev;
                if (prev->mNext != NULL) prev = prev->mNext;
                else break;
            }
            prev->mNext = new NamespaceUnit();
            prev = prev->mNext;
            prev->mNameSpace = nameSpace;
            return prev;
        }
    }

public:
    static const int TYPE_NONE = 0;
    static const int TYPE_INTERFACE = 1;
    static const int TYPE_CLASS = 2;

private:
    PLUBECTX mContext;
    NamespaceUnit* mHead;
};

IMPL_USERFUNC(GenerateModuleDeclaration)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pModule && pvArg == pCtx->m_pModule);
    CLSModule* module = pCtx->m_pModule;

    ModuleDeclaration* moduleDecl = new ModuleDeclaration(pCtx);

    //collection interfaces
    for (int i = 0; i < module->mDefinedInterfaceCount; i++) {
        InterfaceDirEntry* itfDir = module->mInterfaceDirs[module->mDefinedInterfaceIndexes[i]];
        moduleDecl->AddInterfaceDeclaration(itfDir);
    }

    //collection classes
    for (int i = 0; i < module->mClassCount; i++) {
        if (!(module->mClassDirs[i]->mDesc->mAttribs & ClassAttrib_t_external) &&
            (module->mClassDirs[i]->mNameSpace == NULL ||
            strcmp("systypes", module->mClassDirs[i]->mNameSpace))) {
            ClassDirEntry* clsDir = module->mClassDirs[i];
            moduleDecl->AddClassDeclaration(clsDir);
        }
    }

    moduleDecl->Print();

    return LUBE_OK;
}

void ConvertToDefine(const char* srcStr, char* destStr)
{
    strcpy(destStr, srcStr);
    char* begin = destStr;
    while (begin != NULL) {
        char* dot = strchr(begin, '.');
        if (dot != NULL) {
            *dot = '_';
            begin = dot + 1;
        }
        else begin = NULL;
    }
}

void ConvertToCPlusPlusNamespace(const char* srcNs, char* destNs)
{
    char *pszNamespace = (char*)malloc(strlen(srcNs) + 1);
    strcpy(pszNamespace, srcNs);
    char *begin = pszNamespace;
    while (begin != NULL) {
        char *dot = strchr(begin, '.');
        if (dot != NULL) *dot = '\0';
        strcat(destNs, begin);
        strcat(destNs, "::");
        if (dot != NULL) begin = dot + 1;
        else begin = NULL;
    }
    free(pszNamespace);
}

void NamespaceBegin(const char* ns, FILE* pFile)
{
    char *pszNamespace = (char*)malloc(strlen(ns) + 1);
    strcpy(pszNamespace, ns);
    char *begin = pszNamespace;
    while (begin != NULL) {
        char *dot = strchr(begin, '.');
        if (dot != NULL) *dot = '\0';
        fprintf(pFile, "namespace %s {\n", begin);
        if (dot != NULL) begin = dot + 1;
        else begin = NULL;
    }
    free(pszNamespace);
}

void NamespaceEnd(const char* ns, FILE* pFile)
{
    char *pszNamespace = (char*)malloc(strlen(ns) + 1);
    strcpy(pszNamespace, ns);
    char *begin = pszNamespace;
    while (begin != NULL) {
        char *dot = strchr(begin, '.');
        if (dot != NULL) *dot = '\0';
        fprintf(pFile, "}\n", begin);
        if (dot != NULL) begin = dot + 1;
        else begin = NULL;
    }
    free(pszNamespace);
}

const char *ParamNamespaceType(ParamDescriptor* pParamDesc, char *szType, CLSModule* pModule)
{
    const char *pszType;
    TypeDescriptor type;
    memset(&type, 0, sizeof(type));

    TypeDescriptor *pType = &pParamDesc->mType;
    DWORD attribs = pParamDesc->mAttribs;

    if ((Type_struct == pType->mType)
            || (Type_EMuid == pType->mType)
            || (Type_EGuid == pType->mType)) {
        if (0 == pType->mPointer) {
            sprintf(szType, "const %s &",
                    Type2CString(pModule, pType));
            pszType = szType;
        }
        else {
            pszType = Type2CString(pModule, pType);
        }
    }
    else if (Type_alias == pType->mType) {
        GetOriginalType(pModule, pType, &type);
        if ((Type_EMuid == type.mType)
                || (Type_EGuid == type.mType)
                || (Type_struct == type.mType)) {
            if (attribs & ParamAttrib_in) {
                if (0 == type.mPointer) {
                    assert(0 == pType->mPointer);
                    sprintf(szType, "const %s &",
                            Type2CString(pModule, pType));
                    pszType = szType;
                }
                else {
                    pszType = Type2CString(pModule, pType);
                }
            }
            else pszType = Type2CString(pModule, pType);
        }
        else if (Type_ArrayOf == type.mType) {
            if (attribs & ParamAttrib_in) {
                if (0 == type.mPointer) {
                    assert(0 == pType->mPointer);
                    sprintf(szType, "const %s &",
                            Type2CString(pModule, pType));
                    pszType = szType;
                }
                else {
                    pszType = Type2CString(pModule, pType);
                }
            }
            else {
                if (attribs & ParamAttrib_callee) {
                    if (0 == pType->mPointer) {
                        assert(1 == type.mPointer);
                        sprintf(szType, "%s *", Type2CString(pModule, pType));
                    }
                    else if (1 == pType->mPointer) {
                        assert(0 == type.mPointer);
                        sprintf(szType, "%s **", Type2CString(pModule, pType));
                    }
                    pszType = szType;
                }
                else {
                    assert(0 == pType->mPointer);
                    assert(0 == type.mPointer);
                    sprintf(szType, "%s *", Type2CString(pModule, pType));
                    pszType = szType;
                }
            }
        }
        else pszType = Type2CString(pModule, pType);
    }
    else if (Type_ArrayOf == pType->mType) {
        if (attribs & ParamAttrib_in) {
            if (0 == pType->mPointer) {
                sprintf(szType, "const ArrayOf<%s> &",
                        Type2CString(pModule, pType->mNestedType));
            }
            else sprintf(szType, "%s", Type2CString(pModule, pType));
        }
        else {
            //attribs == ParamAttrib_out
            sprintf(szType, "ArrayOf<%s>",
                    Type2CString(pModule, pType->mNestedType));
            if (0 == pType->mPointer) strcat(szType, " *");
            else strcat(szType, " **");
        }
        pszType = szType;
    }
    else {
        pszType = Type2CString(pModule, pType);
    }

    return pszType;
}

void OutputInterface(InterfaceDirEntry* pItfDir, CLSModule* pModule)
{
    char *pPath;
    int nRet;
    FILE *pFile;

    if (pItfDir->mFileIndex == 0) {
        pPath = (char*)malloc(strlen(pModule->mName) + 4);
        strcpy(pPath, "_");
        strcat(pPath, pModule->mName);
        strcat(pPath, ".h");
    }
    else {
        pPath = (char*)malloc(strlen(pModule->mFileDirs[pItfDir->mFileIndex]->mPath) + 3);
        strcpy(pPath, pModule->mFileDirs[pItfDir->mFileIndex]->mPath);
        strcat(pPath, ".h");
    }

    char buffer[1024];
    ConvertToDefine(pPath, buffer);

    nRet = access(pPath, 0);
    if (nRet != 0) {
        pFile = fopen(pPath, "w+t");

        if (pItfDir->mFileIndex != 0) {
            fprintf(pFile, "#ifndef __%s__\n", buffer);
            fprintf(pFile, "#define __%s__\n", buffer);
            fprintf(pFile, "\n");
            fprintf(pFile, "#include <_%s>\n", pPath);
            fprintf(pFile, "\n");
            fprintf(pFile, "#endif // __%s__\n", buffer);
            fclose(pFile);

            char *pPath2 = (char*)malloc(strlen(pPath) + 2);
            strcpy(pPath2, "_");
            strcat(pPath2, pPath);
            pFile = fopen(pPath2, "w+t");
            ConvertToDefine(pPath2, buffer);
            free(pPath2);
        }

        fprintf(pFile, "#ifndef __%s__\n", buffer);
        fprintf(pFile, "#define __%s__\n", buffer);
        fprintf(pFile, "\n");
        fprintf(pFile, "#include <%s.h>\n", pModule->mName);
        fprintf(pFile, "\n");
        fprintf(pFile, "EXTERN_C ELAPI _Impl_AcquireCallbackHandler(PInterface pServerObj, _ELASTOS REIID iid, PInterface *ppHandler);\n");
        fprintf(pFile, "EXTERN_C ELAPI _Impl_CheckClsId(PInterface pServerObj, const _ELASTOS ClassID* pClassid, PInterface *ppServerObj);\n");
        fprintf(pFile, "\n");
    }
    else {
        if (pItfDir->mFileIndex != 0) {
            char *pPath2 = (char*)malloc(strlen(pPath) + 2);
            strcpy(pPath2, "_");
            strcat(pPath2, pPath);
            pFile = fopen(pPath2, "r+t");
            ConvertToDefine(pPath2, buffer);
            free(pPath2);
        }
        else {
            pFile = fopen(pPath, "r+t");
        }
        assert(pFile != NULL);
        fseek(pFile, -1 * (strlen(buffer) +  15), SEEK_END);
    }

    //namespace begin
    if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0' &&
        strcmp(pItfDir->mNameSpace, "systypes")) {
        NamespaceBegin(pItfDir->mNameSpace, pFile);
    }

    buffer[0] = '\0';
    InterfaceDirEntry* pParentItfDir = pModule->mInterfaceDirs[pItfDir->mDesc->mParentIndex];
    if (pParentItfDir->mNameSpace != NULL && pParentItfDir->mNameSpace[0] != '\0' &&
        strcmp(pParentItfDir->mNameSpace, "systypes")) {
        ConvertToCPlusPlusNamespace(pParentItfDir->mNameSpace, buffer);
    }

    fprintf(pFile, "CAR_INTERFACE(\"%s\")\n", Uuid2IString(&pItfDir->mDesc->mIID, TRUE));
    fprintf(pFile, "%s : public %s%s\n", pItfDir->mName, buffer, pParentItfDir->mName);
    fprintf(pFile, "{\n");

    //interface const
    for (int i = 0; i < pItfDir->mDesc->mConstCount; i++) {
        InterfaceConstDescriptor* pItfConst = pItfDir->mDesc->mConsts[i];
        switch (pItfConst->mType) {
            case TYPE_BOOLEAN:
                fprintf(pFile, "    static const _ELASTOS Boolean %s;\n", pItfConst->mName);
                break;
            case TYPE_CHAR32:
                fprintf(pFile, "    static const _ELASTOS Char32 %s = ", pItfConst->mName);
                if (pItfConst->mV.mInt32Value.mFormat == FORMAT_DECIMAL) {
                    fprintf(pFile, "%d;\n", pItfConst->mV.mInt32Value.mValue);
                }
                else if (pItfConst->mV.mInt32Value.mFormat == FORMAT_HEX) {
                    fprintf(pFile, "0x%08x;\n", pItfConst->mV.mInt32Value.mValue);
                }
                else {
                    switch(pItfConst->mV.mInt32Value.mValue) {
                        case '\a' :
                            fprintf(pFile, "\'\\a\';\n");
                            break;
                        case 'b' :
                            fprintf(pFile, "\'\\b\';\n");
                            break;
                        case 'f' :
                            fprintf(pFile, "\'\\f\';\n");
                            break;
                        case 'n' :
                            fprintf(pFile, "\'\\n\';\n");
                            break;
                        case 'r' :
                            fprintf(pFile, "\'\\r\';\n");
                            break;
                        case 't' :
                            fprintf(pFile, "\'\\t\';\n");
                            break;
                        case 'v' :
                            fprintf(pFile, "\'\\v\';\n");
                            break;
                        case '\\':
                            fprintf(pFile, "\'\\\\\';\n");
                            break;
                        case '\'':
                            fprintf(pFile, "\'\\\'\';\n");
                            break;
                        case '\"':
                            fprintf(pFile, "\'\\\"\';\n");
                            break;
                        case '0' :
                            fprintf(pFile, "\'\\0\';\n");
                            break;
                        default:
                            fprintf(pFile, "\'%c\';\n", pItfConst->mV.mInt32Value.mValue);
                    }
                }
                break;
            case TYPE_BYTE:
                fprintf(pFile, "    static const _ELASTOS Byte %s = ", pItfConst->mName);
                fprintf(pFile, pItfConst->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%02x;\n":"%d;\n",
                        (unsigned char)pItfConst->mV.mInt32Value.mValue & 0xff);
                break;
            case TYPE_INTEGER16:
                fprintf(pFile, "    static const _ELASTOS Int16 %s = ", pItfConst->mName);
                fprintf(pFile, pItfConst->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%04x;\n":"%d;\n",
                        (short)pItfConst->mV.mInt32Value.mValue & 0xffff);
                break;
            case TYPE_INTEGER32:
                fprintf(pFile, "    static const _ELASTOS Int32 %s = ", pItfConst->mName);
                fprintf(pFile, pItfConst->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%08x;\n":"%d;\n",
                        pItfConst->mV.mInt32Value.mValue);
                break;
            case TYPE_INTEGER64:
                fprintf(pFile, "    static const _ELASTOS Int64 %s;\n", pItfConst->mName);
                break;
            case TYPE_FLOAT:
                fprintf(pFile, "    static const _ELASTOS Float %s;\n", pItfConst->mName);
                break;
            case TYPE_DOUBLE:
                fprintf(pFile, "    static const _ELASTOS Double %s;\n", pItfConst->mName);
                break;
            case TYPE_STRING:
                fprintf(pFile, "    static const _ELASTOS String %s;\n", pItfConst->mName);
                break;
            default:
                assert(0);
                break;
        }
    }

    //method
    fprintf(pFile, "    virtual CARAPI_(PInterface) Probe(\n");
    fprintf(pFile, "        /* [in] */ _ELASTOS REIID riid) = 0;\n");
    fprintf(pFile, "\n");
    fprintf(pFile, "    static CARAPI_(%s*) Probe(PInterface pObj)\n", pItfDir->mName);
    fprintf(pFile, "    {\n");
    fprintf(pFile, "        if (pObj == NULL) return NULL;\n");
    fprintf(pFile, "        return (%s*)pObj->Probe(EIID_%s);\n", pItfDir->mName, pItfDir->mName);
    fprintf(pFile, "    }\n");
    fprintf(pFile, "\n");
    fprintf(pFile, "    static CARAPI_(%s*) Probe(IObject* pObj)\n", pItfDir->mName);
    fprintf(pFile, "    {\n");
    fprintf(pFile, "        if (pObj == NULL) return NULL;\n");
    fprintf(pFile, "        return (%s*)pObj->Probe(EIID_%s);\n", pItfDir->mName, pItfDir->mName);
    fprintf(pFile, "    }\n");
    fprintf(pFile, "\n");

    for (int i = 0; i < pItfDir->mDesc->mMethodCount; i++) {
        MethodDescriptor* pMethod = pItfDir->mDesc->mMethods[i];
        fprintf(pFile, "    virtual CARAPI %s(", pMethod->mName);
        for (int j = 0; j < pMethod->mParamCount; j++) {
            ParamDescriptor* pParamDesc = pMethod->mParams[j];
            char szType[128];
            const char *pszType = ParamNamespaceType(pParamDesc, szType, pModule);

            fprintf(pFile, "\n        /* [%s] */ %s %s",
                    ParamAttrib2String(pParamDesc->mAttribs),
                    ImplNamespaceType(pszType),
                    pParamDesc->mName);
            if (j != pMethod->mParamCount - 1) fprintf(pFile, ",");
        }
        fprintf(pFile, ") = 0;\n");
        fprintf(pFile, "\n");
    }

    fprintf(pFile, "};\n");

    //namespace end
    if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0' &&
        strcmp(pItfDir->mNameSpace, "systypes")) {
        NamespaceEnd(pItfDir->mNameSpace, pFile);
    }

    ConvertToDefine(pPath, buffer);
    fprintf(pFile, "\n");
    fprintf(pFile, "#endif // __%s__\n", buffer);

    fclose(pFile);

    free(pPath);
}

IMPL_USERFUNC(GenerateModuleImplementation)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pModule && pvArg == pCtx->m_pModule);
    CLSModule* module = pCtx->m_pModule;

    //handle interfaces
    for (int i = 0; i < module->mDefinedInterfaceCount; i++) {
        InterfaceDirEntry* itfDir = module->mInterfaceDirs[module->mDefinedInterfaceIndexes[i]];
        OutputInterface(itfDir, module);
    }

    return LUBE_OK;
}

void OutputClass(ClassDirEntry* pClsDir, CLSModule* pModule)
{
    char *pPath;
    int nRet;
    FILE *pFile;

    if (pClsDir->mDesc->mAttribs & ClassAttrib_t_clsobj) return;

    if (pClsDir->mFileIndex == 0) {
        pPath = (char*)malloc(strlen(pModule->mName) + 4);
        strcpy(pPath, "_");
        strcat(pPath, pModule->mName);
        strcat(pPath, ".");
        strcat(pPath, "h");
    }
    else {
        pPath = (char*)malloc(strlen(pModule->mFileDirs[pClsDir->mFileIndex]->mPath) + 3);
        strcpy(pPath, pModule->mFileDirs[pClsDir->mFileIndex]->mPath);
        strcat(pPath, ".h");
    }

    char buffer[1024];
    ConvertToDefine(pPath, buffer);

    nRet = access(pPath, 0);
    if (nRet != 0) {
        pFile = fopen(pPath, "w+t");

        if (pClsDir->mFileIndex != 0) {
            fprintf(pFile, "#ifndef __%s__\n", buffer);
            fprintf(pFile, "#define __%s__\n", buffer);
            fprintf(pFile, "\n");
            fprintf(pFile, "#include <_%s>\n", pPath);
            fprintf(pFile, "\n");
            fprintf(pFile, "#endif // __%s__\n", buffer);
            fclose(pFile);

            char *pPath2 = (char*)malloc(strlen(pPath) + 2);
            strcpy(pPath2, "_");
            strcat(pPath2, pPath);
            pFile = fopen(pPath2, "w+t");
            ConvertToDefine(pPath2, buffer);
            free(pPath2);
        }

        fprintf(pFile, "#ifndef __%s__", buffer);
        fprintf(pFile, "\n");
        fprintf(pFile, "#include <%s.h>\n", pModule->mName);
        fprintf(pFile, "EXTERN_C ELAPI _Impl_CheckClsId(PInterface pServerObj, const _ELASTOS ClassID* pClassid, PInterface *ppServerObj);\n");
        fprintf(pFile, "\n");
    }
    else {
        if (pClsDir->mFileIndex != 0) {
            char *pPath2 = (char*)malloc(strlen(pPath) + 2);
            strcpy(pPath2, "_");
            strcat(pPath2, pPath);
            pFile = fopen(pPath2, "r+t");
            ConvertToDefine(pPath2, buffer);
            free(pPath2);
        }
        else {
            pFile = fopen(pPath, "r+t");
        }
        assert(pFile != NULL);
        fseek(pFile, -1 * (strlen(buffer) +  15), SEEK_END);
    }

    //namespace begin
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0' &&
        strcmp(pClsDir->mNameSpace, "systypes")) {
        NamespaceBegin(pClsDir->mNameSpace, pFile);
    }

    fprintf(pFile, "class %s\n", pClsDir->mName);
    fprintf(pFile, "{\n");
    fprintf(pFile, "public:\n");

    if (pClsDir->mDesc->mAttribs & ClassAttrib_hasctor) {
        InterfaceDirEntry* pCtorInterface = pModule->mInterfaceDirs[pClsDir->mDesc->mCtorIndex];
        for (int i = 0; i < pCtorInterface->mDesc->mMethodCount; i++) {
            MethodDescriptor* pMethod = pCtorInterface->mDesc->mMethods[i];
            if (pMethod->mAttribs & MethodAttrib_NonDefaultCtor) {
                if (pClsDir->mDesc->mAttribs & ClassAttrib_singleton) {
                    fprintf(pFile, "    static _ELASTOS ECode _AcquireSingleton(\n");
                }
                else {
                    fprintf(pFile, "    static _ELASTOS ECode _New(\n");
                }
                for (int j = 0; j < pMethod->mParamCount; j++) {
                    ParamDescriptor* pParamDesc = pMethod->mParams[j];
                    if (j < pMethod->mParamCount - 1) {
                        char szType[128];
                        const char *pszType = ParamNamespaceType(pParamDesc, szType, pModule);

                        fprintf(pFile, "        /* [%s] */ %s %s,\n",
                                ParamAttrib2String(pParamDesc->mAttribs),
                                ImplNamespaceType(pszType),
                                pParamDesc->mName);
                    }
                    else {
                        fprintf(pFile, "        /* [out] */ IInterface** __object)\n");
                    }
                }
                fprintf(pFile, "    {\n");
                fprintf(pFile, "        _ELASTOS ECode ec;\n");
                fprintf(pFile, "        I%sClassObject* pClassObject;\n", pClsDir->mName);
                fprintf(pFile, "        PInterface pObject = NULL;\n");
                fprintf(pFile, "\n");
                fprintf(pFile, "        ec = _CObject_AcquireClassFactory(ECLSID_%s, RGM_SAME_DOMAIN,\n", pClsDir->mName);
                fprintf(pFile, "                EIID_I%sClassObject, (IInterface**)&pClassObject);\n", pClsDir->mName);
                fprintf(pFile, "        if (FAILED(ec)) return ec;\n");
                fprintf(pFile, "\n");
                fprintf(pFile, "        ec = pClassObject->%s(", pMethod->mName);
                for (int j = 0; j < pMethod->mParamCount; j++) {
                    ParamDescriptor* pParamDesc = pMethod->mParams[j];
                    if (j < pMethod->mParamCount - 1) {
                        fprintf(pFile, "%s, ", pParamDesc->mName);
                    }
                    else {
                        fprintf(pFile, "&pObject);\n");
                    }
                }
                fprintf(pFile, "\n");
                fprintf(pFile, "        *__object = pObject;\n");
                fprintf(pFile, "\n");
                fprintf(pFile, "        pClassObject->Release();\n");
                fprintf(pFile, "\n");
                fprintf(pFile, "        return ec;\n");
                fprintf(pFile, "    }\n");
                fprintf(pFile, "\n");
            }
        }
    }

    if (!(pClsDir->mDesc->mAttribs & ClassAttrib_hasctor) ||
            (pClsDir->mDesc->mAttribs & ClassAttrib_hasDefaultCtor)) {
        for (int i = 0; i < pClsDir->mDesc->mInterfaceCount; i++) {
            InterfaceDirEntry* pItfDir = pModule->mInterfaceDirs[pClsDir->mDesc->mInterfaces[i]->mIndex];
            if (!(pItfDir->mDesc->mAttribs & InterfaceAttrib_clsobj)) {
                if (pClsDir->mDesc->mAttribs & ClassAttrib_singleton) {
                    fprintf(pFile, "    static _ELASTOS ECode AcquireSingleton(\n");
                }
                else {
                    fprintf(pFile, "    static _ELASTOS ECode New(\n");
                }

                buffer[0] = '\0';
                if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0' &&
                    strcmp(pItfDir->mNameSpace, "systypes")) {
                    ConvertToCPlusPlusNamespace(pItfDir->mNameSpace, buffer);
                }

                fprintf(pFile, "        /* [out] */ %s%s** __object)\n", buffer, pItfDir->mName);
                fprintf(pFile, "    {\n");
                fprintf(pFile, "        return _CObject_CreateInstance(ECLSID_%s, RGM_SAME_DOMAIN, %sEIID_%s, (PInterface*)__object);\n",
                        pClsDir->mName, buffer, pItfDir->mName);
                fprintf(pFile, "    }\n");
                fprintf(pFile, "\n");
            }
        }
    }

    if (pClsDir->mDesc->mAttribs & ClassAttrib_hasctor) {
        InterfaceDirEntry* pCtorInterface = pModule->mInterfaceDirs[pClsDir->mDesc->mCtorIndex];
        for (int i = 0; i < pCtorInterface->mDesc->mMethodCount; i++) {
            MethodDescriptor* pMethod = pCtorInterface->mDesc->mMethods[i];
            if (pMethod->mAttribs & MethodAttrib_NonDefaultCtor) {
                for (int i = 0; i < pClsDir->mDesc->mInterfaceCount; i++) {
                    InterfaceDirEntry* pItfDir = pModule->mInterfaceDirs[pClsDir->mDesc->mInterfaces[i]->mIndex];
                    if (!(pItfDir->mDesc->mAttribs & InterfaceAttrib_clsobj)) {
                        if (pClsDir->mDesc->mAttribs & ClassAttrib_singleton) {
                            fprintf(pFile, "    static _ELASTOS ECode AcquireSingleton(\n");
                        }
                        else {
                            fprintf(pFile, "    static _ELASTOS ECode New(\n");
                        }
                        for (int j = 0; j < pMethod->mParamCount; j++) {
                            ParamDescriptor* pParamDesc = pMethod->mParams[j];
                            if (j < pMethod->mParamCount - 1) {
                                char szType[128];
                                const char *pszType = ParamNamespaceType(pParamDesc, szType, pModule);

                                fprintf(pFile, "        /* [%s] */ %s %s,\n",
                                        ParamAttrib2String(pParamDesc->mAttribs),
                                        ImplNamespaceType(pszType),
                                        pParamDesc->mName);
                            }
                            else {
                                buffer[0] = '\0';
                                if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0' &&
                                    strcmp(pItfDir->mNameSpace, "systypes")) {
                                    ConvertToCPlusPlusNamespace(pItfDir->mNameSpace, buffer);
                                }

                                fprintf(pFile, "        /* [out] */ %s%s** __%s)\n",
                                        buffer, pItfDir->mName, pItfDir->mName);
                            }
                        }
                        fprintf(pFile, "    {\n");
                        fprintf(pFile, "        IInterface* __pNewObj = NULL;\n");
                        fprintf(pFile, "\n");
                        fprintf(pFile, "        _ELASTOS ECode ec = ");
                        if (pClsDir->mDesc->mAttribs & ClassAttrib_singleton) {
                            fprintf(pFile, "_AcquireSingleton(");
                        }
                        else {
                            fprintf(pFile, "_New(");
                        }
                        for (int j = 0; j < pMethod->mParamCount; j++) {
                            ParamDescriptor* pParamDesc = pMethod->mParams[j];
                            if (j < pMethod->mParamCount - 1) {
                                fprintf(pFile, "%s, ", pParamDesc->mName);
                            }
                            else {
                                fprintf(pFile, "&__pNewObj);\n");
                            }
                        }
                        fprintf(pFile, "        if (FAILED(ec)) return ec;\n");
                        fprintf(pFile, "\n");

                        buffer[0] = '\0';
                        if (pItfDir->mNameSpace != NULL && pItfDir->mNameSpace[0] != '\0' &&
                            strcmp(pItfDir->mNameSpace, "systypes")) {
                            ConvertToCPlusPlusNamespace(pItfDir->mNameSpace, buffer);
                        }

                        fprintf(pFile, "        *__%s = (%s%s*)__pNewObj->Probe(%sEIID_%s);\n",
                                pItfDir->mName, buffer, pItfDir->mName, buffer, pItfDir->mName);
                        fprintf(pFile, "        if (*__%s) __pNewObj->AddRef();\n",
                                pItfDir->mName, pItfDir->mName);
                        fprintf(pFile, "        else ec = E_NO_INTERFACE;\n");
                        fprintf(pFile, "        __pNewObj->Release();\n");
                        fprintf(pFile, "\n");
                        fprintf(pFile, "        return ec;\n");
                        fprintf(pFile, "    }\n");
                        fprintf(pFile, "\n");
                    }
                }
            }
        }
    }

    fprintf(pFile, "};\n");

    //namespace end
    if (pClsDir->mNameSpace != NULL && pClsDir->mNameSpace[0] != '\0' &&
        strcmp(pClsDir->mNameSpace, "systypes")) {
        NamespaceEnd(pClsDir->mNameSpace, pFile);
    }

    ConvertToDefine(pPath, buffer);
    fprintf(pFile, "\n");
    fprintf(pFile, "#endif // __%s__\n", buffer);

    fclose(pFile);

    free(pPath);
}

IMPL_USERFUNC(GenerateModuleImplementation2)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pModule && pvArg == pCtx->m_pModule);
    CLSModule* module = pCtx->m_pModule;

    //handle interfaces
    for (int i = 0; i < module->mDefinedInterfaceCount; i++) {
        InterfaceDirEntry* itfDir = module->mInterfaceDirs[module->mDefinedInterfaceIndexes[i]];
        OutputInterface(itfDir, module);
    }

    //handle classes
    for (int i = 0; i < module->mClassCount; i++) {
        ClassDirEntry* clsDir = module->mClassDirs[i];
        OutputClass(clsDir, module);
    }

    return LUBE_OK;
}

class HeaderFile
{
public:
    HeaderFile()
        : mName(NULL)
        , mNext(NULL)
    {}

    ~HeaderFile()
    {
        free(mName);
        mNext = NULL;
    }

    void Clear()
    {
        HeaderFile* current = mNext;
        HeaderFile* next = NULL;
        while (current != NULL) {
            next = current->mNext;
            delete current;
            current = next;
        }
        mNext = NULL;
    }

    void AddHeader(char* file)
    {
        if (mNext == NULL) {
            mNext = new HeaderFile(file);
            return;
        }

        HeaderFile* current = mNext;
        while (true) {
            if (!strcmp(current->mName, file)) return;
            if (current->mNext == NULL) break;
            current = current->mNext;
        }

        current->mNext = new HeaderFile(file);
    }

    void Print(PLUBECTX pCtx)
    {
        HeaderFile* current = mNext;
        while (current != NULL) {
            current->PrintHeader(pCtx);
            current = current->mNext;
        }
    }

    void PrintHeader(PLUBECTX pCtx)
    {
        pCtx->PutString(mName);
    }

private:
    HeaderFile(char* file)
        : mNext(NULL)
    {
        mName = (char*)malloc(strlen(file) + 1);
        strcpy(mName, file);
    }

private:
    char* mName;
    HeaderFile* mNext;
};

static HeaderFile sHeaders;

IMPL_USERFUNC(GenerateDependHeaderForClass)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pClass && pvArg == pCtx->m_pClass);

    sHeaders.Clear();

    ClassDirEntry *pClsDir = pCtx->m_pClass;
    if (pClsDir->mFileIndex != 0) {
        char buffer[512];
        strcpy(buffer, "#include \"");
        strcat(buffer, pCtx->m_pModule->mFileDirs[pClsDir->mFileIndex]->mPath);
        strcat(buffer, ".h\"\n");
        sHeaders.AddHeader(buffer);
    }
    else {
        char buffer[512];
        strcpy(buffer, "#include \"_");
        strcat(buffer, pCtx->m_pModule->mName);
        strcat(buffer, ".h\"\n");
        sHeaders.AddHeader(buffer);
    }
    for (int i = 0; i < pClsDir->mDesc->mInterfaceCount; i++) {
        ClassInterface *pClsIntf = pClsDir->mDesc->mInterfaces[i];
        InterfaceDirEntry *pItfDir = pCtx->m_pModule->mInterfaceDirs[pClsIntf->mIndex];
        if (pItfDir->mFileIndex != 0) {
            char buffer[512];
            strcpy(buffer, "#include \"");
            strcat(buffer, pCtx->m_pModule->mFileDirs[pItfDir->mFileIndex]->mPath);
            strcat(buffer, ".h\"\n");
            sHeaders.AddHeader(buffer);
        }
    }

    sHeaders.Print(pCtx);

    return LUBE_OK;
}

IMPL_USERFUNC(GenerateDependHeaderForModule)(PLUBECTX pCtx, PSTATEDESC pDesc, PVOID pvArg)
{
    assert(NULL != pCtx->m_pModule && pvArg == pCtx->m_pModule);
    CLSModule* module = pCtx->m_pModule;

    for (int i = 1; i < module->mFileCount; i++) {
        pCtx->PutString("#include \"");
        pCtx->PutString(module->mFileDirs[i]->mPath);
        pCtx->PutString(".h\"\n");
    }
    for (int i = 0; i < module->mDefinedInterfaceCount; i++) {
        InterfaceDirEntry* itfDir = module->mInterfaceDirs[module->mDefinedInterfaceIndexes[i]];
        if (itfDir->mFileIndex == 0) {
            char* pPath = (char*)malloc(strlen(module->mName) + 4);
            strcpy(pPath, "_");
            strcat(pPath, module->mName);
            strcat(pPath, ".");
            strcat(pPath, "h");

            pCtx->PutString("#include \"");
            pCtx->PutString(pPath);
            pCtx->PutString("\"\n");
            free(pPath);
            return LUBE_OK;
        }
    }
    for (int i = 0; i < module->mClassCount; i++) {
        ClassDirEntry* clsDir = module->mClassDirs[i];
        if (clsDir->mFileIndex == 0) {
            char* pPath = (char*)malloc(strlen(module->mName) + 4);
            strcpy(pPath, "_");
            strcat(pPath, module->mName);
            strcat(pPath, ".");
            strcat(pPath, "h");

            pCtx->PutString("#include \"");
            pCtx->PutString(pPath);
            pCtx->PutString("\"\n");
            free(pPath);
            return LUBE_OK;
        }
    }

    return LUBE_OK;
}
