//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <new>
#include "CCallbackParcel.h"
#include "elquintet.h"

#define ROUND4(n)       (((n)+3)&~3)   // round up to multiple of 4 bytes
#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

enum Type {
    Type_Unknown = 0,

    Type_Byte, Type_Boolean, Type_Char8, Type_Char16, Type_Char32,
    Type_Int16, Type_Int32, Type_Int64, Type_Float, Type_Double,
    Type_String, Type_CString, Type_Struct, Type_EMuid, Type_EGuid,
    Type_ArrayOf, Type_ArrayOfCString, Type_ArrayOfString,
    Type_BufferOf, Type_BufferOfCString, Type_BufferOfString,
    Type_StringBuf, Type_MemoryBuf, Type_InterfacePtr,

    Type_BytePtr, Type_BooleanPtr, Type_Char8Ptr,  Type_Char16Ptr,
    Type_Int16Ptr, Type_Int32Ptr,  Type_Int64Ptr, Type_FloatPtr,  Type_DoublePtr,
    Type_StringPtr, Type_InterfacePtrPtr,
};

CCallbackParcel::CCallbackParcel()
{
    m_elemCount = 0;

    m_typeBufCapacity = 10;
    m_elemTypes = (Byte*)malloc(m_typeBufCapacity);
    memset(m_elemTypes, 0, m_typeBufCapacity);

    m_elemBufCapacity = 128;
    m_elemBuf = (Byte*)malloc(m_elemBufCapacity);
    memset(m_elemBuf, 0, m_elemBufCapacity);
    m_elemPtr = m_elemBuf;

    m_dataBufCapacity = 128;
    m_dataBuf = (Byte*)malloc(m_dataBufCapacity);
    memset(m_dataBuf, 0, m_dataBufCapacity);
    m_dataPtr = m_dataBuf;
    m_nRefs = 0;
}

CCallbackParcel::~CCallbackParcel()
{
    m_elemPtr = m_elemBuf;

    for (Int32 i = 0; i < m_elemCount; i++) {
        switch(m_elemTypes[i]) {
            case Type_InterfacePtr:
				if (*(IInterface**)m_elemPtr != NULL) {
					(*(IInterface**)m_elemPtr)->Release();
				}
                m_elemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                m_elemPtr = (Byte*)ROUND8((Int32)m_elemPtr);
#endif
				m_elemPtr += 8;
				break;

            case Type_String: {
                String* p = *(String**)m_elemPtr;
                delete p;
                *(String**)m_elemPtr = NULL;
                break;
            }

            default:
                m_elemPtr += 4;
                break;
        }
    }

    free(m_elemTypes);
    free(m_elemBuf);
    free(m_dataBuf);
}

PInterface CCallbackParcel::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IParcel) {
        return (IParcel*)this;
    }

    return NULL;
}

UInt32 CCallbackParcel::AddRef()
{
    Int32 ref = atomic_inc(&m_nRefs);

    return (UInt32)ref;
}

UInt32 CCallbackParcel::Release()
{
    Int32 ref = atomic_dec(&m_nRefs);

    if (0 == ref) {
        delete this;
    }
    assert(ref >= 0);
    return ref;
}

ECode CCallbackParcel::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IParcel *)this) {
        *pIID = EIID_IParcel;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CCallbackParcel::Marshall(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    return NOERROR;
}

ECode CCallbackParcel::Unmarshall(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode CCallbackParcel::GrowTypeBuffer()
{
    m_typeBufCapacity +=  10;
    m_elemTypes = (Byte*)realloc(m_elemTypes, m_typeBufCapacity);
    if (NULL == m_elemTypes) return E_OUT_OF_MEMORY;

    return NOERROR;
}

ECode CCallbackParcel::GrowElemBuffer()
{
    int offset = m_elemPtr - m_elemBuf;
    m_elemBufCapacity += 128;
    m_elemBuf = (Byte*)realloc(m_elemBuf, m_elemBufCapacity);
    if (NULL == m_elemBuf) return E_OUT_OF_MEMORY;
    m_elemPtr = m_elemBuf + offset;

    return NOERROR;
}

ECode CCallbackParcel::GrowDataBuffer(int minSize)
{
    Byte type, *tmpPtr, *orgDataBuf, *elemPtr;
    int offset1, offset2, offset3, used;

    orgDataBuf = m_dataBuf;
    elemPtr = m_elemBuf;
    offset1 = m_dataPtr - m_dataBuf;
	if(minSize < 128)
		minSize = 128;
	else
		minSize += 128;
    m_dataBufCapacity += minSize;
    m_dataBuf = (Byte*)malloc(m_dataBufCapacity);
    if (NULL == m_dataBuf) return E_OUT_OF_MEMORY;
    memcpy(m_dataBuf, orgDataBuf, offset1);
    m_dataPtr = m_dataBuf + offset1;

    for(Int32 i = 0; i < m_elemCount; i++) {
        type = m_elemTypes[i];
        switch(type) {
            case Type_Byte:
            case Type_Boolean:
            case Type_Char8:
            case Type_Char16:
            case Type_Char32:
            case Type_Int16:
            case Type_Int32:
            case Type_Float:
            case Type_BytePtr:
            case Type_BooleanPtr:
            case Type_Char8Ptr:
            case Type_Char16Ptr:
            case Type_Int16Ptr:
            case Type_Int32Ptr:
            case Type_Int64Ptr:
            case Type_FloatPtr:
            case Type_DoublePtr:
            case Type_InterfacePtr:
            case Type_InterfacePtrPtr:
                elemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                elemPtr = (Byte*)ROUND8((Int32)elemPtr);
#endif
				elemPtr += 8;
                break;

            case Type_CString:
            case Type_String:
            case Type_Struct:
            case Type_EMuid:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                *(Byte**)elemPtr = m_dataBuf + offset1;
                elemPtr += 4;
                break;

            case Type_EGuid:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(EGuid**)elemPtr)->pUunm - orgDataBuf;
                *(Byte**)elemPtr = m_dataBuf + offset1;
                (*(EGuid**)elemPtr)->pUunm = (char*)(m_dataBuf + offset2);
                elemPtr += 4;
                break;

            case Type_ArrayOf:
            case Type_BufferOf:
            case Type_MemoryBuf:
            case Type_StringBuf:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(CarQuintet**)(elemPtr))->m_pBuf - orgDataBuf;
                *(Byte**)elemPtr = m_dataBuf + offset1;
                (*(CarQuintet**)(elemPtr))->m_pBuf = (void*)(m_dataBuf + offset2);
                elemPtr += 4;
                break;

            case Type_ArrayOfString:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(CarQuintet**)(elemPtr))->m_pBuf - orgDataBuf;
                tmpPtr = m_dataBuf + offset1;
                ((CarQuintet*)tmpPtr)->m_pBuf = m_dataBuf + offset2;
                used = (*(ArrayOf<String>**)elemPtr)->GetLength();

                for(Int32 i = 0; i < used; i++) {
                    if ((**(ArrayOf<String>**)elemPtr)[i]) {
                        offset3 = (Byte*)(const char*)((**(ArrayOf<String>**)elemPtr)[i]) - orgDataBuf;
                        (*(ArrayOf<String>*)tmpPtr)[i] = (char*)(m_dataBuf + offset3);
                    }
                    else (*(ArrayOf<String>*)tmpPtr)[i] = NULL;
                }
                *(Byte**)elemPtr = tmpPtr;
                elemPtr += 4;
                break;

            case Type_BufferOfString:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(CarQuintet**)(elemPtr))->m_pBuf - orgDataBuf;
                tmpPtr = m_dataBuf + offset1;
                ((CarQuintet*)tmpPtr)->m_pBuf = m_dataBuf + offset2;
                used = (*(BufferOf<String>**)elemPtr)->GetUsed();

                for(Int32 i = 0; i < used; i++) {
                    if ((**(BufferOf<String>**)elemPtr)[i]) {
                        offset3 = (Byte*)(const char*)(**(BufferOf<String>**)elemPtr)[i] - orgDataBuf;
                        (*(BufferOf<String>*)tmpPtr)[i] = (char*)(m_dataBuf + offset3);
                    }
                    else (*(ArrayOf<String>*)tmpPtr)[i] = NULL;
                }
                *(Byte**)elemPtr = tmpPtr;
                elemPtr += 4;
                break;

        default:
            assert(0);
            break;
        }
    }
    free(orgDataBuf);

    return NOERROR;
}

ECode CCallbackParcel::AppendFrom(
    /* [in] */ IParcel* parcel,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::HasFileDescriptors(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteValue(PVoid pValue, Int32 type, Int32 size)
{
    ECode ec;
    Int32 used, len;

    if (m_elemCount >= m_typeBufCapacity) {
        ec = GrowTypeBuffer();
        if (FAILED(ec)) return ec;
    }
    if (m_elemPtr - m_elemBuf + 4 > m_elemBufCapacity) {
        ec = GrowElemBuffer();
        if (FAILED(ec)) return ec;
    }

    m_elemTypes[m_elemCount] = (Byte)type;
    switch(type) {
        case Type_Byte:
        case Type_Boolean:
        case Type_Char8:
            *(Int32*)(m_elemPtr) = *((Byte*)pValue);
            m_elemPtr += 4;

            break;

        case Type_Char16:
        case Type_Int16:
            *(Int32*)(m_elemPtr) = *((Int16*)pValue);
            m_elemPtr += 4;
            break;

        case Type_Char32:
        case Type_Int32:
        case Type_Float:
            *(Int32*)(m_elemPtr) = *(Int32*)pValue;
            m_elemPtr += 4;
            break;

        case Type_Int64:
        case Type_Double:
            if (m_elemPtr - m_elemBuf + 4 + 4 > m_elemBufCapacity) {
                ec = GrowElemBuffer();
                if (FAILED(ec)) return ec;
            }
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
            m_elemPtr = (Byte*)ROUND8((Int32)m_elemPtr);
#endif
            *(Int32*)(m_elemPtr) = (Int32)(*((Int64*)pValue) & 0xffffffff);
            *(Int32*)(m_elemPtr + 4) = (Int32)((*((Int64*)pValue) >> 32) & 0xffffffff);
            m_elemPtr += 8;
            break;

        case Type_CString:
            if (m_dataPtr - m_dataBuf + ROUND4(size) > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size));
                if (FAILED(ec)) return ec;
            }
            if (size == 0) {
                *(char**)(m_elemPtr) = NULL;
            }
            else {
                *(char**)(m_elemPtr) = (char*)m_dataPtr;
            }
            m_elemPtr += 4;
            if (size > 1) {
                strcpy((char*)m_dataPtr, (char*)pValue);
            }
            else {
                *(char*)m_dataPtr = '\0';
            }
            m_dataPtr += ROUND4(size * sizeof(char));
            break;

        case Type_String: {
            String* p = new String();
            *p = *(String*)pValue;
            *(String**)m_elemPtr = p;
            m_elemPtr += 4;
            break;
        }
        case Type_InterfacePtr:
            *(IInterface**)m_elemPtr = *(IInterface**)pValue;
            if ((*(IInterface**)m_elemPtr) != NULL) {
                (*(IInterface**)m_elemPtr)->AddRef();
            }
            m_elemPtr += 4;
            break;

        case Type_BytePtr:
        case Type_BooleanPtr:
        case Type_Char8Ptr:
        case Type_Char16Ptr:
        case Type_Int16Ptr:
        case Type_Int32Ptr:
        case Type_Int64Ptr:
        case Type_FloatPtr:
        case Type_DoublePtr:
        case Type_StringPtr:
        case Type_InterfacePtrPtr:
            *(Handle32*)(m_elemPtr) = (Handle32)pValue;
            m_elemPtr += 4;
            break;

        case Type_Struct:
            if (m_dataPtr - m_dataBuf + ROUND4(size) + 4 > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }
            *(Int32*)m_dataPtr = size;
            m_dataPtr += 4;
            *(Byte**)(m_elemPtr) = m_dataPtr;
            m_elemPtr += 4;
            memcpy(m_dataPtr, pValue, size);
            m_dataPtr += ROUND4(size);
            break;

        case Type_EMuid:
            if (m_dataPtr - m_dataBuf + ROUND4(size) > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size));
                if (FAILED(ec)) return ec;
            }
            *(EMuid**)(m_elemPtr) = (EMuid*)m_dataPtr;
            m_elemPtr += 4;
            memcpy(m_dataPtr, pValue, size);
            m_dataPtr += size;
            break;

        case Type_EGuid:
            if (m_dataPtr - m_dataBuf + ROUND4(size) > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size));
                if (FAILED(ec)) return ec;
            }
            *(EGuid**)(m_elemPtr) = (EGuid*)m_dataPtr;
            m_elemPtr += 4;
            memcpy(m_dataPtr, pValue, sizeof(EGuid));
			((EGuid*)m_dataPtr)->pUunm = (char*)(m_dataPtr + sizeof(EGuid));
            strcpy(((EGuid*)m_dataPtr)->pUunm, ((EGuid*)pValue)->pUunm);
            m_dataPtr += ROUND4(size);
            break;

        case Type_ArrayOf:
        case Type_BufferOf:
        case Type_MemoryBuf:
        case Type_StringBuf:
            if (pValue == NULL) {
                *(Byte**)(m_elemPtr) = NULL;
                m_elemPtr += 4;
            }
            else {
                if (m_dataPtr - m_dataBuf + ROUND4(size + 4) + 4 > m_dataBufCapacity) {
                    ec = GrowDataBuffer(ROUND4(size + 4) + 4);
                    if (FAILED(ec)) return ec;
                }
                *(Int32*)m_dataPtr = size;
                m_dataPtr += 4;
                *(Byte**)(m_elemPtr) = m_dataPtr;
                m_elemPtr += 4;
                memcpy(m_dataPtr, pValue, sizeof(CarQuintet));
                m_dataPtr += ROUND4(sizeof(CarQuintet));
                (*(CarQuintet**)(m_elemPtr - 4))->m_pBuf = (PVoid)(m_dataPtr);
                memcpy(m_dataPtr, ((CarQuintet*)pValue)->m_pBuf, size - sizeof(CarQuintet));
                m_dataPtr += ROUND4(size - sizeof(CarQuintet));
            }
            break;

        case Type_ArrayOfCString:
            used = ((ArrayOf<CString>*)pValue)->GetLength();
            if (m_dataPtr - m_dataBuf + ROUND4(size) + 4 > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }

            *(Int32*)m_dataPtr = size;
            m_dataPtr += 4;
            *(Byte**)(m_elemPtr) = m_dataPtr;
            m_elemPtr += 4;

            memcpy(m_dataPtr, pValue, sizeof(CarQuintet));
            m_dataPtr += ROUND4(sizeof(CarQuintet));
            (*(CarQuintet**)(m_elemPtr - 4))->m_pBuf = (PVoid)(m_dataPtr);
            m_dataPtr += ROUND4(((CarQuintet*)pValue)->m_size);

            for(Int32 i = 0; i < used; i++) {
                (**(ArrayOf<CString>**)(m_elemPtr - 4))[i] = (const char*)m_dataPtr;
                if ((*(ArrayOf<CString>*)pValue)[i]) {
                    len = (strlen((*(ArrayOf<CString>*)pValue)[i]) + 1);
                    memcpy((void*)(const char*)(**(ArrayOf<CString>**)(m_elemPtr - 4))[i],
                            (*(ArrayOf<CString>*)pValue)[i], len);
                    m_dataPtr += ROUND4(len);
                }
                else (**(ArrayOf<CString>**)(m_elemPtr - 4))[i] = NULL;
            }
            break;

        case Type_ArrayOfString:
            used = ((ArrayOf<String>*)pValue)->GetLength();
            if (m_dataPtr - m_dataBuf + ROUND4(size) + 4 > m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }

            *(Int32*)m_dataPtr = size;
            m_dataPtr += 4;
            *(Byte**)(m_elemPtr) = m_dataPtr;
            m_elemPtr += 4;

            memcpy(m_dataPtr, pValue, sizeof(CarQuintet));
            m_dataPtr += ROUND4(sizeof(CarQuintet));
            (*(CarQuintet**)(m_elemPtr - 4))->m_pBuf = (PVoid)(m_dataPtr);
            m_dataPtr += ROUND4(((CarQuintet*)pValue)->m_size);

            for(Int32 i = 0; i < used; i++) {
                (**(ArrayOf<String>**)(m_elemPtr - 4))[i] = (const char*)m_dataPtr;
                if ((*(ArrayOf<String>*)pValue)[i]) {
                    len = (strlen((*(ArrayOf<String>*)pValue)[i]) + 1);
                    memcpy((void*)(const char*)(**(ArrayOf<String>**)(m_elemPtr - 4))[i],
                            (*(ArrayOf<String>*)pValue)[i], len);
                    m_dataPtr += ROUND4(len);
                }
                else (**(ArrayOf<String>**)(m_elemPtr - 4))[i] = NULL;
            }
            break;

        case Type_BufferOfCString:
            used = ((BufferOf<CString>*)pValue)->GetUsed();
            if (m_dataPtr - m_dataBuf + ROUND4(size) + 4> m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }

            *(Int32*)m_dataPtr = size;
            m_dataPtr += 4;
            *(Byte**)(m_elemPtr) = m_dataPtr;
            m_elemPtr += 4;

            memcpy(m_dataPtr, pValue, sizeof(CarQuintet));
            m_dataPtr += ROUND4(sizeof(CarQuintet));
            (*(CarQuintet**)(m_elemPtr - 4))->m_pBuf = (PVoid)(m_dataPtr);
            m_dataPtr += ROUND4(((CarQuintet*)pValue)->m_size);

            for(Int32 i = 0; i < used; i++) {
                (**(BufferOf<CString>**)(m_elemPtr - 4))[i] = (const char*)m_dataPtr;
                if ((*(BufferOf<CString>*)pValue)[i]) {
                    len = (strlen((*(BufferOf<CString>*)pValue)[i]) + 1);
                    memcpy((void*)(const char*)(**(BufferOf<CString>**)(m_elemPtr - 4))[i],
                            (*(BufferOf<CString>*)pValue)[i], len);
                    m_dataPtr += ROUND4(len);
                }
                else (**(BufferOf<CString>**)(m_elemPtr - 4))[i] = NULL;
            }
            break;

        case Type_BufferOfString:
            used = ((BufferOf<String>*)pValue)->GetUsed();
            if (m_dataPtr - m_dataBuf + ROUND4(size) + 4> m_dataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }

            *(Int32*)m_dataPtr = size;
            m_dataPtr += 4;
            *(Byte**)(m_elemPtr) = m_dataPtr;
            m_elemPtr += 4;

            memcpy(m_dataPtr, pValue, sizeof(CarQuintet));
            m_dataPtr += ROUND4(sizeof(CarQuintet));
            (*(CarQuintet**)(m_elemPtr - 4))->m_pBuf = (PVoid)(m_dataPtr);
            m_dataPtr += ROUND4(((CarQuintet*)pValue)->m_size);

            for(Int32 i = 0; i < used; i++) {
                (**(BufferOf<String>**)(m_elemPtr - 4))[i] = (const char*)m_dataPtr;
                if ((*(BufferOf<String>*)pValue)[i]) {
                    len = (strlen((*(BufferOf<String>*)pValue)[i]) + 1);
                    memcpy((void*)(const char*)(**(BufferOf<String>**)(m_elemPtr - 4))[i],
                            (*(BufferOf<String>*)pValue)[i], len);
                    m_dataPtr += ROUND4(len);
                }
                else (**(BufferOf<String>**)(m_elemPtr - 4))[i] = NULL;
            }
            break;

        default:
            assert(0);
            break;
    }
	m_elemCount += 1;

    return NOERROR;
}

ECode CCallbackParcel::Clone(
    /* [in] */ IParcel *pSrcParcel)
{
    Byte type, bv, *pbv, *pSrcElemPtr;
    Int16 i16v, *pi16v;
    Int32 i32v, *pi32v, size;
    Int64 i64v, *pi64v;
    char *str;
    CCallbackParcel *pSrc;

    pSrc = (CCallbackParcel*)pSrcParcel;
    pSrcElemPtr = pSrc->m_elemBuf;
    for(Int32 i = 0; i < pSrc->m_elemCount; i++) {
        type = pSrc->m_elemTypes[i];
        switch(type) {
            case Type_Byte:
            case Type_Boolean:
            case Type_Char8:
                bv = *pSrcElemPtr;
                WriteValue((PVoid)&bv, type, sizeof(Byte));
                pSrcElemPtr += 4;
                break;

            case Type_Char16:
            case Type_Int16:
                i16v = *(Int16*)pSrcElemPtr;
                WriteValue((PVoid)&i16v, type, sizeof(Int16));
                pSrcElemPtr += 4;
                break;

            case Type_Char32:
            case Type_Int32:
            case Type_Float:
            case Type_InterfacePtr:
                i32v = *(Int32*)pSrcElemPtr;
                WriteValue((PVoid)&i32v, type, sizeof(Int32));
                pSrcElemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                pSrcElemPtr = (Byte*)ROUND8((Int32)pSrcElemPtr);
#endif
                i64v = *(UInt32*)(pSrcElemPtr + 4);
                i64v = (i64v << 32) | *(UInt32*)pSrcElemPtr;
                WriteValue((PVoid)&i64v, type, sizeof(Int64));
                pSrcElemPtr += 8;
                break;

            case Type_BytePtr:
            case Type_Char8Ptr:
            case Type_BooleanPtr:
                pbv = *(Byte**)pSrcElemPtr;
                WriteValue((PVoid)pbv, type, sizeof(Byte));
                pSrcElemPtr += 4;
                break;

            case Type_Char16Ptr:
            case Type_Int16Ptr:
                pi16v = *(Int16**)pSrcElemPtr;
                WriteValue((PVoid)pi16v, type, sizeof(Int16));
                pSrcElemPtr += 4;
                break;

            case Type_Int32Ptr:
            case Type_FloatPtr:
            case Type_InterfacePtrPtr:
                pi32v = *(Int32**)pSrcElemPtr;
                WriteValue((PVoid)pi32v, type, sizeof(Int32*));
                pSrcElemPtr += 4;
                break;

            case Type_Int64Ptr:
            case Type_DoublePtr:
                pi64v = *(Int64**)pSrcElemPtr;
                WriteValue((PVoid)pi64v, type, sizeof(Int64*));
                pSrcElemPtr += 4;
                break;

            case Type_CString:
            case Type_String:
                str = *(char**)pSrcElemPtr;
                if (str == NULL) {
                    size = 0;
                }
                else {
                    size = strlen(str) + 1;
                }
                WriteValue((PVoid)str, type, size);
                pSrcElemPtr += 4;
                break;

            case Type_Struct:
                size = *((*(Int32**)pSrcElemPtr) - 1);
                WriteValue((PVoid)(*(Byte**)pSrcElemPtr), type, size);
                pSrcElemPtr += 4;
                break;

            case Type_EMuid:
                WriteValue((PVoid)(*(EMuid**)pSrcElemPtr), type, sizeof(EMuid));
                pSrcElemPtr += 4;
                break;

            case Type_EGuid:
                WriteValue((PVoid)(*(EGuid**)pSrcElemPtr), type, sizeof(EGuid));
                pSrcElemPtr += 4;
                break;

            case Type_ArrayOf:
            case Type_BufferOf:
            case Type_MemoryBuf:
            case Type_StringBuf:
            case Type_ArrayOfCString:
            case Type_ArrayOfString:
            case Type_BufferOfCString:
            case Type_BufferOfString:
                size = *(Int32*)(*(Byte**)pSrcElemPtr - 4);
                WriteValue((PVoid)(*(CarQuintet**)pSrcElemPtr), type, size);
                pSrcElemPtr += 4;
                break;

        default:
            assert(0);
            break;
        }
    }

    return NOERROR;

}

ECode CCallbackParcel::ReadByte(
    /* [out] */ Byte *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBoolean(
    /* [out] */ Boolean *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar8(
    /* [out] */ Char8 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar16(
    /* [out] */ Char16 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar32(
    /* [out] */ Char32 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt16(
    /* [out] */ Int16 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt32(
    /* [out] */ Int32 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt64(
    /* [out] */ Int64 *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadFloat(
    /* [out] */ Float *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadDouble(
    /* [out] */ Double *pValue)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadCString(
    /* [out] */ CString *pStr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadString(
    /* [out] */ String *pStr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadStruct(
    /* [out] */ Handle32 *paddr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEMuid(EMuid *pId)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEGuid(EGuid *pId)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInterfacePtr(
    /* [out] */ Handle32 *pItfPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadArrayOf(
    /* [out] */ Handle32 *ppArray)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadArrayOfCString(
    /* [out, callee] */ ArrayOf<CString>** ppArray)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadArrayOfString(
    /* [out, callee] */ ArrayOf<String>** ppArray)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBufferOf(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBufferOfCString(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBufferOfString(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadMemoryBuf(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadStringBuf(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBytePtr(
    /* [out] */ Handle32 *bypp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBooleanPtr(
    /* [out] */ Handle32 *bopp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar8Ptr(
    /* [out] */ Handle32 *c8pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar16Ptr(
    /* [out] */ Handle32 *c16pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt16Ptr(
    /* [out] */ Handle32 *i16pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt32Ptr(
    /* [out] */ Handle32 *i32pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt64Ptr(
    /* [out] */ Handle32 *i64pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadFloatPtr(
    /* [out] */ Handle32 *fpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadDoublePtr(
    /* [out] */ Handle32 *dpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadStringPtr(
    /* [out] */ Handle32 *spp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadStructPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEMuidPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEGuidPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInterfacePtrPtr(
    /* [out] */ Handle32 *itfpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBufferOfPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadFileDescriptor(
    /* [out] */ Int32* fd)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteByte(Byte value)
{
    return WriteValue((PVoid)&value, Type_Byte, sizeof(Byte));
}

ECode CCallbackParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteValue((PVoid)&value, Type_Boolean, sizeof(Boolean));
}

ECode CCallbackParcel::WriteChar8(
    /* [in] */ Char8 value)
{
    return WriteValue((PVoid)&value, Type_Char8, sizeof(Char8));
}

ECode CCallbackParcel::WriteChar16(
    /* [in] */ Char16 value)
{
    return WriteValue((PVoid)&value, Type_Char16, sizeof(Char16));
}

ECode CCallbackParcel::WriteChar32(
    /* [in] */ Char32 value)
{
    return WriteValue((PVoid)&value, Type_Char32, sizeof(Char32));
}

ECode CCallbackParcel::WriteInt16(Int16 value)
{
    return WriteValue((PVoid)&value, Type_Int16, sizeof(Int16));
}

ECode CCallbackParcel::WriteInt32(Int32 value)
{
    return WriteValue((PVoid)&value, Type_Int32, sizeof(Int32));
}

ECode CCallbackParcel::WriteInt64(
    /* [in] */ Int64 value)
{
    return WriteValue((PVoid)&value, Type_Int64, sizeof(Int64));
}

ECode CCallbackParcel::WriteFloat(
    /* [in] */ Float value)
{
    return WriteValue((PVoid)&value, Type_Float, sizeof(Float));
}

ECode CCallbackParcel::WriteDouble(
    /* [in] */ Double value)
{
    return WriteValue((PVoid)&value, Type_Double, sizeof(Double));
}

ECode CCallbackParcel::WriteCString(CString str)
{
    Int32 size;

    if (str.IsNull()) {
        size = 0;
    }
    else if (str.IsEmpty()) {
        size = 1;
    }
    else {
        size = strlen(str) + 1;
    }

    return WriteValue((PVoid)(const char*)(str), Type_CString, size);
}

ECode CCallbackParcel::WriteString(const String& str)
{
    return WriteValue((PVoid)&str, Type_String, sizeof(String));
}

ECode CCallbackParcel::WriteBytePtr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_BytePtr, sizeof(Byte*));
}

ECode CCallbackParcel::WriteBooleanPtr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_BooleanPtr, sizeof(Boolean*));
}

ECode CCallbackParcel::WriteChar8Ptr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_Char8Ptr, sizeof(Char8*));
}

ECode CCallbackParcel::WriteChar16Ptr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_Char16Ptr, sizeof(Char16*));
}

ECode CCallbackParcel::WriteInt16Ptr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_Int16Ptr, sizeof(Int16*));
}

ECode CCallbackParcel::WriteInt32Ptr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_Int32Ptr, sizeof(Int32*));
}

ECode CCallbackParcel::WriteInt64Ptr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_Int64Ptr, sizeof(Int64*));
}

ECode CCallbackParcel::WriteFloatPtr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_FloatPtr, sizeof(float*));
}

ECode CCallbackParcel::WriteDoublePtr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_DoublePtr, sizeof(double*));
}

ECode CCallbackParcel::WriteStringPtr(
    /* [in] */ Handle32 addr)
{
    return WriteValue((PVoid)addr, Type_StringPtr, sizeof(String*));
}

ECode CCallbackParcel::WriteStruct(Handle32 pValue, Int32 size)
{
    return WriteValue((PVoid)pValue, Type_Struct, size);
}

ECode CCallbackParcel::WriteEMuid(const EMuid& id)
{
    return WriteValue((PVoid)&id, Type_EMuid, sizeof(EMuid));
}

ECode CCallbackParcel::WriteEGuid(const EGuid& id)
{
    return WriteValue((PVoid)&id, Type_EGuid, sizeof(EGuid) + strlen(id.pUunm) + 1);
}

ECode CCallbackParcel::WriteEMuidPtr(
    /* [in] */ Handle32 idPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteEGuidPtr(
    /* [in] */ Handle32 idPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteInterfacePtr(IInterface* pValue)
{
    return WriteValue((PVoid)&pValue, Type_InterfacePtr, sizeof(IInterface*));
}

ECode CCallbackParcel::WriteArrayOf(
    /* [in] */ Handle32 pArray)
{
    Int32 size = pArray != 0 ? sizeof(CarQuintet) + ((CarQuintet*)pArray)->m_size : 0;
    return WriteValue((PVoid)pArray, Type_ArrayOf, size);
}

ECode CCallbackParcel::WriteArrayOfCString(
    /* [in] */ ArrayOf<CString>* array)
{
    Int32 size = sizeof(CarQuintet) + array->m_size;
    Int32 len = array->GetLength();

    for (Int32 i = 0; i < len; i++) {
        if(!(*array)[i].IsNull())
            size += strlen((*array)[i]) + 1;
    }

    return WriteValue((PVoid)array, Type_ArrayOfCString, size);
}

ECode CCallbackParcel::WriteArrayOfString(
    /* [in] */ ArrayOf<String>* array)
{
    Int32 size = sizeof(CarQuintet) + array->m_size;
    Int32 len = array->GetLength();

    for (Int32 i = 0; i < len; i++) {
        if(!(*array)[i].IsNull())
            size += strlen((*array)[i]) + 1;
    }

    return WriteValue((PVoid)array, Type_ArrayOfString, size);
}

ECode CCallbackParcel::WriteBufferOf(
    /* [in] */ Handle32 pBuffer)
{
    Int32 size = sizeof(CarQuintet) + ((CarQuintet*)pBuffer)->m_size;
    return WriteValue((PVoid)pBuffer, Type_BufferOf, size);
}

ECode CCallbackParcel::WriteBufferOfCString(
    /* [in] */ const BufferOf<CString> & pBuffer)
{
    Int32 size = sizeof(CarQuintet) + pBuffer.m_size;
    Int32 used = pBuffer.GetUsed();

    for (Int32 i = 0; i < used; i++) {
        if(!pBuffer[i].IsNull())
            size += strlen(pBuffer[i]) + 1;
    }

    return WriteValue((PVoid)&pBuffer, Type_BufferOfCString, size);
}

ECode CCallbackParcel::WriteBufferOfString(
    /* [in] */ const BufferOf<String> & pBuffer)
{
    Int32 size = sizeof(CarQuintet) + pBuffer.m_size;
    Int32 used = pBuffer.GetUsed();

    for (Int32 i = 0; i < used; i++) {
        if(!pBuffer[i].IsNull())
            size += strlen(pBuffer[i]) + 1;
    }

    return WriteValue((PVoid)&pBuffer, Type_BufferOfString, size);
}

ECode CCallbackParcel::WriteMemoryBuf(
    /* [in] */ Handle32 pBuffer)
{
    Int32 size = sizeof(CarQuintet) + ((CarQuintet*)pBuffer)->m_size;
    return WriteValue((PVoid)pBuffer, Type_MemoryBuf, size);
}

ECode CCallbackParcel::WriteStringBuf(
    /* [in] */ Handle32 pBuffer)
{
    Int32 size = sizeof(CarQuintet) + ((CarQuintet*)pBuffer)->m_used;
    return WriteValue((PVoid)pBuffer, Type_StringBuf, size);
}

ECode CCallbackParcel::WriteInterfacePtrPtr(
    /* [in] */ Handle32 addr2)
{
    return WriteValue((PVoid)addr2, Type_InterfacePtrPtr, sizeof(IInterface*));
}

ECode CCallbackParcel::WriteFileDescriptor(
    /* [in] */ Int32 fd)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteDupFileDescriptor(
    /* [in] */ Int32 fd)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::GetDataPosition(
    /* [out] */ Int32* position)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::SetDataPosition(
    /* [in] */ Int32 position)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::GetElementPayload(
    /* [out] */ Handle32* pBuffer)
{
    if (NULL == pBuffer) return E_INVALID_ARGUMENT;

    *pBuffer = (Handle32)m_elemBuf;

    return NOERROR;
}

ECode CCallbackParcel::GetElementSize(
    /* [in] */ Int32* pSize)
{
    if (NULL == pSize) return E_INVALID_ARGUMENT;

    *pSize = (Int32)(m_elemPtr - m_elemBuf);

    return NOERROR;
}

ELAPI _CCallbackParcel_New(
    /* [out] */ IParcel **ppObj)
{
    if (ppObj == NULL) return E_INVALID_ARGUMENT;
    CCallbackParcel *pObj = NULL;
    void* pLocation = calloc(sizeof(CCallbackParcel), 1);
    if (!pLocation) return E_OUT_OF_MEMORY;

    pObj = (CCallbackParcel *)new(pLocation) CCallbackParcel;
    pObj->AddRef();

    *ppObj = (IParcel*)pObj;

    return NOERROR;
}
