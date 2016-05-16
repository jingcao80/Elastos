
#include "CRemoteParcel.h"
#include <cutils/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/endian.h>

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo **ppClassInfo);

#ifdef __cplusplus
template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};
#define COMPILE_TIME_ASSERT(_exp) \
    template class CompileTimeAssert< (_exp) >;
#endif
#define COMPILE_TIME_ASSERT_FUNCTION_SCOPE(_exp) \
    CompileTimeAssert<( _exp )>();

#define PAD_SIZE(s) (((s) + 3) & ~3)

namespace Elastos {
namespace RPC {

ECode GetRemoteClassInfo(
    /* [in] */ const String& netAddress,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo ** ppClassInfo);

CRemoteParcel::CRemoteParcel(
    /* [in] */ Boolean writeMarshalHeader)
{
    InitState();
    if (writeMarshalHeader) {
        WriteInplace(sizeof(MarshalHeader));
    }
}

CRemoteParcel::~CRemoteParcel()
{
    FreeDataNoInit();
}

PInterface CRemoteParcel::Probe(
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

UInt32 CRemoteParcel::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CRemoteParcel::Release()
{
    return ElRefBase::Release();
}

ECode CRemoteParcel::GetInterfaceID(
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

const UInt8* CRemoteParcel::Data() const
{
    return mData;
}

UInt32 CRemoteParcel::DataSize() const
{
    return (mDataSize > mDataPos ? mDataSize : mDataPos);
}

UInt32 CRemoteParcel::DataAvail() const
{
    // TODO: decide what to do about the possibility that this can
    // report an available-data size that exceeds a Java int's max
    // positive value, causing havoc.  Fortunately this will only
    // happen if someone constructs a Parcel containing more than two
    // gigabytes of data, which on typical phone hardware is simply
    // not possible.
    return DataSize() - DataPosition();
}

UInt32 CRemoteParcel::DataPosition() const
{
    return mDataPos;
}

UInt32 CRemoteParcel::DataCapacity() const
{
    return mDataCapacity;
}

ECode CRemoteParcel::SetDataSize(
    /* [in] */ UInt32 size)
{
    ECode ec = ContinueWrite(size);
    if (SUCCEEDED(ec)) {
        mDataSize = size;
        ALOGV("setDataSize Setting data size of %p to %d\n", this, mDataSize);
    }
    return ec;
}

void CRemoteParcel::SetDataPositionInner(
    /* [in] */ UInt32 pos) const
{
    mDataPos = pos;
    mNextObjectHint = 0;
}

ECode CRemoteParcel::SetDataCapacity(
    /* [in] */ UInt32 size)
{
    if (size > mDataCapacity) return ContinueWrite(size);
    return NOERROR;
}

ECode CRemoteParcel::SetData(
    /* [in] */ const UInt8* buffer,
    /* [in] */ UInt32 len)
{
    ECode ec = RestartWrite(len);
    if (SUCCEEDED(ec)) {
        memcpy(const_cast<UInt8*>(Data()), buffer, len);
        mDataSize = len;
    }
    return ec;
}

ECode CRemoteParcel::HasFileDescriptors(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
}

UInt32 CRemoteParcel::ObjectsCount() const
{
    return mObjectsSize;
}

ECode CRemoteParcel::FinishWrite(
    /* [in] */ UInt32 len)
{
    //printf("Finish write of %d\n", len);
    mDataPos += len;
    ALOGV("finishWrite Setting data pos of %p to %d\n", this, mDataPos);
    if (mDataPos > mDataSize) {
        mDataSize = mDataPos;
        ALOGV("finishWrite Setting data size of %p to %d\n", this, mDataSize);
    }
    //printf("New pos=%d, size=%d\n", mDataPos, mDataSize);
    return NOERROR;
}

ECode CRemoteParcel::Write(
    /* [in] */ const void* data,
    /* [in] */ UInt32 len)
{
    void* const d = WriteInplace(len);
    if (d) {
        memcpy(d, data, len);
        return NOERROR;
    }
    return mError;
}

void* CRemoteParcel::WriteInplace(
    /* [in] */ UInt32 len)
{
    const UInt32 padded = PAD_SIZE(len);

    // sanity check for integer overflow
    if (mDataPos + padded < mDataPos) {
        return NULL;
    }

    if ((mDataPos + padded) <= mDataCapacity) {
restart_write:
        //printf("Writing %ld bytes, padded to %ld\n", len, padded);
        UInt8* const data = mData + mDataPos;

        // Need to pad at end?
        if (padded != len) {
#if BYTE_ORDER == BIG_ENDIAN
            static const UInt32 mask[4] = {
                0x00000000, 0xffffff00, 0xffff0000, 0xff000000
            };
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
            static const UInt32 mask[4] = {
                0x00000000, 0x00ffffff, 0x0000ffff, 0x000000ff
            };
#endif
            //printf("Applying pad mask: %p to %p\n", (void*)mask[padded-len],
            //    *reinterpret_cast<void**>(data+padded-4));
            *reinterpret_cast<UInt32*>(data + padded - 4) &= mask[padded - len];
        }

        FinishWrite(padded);
        return data;
    }

    ECode err = GrowData(padded);
    if (err == NOERROR) goto restart_write;
    return NULL;
}

ECode CRemoteParcel::WriteCStringInner(
    /* [in] */ const char* str)
{
    return Write(str, strlen(str) + 1);
}

ECode CRemoteParcel::WriteByte(
    /* [in] */ Byte value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteChar8(
    /* [in] */ Char8 value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteChar16(
    /* [in] */ Char16 value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteChar32(
    /* [in] */ Char32 value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteInt16(
    /* [in] */ Int16 value)
{
    return WriteInt32((Int32)value);
}

ECode CRemoteParcel::WriteInt32(
    /* [in] */ Int32 value)
{
    return WriteAligned(htonl(value));
}

ECode CRemoteParcel::WriteInt64(
    /* [in] */ Int64 value)
{
    return WriteAligned(value);
}

ECode CRemoteParcel::WriteFloat(
    /* [in] */ Float value)
{
    return WriteAligned(value);
}

ECode CRemoteParcel::WriteDouble(
    /* [in] */ Double value)
{
    return WriteDouble(value);
}

ECode CRemoteParcel::WriteCString(
    /* [in] */ CString str)
{
    if (str.IsNull()) {
        return WriteInt32(MSH_NULL);
    }

    WriteInt32(MSH_NOT_NULL);
    return WriteCStringInner((const char*)str);
}

ECode CRemoteParcel::WriteString(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return WriteInt32(MSH_NULL);
    }

    WriteInt32(MSH_NOT_NULL);
    return WriteCStringInner(str.string());
}

ECode CRemoteParcel::WriteStruct(
    /* [in] */ Handle32 pValue,
    /* [in] */ Int32 size)
{
    if (pValue == 0) {
        return WriteInt32(MSH_NULL);
    }

    WriteInt32(MSH_NOT_NULL);
    WriteInt32(size);
    return Write((void*)pValue, size);
}

ECode CRemoteParcel::WriteEMuid(
    /* [in] */ const EMuid & id)
{
    return Write((void*)&id, sizeof(EMuid));
}

ECode CRemoteParcel::WriteEGuid(
    /* [in] */ const EGuid & id)
{
    ECode ec = Write(&id, sizeof(EGuid));
    if (FAILED(ec)) return ec;
    return WriteCStringInner(id.pUunm);
}

ECode CRemoteParcel::WriteArrayOf(
    /* [in] */ Handle32 pArray)
{
    return WriteArrayOfInner((PCARQUINTET)pArray);
}

ECode CRemoteParcel::WriteArrayOfCString(
    /* [in] */ ArrayOf<CString>* array)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteArrayOfString(
    /* [in] */ ArrayOf<String>* array)
{
    if (array == NULL) {
        return WriteInt32(MSH_NULL);
    }

    Write((void*)array, sizeof(CarQuintet));

    Int32 size = ((PCARQUINTET)array)->m_size
                / sizeof(String);
    String *pBuf = (String*)((PCARQUINTET)array)->m_pBuf;
    for (Int32 i = 0; i < size; i++) {
        if (!pBuf[i].IsNull()) {
            WriteInt32(MSH_NOT_NULL);
            WriteCStringInner(pBuf[i].string());
        }
        else {  // null pointer
            WriteInt32(MSH_NULL);
        }
    }
    return NOERROR;
}

ECode CRemoteParcel::WriteBufferOf(
    /* [in] */ Handle32 pBuffer)
{
    return WriteArrayOfInner((PCARQUINTET)pBuffer);
}

ECode CRemoteParcel::WriteBufferOfCString(
    /* [in] */ const BufferOf<CString> & pBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteBufferOfString(
    /* [in] */ const BufferOf<String> & pBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteStringBuf(
    /* [in] */ Handle32 pBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteMemoryBuf(
    /* [in] */ Handle32 pBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteArrayOfInner(
    /* [in] */ PCARQUINTET pValue)
{
    if (pValue == NULL) {
        return WriteInt32(MSH_NULL);
    }

    WriteInt32(MSH_NOT_NULL);
    Write((void*)pValue, sizeof(CarQuintet));

    if (CarQuintetFlag_Type_IObject
            != (((PCARQUINTET)pValue)->m_flags
                    & CarQuintetFlag_TypeMask)) {
        // copy the storaged data
        //
        if (CarQuintetFlag_Type_String == (((PCARQUINTET)pValue)->m_flags
                & CarQuintetFlag_TypeMask)) {
            Int32 size = ((PCARQUINTET)pValue)->m_size / sizeof(String);
            String* pBuf = (String*)((PCARQUINTET)pValue)->m_pBuf;
            for (Int32 i = 0; i < size; ++i) {
                // ALOGD("i: %d, str: %s", i, pBuf[i].string());
                if (!pBuf[i].IsNull()) {
                    WriteInt32(MSH_NOT_NULL);
                    WriteCStringInner(pBuf[i].string());
                }
                else {  // null pointer
                    WriteInt32(MSH_NULL);
                }
            }
        }
        else {
            Write(((PCARQUINTET)pValue)->m_pBuf,
                    ((PCARQUINTET)pValue)->m_size);
        }
    }
    else {
        Int32 size = ((PCARQUINTET)pValue)->m_size
                    / sizeof(IInterface *);
        Int32 *pBuf = (Int32*)((PCARQUINTET)pValue)->m_pBuf;
        for (Int32 i = 0; i < size; i++) {
            if (pBuf[i]) {
                WriteInt32(MSH_NOT_NULL);

                InterfacePack itfPack;
                ECode ec = StdMarshalInterface(
                        (IInterface *)pBuf[i],
                        &itfPack);
                if (FAILED(ec)) {
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "CRemoteParcel: marshal interface, ec = %x\n", ec));
                    return ec;
                }

                WriteString(itfPack.m_sNetAddress);
                itfPack.m_sNetAddress = NULL;
                Write((void*)&itfPack, sizeof(itfPack));

                IParcelable *pParcelable = \
                        (IParcelable*)((IInterface*)pBuf[i])->Probe(EIID_IParcelable);
                if (pParcelable != NULL) pParcelable->WriteToParcel(this);
            }
            else {  // null pointer
                WriteInt32(MSH_NULL);
            }
        }
    }
    return NOERROR;
}

ECode CRemoteParcel::WriteInterfacePtr(
    /* [in] */ IInterface* pValue)
{
    Boolean enoughData = (mDataPos + sizeof(UInt32) + sizeof(InterfacePack)) <= mDataCapacity;
    Boolean enoughObjects = mObjectsSize < mObjectsCapacity;
    if (enoughData && enoughObjects) {
restart_write:
        *reinterpret_cast<Int32*>(mData + mDataPos) = pValue ? MSH_NOT_NULL : MSH_NULL;
        FinishWrite(sizeof(UInt32));
        if (pValue) {
            ECode ec = StdMarshalInterface(pValue,
                    reinterpret_cast<InterfacePack*>(mData + mDataPos));
            if (FAILED(ec)) return ec;

            FinishWrite(sizeof(InterfacePack));
            IParcelable *pParcelable = \
                    (IParcelable*)((IInterface*)pValue)->Probe(EIID_IParcelable);
            if (pParcelable != NULL) pParcelable->WriteToParcel(this);
        }

        mObjects[mObjectsSize] = mDataPos;
        mObjectsSize++;

        // Need to write meta-data?
        // if (nullMetaData || val.binder != NULL) {
        //     mObjects[mObjectsSize] = mDataPos;
        //     acquire_object(ProcessState::self(), val, this);
        //     mObjectsSize++;
        // }

        // remember if it's a file descriptor
        // if (val.type == BINDER_TYPE_FD) {
        //     if (!mAllowFds) {
        //         return FDS_NOT_ALLOWED;
        //     }
        //     mHasFds = mFdsKnown = true;
        // }

        return NOERROR;
    }

    if (!enoughData) {
        ECode ec = GrowData(sizeof(UInt32) + sizeof(InterfacePack));
        if (FAILED(ec)) return ec;
    }
    if (!enoughObjects) {
        UInt32 newSize = ((mObjectsSize + 2) * 3) / 2;
        UInt32* objects = (UInt32*)realloc(mObjects, newSize * sizeof(UInt32));
        if (objects == NULL) return E_OUT_OF_MEMORY;
        mObjects = objects;
        mObjectsCapacity = newSize;
    }

    goto restart_write;
}

ECode CRemoteParcel::WriteBytePtr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteBooleanPtr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteChar8Ptr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteChar16Ptr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteInt16Ptr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteInt32Ptr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteInt64Ptr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteFloatPtr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteDoublePtr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteStringPtr(
    /* [in] */ Handle32 addr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteEMuidPtr(
    /* [in] */ Handle32 idPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteEGuidPtr(
    /* [in] */ Handle32 idPtr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteInterfacePtrPtr(
    /* [in] */ Handle32 itfpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteFileDescriptor(
    /* [in] */ Int32 fd)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::WriteDupFileDescriptor(
    /* [in] */ Int32 fd)
{
    return E_NOT_IMPLEMENTED;
}

const void* CRemoteParcel::ReadInplace(
    /* [in] */ UInt32 len) const
{
    if ((mDataPos + PAD_SIZE(len)) >= mDataPos && (mDataPos + PAD_SIZE(len)) <= mDataSize) {
        const void* data = mData + mDataPos;
        mDataPos += PAD_SIZE(len);
        ALOGV("readInplace Setting data pos of %p to %d\n", this, mDataPos);
        return data;
    }
    return NULL;
}

template<class T>
ECode CRemoteParcel::ReadAligned(
    /* [in] */ T* value) const
{
    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE(sizeof(T)) == sizeof(T));

    if ((mDataPos + sizeof(T)) <= mDataSize) {
        const void* data = mData + mDataPos;
        mDataPos += sizeof(T);
        *value = *reinterpret_cast<const T*>(data);
        return NOERROR;
    }
    else {
        return E_FAIL;
    }
}

template<class T>
T CRemoteParcel::ReadAligned() const
{
    T result;
    if (ReadAligned(&result) != NOERROR) {
        result = 0;
    }

    return result;
}

template<class T>
ECode CRemoteParcel::WriteAligned(
    /* [in] */ T value)
{
    COMPILE_TIME_ASSERT_FUNCTION_SCOPE(PAD_SIZE(sizeof(T)) == sizeof(T));

    if  ((mDataPos + sizeof(value)) <= mDataCapacity) {
restart_write:
        *reinterpret_cast<T*>(mData + mDataPos) = value;
        return FinishWrite(sizeof(value));
    }

    ECode ec = GrowData(sizeof(value));
    if (ec == NOERROR) goto restart_write;
    return ec;
}

const char* CRemoteParcel::ReadCStringInner() const
{
    UInt32 avail = mDataSize - mDataPos;
    if (avail > 0) {
        const char* str = reinterpret_cast<const char*>(mData + mDataPos);
        // is the string's trailing NUL within the parcel's valid bounds?
        const char* eos = reinterpret_cast<const char*>(memchr(str, 0, avail));
        if (eos) {
            UInt32 len = eos - str;
            mDataPos += PAD_SIZE(len + 1);
            ALOGV("readCString Setting data pos of %p to %d\n", this, mDataPos);
            return str;
        }
    }
    return NULL;
}

ECode CRemoteParcel::ReadByte(
    /* [out] */ Byte* pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (Byte)i : 0;
    return ec;
}

ECode CRemoteParcel::ReadBoolean(
    /* [out] */ Boolean* pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (i != 0 ? TRUE : FALSE) : FALSE;
    return ec;
}

ECode CRemoteParcel::ReadChar8(
    /* [out] */ Char8* pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (Char8)i : 0;
    return ec;
}

ECode CRemoteParcel::ReadChar16(
    /* [out] */ Char16* pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (Char16)i : 0;
    return ec;
}

ECode CRemoteParcel::ReadChar32(
    /* [out] */ Char32* pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (Char32)i : 0;
    return ec;
}

ECode CRemoteParcel::ReadInt16(
    /* [out] */ Int16 *pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    Int32 i;
    ECode ec = ReadInt32(&i);
    *pValue = SUCCEEDED(ec) ? (Int16)i : 0;
    return ec;
}

ECode CRemoteParcel::ReadInt32(
    /* [out] */ Int32 *pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    ECode ec = ReadAligned(pValue);
    if (FAILED(ec)) return ec;
    *pValue = ntohl(*pValue);
    return NOERROR;
}

ECode CRemoteParcel::ReadInt64(
    /* [out] */ Int64 *pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    return ReadAligned(pValue);
}

ECode CRemoteParcel::ReadFloat(
    /* [out] */ Float *pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    return ReadAligned(pValue);
}

ECode CRemoteParcel::ReadDouble(
    /* [out] */ Double *pValue)
{
    if (pValue == NULL) return E_INVALID_ARGUMENT;

    return ReadAligned(pValue);
}

ECode CRemoteParcel::ReadCString(
    /* [out] */ CString* pStr)
{
    if (pStr == NULL) return E_INVALID_ARGUMENT;

    Int32 tag;
    ECode ec = ReadInt32(&tag);
    if (FAILED(ec) || tag == MSH_NULL) {
        *pStr = NULL;
        return ec;
    }

    assert(tag == MSH_NOT_NULL);
    *pStr = CString::Duplicate(ReadCStringInner());
    return NOERROR;
}

ECode CRemoteParcel::ReadString(
    /* [out] */ String* pStr)
{
    if (pStr == NULL) return E_INVALID_ARGUMENT;

    Int32 tag;
    ECode ec = ReadInt32(&tag);
    if (FAILED(ec) || tag == MSH_NULL) {
        *pStr = NULL;
        return ec;
    }

    assert(tag == MSH_NOT_NULL);
    *pStr = ReadCStringInner();
    return NOERROR;
}

ECode CRemoteParcel::ReadStruct(
    /* [out] */ Handle32 *pAddress)
{
    if (pAddress == NULL) return E_INVALID_ARGUMENT;

    Int32 tag;
    ECode ec = ReadInt32(&tag);
    if (FAILED(ec) || tag == MSH_NULL) {
        *pAddress = NULL;
        return ec;
    }

    assert(tag == MSH_NOT_NULL);
    Int32 len;
    ReadInt32(&len);
    return Read(pAddress, len);
}

ECode CRemoteParcel::ReadEMuid(
    /* [out] */ EMuid *pId)
{
    if (pId == NULL) return E_INVALID_ARGUMENT;

    return Read((void*)pId, sizeof(EMuid));
}

ECode CRemoteParcel::ReadEGuid(
    /* [out] */ EGuid *pId)
{
    if (pId == NULL) return E_INVALID_ARGUMENT;

    ECode ec = Read((void*)pId, sizeof(EGuid));
    if (FAILED(ec)) return ec;
    const char* str = ReadCStringInner();
    pId->pUunm = (char*)malloc(strlen(str) + 1);
    strcpy(pId->pUunm, str);
    return NOERROR;
}

ECode CRemoteParcel::ReadArrayOf(
    /* [out] */ Handle32 *ppArray)
{
    if (ppArray == NULL) return E_INVALID_ARGUMENT;

    return ReadArrayOfInner((PCARQUINTET*)ppArray);
}

ECode CRemoteParcel::ReadArrayOfCString(
    /* [out, callee] */ ArrayOf<CString>** ppArray)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadArrayOfString(
    /* [out, callee] */ ArrayOf<String>** ppArray)
{
    if (ppArray == NULL) return E_INVALID_ARGUMENT;

    Int32 tag;
    ReadInt32(&tag);
    if (tag != MSH_NULL) {
        CarQuintet buf;

        Read((void*)&buf, sizeof(buf));
        Int32 size = buf.m_size / sizeof(String);
        ArrayOf<String>* strArray = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            ReadInt32(&tag);
            if (tag != MSH_NULL) {
                const char* str = ReadCStringInner();
                (*strArray)[i] = str;
            }
        }
        *(ArrayOf<String>**)ppArray = strArray;
        strArray->AddRef();
    }
    return NOERROR;
}

ECode CRemoteParcel::ReadBufferOf(
    /* [out] */ Handle32 *ppBuffer)
{
    if (ppBuffer == NULL) return E_INVALID_ARGUMENT;

    return ReadArrayOfInner((PCARQUINTET*)ppBuffer);
}

ECode CRemoteParcel::ReadBufferOfCString(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadBufferOfString(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadStringBuf(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadMemoryBuf(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadArrayOfInner(
    /* [out] */ PCARQUINTET* pValue)
{
    Int32 tag;
    ReadInt32(&tag);
    if (tag != MSH_NULL) {
        CarQuintet q;
        Read((void*)&q, sizeof(CarQuintet));
        Int32 size = q.m_size;
        PCarQuintet qq = _BufferOf_Alloc(size, q.m_flags);
        if (qq == NULL) {
            *pValue = NULL;
            return NOERROR;
        }
        _CarQuintet_AddRef(qq);
        *pValue = qq;
        if (size != 0) {
            if (CarQuintetFlag_Type_IObject
                != (q.m_flags & CarQuintetFlag_TypeMask)) {
                if (CarQuintetFlag_Type_String
                    == (q.m_flags & CarQuintetFlag_TypeMask)) {
                    ArrayOf<String>* strArr = (ArrayOf<String>*)(*pValue);
                    for (Int32 i = 0; i < size / sizeof(String); i++) {
                        ReadInt32(&tag);
                        if (tag != MSH_NULL) {
                            const char* str = ReadCStringInner();
                            (*strArr)[i] = str;
                        }
                    }
                }
                else {
                    Read((void*)qq->m_pBuf, size);
                }
            }
            else {
                IInterface** pBuf = (IInterface**)qq->m_pBuf;
                size = size / sizeof(IInterface *);
                for (int i = 0; i < size; i++) {
                    ReadInt32(&tag);
                    if (tag != MSH_NULL) {
                        InterfacePack ipack;
                        CIClassInfo *pClassInfo = NULL;
                        IParcelable *pParcelable = NULL;
                        ClassID classId;
                        InterfaceID iid;
                        ECode ec;

                        String netAddress;
                        ReadString(&netAddress);
                        Read((void *)&ipack, sizeof(InterfacePack));
                        ipack.m_sNetAddress = netAddress;
                        if (IsParcelable(&ipack, &pClassInfo)) {
                            classId.clsid = ipack.m_clsid;
                            classId.pUunm = pClassInfo->pszUunm;

                            ec = _CObject_CreateInstance(classId, RGM_SAME_DOMAIN,
                                    EIID_IParcelable, (IInterface**)&pParcelable);
                            if (FAILED(ec)) return ec;

                            pParcelable->ReadFromParcel(this);
                            iid = pClassInfo->interfaces[ipack.m_uIndex]->iid;
                            *((IInterface**)pBuf + i) = pParcelable->Probe(iid);
                        }
                        else {
                            ec = StdUnmarshalInterface(
                                    UnmarshalFlag_Noncoexisting,
                                    &ipack,
                                    (IInterface **)&pBuf[i]);
                            if (FAILED(ec)) {
                                MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                        "MshProc: unmsh interface, ec = %x\n", ec));
                                return ec;
                            }
                        }
                    }
                    else {
                        pBuf[i] = NULL;
                    }
                }
            }
        }
    }
    else {
        pValue = NULL;
    }
    return NOERROR;
}

ECode CRemoteParcel::Read(
    /* [in] */ void* outData,
    /* [in] */ UInt32 len) const
{
    if ((mDataPos + PAD_SIZE(len)) >= mDataPos && (mDataPos + PAD_SIZE(len)) <= mDataSize) {
        memcpy(outData, mData + mDataPos, len);
        mDataPos += PAD_SIZE(len);
        ALOGV("read Setting data pos of %p to %d\n", this, mDataPos);
        return NOERROR;
    }
    return E_FAIL;
}

ECode CRemoteParcel::ReadInterfacePtr(
    /* [out] */ Handle32 *pItfPtr)
{
    UInt32 tag;
    ECode ec = ReadAligned(&tag);
    if (FAILED(ec) || tag == MSH_NULL) {
        *pItfPtr = 0;
        return ec;
    }

    assert(tag == MSH_NOT_NULL);
    UInt32 DPOS = mDataPos;
    if (DPOS + sizeof(InterfacePack) <= mDataSize){
        InterfacePack *pItfPack = reinterpret_cast<InterfacePack*>(mData + DPOS);
        mDataPos = DPOS + sizeof(InterfacePack);
        CIClassInfo *pClassInfo = NULL;
        if (IsParcelable(pItfPack, &pClassInfo)) {
            ClassID classId;
            classId.clsid = pItfPack->m_clsid;
            classId.pUunm = pClassInfo->pszUunm;

            IParcelable *pParcelable = NULL;
            ec = _CObject_CreateInstance(classId, RGM_SAME_DOMAIN,
                    EIID_IParcelable, (IInterface**)&pParcelable);
            if (FAILED(ec)) return ec;

            pParcelable->ReadFromParcel(this);
            InterfaceID iid = pClassInfo->interfaces[pItfPack->m_uIndex]->iid;
            *(IInterface**)pItfPtr = pParcelable->Probe(iid);
        }
        else {
            ec = StdUnmarshalInterface(
                    UnmarshalFlag_Noncoexisting,
                    pItfPack,
                    (IInterface **)pItfPtr);
            if (FAILED(ec)) {
                MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                        "MshProc: unmsh interface, ec = %x\n", ec));
                return ec;
            }
        }

        // Ensure that this object is valid...
        UInt32* const OBJS = mObjects;
        const UInt32 N = mObjectsSize;
        UInt32 opos = mNextObjectHint;

        if (N > 0) {
            ALOGV("Parcel %p looking for obj at %d, hint=%d\n",
                 this, DPOS, opos);

            // Start at the current hint position, looking for an object at
            // the current data position.
            if (opos < N) {
                while (opos < (N - 1) && OBJS[opos] < DPOS) {
                    opos++;
                }
            }
            else {
                opos = N - 1;
            }
            if (OBJS[opos] == DPOS) {
                // Found it!
                ALOGV("Parcel %p found obj %d at index %d with forward search",
                     this, DPOS, opos);
                mNextObjectHint = opos + 1;
                ALOGV("readObject Setting data pos of %p to %d\n", this, mDataPos);
                return NOERROR;
            }

            // Look backwards for it...
            while (opos > 0 && OBJS[opos] > DPOS) {
                opos--;
            }
            if (OBJS[opos] == DPOS) {
                // Found it!
                ALOGV("Parcel %p found obj %d at index %d with backward search",
                     this, DPOS, opos);
                mNextObjectHint = opos + 1;
                ALOGV("readObject Setting data pos of %p to %d\n", this, mDataPos);
                return NOERROR;
            }
        }
        ALOGW("Attempt to read object from Parcel %p at offset %d that is not in the object list",
             this, DPOS);
    }
    *pItfPtr = 0;
    return NOERROR;
}

ECode CRemoteParcel::ReadBytePtr(
    /* [out] */ Handle32 *bypp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadBooleanPtr(
    /* [out] */ Handle32 *bopp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadChar8Ptr(
    /* [out] */ Handle32 *c8pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadChar16Ptr(
    /* [out] */ Handle32 *c16pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadInt16Ptr(
    /* [out] */ Handle32 *i16pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadInt32Ptr(
    /* [out] */ Handle32 *i32pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadInt64Ptr(
    /* [out] */ Handle32 *i64pp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadFloatPtr(
    /* [out] */ Handle32 *fpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadDoublePtr(
    /* [out] */ Handle32 *dpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadStringPtr(
    /* [out] */ Handle32 *spp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadStructPtr(
    /* [out] */ Handle32 *paddr)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadEMuidPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadEGuidPtr(
    /* [out] */ Handle32 *idpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadInterfacePtrPtr(
    /* [out] */ Handle32 *itfpp)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadBufferOfPtr(
    /* [out] */ Handle32 *ppBuffer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::ReadFileDescriptor(
    /* [out] */ Int32* fd)
{
    return E_NOT_IMPLEMENTED;
}

void CRemoteParcel::ReleaseObjects()
{
    //need do nothing
}

void CRemoteParcel::FreeData()
{
    FreeDataNoInit();
    InitState();
}

void CRemoteParcel::FreeDataNoInit()
{
    if (mOwner) {
        //ALOGI("Freeing data ref of %p (pid=%d)\n", this, getpid());
        mOwner(this, mData, mDataSize, mObjects, mObjectsSize, mOwnerCookie);
    }
    else {
        ReleaseObjects();
        if (mData) free(mData);
        if (mObjects) free(mObjects);
    }
}

ECode CRemoteParcel::GrowData(
    /* [in] */ UInt32 len)
{
    UInt32 newSize = ((mDataSize + len) * 3) / 2;
    return (newSize <= mDataSize) ? E_OUT_OF_MEMORY : ContinueWrite(newSize);
}

ECode CRemoteParcel::RestartWrite(
    /* [in] */ UInt32 desired)
{
    if (mOwner) {
        FreeData();
        return ContinueWrite(desired);
    }

    UInt8* data = (UInt8*)realloc(mData, desired);
    if (!data && desired > mDataCapacity) {
        mError = E_OUT_OF_MEMORY;
        return E_OUT_OF_MEMORY;
    }

    ReleaseObjects();

    if (data) {
        mData = data;
        mDataCapacity = desired;
    }

    mDataSize = mDataPos = 0;
    ALOGV("restartWrite Setting data size of %p to %d\n", this, mDataSize);
    ALOGV("restartWrite Setting data pos of %p to %d\n", this, mDataPos);

    free(mObjects);
    mObjects = NULL;
    mObjectsSize = mObjectsCapacity = 0;
    mNextObjectHint = 0;

    return NOERROR;
}

ECode CRemoteParcel::ContinueWrite(
    /* [in] */ UInt32 desired)
{
    // If shrinking, first adjust for any objects that appear
    // after the new data size.
    size_t objectsSize = mObjectsSize;
    if (desired < mDataSize) {
        if (desired == 0) {
            objectsSize = 0;
        }
        else {
            while (objectsSize > 0) {
                if (mObjects[objectsSize - 1] < desired) {
                    break;
                }
                objectsSize--;
            }
        }
    }

    if (mOwner) {
        // If the size if going to zero, just release the owner's data.
        if (desired == 0) {
            FreeData();
            return NOERROR;
        }

        // If there is a different owner, we need to take posession.
        UInt8* data = (UInt8*)malloc(desired);
        if (!data) {
            mError = E_OUT_OF_MEMORY;
            return E_OUT_OF_MEMORY;
        }
        UInt32* objects = NULL;
//
//        if (objectsSize) {
//            objects = (size_t*)malloc(objectsSize*sizeof(size_t));
//            if (!objects) {
//                mError = NO_MEMORY;
//                return NO_MEMORY;
//            }
//
//            // Little hack to only acquire references on objects
//            // we will be keeping.
//            size_t oldObjectsSize = mObjectsSize;
//            mObjectsSize = objectsSize;
//            acquireObjects();
//            mObjectsSize = oldObjectsSize;
//        }

        if (mData) {
            memcpy(data, mData, mDataSize < desired ? mDataSize : desired);
        }
//        if (objects && mObjects) {
//            memcpy(objects, mObjects, objectsSize*sizeof(size_t));
//        }
        //ALOGI("Freeing data ref of %p (pid=%d)\n", this, getpid());
        mOwner(this, mData, mDataSize, mObjects, mObjectsSize, mOwnerCookie);
        mOwner = NULL;

        mData = data;
        mObjects = objects;
        mDataSize = (mDataSize < desired) ? mDataSize : desired;
        ALOGV("continueWrite Setting data size of %p to %d\n", this, mDataSize);
        mDataCapacity = desired;
        mObjectsSize = mObjectsCapacity = objectsSize;
        mNextObjectHint = 0;
    }
    else if (mData) {
//        if (objectsSize < mObjectsSize) {
//            // Need to release refs on any objects we are dropping.
//            const sp<ProcessState> proc(ProcessState::self());
//            for (size_t i=objectsSize; i<mObjectsSize; i++) {
//                const flat_binder_object* flat
//                    = reinterpret_cast<flat_binder_object*>(mData+mObjects[i]);
//                if (flat->type == BINDER_TYPE_FD) {
//                    // will need to rescan because we may have lopped off the only FDs
//                    mFdsKnown = false;
//                }
//                release_object(proc, *flat, this);
//            }
//            size_t* objects =
//                (size_t*)realloc(mObjects, objectsSize*sizeof(size_t));
//            if (objects) {
//                mObjects = objects;
//            }
//            mObjectsSize = objectsSize;
//            mNextObjectHint = 0;
//        }

        // We own the data, so we can just do a realloc().
        if (desired > mDataCapacity) {
            UInt8* data = (UInt8*)realloc(mData, desired);
            if (data) {
                mData = data;
                mDataCapacity = desired;
            }
            else if (desired > mDataCapacity) {
                mError = E_OUT_OF_MEMORY;
                return E_OUT_OF_MEMORY;
            }
        }
        else {
            if (mDataSize > desired) {
                mDataSize = desired;
                ALOGV("continueWrite Setting data size of %p to %d\n", this, mDataSize);
            }
            if (mDataPos > desired) {
                mDataPos = desired;
                ALOGV("continueWrite Setting data pos of %p to %d\n", this, mDataPos);
            }
        }
    }
    else {
        // This is the first data, Easy!
        UInt8* data = (UInt8*)malloc(desired);
        if (!data) {
            mError = E_OUT_OF_MEMORY;
            return E_OUT_OF_MEMORY;
        }

        if (!(mDataCapacity == 0 && mObjects == NULL
            && mObjectsCapacity == 0)) {
            ALOGE("continueWrite: %d/%p/%d/%d", mDataCapacity, mObjects, mObjectsCapacity, desired);
        }

        mData = data;
        mDataSize = mDataPos = 0;
        ALOGV("continueWrite Setting data size of %p to %d\n", this, mDataSize);
        ALOGV("continueWrite Setting data pos of %p to %d\n", this, mDataPos);
        mDataCapacity = desired;
    }

    return NOERROR;
}

void CRemoteParcel::InitState()
{
    mError = NOERROR;
    mData = 0;
    mDataSize = 0;
    mDataCapacity = 0;
    mDataPos = 0;
    mObjects = NULL;
    mObjectsSize = 0;
    mObjectsCapacity = 0;
    mNextObjectHint = 0;
    // mHasFds = false;
    // mFdsKnown = true;
    // mAllowFds = true;
    mOwner = NULL;
}

ECode CRemoteParcel::Marshall(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    if (NULL == bytes) return E_INVALID_ARGUMENT;
    *bytes = NULL;

    // do not marshall if there are binder objects in the parcel
    if (ObjectsCount()) {
        ALOGD("RuntimeException Tried to marshall a Parcel that contained Binder objects.");
        return E_FAIL;
    }

    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(DataSize());
    array->Copy(Data(), DataSize());
    *bytes = array;
    (*bytes)->AddRef();
    return NOERROR;
}

ECode CRemoteParcel::Unmarshall(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    if (data == NULL || length < 0 || length > data->GetLength()
            || offset < 0 || offset > data->GetLength() || (data->GetLength() - offset) < length) {
       return NOERROR;
    }

    SetDataSize(length);
    SetDataPositionInner(0);

    void* raw = WriteInplace(length);
    memcpy(raw, (data->GetPayload() + offset), length);

    return NOERROR;
}

ECode CRemoteParcel::AppendFrom(
    /* [in] */ IParcel* parcel,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::Clone(
    /* [in] */ IParcel* pSrcParcel)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::GetDataPosition(
    /* [out] */ Int32* position)
{
    if (position == NULL) return E_NOT_IMPLEMENTED;

    *position = DataPosition();
    return NOERROR;
}

ECode CRemoteParcel::SetDataPosition(
    /* [in] */ Int32 position)
{
    SetDataPositionInner(position);
    return NOERROR;
}

ECode CRemoteParcel::GetElementPayload(
    /* [out] */ Handle32* pBuffer)
{
    *pBuffer = (Handle32)mData;
    return NOERROR;
}

ECode CRemoteParcel::GetElementSize(
    /* [in] */ Int32* pSize)
{
    *pSize = (Int32)DataSize();
    return NOERROR;
}

MarshalHeader* CRemoteParcel::GetMarshalHeader()
{
    return (MarshalHeader*)Data();
}

Boolean CRemoteParcel::IsParcelable(
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ CIClassInfo **ppClassInfo)
{
    CIInterfaceInfo *pInterfaceInfo = NULL;
    ECode ec;

    ec = LookupClassInfo(pInterfacePack->m_clsid, ppClassInfo);
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(pInterfacePack->m_sNetAddress,
                                pInterfacePack->m_clsid,
                                ppClassInfo);
        if (FAILED(ec)) return FALSE;
    }

    for (UInt16 i = 0; i < (*ppClassInfo)->interfaceNum; i++) {
        pInterfaceInfo = (*ppClassInfo)->interfaces[i];
        if (pInterfaceInfo->iid == EIID_IParcelable) {
            return TRUE;
        }
    }

    return FALSE;
}

} // namespace RPC
} // namespace Elastos
