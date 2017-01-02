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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CUrlQuerySanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CUrlQuerySanitizerHelper, Singleton, IUrlQuerySanitizerHelper)

CAR_SINGLETON_IMPL(CUrlQuerySanitizerHelper)

ECode CUrlQuerySanitizerHelper::GetAllIllegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllIllegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButNulLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButNulLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButWhitespaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButWhitespaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetUrlLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetUrlLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetUrlAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetUrlAndSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAmpLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAmpLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAmpAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAmpAndSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButNulAndAngleBracketsLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButNulAndAngleBracketsLegal(result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
