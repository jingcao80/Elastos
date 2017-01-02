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

#include "CURLEncoder.h"
#include "charset/CCharsetHelper.h"
#include "charset/StandardCharsets.h"
#include "Character.h"

using Elastos::IO::Charset::StandardCharsets;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;

namespace Elastos {
namespace Net {

CURLEncoder::MyUriCodec CURLEncoder::ENCODER;

CAR_INTERFACE_IMPL(CURLEncoder, Singleton, IURLEncoder)

CAR_SINGLETON_IMPL(CURLEncoder)

ECode CURLEncoder::Encode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    ENCODER.Encode(s, StandardCharsets::UTF_8, result);
    return NOERROR;
}

ECode CURLEncoder::Encode(
    /* [in] */ const String& s,
    /* [in] */ const String& charsetName,
    /* [out] */ String* result)
{
    AutoPtr<ICharsetHelper> helper;
    AutoPtr<ICharset> charset;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
    FAIL_RETURN(helper->ForName(charsetName, (ICharset**)&charset));
    return ENCODER.Encode(s, charset, result);
}

} // namespace Net
} // namespace Elastos
