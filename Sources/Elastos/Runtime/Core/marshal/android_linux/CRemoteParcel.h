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

#ifndef __CREMOTEPARCEL_H__
#define __CREMOTEPARCEL_H__

#include <binder/Parcel.h>
#include <marshal_ipc.h>

namespace Elastos {
namespace IPC {

class CRemoteParcel
    : public ElRefBase
    , public IParcel
{
public:
    CRemoteParcel(
        /* [in] */ Boolean writeMarshalHeader = TRUE);

    CRemoteParcel(
        /* [in] */ android::Parcel* data);

    ~CRemoteParcel();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI Marshall(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI Unmarshall(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI AppendFrom(
        /* [in] */ IParcel* parcel,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI HasFileDescriptors(
        /* [out] */ Boolean* result);

    CARAPI ReadByte(
        /* [out] */ Byte* value);

    CARAPI WriteByte(
        /* [in] */ Byte value);

    CARAPI ReadBoolean(
        /* [out] */ Boolean* value);

    CARAPI WriteBoolean(
        /* [in] */ Boolean value);

    CARAPI ReadChar(
        /* [out] */ Char32* value);

    CARAPI WriteChar(
        /* [in] */ Char32 value);

    CARAPI ReadInt16(
        /* [out] */ Int16* value);

    CARAPI WriteInt16(
        /* [in] */ Int16 value);

    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI ReadInt64(
        /* [out] */ Int64* value);

    CARAPI WriteInt64(
        /* [in] */ Int64 value);

    CARAPI ReadFloat(
        /* [out] */ Float* value);

    CARAPI WriteFloat(
        /* [in] */ Float value);

    CARAPI ReadDouble(
        /* [out] */ Double* value);

    CARAPI WriteDouble(
        /* [in] */ Double value);

    CARAPI ReadString(
        /* [out] */ String* str);

    CARAPI WriteString(
        /* [in] */ const String& str);

    CARAPI ReadStruct(
        /* [out] */ HANDLE* address);

    CARAPI WriteStruct(
        /* [in] */ HANDLE value,
        /* [in] */ Int32 size);

    CARAPI ReadEMuid(
        /* [out] */ EMuid* id);

    CARAPI WriteEMuid(
        /* [in] */ const EMuid& id);

    CARAPI ReadEGuid(
        /* [out] */ EGuid* id);

    CARAPI WriteEGuid(
        /* [in] */ const EGuid& id);

    CARAPI ReadArrayOf(
        /* [out] */ HANDLE* array);

    CARAPI WriteArrayOf(
        /* [in] */ HANDLE array);

    CARAPI ReadArrayOfString(
        /* [out, callee] */ ArrayOf<String>** array);

    CARAPI WriteArrayOfString(
        /* [in] */ ArrayOf<String>* array);

    CARAPI ReadInterfacePtr(
        /* [out] */ HANDLE* itfPtr);

    CARAPI WriteInterfacePtr(
        /* [in] */ IInterface* value);

    // Place a file descriptor into the parcel.  The given fd must remain
    // valid for the lifetime of the parcel.
    CARAPI WriteFileDescriptor(
        /* [in] */ Int32 fd);

    // Place a file descriptor into the parcel.  A dup of the fd is made, which
    // will be closed once the parcel is destroyed.
    CARAPI WriteDupFileDescriptor(
        /* [in] */ Int32 fd);

    // Retrieve a file descriptor from the parcel.  This returns the raw fd
    // in the parcel, which you do not own -- use dup() to get your own copy.
    CARAPI ReadFileDescriptor(
        /* [out] */ Int32* fd);

    CARAPI ReadValue(
        /* [out] */ IInterface** value);

    CARAPI Clone(
        /* [in] */ IParcel* srcParcel);

    CARAPI GetDataPosition(
        /* [out] */ Int32* position);

    CARAPI SetDataPosition(
        /* [in] */ Int32 position);

    CARAPI GetDataSize(
        /* [out] */ Int32* size);

    CARAPI SetDataSize(
        /* [in] */ Int32 size);

    CARAPI GetDataCapacity(
        /* [out] */ Int32* capacity);

    CARAPI SetDataCapacity(
        /* [in] */ Int32 capacity);

    CARAPI GetDataPayload(
        /* [ouy] */ HANDLE* buffer);

    CARAPI_(MarshalHeader*) GetMarshalHeader();

private:
    CARAPI GrowElemBuffer(
        /* [in] */ Int32 minSize);

    CARAPI ReadValue(
        /* [in] */ PVoid value,
        /* [in] */ Int32 type);

    CARAPI WriteValue(
        /* [in] */ PVoid value,
        /* [in] */ Int32 type,
        /* [in] */ Int32 size);

    CARAPI_(Boolean) IsParcelable(
        /* [in] */ InterfacePack* interfacePack,
        /* [out] */ CIClassInfo** classInfo);

private:
    android::Parcel* mData;
    Boolean mFreeDataTag;
};

} // namespace IPC
} // namespace Elastos

#endif //__CREMOTEPARCEL_H__
