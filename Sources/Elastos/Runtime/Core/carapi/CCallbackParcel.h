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

#ifndef __CCALLBACKPARCEL_H__
#define __CCALLBACKPARCEL_H__

#include <elapi.h>

_ELASTOS_NAMESPACE_BEGIN

class CCallbackParcel
    : public ElLightRefBase
    , public IParcel
{
public:
    CCallbackParcel();

    virtual ~CCallbackParcel();

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
        /* [ou] */ Handle32* address);

    CARAPI WriteStruct(
        /* [in] */ Handle32 value,
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
        /* [out] */ Handle32* array);

    CARAPI WriteArrayOf(
        /* [in] */ Handle32 array);

    CARAPI ReadArrayOfString(
        /* [out, callee] */ ArrayOf<String>** array);

    CARAPI WriteArrayOfString(
        /* [in] */ ArrayOf<String>* array);

    CARAPI ReadInterfacePtr(
        /* [out] */ Handle32* itfPtr);

    CARAPI WriteInterfacePtr(
        /* [in] */ IInterface* value);

    CARAPI WriteFileDescriptor(
        /* [in] */ Int32 fd);

    CARAPI WriteDupFileDescriptor(
        /* [in] */ Int32 fd);

    CARAPI ReadFileDescriptor(
        /* [out] */ Int32* fd);

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
        /* [ouy] */ Handle32* buffer);

private:
    CARAPI GrowTypeBuffer();

    CARAPI GrowElemBuffer();

    CARAPI GrowDataBuffer(
        /* [in] */ Int32 minSize);

    CARAPI WriteValue(
        /* [in] */ PVoid value,
        /* [in] */ Int32 type,
        /* [in] */ Int32 size);

private:
    Int32 mElemCount;

    Byte* mElemTypes;
    Int32 mTypeBufCapacity;

    Byte* mElemBuf;
    Byte* mElemPtr;
    Int32 mElemBufCapacity;

    Byte* mDataBuf;
    Byte* mDataPtr;
    Int32 mDataBufCapacity;
};

_ELASTOS_NAMESPACE_END

#endif
