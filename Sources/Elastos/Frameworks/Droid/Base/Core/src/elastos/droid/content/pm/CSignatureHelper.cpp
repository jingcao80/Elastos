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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CSignatureHelper.h"
#include "elastos/droid/content/pm/CSignature.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CSignatureHelper, Singleton, ISignatureHelper)

CAR_SINGLETON_IMPL(CSignatureHelper)

ECode CSignatureHelper::AreExactMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b,
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match)
    *match = CSignature::AreExactMatch(a, b);
    return NOERROR;
}

ECode CSignatureHelper::AreEffectiveMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b,
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match)
    *match = CSignature::AreEffectiveMatch(a, b);
    return NOERROR;
}

ECode CSignatureHelper::Bounce(
    /* [in] */ ICertificateFactory* cf,
    /* [in] */ ISignature* s,
    /* [out] */ ISignature** sig)
{
    VALIDATE_NOT_NULL(sig)
    return CSignature::Bounce(cf, s, sig);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

