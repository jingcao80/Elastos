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

#ifndef __ELASTOS_IO_CHARSET_CMODIFIEDUTF8_H__
#define __ELASTOS_IO_CHARSET_CMODIFIEDUTF8_H__

#include "_Elastos_IO_Charset_CModifiedUtf8.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CModifiedUtf8)
    , public Singleton
    , public IModifiedUtf8
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /*
     * Decodes a byte array containing modified UTF-8 bytes into
     * a string.
     * Note that although this method decodes the (supposedly
     * impossible) zero byte to U+0000, that's what the RI does too
     */
    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 utfSize,
        /* [out] */ String* string);

    CARAPI CountBytes(
        /* [in] */ const String& s,
        /* [in] */ Boolean shortLength,
        /* [out] */ Int64* num);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* dst,
        /* [in] */ Int32 offset,
        /* [in] */ const String& s);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARSET_CMODIFIEDUTF8_H__
