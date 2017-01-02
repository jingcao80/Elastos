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

#include "CMessageDigestHelper.h"
#include "MessageDigest.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(CMessageDigestHelper, Singleton, IMessageDigestHelper)
CAR_SINGLETON_IMPL(CMessageDigestHelper)

ECode CMessageDigestHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IMessageDigest** instance)
{
    return MessageDigest::GetInstance(algorithm, instance);
}

ECode CMessageDigestHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IMessageDigest** instance)
{
    return MessageDigest::GetInstance(algorithm, provider, instance);
}

ECode CMessageDigestHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IMessageDigest** instance)
{
    return MessageDigest::GetInstance(algorithm, provider, instance);
}

ECode CMessageDigestHelper::IsEqual(
    /* [in] */ ArrayOf<Byte>* digesta,
    /* [in] */ ArrayOf<Byte>* digestb,
    /* [out] */ Boolean* result)
{
    return MessageDigest::IsEqual(digesta, digestb, result);
}

} // namespace Security
} // namespace Elastos