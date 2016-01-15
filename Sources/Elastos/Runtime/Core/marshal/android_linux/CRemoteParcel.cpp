
#include <stdio.h>
#include "CRemoteParcel.h"

#define E_INVALID_PARCEL_DATA 0x81ff0000

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo** classInfo);

namespace Elastos {
namespace IPC {

ECode GetRemoteClassInfo(
    /* [in] */ android::IBinder* remoteObj,
    /* [in] */ REMuid clsId,
    /* [out] */ CIClassInfo** classInfo);

#define ROUND4(n)       (((n)+3)&~3)   // round up to multiple of 4 bytes
#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

enum Type {
    Type_Unknown            = 0,
    Type_Byte               = 1,
    Type_Boolean            = 2,
    Type_Char32             = 3,
    Type_Int16              = 4,
    Type_Int32              = 5,
    Type_Int64              = 6,
    Type_Float              = 7,
    Type_Double             = 8,
    Type_String             = 9,
    Type_Struct             = 10,
    Type_EMuid              = 11,
    Type_EGuid              = 12,
    Type_ArrayOf            = 13,
    Type_ArrayOfString      = 14,
    Type_InterfacePtr       = 15,
    Type_Fd                 = 16,
    Type_Fd_Dup             = 17
};

CRemoteParcel::CRemoteParcel(
    /* [in] */ Boolean writeMarshalHeader)
    : mData(new android::Parcel())
    , mFreeDataTag(TRUE)
{
    if (writeMarshalHeader) mData->writeInplace(sizeof(MarshalHeader));
}

CRemoteParcel::CRemoteParcel(
    /* [in] */ android::Parcel* data)
    : mData(data)
    , mFreeDataTag(FALSE)
{}

CRemoteParcel::~CRemoteParcel()
{
    if (mFreeDataTag) {
        delete mData;
    }
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

ECode CRemoteParcel::Marshall(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    if (NULL == bytes) return E_INVALID_ARGUMENT;
    *bytes = NULL;

    // do not marshall if there are binder objects in the parcel
    if (mData->objectsCount()) {
        ALOGD("RuntimeException Tried to marshall a Parcel that contained Binder objects.");
        return E_NOT_SUPPORTED;
    }

    ArrayOf<Byte>* array = ArrayOf<Byte>::Alloc(mData->dataSize());
    memcpy(array->GetPayload(), mData->data(), mData->dataSize());
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

    mData->setDataSize(length);
    mData->setDataPosition(0);

    void* raw = mData->writeInplace(length);
    memcpy(raw, (data->GetPayload() + offset), length);

    return NOERROR;
}

ECode CRemoteParcel::GrowElemBuffer(
    /* [in] */ Int32 minSize)
{
    return NOERROR;
}

ECode CRemoteParcel::AppendFrom(
    /* [in] */ IParcel* parcel,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    android::Parcel* parcelObj;
    parcel->GetElementPayload((Handle32*)&parcelObj);
    if (parcelObj == NULL) return E_INVALID_ARGUMENT;
    mData->appendFrom(parcelObj, offset, length);
    return NOERROR;
}

ECode CRemoteParcel::HasFileDescriptors(
    /* [out] */ Boolean* result)
{
    if (NULL == result) return E_INVALID_ARGUMENT;
    *result = mData->hasFileDescriptors();
    return NOERROR;
}

ECode CRemoteParcel::ReadValue(
    /* [in] */ PVoid value,
    /* [in] */ Int32 type)
{
    switch(type) {
        case Type_Byte:
            *(Byte*)value = (Byte)mData->readInt32();
            break;

        case Type_Boolean:
            *(Boolean*)value = (Boolean)mData->readInt32();
            break;

        case Type_Char32:
            *(Char32*)value = (Char32)mData->readInt32();
            break;

        case Type_Int16:
            *(Int16*)value = (Int16)mData->readInt32();
            break;

        case Type_Int32:
            *(Int32*)value = (Int32)mData->readInt32();
            break;

        case Type_Int64:
        case Type_Double:
            *(Int64*)value = (Int64)mData->readInt64();
            break;

        case Type_Float:
            *(Float*)value = (Float)mData->readFloat();
            break;

        case Type_String:
            {
                Int32 tag = mData->readInt32();
                if (tag != MSH_NULL) {
                    const char* str = mData->readCString();
                    *(String*)value = str;
                }
                else {
                    *(String*)value = NULL;
                }
            }
            break;

        case Type_Struct:
            {
                Int32 tag = (Int32)mData->readInt32();
                if (tag != MSH_NULL) {
                    Int32 len = (Int32)mData->readInt32();
                    mData->read(value, len);
                }
                else {
                    value = NULL;
                }
            }
            break;

        case Type_EMuid:
            mData->read(value, sizeof(EMuid));

            break;

        case Type_EGuid:
            {
                const char* str;

                mData->read(value, sizeof(EGuid));
                str = mData->readCString();
                (*(EGuid*)value).mUunm = (char*)malloc(strlen(str) + 1);
                strcpy((*(EGuid*)value).mUunm, str);
            }
            break;

        case Type_ArrayOf:
            {
                Int32 tag = (Int32)mData->readInt32();
                if (tag != MSH_NULL) {
                    CarQuintet q;
                    mData->read((void*)&q, sizeof(CarQuintet));
                    Int32 size = q.mSize;
                    PCarQuintet qq = _ArrayOf_Alloc(size, q.mFlags);
                    if (qq == NULL) {
                        *(PCARQUINTET*)value = NULL;
                        break;
                    }
                    _CarQuintet_AddRef(qq);
                    *(PCARQUINTET*)value = qq;
                    if (size != 0) {
                        if (CarQuintetFlag_Type_IObject
                            != (q.mFlags & CarQuintetFlag_TypeMask)) {
                            if (CarQuintetFlag_Type_String
                                == (q.mFlags & CarQuintetFlag_TypeMask)) {
                                ArrayOf<String>* strArr = (ArrayOf<String>*)(*(PCARQUINTET*)value);
                                for (Int32 i = 0; i < (Int32)(size / sizeof(String)); i++) {
                                    Int32 tag = mData->readInt32();
                                    if (tag != MSH_NULL) {
                                        const char* str = mData->readCString();
                                        (*strArr)[i] = str;
                                    }
                                }
                            }
                            else {
                                mData->read((void*)qq->mBuf, size);
                            }
                        }
                        else {
                            IInterface** pBuf = (IInterface**)qq->mBuf;
                            size = size / sizeof(IInterface *);
                            for (int i = 0; i < size; i++) {
                                tag = (Int32)mData->readInt32();
                                if (tag != MSH_NULL) {
                                    InterfacePack ipack;
                                    CIClassInfo* classInfo = NULL;
                                    IParcelable* parcelable = NULL;
                                    ClassID classId;
                                    InterfaceID iid;
                                    ECode ec;

                                    android::sp<android::IBinder> binder = mData->readStrongBinder();
                                    mData->read((void *)&ipack, sizeof(InterfacePack));
                                    assert(ipack.mBinder == NULL);
                                    ipack.mBinder = binder;
                                    if (IsParcelable(&ipack, &classInfo)) {
                                        classId.mClsid = ipack.mClsid;
                                        classId.mUunm = classInfo->mUunm;

                                        ec = _CObject_CreateInstance(classId, RGM_SAME_DOMAIN,
                                                EIID_IParcelable, (IInterface**)&parcelable);
                                        if (FAILED(ec)) return ec;

                                        parcelable->ReadFromParcel(this);
                                        iid = classInfo->mInterfaces[ipack.mIndex]->mIID;
                                        *((IInterface**)pBuf + i) = parcelable->Probe(iid);
                                    }
                                    else {
                                        ec = StdUnmarshalInterface(
                                                UnmarshalFlag_Noncoexisting,
                                                &ipack,
                                                (IInterface **)&pBuf[i]);
                                        if (FAILED(ec)) {
                                            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
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
                    *(UInt32*)value = (UInt32)NULL;
                }
            }
            break;

        case Type_ArrayOfString:
            {
                Int32 tag = (Int32)mData->readInt32();
                if (tag != MSH_NULL) {
                    CarQuintet buf;

                    mData->read((void*)&buf, sizeof(buf));
                    Int32 size = buf.mSize / sizeof(String);
                    ArrayOf<String>* strArray = ArrayOf<String>::Alloc(size);
                    for (Int32 i = 0; i < size; i++) {
                        Int32 tag = mData->readInt32();
                        if (tag != MSH_NULL) {
                            const char* str = mData->readCString();
                            (*strArray)[i] = str;
                        }
                    }
                    *(ArrayOf<String>**)value = strArray;
                    strArray->AddRef();
                }
            }
            break;

        case Type_InterfacePtr:
            {
                Int32 tag = (Int32)mData->readInt32();
                if (tag != MSH_NULL) {
                    InterfacePack ipack;
                    CIClassInfo* classInfo = NULL;
                    IParcelable* parcelable = NULL;
                    ClassID classId;
                    InterfaceID iid;
                    ECode ec;

                    android::sp<android::IBinder> binder = mData->readStrongBinder();
                    mData->read((void *)&ipack, sizeof(InterfacePack));
                    if (ipack.mBinder != NULL) {
                        memset(&ipack, 0, sizeof(InterfacePack));
                        return E_INVALID_PARCEL_DATA;
                    }
                    ipack.mBinder = binder;
                    if (IsParcelable(&ipack, &classInfo)) {
                        classId.mClsid = ipack.mClsid;
                        classId.mUunm = classInfo->mUunm;

                        ec = _CObject_CreateInstance(classId, RGM_SAME_DOMAIN,
                                EIID_IParcelable, (IInterface**)&parcelable);
                        if (FAILED(ec)) return ec;

                        ec = parcelable->ReadFromParcel(this);
                        if (FAILED(ec)) return ec;

                        iid = classInfo->mInterfaces[ipack.mIndex]->mIID;
                        *(IInterface**)value = parcelable->Probe(iid);
                    }
                    else {
                        ec = StdUnmarshalInterface(
                                UnmarshalFlag_Noncoexisting,
                                &ipack,
                                (IInterface **)value);
                        if (FAILED(ec)) {
                            MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
                                    "MshProc: unmsh interface, ec = %x\n", ec));
                            return ec;
                        }
                    }
                }
                else {
                    *(IInterface**)value = NULL;
                }
            }
            break;

        case Type_Fd:
            *(Int32*)value = (Int32)mData->readFileDescriptor();
            break;

        default:
            assert(0);
            break;
    }

    return NOERROR;
}

ECode CRemoteParcel::WriteValue(
    /* [in] */ PVoid value,
    /* [in] */ Int32 type,
    /* [in] */ Int32 size)
{
    ECode ec;

    switch(type) {
        case Type_Byte:
        case Type_Boolean:
            mData->writeInt32(*((Byte*)value));
            break;

        case Type_Int16:
            mData->writeInt32(*((Int16*)value));
            break;

        case Type_Char32:
        case Type_Int32:
            mData->writeInt32(*((Int32*)value));
            break;

        case Type_Float:
            mData->writeFloat(*((Float*)value));
            break;

        case Type_Int64:
            mData->writeInt64(*((Int64*)value));
            break;

        case Type_Double:
            mData->writeDouble(*((Double*)value));
            break;

        case Type_String:
            mData->writeInt32(value ? MSH_NOT_NULL : MSH_NULL);

            if (value) {
                mData->writeCString((const char*)value);
            }
            break;

        case Type_InterfacePtr:
            mData->writeInt32(value ? MSH_NOT_NULL : MSH_NULL);

            if (value) {
                InterfacePack itfPack;
                ec = StdMarshalInterface(
                        (IInterface*)value,
                        &itfPack);
                if (FAILED(ec)) return ec;

                mData->writeStrongBinder(itfPack.mBinder);
                itfPack.mBinder = NULL;
                mData->write((const void *)&itfPack, (int32_t)sizeof(InterfacePack));

                IParcelable* parcelable = \
                        (IParcelable*)((IInterface*)value)->Probe(EIID_IParcelable);
                if (parcelable != NULL) parcelable->WriteToParcel(this);
            }
            break;

        case Type_Struct:
            mData->writeInt32(value ? MSH_NOT_NULL : MSH_NULL);

            if (value) {
                mData->writeInt32(size);
                mData->write(value, size);
            }
            break;

        case Type_EMuid:
            mData->write(value, sizeof(EMuid));

            break;

        case Type_EGuid:
            {
                mData->write(value, sizeof(EGuid));
                mData->writeCString(((EGuid *)value)->mUunm);
            }
            break;

        case Type_ArrayOf:
            mData->writeInt32(value ? MSH_NOT_NULL : MSH_NULL);

            if (value) {
                mData->write(value, sizeof(CarQuintet));

                if (CarQuintetFlag_Type_IObject
                    != (((PCARQUINTET)value)->mFlags
                            & CarQuintetFlag_TypeMask)) {
                    // copy the storaged data
                    //
                    if (CarQuintetFlag_Type_String == (((PCARQUINTET)value)->mFlags
                            & CarQuintetFlag_TypeMask)) {
                        Int32 size = ((PCARQUINTET)value)->mSize / sizeof(String);
                        String* strBuf = (String*)((PCARQUINTET)value)->mBuf;
                        for (Int32 i = 0; i < size; ++i) {
                            // ALOGD("i: %d, str: %s", i, strBuf[i].string());
                            if (!strBuf[i].IsNull()) {
                                mData->writeInt32(MSH_NOT_NULL);
                                mData->writeCString(strBuf[i].string());
                            }
                            else {  // null pointer
                                mData->writeInt32(MSH_NULL);
                            }
                        }
                    }
                    else {
                        mData->write(((PCARQUINTET)value)->mBuf,
                                ((PCARQUINTET)value)->mSize);
                    }
                }
                else {
                    Int32 size = ((PCARQUINTET)value)->mSize
                                / sizeof(IInterface *);
                    Int32* int32Buf = (Int32*)((PCARQUINTET)value)->mBuf;
                    for (Int32 i = 0; i < size; i++) {
                        if (int32Buf[i]) {
                            mData->writeInt32(MSH_NOT_NULL);

                            InterfacePack itfPack;
                            ec = StdMarshalInterface(
                                    (IInterface *)int32Buf[i],
                                    &itfPack);
                            if (FAILED(ec)) {
                                MARSHAL_DBGOUT(MSHDBG_ERROR, ALOGE(
                                        "MshProc: marshal interface, ec = %x\n", ec));
                                return ec;
                            }

                            mData->writeStrongBinder(itfPack.mBinder);
                            itfPack.mBinder = NULL;
                            mData->write((void*)&itfPack, sizeof(itfPack));

                            IParcelable* parcelable = \
                                    (IParcelable*)((IInterface*)int32Buf[i])->Probe(EIID_IParcelable);
                            if (parcelable != NULL) parcelable->WriteToParcel(this);
                        }
                        else {  // null pointer
                            mData->writeInt32(MSH_NULL);
                        }
                    }
                }
            }
            break;

        case Type_ArrayOfString:
            mData->writeInt32(value ? MSH_NOT_NULL : MSH_NULL);

            if (value) {
                mData->write(value, sizeof(CarQuintet));

                Int32 size = ((PCARQUINTET)value)->mSize
                            / sizeof(String);
                String* strBuf = (String*)((PCARQUINTET)value)->mBuf;
                for (Int32 i = 0; i < size; i++) {
                    if (!strBuf[i].IsNull()) {
                        mData->writeInt32(MSH_NOT_NULL);
                        mData->writeCString(strBuf[i].string());
                    }
                    else {  // null pointer
                        mData->writeInt32(MSH_NULL);
                    }
                }
            }
            break;

        case Type_Fd:
            mData->writeFileDescriptor(*((Int32*)value));
            break;

        case Type_Fd_Dup:
            mData->writeDupFileDescriptor(*((Int32*)value));
            break;

        default:
            assert(0);
            break;
    }

    return NOERROR;
}

ECode CRemoteParcel::ReadByte(
    /* [out] */ Byte* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Byte);
}

ECode CRemoteParcel::ReadBoolean(
    /* [out] */ Boolean* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Boolean);
}

ECode CRemoteParcel::ReadChar(
    /* [out] */ Char32* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Char32);
}

ECode CRemoteParcel::ReadInt16(
    /* [out] */ Int16* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Int16);
}

ECode CRemoteParcel::ReadInt32(
    /* [out] */ Int32* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Int32);
}

ECode CRemoteParcel::ReadInt64(
    /* [out] */ Int64* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Int64);
}

ECode CRemoteParcel::ReadFloat(
    /* [out] */ Float* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Float);
}

ECode CRemoteParcel::ReadDouble(
    /* [out] */ Double* value)
{
    if (value == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)value, Type_Double);
}

ECode CRemoteParcel::ReadString(
    /* [out] */ String* str)
{
    if (str == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)str, Type_String);
}

ECode CRemoteParcel::ReadStruct(
    /* [out] */ Handle32* addr)
{
    return ReadValue((PVoid)addr, Type_Struct);
}

ECode CRemoteParcel::ReadEMuid(
    /* [in] */ EMuid* id)
{
    return ReadValue((PVoid)id, Type_EMuid);
}

ECode CRemoteParcel::ReadEGuid(
    /* [in] */ EGuid* id)
{
    return ReadValue((PVoid)id, Type_EGuid);
}

ECode CRemoteParcel::ReadInterfacePtr(
    /* [out] */ Handle32* itfPtr)
{
    assert(itfPtr != NULL);

    return ReadValue((PVoid)itfPtr, Type_InterfacePtr);
}

ECode CRemoteParcel::ReadArrayOf(
    /* [out] */ Handle32* array)
{
    assert(array != NULL);

    return ReadValue((PVoid)array, Type_ArrayOf);
}

ECode CRemoteParcel::ReadArrayOfString(
    /* [out, callee] */ ArrayOf<String>** array)
{
    return ReadValue((PVoid)array, Type_ArrayOfString);
}

// Retrieve a file descriptor from the parcel.  This returns the raw fd
// in the parcel, which you do not own -- use dup() to get your own copy.
ECode CRemoteParcel::ReadFileDescriptor(
    /* [out] */ Int32* fd)
{
    if (fd == NULL) return E_INVALID_ARGUMENT;

    return ReadValue((PVoid)fd, Type_Fd);
}

ECode CRemoteParcel::WriteByte(
    /* [in] */ Byte value)
{
    return WriteValue((PVoid)&value, Type_Byte, 4);
}

ECode CRemoteParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteValue((PVoid)&value, Type_Boolean, 4);
}

ECode CRemoteParcel::WriteChar(
    /* [in] */ Char32 value)
{
    return WriteValue((PVoid)&value, Type_Char32, 4);
}

ECode CRemoteParcel::WriteInt16(
    /* [in] */ Int16 value)
{
    return WriteValue((PVoid)&value, Type_Int16, 4);
}

ECode CRemoteParcel::WriteInt32(
    /* [in] */ Int32 value)
{
    return WriteValue((PVoid)&value, Type_Int32, 4);
}

ECode CRemoteParcel::WriteInt64(
    /* [in] */ Int64 value)
{
    return WriteValue((PVoid)&value, Type_Int64, 8);
}

ECode CRemoteParcel::WriteFloat(
    /* [in] */ Float value)
{
    return WriteValue((PVoid)&value, Type_Float, 4);
}

ECode CRemoteParcel::WriteDouble(
    /* [in] */ Double value)
{
    return WriteValue((PVoid)&value, Type_Double, 8);
}

ECode CRemoteParcel::WriteString(
    /* [in] */ const String& str)
{
    Int32 size = sizeof(UInt32);

    if (!str.IsNull()) {
        size += MSH_ALIGN_4(str.GetByteLength()+ 1) + sizeof(UInt32);
    }

    return WriteValue((PVoid)str.string(), Type_String, size);
}

ECode CRemoteParcel::WriteInterfacePtr(
    /* [in] */ IInterface* value)
{
    return WriteValue((PVoid)value, Type_InterfacePtr, sizeof(UInt32) + sizeof(InterfacePack));
}

ECode CRemoteParcel::WriteStruct(
    /* [in] */ Handle32 value,
    /* [in] */ Int32 size)
{
    return WriteValue((PVoid)value, Type_Struct, size + sizeof(UInt32));
}

ECode CRemoteParcel::WriteEMuid(
    /* [in] */ const EMuid& id)
{
    return WriteValue((PVoid)&id, Type_EMuid, sizeof(EMuid));
}

ECode CRemoteParcel::WriteEGuid(
    /* [in] */ const EGuid& id)
{
    return WriteValue((PVoid)&id, Type_EGuid, sizeof(EGuid) +
            MSH_ALIGN_4(strlen(id.mUunm) + 1) + sizeof(UInt32) * 2);
}

ECode CRemoteParcel::WriteArrayOf(
    /* [in] */ Handle32 array)
{
    Int32 size = array != 0 ? sizeof(UInt32) + sizeof(CarQuintet) + ((CarQuintet*)array)->mSize
        : sizeof(UInt32);
    return WriteValue((PVoid)array, Type_ArrayOf, size);
}

ECode CRemoteParcel::WriteArrayOfString(
    /* [in] */ ArrayOf<String>* array)
{
    Int32 size = array != NULL ? sizeof(UInt32) + sizeof(CarQuintet) + array->mSize : sizeof(UInt32);
    return WriteValue((PVoid)array, Type_ArrayOfString, size);
}

// Place a file descriptor into the parcel.  The given fd must remain
// valid for the lifetime of the parcel.
ECode CRemoteParcel::WriteFileDescriptor(
    /* [in] */ Int32 fd)
{
    return WriteValue((PVoid)&fd, Type_Fd, 4);
}

// Place a file descriptor into the parcel.  A dup of the fd is made, which
// will be closed once the parcel is destroyed.
ECode CRemoteParcel::WriteDupFileDescriptor(
    /* [in] */ Int32 fd)
{
    return WriteValue((PVoid)&fd, Type_Fd_Dup, 4);
}

ECode CRemoteParcel::Clone(
    /* [in] */ IParcel* srcParcel)
{
    return E_NOT_IMPLEMENTED;
}

ECode CRemoteParcel::GetDataPosition(
    /* [out] */ Int32* position)
{
    if (position == NULL) return E_NOT_IMPLEMENTED;

    *position = mData->dataPosition();
    return NOERROR;
}

ECode CRemoteParcel::SetDataPosition(
    /* [in] */ Int32 position)
{
    mData->setDataPosition(position);
    return NOERROR;
}

ECode CRemoteParcel::GetElementPayload(
    /* [out] */ Handle32* buffer)
{
    *buffer = (Handle32)mData;
    return NOERROR;
}

ECode CRemoteParcel::GetElementSize(
    /* [in] */ Int32* size)
{
    *size = (Int32)mData->dataSize();
    return NOERROR;
}

MarshalHeader* CRemoteParcel::GetMarshalHeader()
{
    return (MarshalHeader*)mData->data();
}

Boolean CRemoteParcel::IsParcelable(
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ CIClassInfo** classInfo)
{
    ECode ec = LookupClassInfo(interfacePack->mClsid, classInfo);
    if (FAILED(ec)) {
        ec = GetRemoteClassInfo(interfacePack->mBinder.get(),
                                interfacePack->mClsid,
                                classInfo);
        if (FAILED(ec)) return FALSE;
    }

    for (UInt16 i = 0; i < (*classInfo)->mInterfaceNum; i++) {
        CIInterfaceInfo* interfaceInfo = (*classInfo)->mInterfaces[i];
        if (interfaceInfo->mIID == EIID_IParcelable) {
            return TRUE;
        }
    }

    return FALSE;
}

} // namespace IPC
} // namespace Elastos

ELAPI _CParcel_New(
    /* [out] */ IParcel** parcel)
{
    if (parcel == NULL) return E_INVALID_ARGUMENT;
    void* location = calloc(sizeof(Elastos::IPC::CRemoteParcel), 1);
    if (!location) return E_OUT_OF_MEMORY;

    Elastos::IPC::CRemoteParcel* parcelObj = new(location) Elastos::IPC::CRemoteParcel(FALSE);
    parcelObj->AddRef();

    *parcel = (IParcel*)parcelObj;

    return NOERROR;
}
