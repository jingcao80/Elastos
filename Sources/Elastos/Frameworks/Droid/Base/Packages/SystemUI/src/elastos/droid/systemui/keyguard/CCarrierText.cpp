
#include "elastos/droid/systemui/keyguard/CCarrierText.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_None;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERSO_LOCKED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERM_DISABLED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_NOT_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_UNKNOWN;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

//======================================================================
// CCarrierText::MyKeyguardUpdateMonitorCallback
//======================================================================
ECode CCarrierText::MyKeyguardUpdateMonitorCallback::OnRefreshCarrierInfo(
    /* [in] */ ICharSequence* plmn,
    /* [in] */ ICharSequence* spn)
{
    mPlmn = plmn;
    mSpn = spn;
    return mHost->UpdateCarrierText(mSimState, mPlmn, mSpn);
}

ECode CCarrierText::MyKeyguardUpdateMonitorCallback::OnSimStateChanged(
    /* [in] */ IccCardConstantsState simState)
{
    mSimState = simState;
    return mHost->UpdateCarrierText(mSimState, mPlmn, mSpn);
}

ECode CCarrierText::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    return mHost->SetSelected(FALSE);
}

ECode CCarrierText::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    return mHost->SetSelected(TRUE);
}

//======================================================================
// CCarrierText::CarrierTextTransformationMethod
//======================================================================
CCarrierText::CarrierTextTransformationMethod::CarrierTextTransformationMethod()
    : mAllCaps(FALSE)
{}

ECode CCarrierText::CarrierTextTransformationMethod::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean allCaps)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    configuration->GetLocale((ILocale**)&mLocale);
    mAllCaps = allCaps;
    return SingleLineTransformationMethod::constructor();
}

ECode CCarrierText::CarrierTextTransformationMethod::GetTransformation(
    /* [in] */ ICharSequence* source,
    /* [in] */ IView* view,
    /* [out] */ ICharSequence** formation)
{
    VALIDATE_NOT_NULL(formation);

    AutoPtr<ICharSequence> src;
    SingleLineTransformationMethod::GetTransformation(source, view,
            (ICharSequence**)&src);

    if (mAllCaps && src != NULL) {
        String str;
        src->ToString(&str);
        src = CoreUtils::Convert(str.ToUpperCase(/*mLocale*/));
    }

    *formation = src;
    REFCOUNT_ADD(*formation);
    return NOERROR;
}

//======================================================================
// CCarrierText
//======================================================================
AutoPtr<ICharSequence> CCarrierText::mSeparator;

CAR_OBJECT_IMPL(CCarrierText)

CAR_INTERFACE_IMPL(CCarrierText, TextView, ICarrierText)

CCarrierText::CCarrierText()
{
}

ECode CCarrierText::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CCarrierText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);

    mCallback = new MyKeyguardUpdateMonitorCallback(this);

    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&mLockPatternUtils);
    Boolean useAllCaps;
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::CarrierText);
    AutoPtr<ITypedArray> a;
    theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    //try {
    a->GetBoolean(R::styleable::CarrierText_allCaps, FALSE, &useAllCaps);
    //} finally {
    a->Recycle();
    //}
    AutoPtr<CarrierTextTransformationMethod> method = new CarrierTextTransformationMethod();
    method->constructor(mContext, useAllCaps);
    return SetTransformationMethod(method.Get());
}

ECode CCarrierText::UpdateCarrierText(
    /* [in] */ IccCardConstantsState simState,
    /* [in] */ ICharSequence* plmn,
    /* [in] */ ICharSequence* spn)
{
    return SetText(GetCarrierTextForSimState(simState, plmn, spn));
}

ECode CCarrierText::OnFinishInflate()
{
    TextView::OnFinishInflate();

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String str;
    resources->GetString(R::string::kg_text_message_separator, &str);
    mSeparator = CoreUtils::Convert(str);

    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Boolean screenOn;
    monitor->IsScreenOn(&screenOn);
    return SetSelected(screenOn); // Allow marquee to work.
}

ECode CCarrierText::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    return monitor->RegisterCallback(mCallback);
}

ECode CCarrierText::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    return monitor->RemoveCallback(mCallback);
}

AutoPtr<ICharSequence> CCarrierText::GetCarrierTextForSimState(
    /* [in] */ IccCardConstantsState simState,
    /* [in] */ ICharSequence* plmn,
    /* [in] */ ICharSequence* spn)
{
    AutoPtr<ICharSequence> carrierText;
    CCarrierText::StatusMode status = GetStatusForIccState(simState);
    switch (status) {
        case Normal:
            carrierText = Concatenate(plmn, spn);
            break;

        case SimNotReady:
            carrierText = NULL; // nothing to display yet.
            break;

        case NetworkLocked:
        {
            AutoPtr<ICharSequence> tmp;
            mContext->GetText(R::string::keyguard_network_locked_message, (ICharSequence**)&tmp);
            carrierText = MakeCarrierStringOnEmergencyCapable(tmp, plmn);
            break;
        }
        case SimMissing:
        {
            // Shows "No SIM card | Emergency calls only" on devices that are voice-capable.
            // This depends on mPlmn containing the text "Emergency calls only" when the radio
            // has some connectivity. Otherwise, it should be null or empty and just show
            // "No SIM card"
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ICharSequence> tmp;
            context->GetText(R::string::keyguard_missing_sim_message_short, (ICharSequence**)&tmp);
            carrierText = MakeCarrierStringOnEmergencyCapable(tmp, plmn);
            break;
        }
        case SimPermDisabled:
        {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            context->GetText(R::string::keyguard_permanent_disabled_sim_message_short, (ICharSequence**)&carrierText);
            break;
        }
        case SimMissingLocked:
        {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ICharSequence> tmp;
            context->GetText(R::string::keyguard_missing_sim_message_short, (ICharSequence**)&tmp);
            carrierText = MakeCarrierStringOnEmergencyCapable(tmp, plmn);
            break;
        }
        case SimLocked:
        {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ICharSequence> tmp;
            context->GetText(R::string::keyguard_sim_locked_message, (ICharSequence**)&tmp);
            carrierText = MakeCarrierStringOnEmergencyCapable(tmp, plmn);
            break;
        }
        case SimPukLocked:
        {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ICharSequence> tmp;
            context->GetText(R::string::keyguard_sim_puk_locked_message, (ICharSequence**)&tmp);
            carrierText = MakeCarrierStringOnEmergencyCapable(tmp, plmn);
            break;
        }
    }

    return carrierText;
}

AutoPtr<ICharSequence> CCarrierText::MakeCarrierStringOnEmergencyCapable(
    /* [in] */ ICharSequence* simMessage,
    /* [in] */ ICharSequence* emergencyCallMessage)
{
    Boolean res;
    if (mLockPatternUtils->IsEmergencyCallCapable(&res), res) {
        return Concatenate(simMessage, emergencyCallMessage);
    }
    return simMessage;
}

CCarrierText::StatusMode CCarrierText::GetStatusForIccState(
    /* [in] */ IccCardConstantsState simState)
{
    // Since reading the SIM may take a while, we assume it is present until told otherwise.
    if (simState == IccCardConstantsState_None) {
        return Normal;
    }

    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Boolean res;
    monitor->IsDeviceProvisioned(&res);
    Boolean missingAndNotProvisioned =
            !res && (simState == IccCardConstantsState_ABSENT ||
            simState == IccCardConstantsState_PERM_DISABLED);

    // Assume we're NETWORK_LOCKED if not provisioned
    simState = missingAndNotProvisioned ? IccCardConstantsState_PERSO_LOCKED : simState;
    switch (simState) {
        case IccCardConstantsState_ABSENT:
            return SimMissing;
        case IccCardConstantsState_PERSO_LOCKED:
            return SimMissingLocked;
        case IccCardConstantsState_NOT_READY:
            return SimNotReady;
        case IccCardConstantsState_PIN_REQUIRED:
            return SimLocked;
        case IccCardConstantsState_PUK_REQUIRED:
            return SimPukLocked;
        case IccCardConstantsState_READY:
            return Normal;
        case IccCardConstantsState_PERM_DISABLED:
            return SimPermDisabled;
        case IccCardConstantsState_UNKNOWN:
            return SimMissing;
    }
    return SimMissing;
}

AutoPtr<ICharSequence> CCarrierText::Concatenate(
    /* [in] */ ICharSequence* plmn,
    /* [in] */ ICharSequence* spn)
{
    Boolean plmnValid = !TextUtils::IsEmpty(plmn);
    Boolean spnValid = !TextUtils::IsEmpty(spn);
    if (plmnValid && spnValid) {
        if (Object::Equals(plmn, spn)) {
            return plmn;
        }
        else {
            StringBuilder sb;
            sb.Append(plmn);
            sb.Append(mSeparator);
            sb.Append(spn);
            AutoPtr<ICharSequence> tmp = CoreUtils::Convert(sb.ToString());
            return tmp;
        }
    }
    else if (plmnValid) {
        return plmn;
    }
    else if (spnValid) {
        return spn;
    }
    else {
        AutoPtr<ICharSequence> tmp = CoreUtils::Convert("");
        return tmp;
    }
}

AutoPtr<ICharSequence> CCarrierText::GetCarrierHelpTextForSimState(
    /* [in] */ IccCardConstantsState simState,
    /* [in] */ const String& plmn,
    /* [in] */ const String& spn)
{
    Int32 carrierHelpTextId = 0;
    StatusMode status = GetStatusForIccState(simState);
    switch ((Int32)status) {
        case NetworkLocked:
            carrierHelpTextId = R::string::keyguard_instructions_when_pattern_disabled;
            break;

        case SimMissing:
            carrierHelpTextId = R::string::keyguard_missing_sim_instructions_long;
            break;

        case SimPermDisabled:
            carrierHelpTextId = R::string::keyguard_permanent_disabled_sim_instructions;
            break;

        case SimMissingLocked:
            carrierHelpTextId = R::string::keyguard_missing_sim_instructions;
            break;

        case Normal:
        case SimLocked:
        case SimPukLocked:
            break;
    }

    AutoPtr<ICharSequence> tmp;
    mContext->GetText(carrierHelpTextId, (ICharSequence**)&tmp);
    return tmp;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
