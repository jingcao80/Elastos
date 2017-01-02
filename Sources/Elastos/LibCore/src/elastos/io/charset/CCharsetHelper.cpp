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

#include "CCharsetHelper.h"
#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCharsetHelper, Singleton, ICharsetHelper)

CAR_SINGLETON_IMPL(CCharsetHelper)

ECode CCharsetHelper::AvailableCharsets(
    /* [out] */ ISortedMap** outsm)
{
    return Charset::AvailableCharsets(outsm);
}

ECode CCharsetHelper::ForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** charset)
{
    return Charset::ForName(charsetName, charset);
}

ECode CCharsetHelper::IsSupported(
    /* [in] */ const String& charsetName,
    /* [out] */ Boolean* result)
{
    return Charset::IsSupported(charsetName, result);
}

ECode CCharsetHelper::DefaultCharset(
    /* [out] */ICharset** charset)
{
    return Charset::DefaultCharset(charset);
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
