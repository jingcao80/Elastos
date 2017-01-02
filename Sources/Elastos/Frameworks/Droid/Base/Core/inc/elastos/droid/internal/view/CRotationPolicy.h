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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__

#include "_Elastos_Droid_Internal_View_CRotationPolicy.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CRotationPolicy)
    , public Singleton
    , public IRotationPolicy
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsRotationSupported(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetRotationLockOrientation(
        /* [in] */ IContext* context,
        /* [out] */ Int32* result);

    CARAPI IsRotationLockToggleVisible(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsRotationLocked(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI SetRotationLock(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    CARAPI SetRotationLockForAccessibility(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    CARAPI RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);

    CARAPI RegisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener,
        /* [in] */ Int32 userHandle);

    CARAPI UnregisterRotationPolicyListener(
        /* [in] */ IContext* context,
        /* [in] */ IRotationPolicyListener* listener);
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_CROTATIONPOLICY_H__
