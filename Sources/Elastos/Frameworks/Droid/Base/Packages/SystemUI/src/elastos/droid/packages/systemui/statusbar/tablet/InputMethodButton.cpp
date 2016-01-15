#include "elastos/droid/systemui/statusbar/tablet/InputMethodButton.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

const String InputMethodButton::TAG
    = String("StatusBar/InputMethodButton");
const String InputMethodButton::TAG_TRY_SUPPRESSING_IME_SWITCHER
    = String("TrySuppressingImeSwitcher");

const Int32 InputMethodButton::ID_IME_BUTTON_VISIBILITY_AUTO = 0;
const Int32 InputMethodButton::ID_IME_BUTTON_VISIBILITY_ALWAYS_SHOW = 1;
const Int32 InputMethodButton::ID_IME_BUTTON_VISIBILITY_ALWAYS_HIDE = 2;

InputMethodButton::InputMethodButton()
    : mId(-1)
    , mShowButton(FALSE)
    , mScreenLocked(FALSE)
    , mHardKeyboardAvailable(FALSE)
{}

InputMethodButton::InputMethodButton(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : ImageView(context, attrs)
    , mId(-1)
    , mShowButton(FALSE)
    , mScreenLocked(FALSE)
    , mHardKeyboardAvailable(FALSE)
{
    // Resource Id of the input method button. This id is defined in status_bar.xml
    mId = GetId();
    // IME hookup
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&tmpObj);
    mImm = IInputMethodManager::Probe(tmpObj.Get());
    if (mImm == NULL) {
        Logger::E(TAG, "Failed to get input method service!");
    }
}

ECode InputMethodButton::InputMethodButton::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ASSERT_SUCCEEDED(ImageView::Init(context, attrs));

    // Resource Id of the input method button. This id is defined in status_bar.xml
    mId = GetId();
    // IME hookup
    AutoPtr<IInterface> tmpObj;
    mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&tmpObj);
    mImm = IInputMethodManager::Probe(tmpObj.Get());
    if (mImm == NULL) {
        Logger::E(TAG, "Failed to get input method service!");
    }
    return NOERROR;
}

//@Override
ECode InputMethodButton::OnAttachedToWindow()
{
    AutoPtr<IView> tmpView = FindViewById(mId);
    mIcon = IImageView::Probe(tmpView.Get());

    RefreshStatusIcon();
    return NOERROR;
}

// Refer to InputMethodManagerService.needsToShowImeSwitchOngoingNotification()
Boolean InputMethodButton::NeedsToShowIMEButtonWhenVisibilityAuto()
{
    if (mImm == NULL) return FALSE;

    AutoPtr<IObjectContainer> inputMethodList;
    mImm->GetEnabledInputMethodList((IObjectContainer**)&inputMethodList);
    Int32 N;
    inputMethodList->GetObjectCount(&N);
    if (N > 2) return TRUE;
    if (N < 1) return FALSE;

    Int32 subtypeCount;
    Int32 nonAuxCount = 0;
    Int32 auxCount = 0;
    AutoPtr<IInputMethodSubtype> nonAuxSubtype;
    AutoPtr<IInputMethodSubtype> auxSubtype;

    AutoPtr<IObjectEnumerator> emu, subtypeEmu;
    inputMethodList->GetObjectEnumerator((IObjectEnumerator**)&emu);
    Boolean hasNext, subtypeHasNext, isuxiliary;
    while ((emu->MoveNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> tmpObj;
        emu->Current((IInterface**)&tmpObj);
        AutoPtr<IInputMethodInfo> imi = IInputMethodInfo::Probe(tmpObj.Get());

        AutoPtr<IObjectContainer> subtypes;
        mImm->GetEnabledInputMethodSubtypeList(imi, TRUE, (IObjectContainer**)&subtypes);
        subtypes->GetObjectCount(&subtypeCount);
        if (subtypeCount == 0) {
            ++nonAuxCount;
        }
        else {
            subtypeEmu = NULL;
            subtypes->GetObjectEnumerator((IObjectEnumerator**)&subtypeEmu);
            while ((subtypeEmu->MoveNext(&subtypeHasNext), subtypeHasNext)) {
                AutoPtr<IInterface> tmpSubtypeObj;
                subtypeEmu->Current((IInterface**)&tmpSubtypeObj);
                AutoPtr<IInputMethodSubtype> subtype = IInputMethodSubtype::Probe(tmpSubtypeObj.Get());
                subtype->IsAuxiliary(&isuxiliary);
                if (!isuxiliary) {
                    ++nonAuxCount;
                    nonAuxSubtype = subtype;
                }
                else {
                    ++auxCount;
                    auxSubtype = subtype;
                }
            }
        }
    }

    if (nonAuxCount > 1 || auxCount > 1) {
        return TRUE;
    }
    else if (nonAuxCount == 1 && auxCount == 1) {
        if (nonAuxSubtype != NULL && auxSubtype != NULL) {
            String nasLocale, asLocale;
            Boolean nEnable, enable, contains;
            nonAuxSubtype->GetLocale(&nasLocale);
            auxSubtype->GetLocale(&asLocale);
            nonAuxSubtype->OverridesImplicitlyEnabledSubtype(&nEnable);
            auxSubtype->OverridesImplicitlyEnabledSubtype(&enable);
            nonAuxSubtype->ContainsExtraValueKey(TAG_TRY_SUPPRESSING_IME_SWITCHER, &contains);
            if ((nasLocale.Equals(asLocale) || nEnable || enable) && contains) {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

Boolean InputMethodButton::NeedsToShowIMEButton()
{
    if (!mShowButton || mScreenLocked) return FALSE;

    if (mHardKeyboardAvailable) {
        return TRUE;
    }

    Int32 visibility = LoadInputMethodSelectorVisibility();
    switch (visibility) {
        case ID_IME_BUTTON_VISIBILITY_AUTO:
            return NeedsToShowIMEButtonWhenVisibilityAuto();
        case ID_IME_BUTTON_VISIBILITY_ALWAYS_SHOW:
            return TRUE;
        case ID_IME_BUTTON_VISIBILITY_ALWAYS_HIDE:
            return FALSE;
    }
    return FALSE;
}

void InputMethodButton::RefreshStatusIcon()
{
    if (mIcon == NULL) {
        return;
    }
    if (!NeedsToShowIMEButton()) {
        SetVisibility(IView::GONE);
        return;
    }
    else {
        SetVisibility(IView::VISIBLE);
    }
    mIcon->SetImageResource(SystemUIR::drawable::ic_sysbar_ime);
}

Int32 InputMethodButton::LoadInputMethodSelectorVisibility()
{
    Int32 result;
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context = GetContext();
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    ss->GetInt32(resolver,
        ISettingsSecure::INPUT_METHOD_SELECTOR_VISIBILITY,
        ID_IME_BUTTON_VISIBILITY_AUTO, &result);
    return result;
}

ECode InputMethodButton::SetIconImage(
    /* [in] */ Int32 resId)
{
    if (mIcon != NULL) {
        mIcon->SetImageResource(resId);
    }
    return NOERROR;
}

ECode InputMethodButton::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean showButton)
{
    mToken = token;
    mShowButton = showButton;
    RefreshStatusIcon();
    return NOERROR;
}

ECode InputMethodButton::SetHardKeyboardStatus(
    /* [in] */ Boolean available)
{
    if (mHardKeyboardAvailable != available) {
        mHardKeyboardAvailable = available;
        RefreshStatusIcon();
    }
    return NOERROR;
}

ECode InputMethodButton::SetScreenLocked(
    /* [in] */ Boolean locked)
{
    mScreenLocked = locked;
    RefreshStatusIcon();
    return NOERROR;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
