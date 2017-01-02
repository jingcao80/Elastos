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

#include "elastos/droid/teleservice/phone/CEmergencyDialer.h"
#include "elastos/droid/teleservice/phone/CEmergencyDialerBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/SpecialCharSequenceMgr.h"
#include "elastos/droid/phone/common/CHapticFeedback.h"
#include "elastos/droid/phone/common/util/ViewUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Phone::Common::CHapticFeedback;
using Elastos::Droid::Phone::Common::Dialpad::IDialpadKeyButton;
using Elastos::Droid::Phone::Common::Dialpad::EIID_IDialpadKeyButtonOnPressedListener;
using Elastos::Droid::Phone::Common::Util::ViewUtil;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL_5(CEmergencyDialer::InnerListener, Object, IViewOnClickListener, \
    IViewOnLongClickListener, IViewOnKeyListener, \
    ITextWatcher, IDialpadKeyButtonOnPressedListener)

CEmergencyDialer::InnerListener::InnerListener(
    /* [in] */ CEmergencyDialer* host)
    : mHost(host)
{
}

ECode CEmergencyDialer::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CEmergencyDialer::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 changeCount)
{
    return mHost->OnTextChanged(input, start, before, changeCount);
}

ECode CEmergencyDialer::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* input)
{
    return mHost->AfterTextChanged(input);
}

ECode CEmergencyDialer::InnerListener::OnKey(
    /* [in] */ IView* view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnKey(view, keyCode, event, result);
}

ECode CEmergencyDialer::InnerListener::OnClick(
    /* [in] */ IView* view)
{
    return mHost->OnClick(view);
}

ECode CEmergencyDialer::InnerListener::OnPressed(
    /* [in] */ IView* view,
    /* [in] */ Boolean pressed)
{
    return mHost->OnPressed(view, pressed);
}

ECode CEmergencyDialer::InnerListener::OnLongClick(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    return mHost->OnLongClick(view, result);
}


ECode CEmergencyDialer::MyBroadcastReceiver::constructor(
    /* [in] */ IEmergencyDialer* host)
{
    mHost = (CEmergencyDialer*)host;
    return BroadcastReceiver::constructor();
}

ECode CEmergencyDialer::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->Finish();
    }
    return NOERROR;
}

const String CEmergencyDialer::LAST_NUMBER("lastNumber");

static AutoPtr<ArrayOf<Int32> > initDIALER_KEYS()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(12);

    (*array)[0] = Elastos::Droid::TeleService::R::id::one;
    (*array)[1] = Elastos::Droid::TeleService::R::id::two;
    (*array)[2] = Elastos::Droid::TeleService::R::id::three;
    (*array)[3] = Elastos::Droid::TeleService::R::id::four;
    (*array)[4] = Elastos::Droid::TeleService::R::id::five;
    (*array)[5] = Elastos::Droid::TeleService::R::id::six;
    (*array)[6] = Elastos::Droid::TeleService::R::id::seven;
    (*array)[7] = Elastos::Droid::TeleService::R::id::eight;
    (*array)[8] = Elastos::Droid::TeleService::R::id::nine;
    (*array)[9] = Elastos::Droid::TeleService::R::id::star;
    (*array)[10] = Elastos::Droid::TeleService::R::id::zero;
    (*array)[11] = Elastos::Droid::TeleService::R::id::pound;
    return array;
}

const AutoPtr<ArrayOf<Int32> > CEmergencyDialer::DIALER_KEYS = initDIALER_KEYS();

const Boolean CEmergencyDialer::DBG = FALSE;
const String CEmergencyDialer::TAG("EmergencyDialer");

const Int32 CEmergencyDialer::TONE_LENGTH_MS = 150;

const Int32 CEmergencyDialer::TONE_RELATIVE_VOLUME = 80;

const Int32 CEmergencyDialer::DIAL_TONE_STREAM_TYPE = IAudioManager::STREAM_DTMF;

const Int32 CEmergencyDialer::BAD_EMERGENCY_NUMBER_DIALOG = 0;

CAR_INTERFACE_IMPL(CEmergencyDialer, Activity, IEmergencyDialer)

CAR_OBJECT_IMPL(CEmergencyDialer)

CEmergencyDialer::CEmergencyDialer()
    : mDTMFToneEnabled(FALSE)
    , mLastNumber(NULL)
{
    CHapticFeedback::New((IHapticFeedback**)&mHaptic);

    CEmergencyDialerBroadcastReceiver::New(this, (IBroadcastReceiver**)&mBroadcastReceiver);
}

ECode CEmergencyDialer::constructor()
{
    return Activity::constructor();
}

ECode CEmergencyDialer::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    // Do nothing
    return NOERROR;
}

ECode CEmergencyDialer::OnTextChanged(
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 changeCount)
{
    // Do nothing
    return NOERROR;
}

ECode CEmergencyDialer::AfterTextChanged(
    /* [in] */ IEditable* input)
{
    // Check for special sequences, in particular the "**04" or "**05"
    // sequences that allow you to enter PIN or PUK-related codes.
    //
    // But note we *don't* allow most other special sequences here,
    // like "secret codes" (*#*#<code>#*#*) or IMEI display ("*#06#"),
    // since those shouldn't be available if the device is locked.
    //
    // So we call SpecialCharSequenceMgr.handleCharsForLockedDevice()
    // here, not the regular handleChars() method.
    String str;
    IObject::Probe(input)->ToString(&str);
    if (SpecialCharSequenceMgr::HandleCharsForLockedDevice(this, str, this)) {
        // A special sequence was entered, clear the digits
        AutoPtr<IEditable> chars;
        ITextView::Probe(mDigits)->GetEditableText((IEditable**)&chars);
        if (chars != NULL) chars->Clear();
    }

    return UpdateDialAndDeleteButtonStateEnabledAttr();
}

ECode CEmergencyDialer::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    mStatusBarManager = IStatusBarManager::Probe(obj);
    AutoPtr<IInterface> obj2;
    GetSystemService(ACCESSIBILITY_SERVICE, (IInterface**)&obj2);
    mAccessibilityManager = IAccessibilityManager::Probe(obj2);

    // Allow this activity to be displayed in front of the keyguard / lockscreen.
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);
    Int32 flags;
    lp->GetFlags(&flags);
    lp->SetFlags(flags |= IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);

    // When no proximity sensor is available, use a shorter timeout.
    // TODO: Do we enable this for non proximity devices any more?
    // lp.userActivityTimeout = USER_ACTIVITY_TIMEOUT_WHEN_NO_PROX_SENSOR;

    window->SetAttributes(lp);
    SetContentView(Elastos::Droid::TeleService::R::layout::emergency_dialer);

    AutoPtr<IView> _view;
    FindViewById(Elastos::Droid::TeleService::R::id::digits, (IView**)&_view);
    mDigits = IEditText::Probe(_view);

    mListener = new InnerListener(this);

    AutoPtr<IDialerKeyListenerHelper> helper;
    CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper);
    AutoPtr<IDialerKeyListener> listener;
    helper->GetInstance((IDialerKeyListener**)&listener);
    ITextView::Probe(mDigits)->SetKeyListener(IKeyListener::Probe(listener));
    IView::Probe(mDigits)->SetOnClickListener(mListener);
    IView::Probe(mDigits)->SetOnKeyListener(mListener);
    IView::Probe(mDigits)->SetLongClickable(FALSE);
    Boolean result;
    if (mAccessibilityManager->IsEnabled(&result), result) {
        // The text view must be selected to send accessibility events.
        IView::Probe(mDigits)->SetSelected(TRUE);
    }
    MaybeAddNumberFormatting();

    // Check for the presence of the keypad
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::one, (IView**)&view);
    if (view != NULL) {
        SetupKeypad();
    }

    FindViewById(Elastos::Droid::TeleService::R::id::deleteButton, (IView**)&mDelete);
    mDelete->SetOnClickListener(mListener);
    mDelete->SetOnLongClickListener(mListener);

    FindViewById(Elastos::Droid::TeleService::R::id::floating_action_button, (IView**)&mDialButton);

    // Check whether we should show the onscreen "Dial" button and co.
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    if (res->GetBoolean(Elastos::Droid::TeleService::R::bool_::config_show_onscreen_dial_button, &result),
            result) {
        mDialButton->SetOnClickListener(mListener);
    }
    else {
        mDialButton->SetVisibility(IView::GONE);
    }

    AutoPtr<IView> floatingActionButtonContainer;
    FindViewById(Elastos::Droid::TeleService::R::id::floating_action_button_container,
            (IView**)&floatingActionButtonContainer);
    AutoPtr<IResources> tmp;
    GetResources((IResources**)&tmp);
    ViewUtil::SetupFloatingActionButton(floatingActionButtonContainer, tmp);

    if (icicle != NULL) {
        Activity::OnRestoreInstanceState(icicle);
    }

    // Extract phone number from intent
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    if (data != NULL) {
        String str;
        data->GetScheme(&str);
        if (IPhoneAccount::SCHEME_TEL.Equals(str)) {
            AutoPtr<IIntent> _intent;
            GetIntent((IIntent**)&_intent);
            AutoPtr<IPhoneNumberUtils> helper;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
            String number;
            helper->GetNumberFromIntent(_intent, this, &number);
            if (!number.IsNull()) {
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(number);
                ITextView::Probe(mDigits)->SetText(cchar);
            }
        }
    }

    // if the mToneGenerator creation fails, just continue without it.  It is
    // a local audio signal, and is not as important as the dtmf tone itself.
    {
        AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            //try {
            ECode ec = CToneGenerator::New(DIAL_TONE_STREAM_TYPE, TONE_RELATIVE_VOLUME, (IToneGenerator**)&mToneGenerator);
            //} catch (RuntimeException e) {
            if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                Logger::W(TAG, "Exception caught while creating local tone generator: %d", ec);
                mToneGenerator = NULL;
            }
            //}
        }
    }

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IIntent> tmpintent;
    RegisterReceiver(mBroadcastReceiver, intentFilter, (IIntent**)&tmpintent);

    //try {
    res->GetBoolean(R::bool_::config_enable_dialer_key_vibration, &result);
    // //} catch (Resources.NotFoundException nfe) {
    ECode ec = mHaptic->Init(this, result);
    if (ec == (ECode)E_RESOURCES_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Vibrate control bool missing. %d", ec);
    }
    //}
    return NOERROR;
}

ECode CEmergencyDialer::OnDestroy()
{
    Activity::OnDestroy();

    {
        AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator != NULL) {
            mToneGenerator->ReleaseResources();
            mToneGenerator = NULL;
        }
    }
    return UnregisterReceiver(mBroadcastReceiver);
}

ECode CEmergencyDialer::OnRestoreInstanceState(
    /* [in] */ IBundle* icicle)
{
    return icicle->GetString(LAST_NUMBER, &mLastNumber);
}

ECode CEmergencyDialer::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Activity::OnSaveInstanceState(outState);
    return outState->PutString(LAST_NUMBER, mLastNumber);
}

ECode CEmergencyDialer::MaybeAddNumberFormatting()
{
    // Do nothing.
    return NOERROR;
}

ECode CEmergencyDialer::OnPostCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnPostCreate(savedInstanceState);

    // This can't be done in onCreate(), since the auto-restoring of the digits
    // will play DTMF tones for all the old digits if it is when onRestoreSavedInstanceState()
    // is called. This method will be called every time the activity is created, and
    // will always happen after onRestoreSavedInstanceState().
    return ITextView::Probe(mDigits)->AddTextChangedListener(mListener);
}

void CEmergencyDialer::SetupKeypad()
{
    // Setup the listeners for the buttons
    for (Int32 i = 0; i < DIALER_KEYS->GetLength(); i++) {
        Int32 id = (*DIALER_KEYS)[i];

        AutoPtr<IView> _view;
        FindViewById(id, (IView**)&_view);
        AutoPtr<IDialpadKeyButton> key = IDialpadKeyButton::Probe(_view);
        key->SetOnPressedListener(mListener);
    }

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::zero, (IView**)&view);
    view->SetOnLongClickListener(mListener);
}

ECode CEmergencyDialer::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (keyCode) {
        // Happen when there's a "Call" hard button.
        case IKeyEvent::KEYCODE_CALL:
        {
            AutoPtr<ICharSequence> text;
            ITextView::Probe(mDigits)->GetText((ICharSequence**)&text);
            String str;
            text->ToString(&str);
            if (TextUtils::IsEmpty(str)) {
                // if we are adding a call from the InCallScreen and the phone
                // number entered is empty, we just close the dialer to expose
                // the InCallScreen under it.
                Finish();
            }
            else {
                // otherwise, we place the call.
                PlaceCall();
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    return Activity::OnKeyDown(keyCode, event, result);
}

void CEmergencyDialer::KeyPressed(
        /* [in] */ Int32 keyCode)
{
    mHaptic->Vibrate();
    AutoPtr<IKeyEvent> event;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&event);
    Boolean res;
    IView::Probe(mDigits)->OnKeyDown(keyCode, event, &res);
}

ECode CEmergencyDialer::OnKey(
    /* [in] */ IView* view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    view->GetId(&id);
    switch (id) {
        case Elastos::Droid::TeleService::R::id::digits:
            // Happen when "Done" button of the IME is pressed. This can happen when this
            // Activity is forced into landscape mode due to a desk dock.
            if (keyCode == IKeyEvent::KEYCODE_ENTER) {
                Int32 action;
                event->GetAction(&action);
                if (action == IKeyEvent::ACTION_UP) {
                    PlaceCall();
                    *result = TRUE;
                    return NOERROR;
                }
            }
            break;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CEmergencyDialer::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case Elastos::Droid::TeleService::R::id::deleteButton:
        {
            KeyPressed(IKeyEvent::KEYCODE_DEL);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::floating_action_button:
        {
            mHaptic->Vibrate();  // Vibrate here too, just like we do for the regular keys
            PlaceCall();
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::digits:
        {
            Int32 length;
            ITextView::Probe(mDigits)->GetLength(&length);
            if (length != 0) {
                ITextView::Probe(mDigits)->SetCursorVisible(TRUE);
            }
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CEmergencyDialer::OnPressed(
    /* [in] */ IView* view,
    /* [in] */ Boolean pressed)
{
    if (!pressed) {
        return NOERROR;
    }

    Int32 id;
    view->GetId(&id);
    switch (id) {
        case Elastos::Droid::TeleService::R::id::one: {
            PlayTone(IToneGenerator::TONE_DTMF_1);
            KeyPressed(IKeyEvent::KEYCODE_1);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::two: {
            PlayTone(IToneGenerator::TONE_DTMF_2);
            KeyPressed(IKeyEvent::KEYCODE_2);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::three: {
            PlayTone(IToneGenerator::TONE_DTMF_3);
            KeyPressed(IKeyEvent::KEYCODE_3);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::four: {
            PlayTone(IToneGenerator::TONE_DTMF_4);
            KeyPressed(IKeyEvent::KEYCODE_4);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::five: {
            PlayTone(IToneGenerator::TONE_DTMF_5);
            KeyPressed(IKeyEvent::KEYCODE_5);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::six: {
            PlayTone(IToneGenerator::TONE_DTMF_6);
            KeyPressed(IKeyEvent::KEYCODE_6);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::seven: {
            PlayTone(IToneGenerator::TONE_DTMF_7);
            KeyPressed(IKeyEvent::KEYCODE_7);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::eight: {
            PlayTone(IToneGenerator::TONE_DTMF_8);
            KeyPressed(IKeyEvent::KEYCODE_8);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::nine: {
            PlayTone(IToneGenerator::TONE_DTMF_9);
            KeyPressed(IKeyEvent::KEYCODE_9);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::zero: {
            PlayTone(IToneGenerator::TONE_DTMF_0);
            KeyPressed(IKeyEvent::KEYCODE_0);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::pound: {
            PlayTone(IToneGenerator::TONE_DTMF_P);
            KeyPressed(IKeyEvent::KEYCODE_POUND);
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::star: {
            PlayTone(IToneGenerator::TONE_DTMF_S);
            KeyPressed(IKeyEvent::KEYCODE_STAR);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CEmergencyDialer::OnLongClick(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    view->GetId(&id);
    switch (id) {
        case Elastos::Droid::TeleService::R::id::deleteButton:
        {
            AutoPtr<IEditable> text;
            ITextView::Probe(mDigits)->GetEditableText((IEditable**)&text);
            if (text != NULL) text->Clear();

            // TODO: The framework forgets to clear the pressed
            // status of disabled button. Until this is fixed,
            // clear manually the pressed status. b/2133127
            mDelete->SetPressed(FALSE);
            *result = TRUE;
            return NOERROR;
        }
        case Elastos::Droid::TeleService::R::id::zero: {
            KeyPressed(IKeyEvent::KEYCODE_PLUS);
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CEmergencyDialer::OnResume()
{
    Activity::OnResume();

    // retrieve the DTMF tone play back setting.

    AutoPtr<ISettingsSystem> helper;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsSystem::DTMF_TONE_WHEN_DIALING, 1, &tmp);

    mDTMFToneEnabled = (tmp == 1);

    // Retrieve the haptic feedback setting.
    mHaptic->CheckSystemSetting();

    // if the mToneGenerator creation fails, just continue without it.  It is
    // a local audio signal, and is not as important as the dtmf tone itself.
    {
        AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            //try {
            ECode ec = CToneGenerator::New(IAudioManager::STREAM_DTMF,
                    TONE_RELATIVE_VOLUME, (IToneGenerator**)&mToneGenerator);
            //} catch (RuntimeException e) {
            if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                Logger::W(TAG, "Exception caught while creating local tone generator: %d", ec);
                mToneGenerator = NULL;
            }
            //}
        }
    }

    // Disable the status bar and set the poke lock timeout to medium.
    // There is no need to do anything with the wake lock.
    if (DBG) Logger::D(TAG, "disabling status bar, set to long timeout");
    mStatusBarManager->Disable(IStatusBarManager::DISABLE_EXPAND);

    return UpdateDialAndDeleteButtonStateEnabledAttr();
}

ECode CEmergencyDialer::OnPause()
{
    // Reenable the status bar and set the poke lock timeout to default.
    // There is no need to do anything with the wake lock.
    if (DBG) Logger::D(TAG, "reenabling status bar and closing the dialer");
    mStatusBarManager->Disable(IStatusBarManager::DISABLE_NONE);

    Activity::OnPause();

    {
        AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator != NULL) {
            mToneGenerator->ReleaseResources();
            mToneGenerator = NULL;
        }
    }
    return NOERROR;
}

void CEmergencyDialer::PlaceCall()
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mDigits)->GetText((ICharSequence**)&text);
    text->ToString(&mLastNumber);

    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    Boolean res;
    helper->IsLocalEmergencyNumber(this, mLastNumber, &res);
    if (res) {
        if (DBG) {
            StringBuilder sb;
            sb += "placing call to ";
            sb += mLastNumber;
            Logger::D(TAG, sb.ToString());
        }

        // place the call if it is a valid number
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mLastNumber);
        if (mLastNumber.IsNull() || !TextUtils::IsGraphic(cchar)) {
            // There is no number entered.
            PlayTone(IToneGenerator::TONE_PROP_NACK);
            return;
        }
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_CALL_EMERGENCY, (IIntent**)&intent);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_TEL, mLastNumber, String(NULL), (IUri**)&uri);
        intent->SetData(uri);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        StartActivity(intent);
        Finish();
    }
    else {
        if (DBG) {
            StringBuilder sb;
            sb += "rejecting bad requested number ";
            sb += mLastNumber;
            Logger::D(TAG, sb.ToString());
        }

        // erase the number and throw up an alert dialog.
        AutoPtr<IEditable> text;
        ITextView::Probe(mDigits)->GetEditableText((IEditable**)&text);
        Int32 length;
        ICharSequence::Probe(text)->GetLength(&length);
        if (text != NULL) text->Delete(0, length);
        ShowDialog(BAD_EMERGENCY_NUMBER_DIALOG);
    }
}

ECode CEmergencyDialer::PlayTone(
    /* [in] */ Int32 tone)
{
    // if local tone playback is disabled, just return.
    if (!mDTMFToneEnabled) {
        return NOERROR;
    }

    // Also do nothing if the phone is in silent mode.
    // We need to re-check the ringer mode for *every* playTone()
    // call, rather than keeping a local flag that's updated in
    // onResume(), since it's possible to toggle silent mode without
    // leaving the current activity (via the ENDCALL-longpress menu.)
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    Int32 ringerMode;
    audioManager->GetRingerMode(&ringerMode);
    if ((ringerMode == IAudioManager::RINGER_MODE_SILENT)
        || (ringerMode == IAudioManager::RINGER_MODE_VIBRATE)) {
        return NOERROR;
    }

    {
        AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            Logger::W(TAG, "playTone: mToneGenerator == null, tone: %d", tone);
            return NOERROR;
        }

        // Start the new tone (will stop any playing tone)
        Boolean res;
        mToneGenerator->StartTone(tone, TONE_LENGTH_MS, &res);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> CEmergencyDialer::CreateErrorMessage(
    /* [in] */ const String& number)
{
    AutoPtr<ICharSequence> obj;
    if (!TextUtils::IsEmpty(number)) {
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mLastNumber);
        array->Set(0, TO_IINTERFACE(cchar));
        String str;
        GetString(Elastos::Droid::TeleService::R::string::dial_emergency_error, array, &str);
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(str);
    }
    else {
        AutoPtr<ICharSequence> obj;
        GetText(Elastos::Droid::TeleService::R::string::dial_emergency_empty_error, (ICharSequence**)&obj);
    }
    return obj;
}

ECode CEmergencyDialer::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** outdialog)
{
    VALIDATE_NOT_NULL(outdialog)

    AutoPtr<IAlertDialog> dialog;
    if (id == BAD_EMERGENCY_NUMBER_DIALOG) {
        // construct dialog
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        AutoPtr<ICharSequence> text;
        GetText(Elastos::Droid::TeleService::R::string::emergency_enable_radio_dialog_title,
                (ICharSequence**)&text);
        builder->SetTitle(text);
        AutoPtr<ICharSequence> m = CreateErrorMessage(mLastNumber);
        builder->SetMessage(m);
        builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::ok, NULL);
        builder->SetCancelable(TRUE);
        builder->Create((IAlertDialog**)&dialog);

        // blur stuff behind the dialog
        AutoPtr<IWindow> window;
        IDialog::Probe(dialog)->GetWindow((IWindow**)&window);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_BLUR_BEHIND);
        window->AddFlags(IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
    }
    *outdialog = IDialog::Probe(dialog);
    REFCOUNT_ADD(*outdialog)
    return NOERROR;
}

void CEmergencyDialer::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    Activity::OnPrepareDialog(id, dialog);
    if (id == BAD_EMERGENCY_NUMBER_DIALOG) {
        AutoPtr<IAlertDialog> alert = IAlertDialog::Probe(dialog);
        alert->SetMessage(CreateErrorMessage(mLastNumber));
    }
}

ECode CEmergencyDialer::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {
        OnBackPressed();
        *result = TRUE;
    }
    return Activity::OnOptionsItemSelected(item, result);
}

ECode CEmergencyDialer::UpdateDialAndDeleteButtonStateEnabledAttr()
{
    Int32 length;
    ITextView::Probe(mDigits)->GetLength(&length);
    Boolean notEmpty = length != 0;

    return mDelete->SetEnabled(notEmpty);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos