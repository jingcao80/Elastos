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

#include "StandardCharsets.h"
#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

const AutoPtr<ICharset> StandardCharsets::ISO_8859_1;
const AutoPtr<ICharset> StandardCharsets::US_ASCII;
const AutoPtr<ICharset> StandardCharsets::UTF_8;
const AutoPtr<ICharset> StandardCharsets::UTF_16;
const AutoPtr<ICharset> StandardCharsets::UTF_16BE;
const AutoPtr<ICharset> StandardCharsets::UTF_16LE;
Boolean StandardCharsets::sIsInit = Init();

StandardCharsets::StandardCharsets()
{}

Boolean StandardCharsets::Init()
{
    Charset::ForName(String("ISO-8859-1"), (ICharset**)&ISO_8859_1);
    Charset::ForName(String("US-ASCII"), (ICharset**)&US_ASCII);
    Charset::ForName(String("UTF-8"), (ICharset**)&UTF_8);
    Charset::ForName(String("UTF-16"), (ICharset**)&UTF_16);
    Charset::ForName(String("UTF-16BE"), (ICharset**)&UTF_16BE);
    Charset::ForName(String("UTF-16LE"), (ICharset**)&UTF_16LE);
    return TRUE;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
