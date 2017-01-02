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

#ifndef __DATAWRAPPER_H__
#define __DATAWRAPPER_H__

#include <elastos.h>

using namespace Elastos;

class DataWrapper : public ElLightRefBase
{
public:
    DataWrapper(
        /* [in] */ Byte* data,
        /* [in] */ Int32 size);

    CARAPI ReadString(
        /* [out] */ String* value);

    CARAPI WriteString(
        /* [in] */ const String& value);

    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI ReadEMuid(
        /* [out] */ EMuid* value);

    CARAPI WriteEMuid(
        /* [in] */ const EMuid& value);

    CARAPI_(Byte*) Data();

    CARAPI_(UInt32) DataSize();

private:
    Byte* mData;
    UInt32 mDataCapacity;
    UInt32 mDataPos;
};

#endif //__DATAWRAPPER_H__
