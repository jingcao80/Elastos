
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CCARRIERTEXT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CCARRIERTEXT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CCarrierText.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Widget::TextView;

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
        TO_STRING_IMPL("CCarrierText::MyKeyguardUpdateMonitorCallback")

        CarrierTextTransformationMethod(
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
