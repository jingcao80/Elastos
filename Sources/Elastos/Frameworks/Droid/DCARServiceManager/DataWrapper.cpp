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

#include "DataWrapper.h"
#include <netinet/in.h>

struct __UUID
{
    UInt32 Data1;
    UInt32 Data2;
    UInt32 Data3;
    UInt32 Data4;
};

DataWrapper::DataWrapper(
    /* [in] */ Byte* data,
    /* [in] */ Int32 size)
    : mData(data)
    , mDataCapacity(size)
    , mDataPos(0)
{}

ECode DataWrapper::ReadString(
    /* [out] */ String* value)
{
    *value = NULL;
    if (mDataPos >= mDataCapacity || mData[mDataCapacity] != '\0') return E_FAIL;
    *value = (char*)(mData + mDataPos);
    mDataPos += value->GetByteLength() + 1;
    return NOERROR;
}

ECode DataWrapper::WriteString(
    /* [in] */ const String& value)
{
    Int32 len = strlen(value.string());
    if (mDataPos + len + 1 > mDataCapacity) return E_FAIL;
    strcpy((char*)(mData + mDataPos), value.string());
    mDataPos += len;
    mData[mDataPos] = '\0';
    mDataPos++;
    return NOERROR;
}

ECode DataWrapper::ReadInt32(
    /* [out] */ Int32* value)
{
    *value = 0;
    if (mDataPos + 4 >= mDataCapacity) return E_FAIL;
    *value = ntohl(*(Int32*)(mData + mDataPos));
    mDataPos += 4;
    return NOERROR;
}

ECode DataWrapper::WriteInt32(
    /* [in] */ Int32 value)
{
    if (mDataPos + 4 >= mDataCapacity)  return E_FAIL;
    *(Int32*)(mData + mDataPos) = htonl(value);
    mDataPos += 4;
    return NOERROR;
}

ECode DataWrapper::ReadEMuid(
    /* [out] */ EMuid* value)
{
    if (mDataPos + sizeof(EMuid) >= mDataCapacity) return E_FAIL;

    Int32 data1, data2, data3, data4;
    ReadInt32(&data1);
    ReadInt32(&data2);
    ReadInt32(&data3);
    ReadInt32(&data4);

    struct __UUID* id = (struct __UUID*)value;
    id->Data1 = data1;
    id->Data2 = data2;
    id->Data3 = data3;
    id->Data4 = data4;
    return NOERROR;
}

ECode DataWrapper::WriteEMuid(
    /* [in] */ const EMuid& value)
{
    if (mDataPos + sizeof(EMuid) >= mDataCapacity) return E_FAIL;

    struct __UUID* id = (struct __UUID*)&value;
    WriteInt32(id->Data1);
    WriteInt32(id->Data2);
    WriteInt32(id->Data3);
    WriteInt32(id->Data4);
    return NOERROR;
}

Byte* DataWrapper::Data()
{
    return mData;
}

UInt32 DataWrapper::DataSize()
{
    return mDataPos;
}
