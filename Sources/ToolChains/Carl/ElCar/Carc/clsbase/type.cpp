
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include <elatypes.h>
#include "clsbase.h"

//alignment = 4;
#define STRUCT_TYPE_ALIGNMENT(t) \
        (t == Type_Int8 ? 1 : \
        (t == Type_Byte ? 1 : \
        (t == Type_Boolean ? 1 : \
        (t == Type_Int16 ? 2 : \
        (t == Type_UInt16 ? 2 : \
        (t == Type_Char16 ? 2 : \
        (t == Type_Char32 ? 4 : \
        (t == Type_Int32 ? 4 : \
        (t == Type_UInt32 ? 4 : \
        (t == Type_ECode ? 4 : \
        (t == Type_enum  ? 4 : \
        (t == Type_Float ? 4 : \
        (t == Type_Int64 ? 8 : \
        (t == Type_UInt64 ? 8 : \
        (t == Type_Double ? 8 : 0)))))))))))))))

#define STRUCT_TYPE_TO_SIZE(t) \
        (t == Type_Int8 ? 1 : \
        (t == Type_Byte ? 1 : \
        (t == Type_Boolean ? 1 : \
        (t == Type_Int16 ? 2 : \
        (t == Type_UInt16 ? 2 : \
        (t == Type_Char16 ? 2 : \
        (t == Type_Char32 ? 4 : \
        (t == Type_Int32 ? 4 : \
        (t == Type_UInt32 ? 4 : \
        (t == Type_ECode ? 4 : \
        (t == Type_enum  ? 4 : \
        (t == Type_Float ? 4 : \
        (t == Type_Int64 ? 8 : \
        (t == Type_UInt64 ? 8 : \
        (t == Type_Double ? 8 : 0)))))))))))))))

#define STRUCT_IS_PRIMARY_TYPE(t) \
        (t == Type_Byte ? TRUE : \
        (t == Type_Boolean ? TRUE : \
        (t == Type_Int16 ? TRUE : \
        (t == Type_Char16 ? TRUE : \
        (t == Type_Char32 ? TRUE : \
        (t == Type_Int32 ? TRUE : \
        (t == Type_ECode ? TRUE : \
        (t == Type_enum  ? TRUE : \
        (t == Type_Float ? TRUE : \
        (t == Type_Int64 ? TRUE : \
        (t == Type_Double ? TRUE : FALSE)))))))))))

#define STRUCT_ALIGN_SIZE(size, align) \
        ((size % align) == 0 ? (size) : ((size / align + 1) * align))

int GetIdentifyType(
    const char *pszName, const char *pszNamespaces, const CLSModule *pModule, TypeDescriptor *pType)
{
    int n;

    n = SelectAliasDirEntry(pszName, pModule);
    if (n >= 0) {
        pType->mType = Type_alias;
        pType->mIndex = n;
        _Return (CLS_NoError);
    }

    n = SelectInterfaceDirEntry(pszName, pszNamespaces, pModule);
    if (n >= 0) {
        pType->mType = Type_interface;
        pType->mIndex = n;
        _Return (CLS_NoError);
    }

    n = SelectEnumDirEntry(pszName, pszNamespaces, pModule);
    if (n >= 0) {
        pType->mType = Type_enum;
        pType->mIndex = n;
        _Return (CLS_NoError);
    }

    n = SelectStructDirEntry(pszName, pModule);
    if (n >= 0) {
        pType->mType = Type_struct;
        pType->mIndex = n;
        _Return (CLS_NoError);
    }

    _ReturnError (CLSError_NotFound);
}

BOOL IsLocalStruct(const CLSModule *pModule, StructDescriptor *pDesc)
{
    unsigned short i;
    TypeDescriptor orgtype, basetype, *pt = NULL;

    for (i = 0; i < pDesc->mElementCount; i++) {
        pt = &(pDesc->mElements[i]->mType);
Loop:
        if (pt->mType == Type_Array) {
            memset(&basetype, 0, sizeof(TypeDescriptor));
            GetArrayBaseType(pModule, pt, &basetype);
            pt = &basetype;
        }
        if (pt->mType == Type_alias) {
            memset(&orgtype, 0, sizeof(TypeDescriptor));
            GetOriginalType(pModule, pt, &orgtype);
            pt = &orgtype;
            goto Loop;
        }

        if (pt->mPointer) return TRUE;

        if (!STRUCT_IS_PRIMARY_TYPE(pt->mType)) {
            if (pt->mType == Type_struct) {
                if (IsLocalStruct(pModule, pModule->mStructDirs[pt->mIndex]->mDesc)) {
                    return TRUE;
                }
                else continue;
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOL IsLocalCarQuintet(const CLSModule *pModule,
    const TypeDescriptor *pType, DWORD attribs)
{
    switch (pType->mType) {
        case Type_Int8:
        case Type_UInt16:
        case Type_UInt32:
        case Type_UInt64:
        case Type_PVoid:
            _ReturnOK (TRUE);

        case Type_interface:
            if (!(pModule->mInterfaceDirs[pType->mIndex]->mDesc->mAttribs
                    & InterfaceAttrib_parcelable) &&
                    (pModule->mInterfaceDirs[pType->mIndex]->mDesc->mAttribs
                    & InterfaceAttrib_local)) _ReturnOK (TRUE);
            break;

        default:
            if (1 <= pType->mPointer) _ReturnOK (TRUE);
            break;
    }

    _ReturnOK (FALSE);
}

static void GetTypeFromIndex(const CLSModule *pModule,
    StructDescriptor *pDesc, unsigned short index, TypeDescriptor *pTypeDesc)
{
    TypeDescriptor basetype, orgtype, *pt = NULL;
    pt = &(pDesc->mElements[index]->mType);
    if (pt->mType == Type_Array) {
        memset(&basetype, 0, sizeof(TypeDescriptor));
        GetArrayBaseType(pModule, pt, &basetype);
        pt = &basetype;
    }
    if (pt->mType == Type_alias) {
        memset(&orgtype, 0, sizeof(TypeDescriptor));
        GetOriginalType(pModule, pt, &orgtype);
        pt = &orgtype;
    }

    memcpy(pTypeDesc, pt, sizeof(TypeDescriptor));
}

static UINT GetMaxAlignmentOfStructElems(const CLSModule *pModule, StructDescriptor *pDesc)
{
    UINT uMaxAlign = 0;
    unsigned short i;
    UINT uAlign;
    TypeDescriptor typeDesc;

    for (i = 0; i < pDesc->mElementCount; i++) {
        GetTypeFromIndex(pModule, pDesc, i, &typeDesc);

        if (Type_struct == typeDesc.mType) {
            uAlign = GetMaxAlignmentOfStructElems(pModule, pModule->mStructDirs[typeDesc.mIndex]->mDesc);
        }
        else {
            uAlign = STRUCT_TYPE_ALIGNMENT(typeDesc.mType);
        }

        if (0 == uAlign) return 0;

        if (uAlign > uMaxAlign) uMaxAlign = uAlign;
    }
    return uMaxAlign;
}

static UINT CalcArrayElements(const CLSModule *pModule, TypeDescriptor *pDesc)
{
    assert(Type_Array == pDesc->mType);
    UINT elementCount;
    TypeDescriptor *pSubDesc;

    pSubDesc = &pModule->mArrayDirs[pDesc->mIndex]->mType;
    elementCount = pModule->mArrayDirs[pDesc->mIndex]->mElementCount;

    while (Type_Array == pSubDesc->mType) {
        elementCount *= pModule->mArrayDirs[pSubDesc->mIndex]->mElementCount;
        pSubDesc = &pModule->mArrayDirs[pSubDesc->mIndex]->mType;
    }

    return elementCount;
}

static UINT SizeOfStruct(const CLSModule *pModule, StructDescriptor *pDesc, UINT *pAlignment)
{
    UINT uAlignment = GetMaxAlignmentOfStructElems(pModule, pDesc);
    if (uAlignment > STRUCT_MAX_ALIGN_SIZE) uAlignment = STRUCT_MAX_ALIGN_SIZE;
    if (pAlignment) *pAlignment = uAlignment;
    if (pDesc->mAlignSize) return pDesc->mAlignSize;

    UINT uSizeOfStruct = 0, uTryAlignSize = 0;
    UINT uNextAlignment = 0;
    UINT uCurSize = 0, uNextSize = 0;
    unsigned short i = 0;
    TypeDescriptor orgTypeDesc;

    //the size of the first element of the current struct
    GetTypeFromIndex(pModule, pDesc, 0, &orgTypeDesc);
    if (Type_struct == orgTypeDesc.mType) {
        uCurSize = SizeOfStruct(pModule, \
                    pModule->mStructDirs[orgTypeDesc.mIndex]->mDesc, NULL);
    }
    else {
        uCurSize = STRUCT_TYPE_TO_SIZE(orgTypeDesc.mType);
    }

    if (Type_Array == pDesc->mElements[0]->mType.mType) {
        uCurSize *= CalcArrayElements(pModule, &pDesc->mElements[0]->mType);
    }

    while (i < pDesc->mElementCount) {
        if (uCurSize && ((uCurSize % uAlignment) == 0)) {
            if (uTryAlignSize) {
                uSizeOfStruct += STRUCT_ALIGN_SIZE(uTryAlignSize, uAlignment);
                uTryAlignSize = 0;
            }
            uSizeOfStruct += uCurSize;
            i++;
            if (i < pDesc->mElementCount) {
                GetTypeFromIndex(pModule, pDesc, i, &orgTypeDesc);
                if (Type_struct == orgTypeDesc.mType) {
                    uCurSize = SizeOfStruct(pModule, \
                                pModule->mStructDirs[orgTypeDesc.mIndex]->mDesc, NULL);
                }
                else {
                    uCurSize = STRUCT_TYPE_TO_SIZE(orgTypeDesc.mType);
                }

                if (Type_Array == pDesc->mElements[i]->mType.mType) {
                    uCurSize *= CalcArrayElements(pModule, &pDesc->mElements[i]->mType);
                }
            }
            continue;
        }

        if ((i + 1) >= pDesc->mElementCount) {
            if (uTryAlignSize && ((uTryAlignSize % uAlignment) == 0)) {
                uSizeOfStruct += uTryAlignSize;
                uTryAlignSize = 0;
            }
            uTryAlignSize += uCurSize;
            uSizeOfStruct += STRUCT_ALIGN_SIZE(uTryAlignSize, uAlignment);
            return uSizeOfStruct;
        }

        //Get the next element size
        GetTypeFromIndex(pModule, pDesc, i + 1, &orgTypeDesc);
        if (Type_struct == orgTypeDesc.mType) {
            uNextSize = SizeOfStruct(pModule, pModule->mStructDirs[orgTypeDesc.mIndex]->mDesc, &uNextAlignment);
        }
        else {
            uNextSize = STRUCT_TYPE_TO_SIZE(orgTypeDesc.mType);
            uNextAlignment = STRUCT_TYPE_ALIGNMENT(orgTypeDesc.mType);
        }

        if (Type_Array == pDesc->mElements[i + 1]->mType.mType) {
            uNextSize *= CalcArrayElements(pModule, &pDesc->mElements[i+1]->mType);
        }

        //align size of the next alignment
        uTryAlignSize += uCurSize;
        uTryAlignSize = STRUCT_ALIGN_SIZE(uTryAlignSize, uNextAlignment);
        if (uTryAlignSize && ((uTryAlignSize % uAlignment) == 0)) {
            uSizeOfStruct += uTryAlignSize;
            uTryAlignSize = 0;
        }

        uCurSize = uNextSize;
        i++;
    } //end while

    return uSizeOfStruct;
}

void CalcStructAlignedSize(const CLSModule *pModule, StructDescriptor *pDesc)
{
    if ((pDesc->mAlignSize != 0) || IsLocalStruct(pModule, pDesc)) return;
    pDesc->mAlignSize = SizeOfStruct(pModule, pDesc, NULL);
}

#define NAMESPACE_COPY(pSrcCLS, pSrc, pDest, bNameSpace)                \
    if (pSrc->mNameSpace) {                                           \
        pDest->mNameSpace = new char[strlen(pSrc->mNameSpace) + 1]; \
        if (!pDest->mNameSpace) _ReturnError (CLSError_OutOfMemory);  \
        strcpy(pDest->mNameSpace, pSrc->mNameSpace);                \
    }

KeyValuePair* KeyValuePairCopy(KeyValuePair *pSrc)
{
    KeyValuePair *pDest = new KeyValuePair();

    if (pSrc->mKey != NULL) {
        pDest->mKey = new char[strlen(pSrc->mKey) + 1];
        strcpy(pDest->mKey, pSrc->mKey);
    }
    else pDest->mKey = NULL;

    if (pSrc->mValue != NULL) {
        pDest->mValue = new char[strlen(pSrc->mValue) + 1];
        strcpy(pDest->mValue, pSrc->mValue);
    }
    else pDest->mValue = NULL;

    return pDest;
}

AnnotationDescriptor* AnnotationCopy(AnnotationDescriptor *pSrc)
{
    AnnotationDescriptor *pDest = new AnnotationDescriptor();
    pDest->mName = new char[strlen(pSrc->mName) + 1];
    strcpy(pDest->mName, pSrc->mName);

    if (pSrc->mNameSpace != NULL) {
        pDest->mNameSpace = new char[strlen(pSrc->mNameSpace) + 1];
        strcpy(pDest->mNameSpace, pSrc->mNameSpace);
    }
    else pDest->mNameSpace = NULL;

    pDest->mKeyValuePairCount = pSrc->mKeyValuePairCount;
    if (pDest->mKeyValuePairCount > 0) {
        pDest->mKeyValuePairs = new KeyValuePair *[pDest->mKeyValuePairCount];
        for (int n = 0; n < pDest->mKeyValuePairCount; n++) {
            pDest->mKeyValuePairs[n] = KeyValuePairCopy(pSrc->mKeyValuePairs[n]);
        }
    }
    else pDest->mKeyValuePairs = NULL;

    return pDest;
}

int ClassDescriptorCopy(
    const CLSModule *pSrcModule,
    const ClassDescriptor *pSrc,
    CLSModule *pDestModule,
    ClassDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m, i;

    memcpy(&pDest->mClsid, &pSrc->mClsid, sizeof(CLSID));
    pDest->mAttribs = pSrc->mAttribs;

    if (pSrc->mAttribs & ClassAttrib_hasparent) {
        m = ClassCopy(pSrcModule, pSrc->mParentIndex,
                    pDestModule, bNameSpace);
        if (m < 0) _Return (m);
        pDest->mParentIndex = m;
    }

    if (pSrc->mAttribs & ClassAttrib_hasctor) {
        i = InterfaceCopy(pSrcModule,
                    pSrc->mCtorIndex, pDestModule, bNameSpace);
        if (i < 0) _Return (i);
        pDest->mCtorIndex = i;
    }

    for (n = 0; n < pSrc->mAggregateCount; n++) {
        m = ClassCopy(pSrcModule,
                    pSrc->mAggrIndexes[n], pDestModule, bNameSpace);
        if (m < 0) _Return (m);
        pDest->mAggrIndexes[pDest->mAggregateCount++] = m;
    }

    for (n = 0; n < pSrc->mAspectCount; n++) {
        m = ClassCopy(pSrcModule,
                    pSrc->mAspectIndexes[n], pDestModule, bNameSpace);
        if (m < 0) _Return (m);
        pDest->mAspectIndexes[pDest->mAspectCount++] = m;
    }

    for (n = 0; n < pSrc->mClassCount; n++) {
        m = ClassCopy(pSrcModule,
                    pSrc->mClassIndexes[n], pDestModule, bNameSpace);
        if (m < 0) _Return (m);
        pDest->mClassIndexes[pDest->mClassCount++] = m;
    }

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mInterfaceCount; n++) {
        i = InterfaceCopy(pSrcModule,
                    pSrc->mInterfaces[n]->mIndex, pDestModule, bNameSpace);
        if (i < 0) _Return (i);

        m = CreateClassInterface(i, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        pDest->mInterfaces[m]->mAttribs = pSrc->mInterfaces[n]->mAttribs;
    }

    if (pSrc->mAttribs & ClassAttrib_hascallback) {
        for (n = 0; n < pSrc->mInterfaceCount; n++) {
            if (pSrc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback) {
                char szName[255];
                strcpy(szName, pSrcModule->mInterfaceDirs[pSrc->mInterfaces[n]->mIndex]->mName);
                strcat(szName, "Handler");
                i = SelectInterfaceDirEntry(szName, NULL, pSrcModule);
                if (i < 0) _Return (i);

                i = InterfaceCopy(pSrcModule, i, pDestModule, bNameSpace);
                if (i < 0) _Return (i);
            }
        }
    }

    _ReturnOK (CLS_NoError);
}

int ClassDescriptorXCopy(
    const CLSModule *pSrcModule,
    const ClassDescriptor *pSrc,
    CLSModule *pDestModule,
    ClassDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    memcpy(&pDest->mClsid, &pSrc->mClsid, sizeof(CLSID));
    pDest->mAttribs = pSrc->mAttribs;

    if (pSrc->mAttribs & ClassAttrib_hasparent) {
        pDest->mParentIndex = pSrc->mParentIndex;
    }

    if (pSrc->mAttribs & ClassAttrib_hasctor) {
        pDest->mCtorIndex = pSrc->mCtorIndex;
    }

    for (n = 0; n < pSrc->mAggregateCount; n++) {
        pDest->mAggrIndexes[pDest->mAggregateCount++] = pSrc->mAggrIndexes[n];
    }

    for (n = 0; n < pSrc->mAspectCount; n++) {
        pDest->mAspectIndexes[pDest->mAspectCount++] = pSrc->mAspectIndexes[n];
    }

    for (n = 0; n < pSrc->mClassCount; n++) {
        pDest->mClassIndexes[pDest->mClassCount++] = pSrc->mClassIndexes[n];
    }

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mInterfaceCount; n++) {
        m = CreateClassInterface(pSrc->mInterfaces[n]->mIndex, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        pDest->mInterfaces[m]->mAttribs = pSrc->mInterfaces[n]->mAttribs;
    }

    pDest->mParentIndex = pSrc->mParentIndex;

    _ReturnOK (CLS_NoError);
}

int ClassCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r;
    ClassDirEntry *pSrc, *pDest;
    char szClassObjName[64];

    pSrc = pSrcModule->mClassDirs[nIndex];
    n = SelectClassDirEntry(pSrc->mName, NULL, pDestModule);
    if (n >= 0) {
        if (pDestModule->mClassDirs[n]->mDesc->mInterfaceCount > 0)
            _Return (n);
    }
    else {
        n = CreateClassDirEntry(pSrc->mName,
                        pDestModule, pSrc->mDesc->mAttribs);
        if (n < 0) _Return (n);
    }
    pDest = pDestModule->mClassDirs[n];

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = ClassDescriptorCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mAttribs |= ClassAttrib_t_external;

    if ((pDest->mDesc->mAttribs & ClassAttrib_hasctor) &&
        !(pDest->mDesc->mAttribs & ClassAttrib_t_clsobj)) {
        sprintf(szClassObjName, "_%sClassObject_", pDest->mName);
        r = SelectClassDirEntry(szClassObjName, NULL, pSrcModule);
        if (r >= 0) {
            r = ClassCopy(pSrcModule, r, pDestModule, bNameSpace);
            if (r < 0) _Return (r);
        }
    }
    _Return (n);
}

int ClassXCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r;
    ClassDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mClassDirs[nIndex];
    n = SelectClassDirEntry(pSrc->mName, NULL, pDestModule);
    if (n >= 0) {
        if (pDestModule->mClassDirs[n]->mDesc->mInterfaceCount > 0)
            _Return (n);
    }
    else {
        n = CreateClassDirEntry(pSrc->mName,
                        pDestModule, pSrc->mDesc->mAttribs);
        if (n < 0) _Return (n);
    }
    pDest = pDestModule->mClassDirs[n];

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = ClassDescriptorXCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mAttribs |= ClassAttrib_t_external;

    _Return (n);
}

int InterfaceConstCopy(
    const CLSModule *pSrcModule,
    const InterfaceConstDescriptor *pSrc,
    CLSModule *pDestModule,
    InterfaceConstDescriptor *pDest,
    BOOL bNameSpace)
{
    pDest->mType = pSrc->mType;
    if (pDest->mType == TYPE_BOOLEAN) {
        pDest->mV.mBoolValue = pSrc->mV.mBoolValue;
    }
    else if (pDest->mType == TYPE_CHAR32) {
        pDest->mV.mInt32Value.mValue = pSrc->mV.mInt32Value.mValue;
    }
    else if (pDest->mType == TYPE_BYTE) {
        pDest->mV.mInt32Value.mValue = pSrc->mV.mInt32Value.mValue;
        pDest->mV.mInt32Value.mFormat = pSrc->mV.mInt32Value.mFormat;
    }
    else if (pDest->mType == TYPE_INTEGER16) {
        pDest->mV.mInt32Value.mValue = pSrc->mV.mInt32Value.mValue;
        pDest->mV.mInt32Value.mFormat = pSrc->mV.mInt32Value.mFormat;
    }
    else if (pDest->mType == TYPE_INTEGER32) {
        pDest->mV.mInt32Value.mValue = pSrc->mV.mInt32Value.mValue;
        pDest->mV.mInt32Value.mFormat = pSrc->mV.mInt32Value.mFormat;
    }
    else if (pDest->mType == TYPE_INTEGER64) {
        pDest->mV.mInt64Value.mValue = pSrc->mV.mInt64Value.mValue;
        pDest->mV.mInt64Value.mFormat = pSrc->mV.mInt64Value.mFormat;
    }
    else if (pDest->mType == TYPE_FLOAT || pDest->mType == TYPE_DOUBLE) {
        pDest->mV.mDoubleValue = pSrc->mV.mDoubleValue;
    }
    else {
        assert(pDest->mType == TYPE_STRING);
        if (pSrc->mV.mStrValue != NULL) {
            pDest->mV.mStrValue = (char*)malloc(strlen(pSrc->mV.mStrValue) + 1);
            strcpy(pDest->mV.mStrValue, pSrc->mV.mStrValue);
        }
    }

    _Return (CLS_NoError);
}

int MethodCopy(
    const CLSModule *pSrcModule,
    const MethodDescriptor *pSrc,
    CLSModule *pDestModule,
    MethodDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    pDest->mSignature = new char[strlen(pSrc->mSignature) + 1];
    strcpy(pDest->mSignature, pSrc->mSignature);

    n = TypeCopy(pSrcModule, &pSrc->mType,
                pDestModule, &pDest->mType, bNameSpace);
    if (n < 0) _Return (n);

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mParamCount; n++) {
        m = CreateMethodParam(pSrc->mParams[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        pDest->mParams[m]->mAttribs = pSrc->mParams[n]->mAttribs;
        m = TypeCopy(pSrcModule, &pSrc->mParams[n]->mType,
                    pDestModule, &pDest->mParams[m]->mType, bNameSpace);
        if (m < 0) _Return (m);
    }

    pDest->mAttribs = pSrc->mAttribs;

    _Return (CLS_NoError);
}

int MethodXCopy(
    const CLSModule *pSrcModule,
    const MethodDescriptor *pSrc,
    CLSModule *pDestModule,
    MethodDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    pDest->mSignature = new char[strlen(pSrc->mSignature) + 1];
    strcpy(pDest->mSignature, pSrc->mSignature);

    n = TypeXCopy(pSrcModule, &pSrc->mType,
                pDestModule, &pDest->mType, bNameSpace);
    if (n < 0) _Return (n);

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mParamCount; n++) {
        m = CreateMethodParam(pSrc->mParams[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        pDest->mParams[m]->mAttribs = pSrc->mParams[n]->mAttribs;
        m = TypeXCopy(pSrcModule, &pSrc->mParams[n]->mType,
                    pDestModule, &pDest->mParams[m]->mType, bNameSpace);
        if (m < 0) _Return (m);
    }

    pDest->mAttribs = pSrc->mAttribs;

    _Return (CLS_NoError);
}

int InterfaceDescriptorCopy(
    const CLSModule *pSrcModule,
    const InterfaceDescriptor *pSrc,
    CLSModule *pDestModule,
    InterfaceDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    pDest->mAttribs = pSrc->mAttribs;
    memcpy(&pDest->mIID, &pSrc->mIID, sizeof(IID));

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mConstCount; n++) {
        m = CreateInterfaceConstDirEntry(pSrc->mConsts[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = InterfaceConstCopy(pSrcModule, pSrc->mConsts[n],
                    pDestModule, pDest->mConsts[m], bNameSpace);
        if (m < 0) _Return (m);
    }

    for (n = 0; n < pSrc->mMethodCount; n++) {
        m = CreateInterfaceMethod(pSrc->mMethods[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = MethodCopy(pSrcModule, pSrc->mMethods[n],
                    pDestModule, pDest->mMethods[m], bNameSpace);
        if (m < 0) _Return (m);
    }

    _ReturnOK (CLS_NoError);
}

int InterfaceDescriptorXCopy(
    const CLSModule *pSrcModule,
    const InterfaceDescriptor *pSrc,
    CLSModule *pDestModule,
    InterfaceDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    pDest->mAttribs = pSrc->mAttribs;
    memcpy(&pDest->mIID, &pSrc->mIID, sizeof(IID));

    for (n = 0; n < pSrc->mAnnotationCount; n++) {
        pDest->mAnnotations[n] = AnnotationCopy(pSrc->mAnnotations[n]);
    }

    for (n = 0; n < pSrc->mConstCount; n++) {
        m = CreateInterfaceConstDirEntry(pSrc->mConsts[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = InterfaceConstCopy(pSrcModule, pSrc->mConsts[n],
                    pDestModule, pDest->mConsts[m], bNameSpace);
        if (m < 0) _Return (m);
    }

    for (n = 0; n < pSrc->mMethodCount; n++) {
        m = CreateInterfaceMethod(pSrc->mMethods[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = MethodXCopy(pSrcModule, pSrc->mMethods[n],
                    pDestModule, pDest->mMethods[m], bNameSpace);
        if (m < 0) _Return (m);
    }

    _ReturnOK (CLS_NoError);
}

int InterfaceCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r, nParent;
    InterfaceDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mInterfaceDirs[nIndex];

    if (0 != pSrc->mDesc->mParentIndex) {
        nParent = InterfaceCopy(pSrcModule, pSrc->mDesc->mParentIndex,
                            pDestModule, bNameSpace);
        if (nParent < 0) _Return (nParent);
    }
    else {
        nParent = 0;
    }

    n = SelectInterfaceDirEntry(pSrc->mName, pSrc->mNameSpace, pDestModule);
    if (n >= 0) {
        if (pDestModule->mInterfaceDirs[n]->mDesc->mConstCount > 0 ||
                pDestModule->mInterfaceDirs[n]->mDesc->mMethodCount > 0)
            _Return (n);
    }
    else {
        n = CreateInterfaceDirEntry(
                pSrc->mName, pDestModule, pSrc->mDesc->mAttribs);
        if (n < 0) _Return (n);
        if (!bNameSpace) {
            pDestModule->mDefinedInterfaceIndexes[pDestModule->mDefinedInterfaceCount++] = n;
        }
    }
    pDest = pDestModule->mInterfaceDirs[n];
    pDest->mDesc->mParentIndex = nParent;

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = InterfaceDescriptorCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mAttribs |= InterfaceAttrib_t_external;

    if (pSrc->mFileIndex != 0) {
        r = CreateFileDirEntry(pSrcModule->mFileDirs[pSrc->mFileIndex]->mPath, pDestModule);
        pDest->mFileIndex = r;
    }

    _Return (n);
}

int InterfaceXCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r, nParent;
    InterfaceDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mInterfaceDirs[nIndex];

    nParent = pSrc->mDesc->mParentIndex;

    n = SelectInterfaceDirEntry(pSrc->mName, NULL, pDestModule);
    if (n >= 0) {
        if (pDestModule->mInterfaceDirs[n]->mDesc->mMethodCount > 0)
            _Return (n);
    }
    else {
        n = CreateInterfaceDirEntry(
                pSrc->mName, pDestModule, pSrc->mDesc->mAttribs);
        if (n < 0) {
            _Return (n);
        }
    }
    pDest = pDestModule->mInterfaceDirs[n];
    pDest->mDesc->mParentIndex = nParent;

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = InterfaceDescriptorXCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) {
        _Return (r);
    }

    if (bNameSpace) pDest->mDesc->mAttribs |= InterfaceAttrib_t_external;

    if (pSrc->mFileIndex != 0) {
        r = CreateFileDirEntry(pSrcModule->mFileDirs[pSrc->mFileIndex]->mPath, pDestModule);
        pDest->mFileIndex = r;
    }

    _Return (n);
}

int ArrayCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, m;
    ArrayDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mArrayDirs[nIndex];
    n = SelectArrayDirEntry(pSrc->mElementCount, pSrc->mType, pDestModule);
    if (n >= 0) {
        _Return (n);
    }
    else {
        n = CreateArrayDirEntry(pDestModule);
        if (n < 0) _Return (n);
    }

    pDest = pDestModule->mArrayDirs[n];

    pDest->mElementCount = pSrc->mElementCount;
    m = TypeCopy(pSrcModule, &pSrc->mType,
            pDestModule, &pDest->mType, bNameSpace);
    if (m < 0) _Return (m);

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    _Return (n);
}

int ArrayXCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, m;
    ArrayDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mArrayDirs[nIndex];
    n = SelectArrayDirEntry(pSrc->mElementCount, pSrc->mType, pDestModule);
    if (n >= 0) {
        _Return (n);
    }
    else {
        n = CreateArrayDirEntry(pDestModule);
        if (n < 0) _Return (n);
    }

    pDest = pDestModule->mArrayDirs[n];

    pDest->mElementCount = pSrc->mElementCount;
    m = TypeXCopy(pSrcModule, &pSrc->mType,
            pDestModule, &pDest->mType, bNameSpace);
    if (m < 0) _Return (m);

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    _Return (n);
}

int StructDescriptorCopy(
    const CLSModule *pSrcModule,
    const StructDescriptor *pSrc,
    CLSModule *pDestModule,
    StructDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    for (n = 0; n < pSrc->mElementCount; n++) {
        m = CreateStructElement(pSrc->mElements[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = TypeCopy(pSrcModule, &pSrc->mElements[n]->mType,
                    pDestModule, &pDest->mElements[m]->mType, bNameSpace);
        if (m < 0) _Return (m);
    }
    pDest->mAlignSize = pSrc->mAlignSize;

    _ReturnOK (CLS_NoError);
}

int StructDescriptorXCopy(
    const CLSModule *pSrcModule,
    const StructDescriptor *pSrc,
    CLSModule *pDestModule,
    StructDescriptor *pDest,
    BOOL bNameSpace)
{
    int n, m;

    for (n = 0; n < pSrc->mElementCount; n++) {
        m = CreateStructElement(pSrc->mElements[n]->mName, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        m = TypeXCopy(pSrcModule, &pSrc->mElements[n]->mType,
                    pDestModule, &pDest->mElements[m]->mType, bNameSpace);
        if (m < 0) _Return (m);
    }
    pDest->mAlignSize = pSrc->mAlignSize;

    _ReturnOK (CLS_NoError);
}

int StructCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r;
    StructDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mStructDirs[nIndex];
    n = SelectStructDirEntry(pSrc->mName, pDestModule);
    if (n >= 0) {
        if (pDestModule->mStructDirs[n]->mDesc->mElementCount > 0)
            _Return (n);
    }
    else {
        n = CreateStructDirEntry(pSrc->mName, pDestModule);
        if (n < 0) _Return (n);
    }
    pDest = pDestModule->mStructDirs[n];

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = StructDescriptorCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mR.mReserved |= StructAttrib_t_external;

    _Return (n);
}

int StructXCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r;
    StructDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mStructDirs[nIndex];
    n = SelectStructDirEntry(pSrc->mName, pDestModule);
    if (n >= 0) {
        if (pDestModule->mStructDirs[n]->mDesc->mElementCount > 0)
            _Return (n);
    }
    else {
        n = CreateStructDirEntry(pSrc->mName, pDestModule);
        if (n < 0) _Return (n);
    }
    pDest = pDestModule->mStructDirs[n];

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = StructDescriptorXCopy(pSrcModule, pSrc->mDesc,
                    pDestModule, pDest->mDesc, bNameSpace);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mR.mReserved |= StructAttrib_t_external;

    _Return (n);
}

int EnumDescriptorCopy(
    const EnumDescriptor *pSrc,
    CLSModule *pDestModule,
    EnumDescriptor *pDest)
{
    int n, m;

    for (n = 0; n < pSrc->mElementCount; n++) {
        m = CreateEnumElement(pSrc->mElements[n]->mName, pDestModule, pDest);
        if (m < 0) _Return (m);
        assert(m == n);
        pDest->mElements[m]->mValue = pSrc->mElements[n]->mValue;
        pDest->mElements[m]->mIsHexFormat = pSrc->mElements[n]->mIsHexFormat;
    }
    _ReturnOK (CLS_NoError);
}

int EnumCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n, r;
    EnumDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mEnumDirs[nIndex];
    n = SelectEnumDirEntry(pSrc->mName, pSrc->mNameSpace, pDestModule);
    if (n >= 0) {
        if (pDestModule->mEnumDirs[n]->mDesc->mElementCount > 0)
            _Return (n);
    }
    else {
        n = CreateEnumDirEntry(pSrc->mName, pDestModule);
        if (n < 0) _Return (n);
    }
    pDest = pDestModule->mEnumDirs[n];

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    r = EnumDescriptorCopy(
                pSrc->mDesc, pDestModule, pDest->mDesc);
    if (r < 0) _Return (r);

    if (bNameSpace) pDest->mDesc->mR.mReserved |= EnumAttrib_t_external;

    _Return (n);
}

int AliasCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n;
    TypeDescriptor orgType, destType;
    AliasDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mAliasDirs[nIndex];
    n = SelectAliasDirEntry(pSrc->mName, pDestModule);
    if (n >= 0) _Return (n);

    GetOriginalType(pSrcModule, &pSrc->mType, &orgType);
    n = TypeCopy(pSrcModule, &orgType, pDestModule, &destType, bNameSpace);
    if (n < 0) _Return (n);

    n = CreateAliasDirEntry(pSrc->mName, pDestModule, &destType);
    if (n < 0) _Return (n);
    pDest = pDestModule->mAliasDirs[n];
    pDest->mIsDummyType = pSrc->mIsDummyType;

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    _Return (n);
}

int AliasXCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n;
    TypeDescriptor orgType, destType;
    AliasDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mAliasDirs[nIndex];
    n = SelectAliasDirEntry(pSrc->mName, pDestModule);
    if (n >= 0) _Return (n);

    GetOriginalType(pSrcModule, &pSrc->mType, &orgType);
    n = TypeXCopy(pSrcModule, &orgType, pDestModule, &destType, bNameSpace);
    if (n < 0) _Return (n);

    n = CreateAliasDirEntry(pSrc->mName, pDestModule, &destType);
    if (n < 0) _Return (n);
    pDest = pDestModule->mAliasDirs[n];
    pDest->mIsDummyType = pSrc->mIsDummyType;

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    _Return (n);
}

int ConstCopy(
    const CLSModule *pSrcModule,
    int nIndex,
    CLSModule *pDestModule,
    BOOL bNameSpace)
{
    int n;
    ConstDirEntry *pSrc, *pDest;

    pSrc = pSrcModule->mConstDirs[nIndex];
    n = SelectConstDirEntry(pSrc->mName, pDestModule);
    if (n >= 0) _Return (n);

    n = CreateConstDirEntry(pSrc->mName, pDestModule);
    if (n < 0) _Return (n);
    pDest = pDestModule->mConstDirs[n];
    pDest->mType = pSrc->mType;
    if (pDest->mType == TYPE_INTEGER32) {
        pDest->mV.mInt32Value.mValue = pSrc->mV.mInt32Value.mValue;
        pDest->mV.mInt32Value.mIsHexFormat = pSrc->mV.mInt32Value.mIsHexFormat;
    }
    else {
        assert(pDest->mType == TYPE_STRING);
        if (pSrc->mV.mStrValue.mValue != NULL) {
            pDest->mV.mStrValue.mValue = (char*)malloc(strlen(pSrc->mV.mStrValue.mValue) + 1);
            strcpy(pDest->mV.mStrValue.mValue, pSrc->mV.mStrValue.mValue);
        }
    }

    NAMESPACE_COPY(pSrcModule, pSrc, pDest, bNameSpace);

    _Return (n);
}

int TypeCopy(
    const CLSModule *pSrcModule,
    const TypeDescriptor *pSrcType,
    CLSModule *pDestModule,
    TypeDescriptor *pDestType,
    BOOL bNameSpace)
{
    int n;

    memcpy(pDestType, pSrcType, sizeof(TypeDescriptor));

    switch (pSrcType->mType) {
        case Type_interface:
            n = InterfaceCopy(
                pSrcModule, pSrcType->mIndex, pDestModule, bNameSpace);
            if (n < 0) _Return (n);
            pDestType->mIndex = n;
            _Return (CLS_NoError);
        case Type_Array:
            n = ArrayCopy(pSrcModule, pSrcType->mIndex, pDestModule, bNameSpace);
            if (n < 0) _Return (n);
            pDestType->mIndex = n;
            _Return (CLS_NoError);

        case Type_struct:
            n = StructCopy(pSrcModule, pSrcType->mIndex, pDestModule, bNameSpace);
            if (n < 0) _Return (n);
            pDestType->mIndex = n;
            _Return (CLS_NoError);

        case Type_enum:
            n = EnumCopy(pSrcModule, pSrcType->mIndex, pDestModule, bNameSpace);
            if (n < 0) _Return (n);
            pDestType->mIndex = n;
            _Return (CLS_NoError);

        case Type_alias:
            n = AliasCopy(pSrcModule, pSrcType->mIndex, pDestModule, bNameSpace);
            if (n < 0) _Return (n);
            pDestType->mIndex = n;
            _Return (CLS_NoError);

        default:
            break;
    }

    if (pSrcType->mNestedType) {
        pDestType->mNestedType = new TypeDescriptor;
        if (!pDestType->mNestedType) _ReturnError (CLSError_OutOfMemory);
        n = TypeCopy(pSrcModule, pSrcType->mNestedType,
                    pDestModule, pDestType->mNestedType, bNameSpace);
        if (n < 0) {
            delete pDestType->mNestedType;
            pDestType->mNestedType = NULL;
            _Return (n);
        }
    }

    _ReturnOK (CLS_NoError);
}

int TypeXCopy(
    const CLSModule *pSrcModule,
    const TypeDescriptor *pSrcType,
    CLSModule *pDestModule,
    TypeDescriptor *pDestType,
    BOOL bNameSpace)
{
    int n;

    memcpy(pDestType, pSrcType, sizeof(TypeDescriptor));

    switch (pSrcType->mType) {
        case Type_interface:
        case Type_Array:
        case Type_struct:
        case Type_enum:
        case Type_alias:
            pDestType->mIndex = pSrcType->mIndex;
            _Return (CLS_NoError);

        default:
            break;
    }

    if (pSrcType->mNestedType) {
        pDestType->mNestedType = new TypeDescriptor;
        if (!pDestType->mNestedType) _ReturnError (CLSError_OutOfMemory);
        n = TypeXCopy(pSrcModule, pSrcType->mNestedType,
                    pDestModule, pDestType->mNestedType, bNameSpace);
        if (n < 0) {
            delete pDestType->mNestedType;
            pDestType->mNestedType = NULL;
            _Return (n);
        }
    }

    _ReturnOK (CLS_NoError);
}

int TypeCopy(const TypeDescriptor *pSrc, TypeDescriptor *pDest)
{
    memcpy(pDest, pSrc, sizeof(TypeDescriptor));
    if (pSrc->mNestedType) {
        pDest->mNestedType = new TypeDescriptor;
        if (!pDest->mNestedType) _ReturnError (CLSError_OutOfMemory);
        memcpy(pDest->mNestedType, pSrc->mNestedType, sizeof(TypeDescriptor));
    }
    _ReturnOK (CLS_NoError);
}
