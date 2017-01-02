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

#include "URLDecoder.h"
#include "UriCodec.h"
#include "charset/CCharsetHelper.h"

namespace Elastos {
namespace Net {

using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharset;
using Libcore::Net::UriCodec;

ECode URLDecoder::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* decodedS)
{
    VALIDATE_NOT_NULL(decodedS);
    AutoPtr<ICharset> charset;
    AutoPtr<ICharsetHelper> charsetHelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&charsetHelper);
    charsetHelper->DefaultCharset((ICharset**)&charset);
    return UriCodec::Decode(s, TRUE, charset, TRUE, decodedS);
}

ECode URLDecoder::Decode(
    /* [in] */ const String& s,
    /* [in] */  const String& encoding,
    /* [out] */ String* decodedS)
{
    VALIDATE_NOT_NULL(decodedS);
    AutoPtr<ICharset> charset;
    AutoPtr<ICharsetHelper> charsetHelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&charsetHelper);
    FAIL_RETURN(charsetHelper->ForName(encoding, (ICharset**)&charset));
    return UriCodec::Decode(s, TRUE, charset, TRUE, decodedS);
}

} // namespace Net
} // namespace Elastos
