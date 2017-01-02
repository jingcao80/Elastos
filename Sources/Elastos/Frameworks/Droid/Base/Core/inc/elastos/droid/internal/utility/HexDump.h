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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_HEXDUMP_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_HEXDUMP_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class HexDump
{
public:
    static CARAPI_(String) DumpHexString(
        /* [in] */ ArrayOf<Byte>* array);

    static CARAPI_(String) DumpHexString(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI_(String) ToHexString(
        /* [in] */ Byte b);

    static CARAPI_(String) ToHexString(
        /* [in] */ ArrayOf<Byte>* array);

    static CARAPI_(String) ToHexString(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI_(String) ToHexString(
        /* [in] */ Int32 i);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ToByteArray(
        /* [in] */ Byte b);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ToByteArray(
        /* [in] */ Int32 i);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) HexStringToByteArray(
        /* [in] */ const String& hexString);

private:
    HexDump();

    static CARAPI_(Int32) ToByte(
        /* [in] */ Char32 c);

private:
    static const Char32 HEX_DIGITS[16];
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_HEXDUMP_H__
