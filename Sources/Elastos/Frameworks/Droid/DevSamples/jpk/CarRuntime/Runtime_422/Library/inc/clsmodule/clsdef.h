//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CLSDEF_H__
#define __CLSDEF_H__

#include <string.h>
#include "clstype.h"

const char * const c_szMagic = "CAR ClassInfo\r\n\x1a";
const int c_nMagicSize = 16;
const unsigned short c_sMajorVersion = 2;
const unsigned short c_sMinorVersion = 0;
const int c_nCLSModuleVersion = 1;

typedef struct ClassDirEntry ClassDirEntry;
typedef struct InterfaceDirEntry InterfaceDirEntry;
typedef struct StructDirEntry StructDirEntry;
typedef struct EnumDirEntry EnumDirEntry;
typedef struct AliasDirEntry AliasDirEntry;
typedef struct ArrayDirEntry ArrayDirEntry;
typedef struct ConstDirEntry ConstDirEntry;

const int c_nMaxClassNumber = 4096;
const int c_nMaxInterfaceNumber = 4096;
const int c_nMaxDefinedInterfaceNumber = 4096;
const int c_nMaxStructNumber = 4096;
const int c_nMaxEnumNumber = 4096;
const int c_nMaxAliasNumber = 4096;
const int c_nMaxLibNumber = 32;
const int c_nMaxArrayNumber = 4096;
const int c_nMaxConstNumber = 4096;

const unsigned int c_nStructMaxAlignSize = 4;

typedef struct CLSModule
{
    char                szMagic[c_nMagicSize];
    unsigned char       cMajorVersion; // Major version of given version(*.*)
    unsigned char       cMinorVersion; // Minor version
    int                 nCLSModuleVersion; //version of this CLSModule struct
    int                 nSize;
    unsigned int        nChecksum;
    unsigned int        nBarcode;

    char                *pszUunm;
    GUID                uuid;
    DWORD               dwAttribs;
    char                *pszName;
    char                *pszServiceName;

    unsigned short      cClasses;
    unsigned short      cInterfaces;
    unsigned short      cDefinedInterfaces;
    unsigned short      cStructs;
    unsigned short      cEnums;
    unsigned short      cAliases;
    unsigned short      cLibraries;
    unsigned short      cArrays;
    unsigned short      cConsts;

    ClassDirEntry       **ppClassDir;
    InterfaceDirEntry   **ppInterfaceDir;
    int                 *pDefinedInterfaceIndex;
    StructDirEntry      **ppStructDir;
    EnumDirEntry        **ppEnumDir;
    AliasDirEntry       **ppAliasDir;
    char                **ppLibNames;
    ArrayDirEntry       **ppArrayDir;
    ConstDirEntry       **ppConstDir;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
} CLSModule;

typedef struct TypeDescriptor
{
    CARDataType         type;
    unsigned short      sIndex;
    unsigned char       nPointer;
    unsigned char       bUnsigned;
    int                 nSize;          //n value in the StringBuf_<n> or Char8Array_<n> etc
    unsigned char       bNested;
    TypeDescriptor      *pNestedType;   // just for EzArry and EzEnum
} TypeDescriptor;

typedef struct ClassDescriptor ClassDescriptor;
typedef struct ClassInterface ClassInterface;

struct ClassDirEntry
{
    char                *pszName;
    char                *pszNameSpace;
    ClassDescriptor     *pDesc;
};

const char c_nMaxClassInterfaces = 32;
const char c_nMaxClassAspects = 32;
const char c_nMaxAggrClassesOfAspect = 32;

struct ClassDescriptor
{
    CLSID               clsid;
    unsigned short      sParentIndex;
    unsigned short      sCtorIndex;
    unsigned short      cInterfaces;
    unsigned short      cAggregates;
    unsigned short      cClasses;               // only for aspect
    unsigned short      cAspects;
    unsigned long       dwAttribs;

    ClassInterface      **ppInterfaces;
    unsigned short      *pAggrIndexs;
    unsigned short      *pAspectIndexs;
    unsigned short      *pClassIndexs;          // only for aspect

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct ClassInterface
{
    unsigned short      sIndex;
    unsigned short      wAttribs;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

typedef struct InterfaceDescriptor InterfaceDescriptor;
typedef struct InterfaceConstDescriptor InterfaceConstDescriptor;
typedef struct MethodDescriptor MethodDescriptor;
typedef struct ParamDescriptor ParamDescriptor;

const int c_nMaxInterfaceConsts = 512;
const int c_nMaxMethods = 512;

struct InterfaceDirEntry
{
    char                *pszName;
    char                *pszNameSpace;
    InterfaceDescriptor *pDesc;
};

struct InterfaceDescriptor
{
    IID                 iid;
    unsigned short      cConsts;
    unsigned short      cMethods;
    unsigned short      sParentIndex;
    DWORD               dwAttribs;

    InterfaceConstDescriptor **ppConsts;
    MethodDescriptor    **ppMethods;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

const int c_nMaxParams = 32;

struct MethodDescriptor
{
    char                *pszName;
    TypeDescriptor      type;
    unsigned short      cParams;
    ParamDescriptor     **ppParams;
    DWORD               dwAttribs;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct ParamDescriptor
{
    char                *pszName;
    TypeDescriptor      type;
    DWORD               dwAttribs;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

typedef struct StructDescriptor StructDescriptor;
typedef struct StructElement StructElement;

const int c_nMaxStructElements = 128;

struct StructDirEntry
{
    char                *pszName;
    char                *pszNameSpace;
    StructDescriptor    *pDesc;
};

struct StructDescriptor
{
    unsigned short      cElems;
    StructElement       **ppElems;
    unsigned int        nAlignSize;
    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct StructElement
{
    char                *pszName;
    TypeDescriptor      type;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

typedef struct EnumDescriptor EnumDescriptor;
typedef struct EnumElement EnumElement;

const int c_nMaxEnumElements = 512;

struct EnumDirEntry
{
    char                *pszName;
    char                *pszNameSpace;
    EnumDescriptor      *pDesc;
};

struct EnumDescriptor
{
    unsigned short      cElems;
    EnumElement         **ppElems;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct EnumElement
{
    char                *pszName;
    int                 nValue;
    BOOL                bHexFormat;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct AliasDirEntry
{
    char                *pszName;
    char                *pszNameSpace;

    TypeDescriptor      type;
    BOOL                bDummyType;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct ArrayDirEntry
{
    char                *pszNameSpace;
    unsigned short      nElements;
    TypeDescriptor      type;
};

#define TYPE_BOOLEAN 0
#define TYPE_CHAR32 1
#define TYPE_BYTE 2
#define TYPE_INTEGER16 3
#define TYPE_INTEGER32 4
#define TYPE_INTEGER64 5
#define TYPE_FLOAT 6
#define TYPE_DOUBLE 7
#define TYPE_STRING 8

#define FORMAT_DECIMAL 1
#define FORMAT_HEX 2

struct ConstDirEntry
{
    char                *pszName;
    char                *pszNameSpace;
    unsigned char       type;     // type == 0 integer; type == 1 string;
    union
    {
        struct {
            int         nValue;
            BOOL        bHexFormat;
        }               intValue;
        struct {
            char        *pszValue;
        }               strValue;
    }                   v;

    union
    {
        int             nReserved;
        void            *pvBindData;
    } r;
};

struct InterfaceConstDescriptor
{
    char                *pszName;
    unsigned char      type;
    union
    {
        struct
        {
            int         nValue;
            unsigned char format;
        }                int32Value;
        struct
        {
            long long int nValue;
            unsigned char format;
        }                int64Value;
        BOOL            bValue;
        double          dValue;
        char            *pStrValue;
    }                    v;
};

inline BOOL IsValidUUID(const GUID *pGUID)
{
    return memcmp(pGUID,
        "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0", sizeof(GUID));
}

#define IsNullUUID(uuid)    !IsValidUUID(uuid)

inline BOOL IsEqualUUID(const GUID *pSrc, const GUID *pDest)
{
    return !memcmp(pSrc, pDest, sizeof(GUID));
}

typedef enum CLSError
{
    CLS_NoError                 = 0,
    CLSError_DupEntry           = -1,
    CLSError_FullEntry          = -2,
    CLSError_OutOfMemory        = -3,
    CLSError_NameConflict       = -4,
    CLSError_NotFound           = -5,
    CLSError_FormatSize         = -6,
    CLSError_FormatMagic        = -7,
    CLSError_LoadResource       = -8,
    CLSError_OpenFile           = -9,
    CLSError_UnknownFileType    = -10,
    CLSError_StringTooLong      = -11,
    CLSError_Compress           = -12,
    CLSError_Uncompress         = -13,
    CLSError_CLSModuleVersion   = -14,
}   CLSError;

extern int CLSLastError();
extern const char *CLSLastErrorMessage();

extern CLSModule *CreateCLS();
extern void DestroyCLS(CLSModule *);

extern int IsValidCLS(CLSModule *, int, const char *);

extern int CreateClassDirEntry(const char *, CLSModule *, unsigned long);
extern int CreateInterfaceDirEntry(const char *, CLSModule *, unsigned long);
extern int CreateStructDirEntry(const char *, CLSModule *);
extern int CreateEnumDirEntry(const char *, CLSModule *);
extern int CreateAliasDirEntry(const char *, CLSModule *, TypeDescriptor *);
extern int CreateArrayDirEntry(CLSModule *);
extern int CreateConstDirEntry(const char *, CLSModule *);

extern int CreateClassInterface(unsigned short, ClassDescriptor *);
extern int CreateInterfaceConstDirEntry(const char *, InterfaceDescriptor *);
extern int CreateInterfaceMethod(const char *, InterfaceDescriptor *);
extern int CreateMethodParam(const char *, MethodDescriptor *);
extern int CreateStructElement(const char *, StructDescriptor *);
extern int CreateEnumElement(const char *, CLSModule *, EnumDescriptor *);

extern int SelectClassDirEntry(const char *, const char *, const CLSModule *);
extern int SelectInterfaceDirEntry(const char *, const char *, const CLSModule *);
extern int SelectStructDirEntry(const char *, const CLSModule *);
extern int SelectEnumDirEntry(const char *, const char *, const CLSModule *);
extern int SelectAliasDirEntry(const char *, const CLSModule *);
extern int SelectArrayDirEntry(unsigned short nElems,
    const TypeDescriptor &desp, const CLSModule *pModule);
extern int SelectConstDirEntry(const char *, const CLSModule *);

extern int SelectClassInterface(unsigned short, const ClassDescriptor *);
extern int SelectInterfaceConstDirEntry(const char *, const InterfaceDescriptor *);
extern int SelectInterfaceMethod(const char *, const InterfaceDescriptor *);
extern int SelectMethodParam(const char *, const MethodDescriptor *);
extern int SelectStructElement(const char *, const StructDescriptor *);
extern int SelectEnumElement(const char *, const EnumDescriptor *);

typedef enum GlobalSymbolType
{
    GType_None              = 0,
    GType_Class             = Type_Char8,
    GType_Interface         = Type_interface,
    GType_Struct            = Type_struct,
    GType_Enum              = Type_enum,
    GType_Alias             = Type_alias,
    GType_Const             = Type_const,
}   GlobalObjectType;

extern int GlobalSelectSymbol(const char *, const char *,
                const CLSModule *, GlobalSymbolType, GlobalSymbolType *);
extern EnumElement *GlobalSelectEnumElement(const char *, const CLSModule *);
extern int SelectInterfaceMemberSymbol(const char *, InterfaceDescriptor *);

extern int GetIdentifyType(const char *, const char*, const CLSModule *, TypeDescriptor *);
int GetArrayBaseType(const CLSModule *pModule,
    const TypeDescriptor *pSrc, TypeDescriptor *pDest);
extern int GetOriginalType(const CLSModule *,
                const TypeDescriptor *, TypeDescriptor *);
extern BOOL IsEqualType(const CLSModule *,
                const TypeDescriptor *, const TypeDescriptor *);
EXTERN_C BOOL IsLocalCarQuintet(const CLSModule *pModule,
    const TypeDescriptor *pType, DWORD dwAttribs);
EXTERN_C BOOL IsLocalStruct(const CLSModule *pModule, StructDescriptor *pDesc);

extern int TypeCopy(const TypeDescriptor *, TypeDescriptor *);
EXTERN_C int TypeCopy(const CLSModule *, const TypeDescriptor *,
                CLSModule *, TypeDescriptor *, BOOL);
EXTERN_C int ArrayCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int AliasCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int EnumCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int StructCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int InterfaceCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int ClassCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int ConstCopy(const CLSModule *, int, CLSModule *, BOOL);

EXTERN_C int TypeXCopy(const CLSModule *, const TypeDescriptor *,
                CLSModule *, TypeDescriptor *, BOOL);
EXTERN_C int ArrayXCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int AliasXCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int StructXCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int InterfaceXCopy(const CLSModule *, int, CLSModule *, BOOL);
EXTERN_C int ClassXCopy(const CLSModule *, int, CLSModule *, BOOL);

extern int InterfaceMethodsAppend(const CLSModule *,
                const InterfaceDescriptor *, InterfaceDescriptor *);
extern int MethodAppend(const MethodDescriptor *, InterfaceDescriptor *);

const int c_nMaxSeedSize = 255;
EXTERN_C int GuidFromSeedString(const char *, GUID *);
EXTERN_C int SeedStringFromGuid(REFGUID, char *);

extern int FlatCLS(const CLSModule *, void **);
extern int DisposeFlattedCLS(void *);
extern int RelocFlattedCLS(const void *, int, CLSModule **);

extern int CompressCLS(void *);
extern int UncompressCLS(const void *, int, CLSModule *);

extern void SetLibraryPath(const char *);
extern int CopyCLS(const CLSModule *, CLSModule *);
extern int LoadCLS(const char *, CLSModule **);
extern int LoadCLSFromDll(const char *, CLSModule **);
extern int LoadCLSFromFile(const char *, CLSModule **);
extern int SaveCLS(const char *, const CLSModule *);

extern int AbridgeCLS(const CLSModule *, void **);
extern void DisposeAbridgedCLS(void *);
extern void RelocAbridgedCLS(void *);

extern void GetNakedFileName(const char *, char *);
extern void GetNakedFileType(const char *, char *);
extern int SearchFileFromPath(const char *, const char *, char *);

#ifdef _DEBUG
extern void GuidPrint(REFGUID);
#endif // _DEBUG

//path separator
#define IS_PATH_SEPARATOR(x) ((x) == '/' || (x) == '\\')

#endif // __CLSDEF_H__
