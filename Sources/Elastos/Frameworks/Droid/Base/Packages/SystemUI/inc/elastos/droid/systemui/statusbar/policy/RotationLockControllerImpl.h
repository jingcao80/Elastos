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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/view/RotationPolicy.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::View::IRotationPolicyListener;
using Elastos::Droid::Internal::View::RotationPolicy;
using Elastos::Core::Object;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/** Platform implementation of the rotation lock controller. **/
class RotationLockControllerImpl
    : public Object
    , public IRotationLockController
    , public IListenable
{
private:
    class LocalRotationPolicyListener: public RotationPolicy::RotationPolicyListener
    {
    public:
        LocalRotationPolicyListener(
            /* [in] */ RotationLockControllerImpl* host);

        // @Override
        CARAPI OnChange();

    private:
        RotationLockControllerImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI AddRotationLockControllerCallback(
        /* [in] */ IRotationLockControllerCallback* callback);

    CARAPI RemoveRotationLockControllerCallback(
        /* [in] */ IRotationLockControllerCallback* callback);

    CARAPI GetRotationLockOrientation(
        /* [out] */ Int32* result);

    CARAPI IsRotationLocked(
        /* [out] */ Boolean* result);

    CARAPI SetRotationLocked(
        /* [in] */ Boolean locked);

    CARAPI IsRotationLockAffordanceVisible(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetListening(
        /* [in] */ Boolean listening);

private:
    CARAPI_(void) NotifyChanged();

    CARAPI_(void) NotifyChanged(
        /* [in] */ IRotationLockControllerCallback* callback);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ICopyOnWriteArrayList> mCallbacks;  /*<RotationLockControllerCallback*/

    AutoPtr<IRotationPolicyListener> mRotationPolicyListener;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
