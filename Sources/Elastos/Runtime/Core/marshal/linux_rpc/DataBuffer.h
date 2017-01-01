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

#ifndef __DATABUFFER_H__
#define __DATABUFFER_H__

#include <eltypes.h>

namespace Elastos {
namespace RPC {

class DataBuffer : public ElLightRefBase
{
public:
    DataBuffer(
        /* [in] */ Int32 cap = 128);

    ~DataBuffer();

    CARAPI_(void) Append(
        /* [in] */ const void* buf,
        /* [in] */ Int32 size);

    CARAPI_(void*) Data();

    CARAPI_(Int32) DataSize();

    CARAPI_(void) Reset();

private:
    Int32 mSize;
    Int32 mCapacity;
    void* mBuf;
};

} // namespace RPC
} // namespace Elastos

#endif //__DATABUFFER_H__
