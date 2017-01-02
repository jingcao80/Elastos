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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"

using Elastos::Droid::SystemUI::Keyguard::IKeyguardUpdateMonitor;
using Elastos::Droid::SystemUI::Keyguard::KeyguardUpdateMonitorCallback;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardUpdateMonitorCallback;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Caches whether the current unlock method is insecure, taking trust into account. This information
 * might be a little bit out of date and should not be used for actual security decisions; it should
 * be only used for visual indications.
 */
class UnlockMethodCache: public Object
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        MyKeyguardUpdateMonitorCallback(
            /* [in] */ UnlockMethodCache* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnUserSwitchComplete(Int32 userId) {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }

        //@Override
        CARAPI OnTrustChanged(Int32 userId) {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }

        //@Override
        CARAPI OnTrustManagedChanged(Int32 userId) {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }

        //@Override
        CARAPI OnScreenTurnedOn() {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }

        //@Override
        CARAPI OnFingerprintRecognized(Int32 userId) {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }

        //@Override
        CARAPI OnFaceUnlockStateChanged(Boolean running, Int32 userId) {
            return mHost->UpdateMethodSecure(FALSE /* updateAlways */);
        }
    private:
        UnlockMethodCache* mHost;
    };

public:
    static CARAPI_(AutoPtr<UnlockMethodCache>) GetInstance(
        /* [in] */ IContext* context);

    /**
     * @return whether the current security method is secure, i. e. the bouncer will be shown
     */
    CARAPI_(Boolean) IsMethodInsecure();

    CARAPI_(void) AddListener(
        /* [in] */ IOnUnlockMethodChangedListener* listener);

    CARAPI_(void) RemoveListener(
        /* [in] */ IOnUnlockMethodChangedListener* listener);

    CARAPI_(Boolean) IsTrustManaged();

    CARAPI_(Boolean) IsFaceUnlockRunning();

private:
    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI UpdateMethodSecure(
        /* [in] */ Boolean updateAlways);

    CARAPI_(void) NotifyListeners(
        /* [in] */ Boolean secure);

private:
    static AutoPtr<UnlockMethodCache> sInstance;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IKeyguardUpdateMonitor> mKeyguardUpdateMonitor;
    AutoPtr<IKeyguardUpdateMonitorCallback> mCallback;
    AutoPtr<IArrayList> mListeners;  /*<OnUnlockMethodChangedListener*/
    Boolean mMethodInsecure;
    Boolean mTrustManaged;
    Boolean mFaceUnlockRunning;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__
