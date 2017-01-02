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

#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__

#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

class StandardCharsets
{
private:
    StandardCharsets();

public:
    /**
    * The ISO-8859-1 charset.
    */
    static const AutoPtr<ICharset> ISO_8859_1;

    /**
    * The US-ASCII charset.
    */
    static const AutoPtr<ICharset> US_ASCII;

    /**
    * The UTF-8 charset.
    */
    static const AutoPtr<ICharset> UTF_8;

    /**
    * The UTF-16 charset.
    */
    static const AutoPtr<ICharset> UTF_16;

    /**
    * The UTF-16BE (big-endian) charset.
    */
    static const AutoPtr<ICharset> UTF_16BE;

    /**
    * The UTF-16LE (little-endian) charset.
    */
    static const AutoPtr<ICharset> UTF_16LE;

private:
    static Boolean Init();
    static Boolean sIsInit;
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__
