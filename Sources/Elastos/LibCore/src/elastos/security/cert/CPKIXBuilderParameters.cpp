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

#include "CPKIXBuilderParameters.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXBuilderParameters)
CPKIXBuilderParameters::CPKIXBuilderParameters()
    : mMaxPathLength(5)
{}

ECode CPKIXBuilderParameters::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("[\n");
    String base;
    PKIXParameters::ToString(&base);
    sb.Append(base);
    sb.Append(" Max Path Length: ");
    sb.Append(mMaxPathLength);
    sb.Append("\n]");
    return sb.ToString(str);

}

ECode CPKIXBuilderParameters::GetMaxPathLength(
    /* [out] */ Int32 *length)
{
    VALIDATE_NOT_NULL(length)
    *length = mMaxPathLength;
    return NOERROR;
}

ECode CPKIXBuilderParameters::SetMaxPathLength(
    /* [in] */ Int32 maxPathLength)
{
    if (mMaxPathLength < -1) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    mMaxPathLength = maxPathLength;
    return NOERROR;
}

ECode CPKIXBuilderParameters::constructor()
{
    return NOERROR;
}

ECode CPKIXBuilderParameters::constructor(
    /* [in] */ ISet *trustAnchors,
    /* [in] */ ICertSelector *targetConstraints)
{
    PKIXParameters::constructor(trustAnchors);
    return PKIXParameters::SetTargetCertConstraints(targetConstraints);
}

ECode CPKIXBuilderParameters::constructor(
    /* [in] */ IKeyStore *keyStore,
    /* [in] */ ICertSelector *targetConstraints)
{
    PKIXParameters::constructor(keyStore);
    return PKIXParameters::SetTargetCertConstraints(targetConstraints);
}

}
}
}
