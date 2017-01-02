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

#include "elastos/droid/internal/view/CRotationPolicy.h"
#include "elastos/droid/internal/view/RotationPolicy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CRotationPolicy, Singleton, IRotationPolicy)

CAR_SINGLETON_IMPL(CRotationPolicy)

ECode CRotationPolicy::IsRotationSupported(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationSupported(context);
    return NOERROR;
}

ECode CRotationPolicy::GetRotationLockOrientation(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::GetRotationLockOrientation(context);
    return NOERROR;
}

ECode CRotationPolicy::IsRotationLockToggleVisible(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationLockToggleVisible(context);
    return NOERROR;
}

ECode CRotationPolicy::IsRotationLocked(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = RotationPolicy::IsRotationLocked(context);
    return NOERROR;
}

ECode CRotationPolicy::SetRotationLock(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    RotationPolicy::SetRotationLock(context, enabled);
    return NOERROR;
}

ECode CRotationPolicy::SetRotationLockForAccessibility(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    RotationPolicy::SetRotationLockForAccessibility(context, enabled);
    return NOERROR;
}

ECode CRotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    RotationPolicy::RegisterRotationPolicyListener(context, listener);
    return NOERROR;
}

ECode CRotationPolicy::RegisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener,
    /* [in] */ Int32 userHandle)
{
    RotationPolicy::RegisterRotationPolicyListener(context, listener, userHandle);
    return NOERROR;
}

ECode CRotationPolicy::UnregisterRotationPolicyListener(
    /* [in] */ IContext* context,
    /* [in] */ IRotationPolicyListener* listener)
{
    RotationPolicy::UnregisterRotationPolicyListener(context, listener);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
