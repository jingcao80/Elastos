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

#include "CUriCodecHelper.h"
#include "UriCodec.h"

namespace Libcore {
namespace Net {

CAR_INTERFACE_IMPL(CUriCodecHelper, Singleton, IUriCodecHelper)

CAR_SINGLETON_IMPL(CUriCodecHelper)

ECode CUriCodecHelper::ValidateSimple(
    /* [in] */ const String& s,
    /* [in] */ const String& legal)
{
    return UriCodec::ValidateSimple(s,legal);
}

ECode CUriCodecHelper::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return UriCodec::Decode(s,result);
}

ECode CUriCodecHelper::Decode(
    /* [in] */ const String& s,
    /* [in] */ Boolean convertPlus,
    /* [in] */ ICharset* charset,
    /* [in] */ Boolean throwOnFailure,
    /* [out] */ String* decodedString)
{
    return UriCodec::Decode(s,convertPlus,charset,throwOnFailure,decodedString);
}

} // namespace Net
} // namespace Libcore
