//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CCallbackParcel.h"

#define ROUND4(n)       (((n)+3)&~3)   // round up to multiple of 4 bytes
#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

_ELASTOS_NAMESPACE_BEGIN

enum Type {
    Type_Unknown = 0,

    Type_Byte, Type_Boolean, Type_Char32,
    Type_Int16, Type_Int32, Type_Int64, Type_Float, Type_Double,
    Type_String, Type_Struct, Type_EMuid, Type_EGuid,
    Type_ArrayOf, Type_ArrayOfString, Type_InterfacePtr,
};

CCallbackParcel::CCallbackParcel()
{
    mElemCount = 0;

    mTypeBufCapacity = 10;
    mElemTypes = (Byte*)malloc(mTypeBufCapacity);
    memset(mElemTypes, 0, mTypeBufCapacity);

    mElemBufCapacity = 128;
    mElemBuf = (Byte*)malloc(mElemBufCapacity);
    memset(mElemBuf, 0, mElemBufCapacity);
    mElemPtr = mElemBuf;

    mDataBufCapacity = 128;
    mDataBuf = (Byte*)malloc(mDataBufCapacity);
    memset(mDataBuf, 0, mDataBufCapacity);
    mDataPtr = mDataBuf;
}

CCallbackParcel::~CCallbackParcel()
{
    mElemPtr = mElemBuf;

    for (Int32 i = 0; i < mElemCount; i++) {
        switch(mElemTypes[i]) {
            case Type_InterfacePtr:
                if (*(IInterface**)mElemPtr != NULL) {
                    (*(IInterface**)mElemPtr)->Release();
                }
                mElemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                mElemPtr = (Byte*)ROUND8((Int32)mElemPtr);
#endif
                mElemPtr += 8;
                break;

            case Type_String: {
                String* p = *(String**)mElemPtr;
                delete p;
                *(String**)mElemPtr = NULL;
                break;
            }

            default:
                mElemPtr += 4;
                break;
        }
    }

    free(mElemTypes);
    free(mElemBuf);
    free(mDataBuf);
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
    return ElLightRefBase::AddRef();
}

UInt32 CCallbackParcel::Release()
{
    return ElLightRefBase::Release();
}

ECode CCallbackParcel::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (NULL == iid) return E_INVALID_ARGUMENT;

    if (object == (IInterface *)(IParcel *)this) {
        *iid = EIID_IParcel;
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
    mTypeBufCapacity +=  10;
    mElemTypes = (Byte*)realloc(mElemTypes, mTypeBufCapacity);
    if (NULL == mElemTypes) return E_OUT_OF_MEMORY;

    return NOERROR;
}

ECode CCallbackParcel::GrowElemBuffer()
{
    int offset = mElemPtr - mElemBuf;
    mElemBufCapacity += 128;
    mElemBuf = (Byte*)realloc(mElemBuf, mElemBufCapacity);
    if (NULL == mElemBuf) return E_OUT_OF_MEMORY;
    mElemPtr = mElemBuf + offset;

    return NOERROR;
}

ECode CCallbackParcel::GrowDataBuffer(
    /* [in] */ Int32 minSize)
{
    Byte type, *tmpPtr, *orgDataBuf, *elemPtr;
    int offset1, offset2, offset3, used;

    orgDataBuf = mDataBuf;
    elemPtr = mElemBuf;
    offset1 = mDataPtr - mDataBuf;
    if(minSize < 128) {
        minSize = 128;
    }
    else {
        minSize += 128;
    }
    mDataBufCapacity += minSize;
    mDataBuf = (Byte*)malloc(mDataBufCapacity);
    if (NULL == mDataBuf) return E_OUT_OF_MEMORY;
    memcpy(mDataBuf, orgDataBuf, offset1);
    mDataPtr = mDataBuf + offset1;

    for(Int32 i = 0; i < mElemCount; i++) {
        type = mElemTypes[i];
        switch(type) {
            case Type_Byte:
            case Type_Boolean:
            case Type_Char32:
            case Type_Int16:
            case Type_Int32:
            case Type_Float:
            case Type_InterfacePtr:
                elemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                elemPtr = (Byte*)ROUND8((Int32)elemPtr);
#endif
                elemPtr += 8;
                break;

            case Type_String:
            case Type_Struct:
            case Type_EMuid:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                *(Byte**)elemPtr = mDataBuf + offset1;
                elemPtr += 4;
                break;

            case Type_EGuid:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(EGuid**)elemPtr)->mUunm - orgDataBuf;
                *(Byte**)elemPtr = mDataBuf + offset1;
                (*(EGuid**)elemPtr)->mUunm = (char*)(mDataBuf + offset2);
                elemPtr += 4;
                break;

            case Type_ArrayOf:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(CarQuintet**)(elemPtr))->mBuf - orgDataBuf;
                *(Byte**)elemPtr = mDataBuf + offset1;
                (*(CarQuintet**)(elemPtr))->mBuf = (void*)(mDataBuf + offset2);
                elemPtr += 4;
                break;

            case Type_ArrayOfString:
                offset1 = *(Byte**)elemPtr - orgDataBuf;
                offset2 = (Byte*)(*(CarQuintet**)(elemPtr))->mBuf - orgDataBuf;
                tmpPtr = mDataBuf + offset1;
                ((CarQuintet*)tmpPtr)->mBuf = mDataBuf + offset2;
                used = (*(ArrayOf<String>**)elemPtr)->GetLength();

                for (Int32 i = 0; i < used; i++) {
                    if ((**(ArrayOf<String>**)elemPtr)[i]) {
                        offset3 = (Byte*)(const char*)((**(ArrayOf<String>**)elemPtr)[i]) - orgDataBuf;
                        (*(ArrayOf<String>*)tmpPtr)[i] = (char*)(mDataBuf + offset3);
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

ECode CCallbackParcel::WriteValue(
    /* [in] */ PVoid value,
    /* [in] */ Int32 type,
    /* [in] */ Int32 size)
{
    ECode ec;
    Int32 used, len;

    if (mElemCount >= mTypeBufCapacity) {
        ec = GrowTypeBuffer();
        if (FAILED(ec)) return ec;
    }
    if (mElemPtr - mElemBuf + 4 > mElemBufCapacity) {
        ec = GrowElemBuffer();
        if (FAILED(ec)) return ec;
    }

    mElemTypes[mElemCount] = (Byte)type;
    switch(type) {
        case Type_Byte:
        case Type_Boolean:
            *(Int32*)(mElemPtr) = *((Byte*)value);
            mElemPtr += 4;

            break;

        case Type_Int16:
            *(Int32*)(mElemPtr) = *((Int16*)value);
            mElemPtr += 4;
            break;

        case Type_Char32:
        case Type_Int32:
        case Type_Float:
            *(Int32*)(mElemPtr) = *(Int32*)value;
            mElemPtr += 4;
            break;

        case Type_Int64:
        case Type_Double:
            if (mElemPtr - mElemBuf + 4 + 4 > mElemBufCapacity) {
                ec = GrowElemBuffer();
                if (FAILED(ec)) return ec;
            }
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
            mElemPtr = (Byte*)ROUND8((Int32)mElemPtr);
#endif
            *(Int32*)(mElemPtr) = (Int32)(*((Int64*)value) & 0xffffffff);
            *(Int32*)(mElemPtr + 4) = (Int32)((*((Int64*)value) >> 32) & 0xffffffff);
            mElemPtr += 8;
            break;

        case Type_String: {
            String* p = new String();
            *p = *(String*)value;
            *(String**)mElemPtr = p;
            mElemPtr += 4;
            break;
        }
        case Type_InterfacePtr:
            *(IInterface**)mElemPtr = *(IInterface**)value;
            if ((*(IInterface**)mElemPtr) != NULL) {
                (*(IInterface**)mElemPtr)->AddRef();
            }
            mElemPtr += 4;
            break;

        case Type_Struct:
            if (mDataPtr - mDataBuf + ROUND4(size) + 4 > mDataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }
            *(Int32*)mDataPtr = size;
            mDataPtr += 4;
            *(Byte**)(mElemPtr) = mDataPtr;
            mElemPtr += 4;
            memcpy(mDataPtr, value, size);
            mDataPtr += ROUND4(size);
            break;

        case Type_EMuid:
            if (mDataPtr - mDataBuf + ROUND4(size) > mDataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size));
                if (FAILED(ec)) return ec;
            }
            *(EMuid**)(mElemPtr) = (EMuid*)mDataPtr;
            mElemPtr += 4;
            memcpy(mDataPtr, value, size);
            mDataPtr += size;
            break;

        case Type_EGuid:
            if (mDataPtr - mDataBuf + ROUND4(size) > mDataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size));
                if (FAILED(ec)) return ec;
            }
            *(EGuid**)(mElemPtr) = (EGuid*)mDataPtr;
            mElemPtr += 4;
            memcpy(mDataPtr, value, sizeof(EGuid));
            ((EGuid*)mDataPtr)->mUunm = (char*)(mDataPtr + sizeof(EGuid));
            strcpy(((EGuid*)mDataPtr)->mUunm, ((EGuid*)value)->mUunm);
            mDataPtr += ROUND4(size);
            break;

        case Type_ArrayOf:
            if (value == NULL) {
                *(Byte**)(mElemPtr) = NULL;
                mElemPtr += 4;
            }
            else {
                if (mDataPtr - mDataBuf + ROUND4(size + 4) + 4 > mDataBufCapacity) {
                    ec = GrowDataBuffer(ROUND4(size + 4) + 4);
                    if (FAILED(ec)) return ec;
                }
                *(Int32*)mDataPtr = size;
                mDataPtr += 4;
                *(Byte**)(mElemPtr) = mDataPtr;
                mElemPtr += 4;
                memcpy(mDataPtr, value, sizeof(CarQuintet));
                mDataPtr += ROUND4(sizeof(CarQuintet));
                (*(CarQuintet**)(mElemPtr - 4))->mBuf = (PVoid)(mDataPtr);
                memcpy(mDataPtr, ((CarQuintet*)value)->mBuf, size - sizeof(CarQuintet));
                mDataPtr += ROUND4(size - sizeof(CarQuintet));
            }
            break;

        case Type_ArrayOfString:
            used = ((ArrayOf<String>*)value)->GetLength();
            if (mDataPtr - mDataBuf + ROUND4(size) + 4 > mDataBufCapacity) {
                ec = GrowDataBuffer(ROUND4(size) + 4);
                if (FAILED(ec)) return ec;
            }

            *(Int32*)mDataPtr = size;
            mDataPtr += 4;
            *(Byte**)(mElemPtr) = mDataPtr;
            mElemPtr += 4;

            memcpy(mDataPtr, value, sizeof(CarQuintet));
            mDataPtr += ROUND4(sizeof(CarQuintet));
            (*(CarQuintet**)(mElemPtr - 4))->mBuf = (PVoid)(mDataPtr);
            mDataPtr += ROUND4(((CarQuintet*)value)->mSize);

            for(Int32 i = 0; i < used; i++) {
                (**(ArrayOf<String>**)(mElemPtr - 4))[i] = (const char*)mDataPtr;
                if ((*(ArrayOf<String>*)value)[i]) {
                    len = (strlen((*(ArrayOf<String>*)value)[i]) + 1);
                    memcpy((void*)(const char*)(**(ArrayOf<String>**)(mElemPtr - 4))[i],
                            (*(ArrayOf<String>*)value)[i], len);
                    mDataPtr += ROUND4(len);
                }
                else (**(ArrayOf<String>**)(mElemPtr - 4))[i] = NULL;
            }
            break;

        default:
            assert(0);
            break;
    }
    mElemCount += 1;

    return NOERROR;
}

ECode CCallbackParcel::Clone(
    /* [in] */ IParcel* srcParcel)
{
    Byte type, bv, *srcElemPtr;
    Int16 i16v;
    Int32 i32v, size;
    Int64 i64v;
    char *str;
    CCallbackParcel *src;

    src = (CCallbackParcel*)srcParcel;
    srcElemPtr = src->mElemBuf;
    for(Int32 i = 0; i < src->mElemCount; i++) {
        type = src->mElemTypes[i];
        switch(type) {
            case Type_Byte:
            case Type_Boolean:
                bv = *srcElemPtr;
                WriteValue((PVoid)&bv, type, sizeof(Byte));
                srcElemPtr += 4;
                break;

            case Type_Int16:
                i16v = *(Int16*)srcElemPtr;
                WriteValue((PVoid)&i16v, type, sizeof(Int16));
                srcElemPtr += 4;
                break;

            case Type_Char32:
            case Type_Int32:
            case Type_Float:
            case Type_InterfacePtr:
                i32v = *(Int32*)srcElemPtr;
                WriteValue((PVoid)&i32v, type, sizeof(Int32));
                srcElemPtr += 4;
                break;

            case Type_Int64:
            case Type_Double:
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                srcElemPtr = (Byte*)ROUND8((Int32)srcElemPtr);
#endif
                i64v = *(UInt32*)(srcElemPtr + 4);
                i64v = (i64v << 32) | *(UInt32*)srcElemPtr;
                WriteValue((PVoid)&i64v, type, sizeof(Int64));
                srcElemPtr += 8;
                break;

            case Type_String:
                str = *(char**)srcElemPtr;
                if (str == NULL) {
                    size = 0;
                }
                else {
                    size = strlen(str) + 1;
                }
                WriteValue((PVoid)str, type, size);
                srcElemPtr += 4;
                break;

            case Type_Struct:
                size = *((*(Int32**)srcElemPtr) - 1);
                WriteValue((PVoid)(*(Byte**)srcElemPtr), type, size);
                srcElemPtr += 4;
                break;

            case Type_EMuid:
                WriteValue((PVoid)(*(EMuid**)srcElemPtr), type, sizeof(EMuid));
                srcElemPtr += 4;
                break;

            case Type_EGuid:
                WriteValue((PVoid)(*(EGuid**)srcElemPtr), type, sizeof(EGuid));
                srcElemPtr += 4;
                break;

            case Type_ArrayOf:
            case Type_ArrayOfString:
                size = *(Int32*)(*(Byte**)srcElemPtr - 4);
                WriteValue((PVoid)(*(CarQuintet**)srcElemPtr), type, size);
                srcElemPtr += 4;
                break;

        default:
            assert(0);
            break;
        }
    }

    return NOERROR;

}

ECode CCallbackParcel::ReadByte(
    /* [out] */ Byte* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadBoolean(
    /* [out] */ Boolean* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadChar(
    /* [out] */ Char32* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt16(
    /* [out] */ Int16* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt32(
    /* [out] */ Int32* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInt64(
    /* [out] */ Int64* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadFloat(
    /* [out] */ Float* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadDouble(
    /* [out] */ Double* value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadStruct(
    /* [out] */ HANDLE* addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEMuid(
    /* [in] */ EMuid* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadEGuid(
    /* [in] */ EGuid* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadInterfacePtr(
    /* [out] */ HANDLE* itfPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadArrayOf(
    /* [out] */ HANDLE* array)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadArrayOfString(
    /* [out, callee] */ ArrayOf<String>** array)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadFileDescriptor(
    /* [out] */ Int32* fd)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::ReadValue(
    /* [out] */ IInterface** value)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::WriteByte(
    /* [in] */ Byte value)
{
    return WriteValue((PVoid)&value, Type_Byte, sizeof(Byte));
}

ECode CCallbackParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteValue((PVoid)&value, Type_Boolean, sizeof(Boolean));
}

ECode CCallbackParcel::WriteChar(
    /* [in] */ Char32 value)
{
    return WriteValue((PVoid)&value, Type_Char32, sizeof(Char32));
}

ECode CCallbackParcel::WriteInt16(
    /* [in] */ Int16 value)
{
    return WriteValue((PVoid)&value, Type_Int16, sizeof(Int16));
}

ECode CCallbackParcel::WriteInt32(
    /* [in] */ Int32 value)
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

ECode CCallbackParcel::WriteString(
    /* [in] */ const String& str)
{
    return WriteValue((PVoid)&str, Type_String, sizeof(String));
}

ECode CCallbackParcel::WriteStruct(
    /* [in] */ HANDLE value,
    /* [in] */ Int32 size)
{
    return WriteValue((PVoid)value, Type_Struct, size);
}

ECode CCallbackParcel::WriteEMuid(
    /* [in] */ const EMuid& id)
{
    return WriteValue((PVoid)&id, Type_EMuid, sizeof(EMuid));
}

ECode CCallbackParcel::WriteEGuid(
    /* [in] */ const EGuid& id)
{
    return WriteValue((PVoid)&id, Type_EGuid, sizeof(EGuid) + strlen(id.mUunm) + 1);
}

ECode CCallbackParcel::WriteInterfacePtr(
    /* [in] */ IInterface* value)
{
    return WriteValue((PVoid)&value, Type_InterfacePtr, sizeof(IInterface*));
}

ECode CCallbackParcel::WriteArrayOf(
    /* [in] */ HANDLE pArray)
{
    Int32 size = pArray != 0 ? sizeof(CarQuintet) + ((CarQuintet*)pArray)->mSize : 0;
    return WriteValue((PVoid)pArray, Type_ArrayOf, size);
}

ECode CCallbackParcel::WriteArrayOfString(
    /* [in] */ ArrayOf<String>* array)
{
    Int32 size = sizeof(CarQuintet) + array->mSize;
    Int32 len = array->GetLength();

    for (Int32 i = 0; i < len; i++) {
        if(!(*array)[i].IsNull())
            size += strlen((*array)[i]) + 1;
    }

    return WriteValue((PVoid)array, Type_ArrayOfString, size);
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

ECode CCallbackParcel::GetDataPayload(
    /* [out] */ HANDLE* buffer)
{
    if (NULL == buffer) return E_INVALID_ARGUMENT;

    *buffer = (HANDLE)mElemBuf;

    return NOERROR;
}

ECode CCallbackParcel::GetDataSize(
    /* [in] */ Int32* size)
{
    if (NULL == size) return E_INVALID_ARGUMENT;

    *size = (Int32)(mElemPtr - mElemBuf);

    return NOERROR;
}

ECode CCallbackParcel::SetDataSize(
    /* [in] */ Int32 size)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::GetDataCapacity(
    /* [out] */ Int32* capacity)
{
    return E_NOT_IMPLEMENTED;
}

ECode CCallbackParcel::SetDataCapacity(
    /* [in] */ Int32 capacity)
{
    return E_NOT_IMPLEMENTED;
}

_ELASTOS_NAMESPACE_END

ELAPI _CCallbackParcel_New(
    /* [out] */ IParcel** parcel)
{
    if (parcel == NULL) return E_INVALID_ARGUMENT;
    Elastos::CCallbackParcel* obj = NULL;
    void* location = calloc(sizeof(Elastos::CCallbackParcel), 1);
    if (!location) return E_OUT_OF_MEMORY;

    obj = (Elastos::CCallbackParcel *)new(location) Elastos::CCallbackParcel;

    *parcel = (IParcel*)obj;
    (*parcel)->AddRef();
    return NOERROR;
}
