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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_KEYGUARDINDICATIONCONTROLLER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_KEYGUARDINDICATIONCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::SystemUI::StatusBar::Phone::IKeyguardIndicationTextView;
using Elastos::Droid::SystemUI::Keyguard::KeyguardUpdateMonitorCallback;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardUpdateMonitorCallback;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardUpdateMonitorBatteryStatus;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Controls the little text indicator on the keyguard.
 */
class KeyguardIndicationController
    : public Object
    , public IKeyguardIndicationController
{
private:

    class UpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        UpdateMonitorCallback(
            /* [in] */ KeyguardIndicationController* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnRefreshBatteryInfo(
            /* [in] */ IKeyguardUpdateMonitorBatteryStatus* status);

    private:
        KeyguardIndicationController* mHost;
    };

    class ControllerBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("KeyguardIndicationController.ControllerBroadcastReceiver")

        ControllerBroadcastReceiver(
            /* [in] */ KeyguardIndicationController* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        KeyguardIndicationController* mHost;
    };

    class ControllerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("KeyguardIndicationController::ControllerHandler")

        ControllerHandler(
            /* [in] */ KeyguardIndicationController* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        KeyguardIndicationController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    KeyguardIndicationController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IKeyguardIndicationTextView* textView);

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    /**
     * Sets the indication that is shown if nothing else is showing.
     */
    CARAPI SetRestingIndication(
        /* [in] */ const String& restingIndication);

    /**
     * Hides transient indication in {@param delayMs}.
     */
    CARAPI HideTransientIndicationDelayed(
        /* [in] */ Int64 delayMs);

    /**
     * Shows {@param transientIndication} until it is hidden by {@link #hideTransientIndication}.
     */
    CARAPI ShowTransientIndication(
        /* [in] */ Int32 transientIndication);

    /**
     * Shows {@param transientIndication} until it is hidden by {@link #hideTransientIndication}.
     */
    CARAPI ShowTransientIndication(
        /* [in] */ const String& transientIndication);

    /**
     * Hides transient indication.
     */
    CARAPI HideTransientIndication();

private:
    CARAPI_(void) UpdateIndication();

    CARAPI_(String) ComputeIndication();

    CARAPI_(String) ComputePowerIndication();

private:
    static const String TAG;
    static const Int32 MSG_HIDE_TRANSIENT;

    AutoPtr<IContext> mContext;
    AutoPtr<IKeyguardIndicationTextView> mTextView;
    AutoPtr<IIBatteryStats> mBatteryInfo;

    String mRestingIndication;
    String mTransientIndication;
    Boolean mVisible;

    Boolean mPowerPluggedIn;
    Boolean mPowerCharged;

    AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateMonitor;
    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IHandler> mHandler;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_KEYGUARDINDICATIONCONTROLLER_H__
