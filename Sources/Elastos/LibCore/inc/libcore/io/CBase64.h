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

#ifndef __LIBCORE_IO_CBASE64_H__
#define __LIBCORE_IO_CBASE64_H__

#include "Singleton.h"
#include "_Libcore_IO_CBase64.h"

using Elastos::Core::Singleton;

namespace Libcore {
namespace IO {

CarClass(CBase64)
    , public Singleton
    , public IBase64
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* inData,
        /* [out, callee] */ ArrayOf<Byte>** outData);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* inData,
        /* [in] */ Int32 len,
        /* [out, callee] */ ArrayOf<Byte>** outData);

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* inData,
        /* [out] */ String* outData);

private:
    static const Byte sMap[];
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_CBASE64_H__
