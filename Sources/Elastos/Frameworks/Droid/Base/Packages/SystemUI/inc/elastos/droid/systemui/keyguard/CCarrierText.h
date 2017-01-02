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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CCARRIERTEXT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CCARRIERTEXT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CCarrierText.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include "elastos/droid/text/method/SingleLineTransformationMethod.h"
#include <elastos/droid/widget/TextView.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Text::Method::SingleLineTransformationMethod;
using Elastos::Droid::View::IView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::TextView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CCarrierText)
    , public TextView
    , public ICarrierText
{
private:
    /**
     * The status of this lock screen. Primarily used for widgets on LockScreen.
     */
    enum StatusMode {
        Normal, // Normal case (sim card present, it's not locked)
        NetworkLocked, // SIM card is 'network locked'.
        SimMissing, // SIM card is missing.
        SimMissingLocked, // SIM card is missing, and device isn't provisioned; don't allow access
        SimPukLocked, // SIM card is PUK locked because SIM entered wrong too many times
        SimLocked, // SIM card is currently locked
        SimPermDisabled, // SIM card is permanently disabled due to PUK unlock failure
        SimNotReady // SIM is not ready yet. May never be on devices w/o a SIM.
    };

    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CCarrierText::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CCarrierText* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnRefreshCarrierInfo(
            /* [in] */ ICharSequence* plmn,
            /* [in] */ ICharSequence* spn);

        //@Override
        CARAPI OnSimStateChanged(
            /* [in] */ IccCardConstantsState simState);

        CARAPI OnScreenTurnedOff(
            /* [in] */ Int32 why);

        CARAPI OnScreenTurnedOn();

    private:
        AutoPtr<ICharSequence> mPlmn;
        AutoPtr<ICharSequence> mSpn;
        IccCardConstantsState mSimState;
        CCarrierText* mHost;
    };

    class CarrierTextTransformationMethod
        : public SingleLineTransformationMethod
    {
    public:
        TO_STRING_IMPL("CCarrierText::CarrierTextTransformationMethod")

        CarrierTextTransformationMethod();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Boolean allCaps);

        //@Override
        CARAPI GetTransformation(
            /* [in] */ ICharSequence* source,
            /* [in] */ IView* view,
            /* [out] */ ICharSequence** formation);

    private:
        AutoPtr<ILocale> mLocale;
        Boolean mAllCaps;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCarrierText();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    CARAPI UpdateCarrierText(
        /* [in] */ IccCardConstantsState simState,
        /* [in] */ ICharSequence* plmn,
        /* [in] */ ICharSequence* spn);

    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

private:
    /**
     * Top-level function for creating carrier text. Makes text based on simState, PLMN
     * and SPN as well as device capabilities, such as being emergency call capable.
     *
     * @param simState
     * @param plmn
     * @param spn
     * @return
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCarrierTextForSimState(
        /* [in] */ IccCardConstantsState simState,
        /* [in] */ ICharSequence* plmn,
        /* [in] */ ICharSequence* spn);

    /*
     * Add emergencyCallMessage to carrier string only if phone supports emergency calls.
     */
    CARAPI_(AutoPtr<ICharSequence>) MakeCarrierStringOnEmergencyCapable(
        /* [in] */ ICharSequence* simMessage,
        /* [in] */ ICharSequence* emergencyCallMessage);

    /**
     * Determine the current status of the lock screen given the SIM state and other stuff.
     */
    CARAPI_(StatusMode) GetStatusForIccState(
        /* [in] */ IccCardConstantsState simState);

    static CARAPI_(AutoPtr<ICharSequence>) Concatenate(
        /* [in] */ ICharSequence* plmn,
        /* [in] */ ICharSequence* spn);

    CARAPI_(AutoPtr<ICharSequence>) GetCarrierHelpTextForSimState(
        /* [in] */ IccCardConstantsState simState,
        /* [in] */ const String& plmn,
        /* [in] */ const String& spn);

private:
    static AutoPtr<ICharSequence> mSeparator;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    AutoPtr<KeyguardUpdateMonitorCallback> mCallback;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CCARRIERTEXT_H__
