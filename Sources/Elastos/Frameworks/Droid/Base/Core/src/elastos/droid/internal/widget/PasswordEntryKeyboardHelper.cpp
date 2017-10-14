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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/widget/PasswordEntryKeyboardHelper.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::InputMethodService::IKeyboard;
using Elastos::Droid::InputMethodService::EIID_IKeyboard;
using Elastos::Droid::InputMethodService::EIID_IOnKeyboardActionListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String PasswordEntryKeyboardHelper::TAG("PasswordEntryKeyboardHelper");
const Int32 PasswordEntryKeyboardHelper::KEYBOARD_STATE_NORMAL = 0;
const Int32 PasswordEntryKeyboardHelper::KEYBOARD_STATE_SHIFTED = 1;
const Int32 PasswordEntryKeyboardHelper::KEYBOARD_STATE_CAPSLOCK = 2;
const Int32 PasswordEntryKeyboardHelper::NUMERIC = 0;
const Int32 PasswordEntryKeyboardHelper::QWERTY = 1;
const Int32 PasswordEntryKeyboardHelper::QWERTY_SHIFTED = 2;
const Int32 PasswordEntryKeyboardHelper::SYMBOLS = 3;
const Int32 PasswordEntryKeyboardHelper::SYMBOLS_SHIFTED = 4;

CAR_INTERFACE_IMPL(PasswordEntryKeyboardHelper::InnerListener, Object, IOnKeyboardActionListener)

PasswordEntryKeyboardHelper::InnerListener::InnerListener(
    /* [in] */ PasswordEntryKeyboardHelper* host)
    : mHost(host)
{}

ECode PasswordEntryKeyboardHelper::InnerListener::OnKey(
    /* [in] */ Int32 primaryCode,
    /* [in] */ ArrayOf<Int32>* keyCodes)
{
    return  mHost->OnKey(primaryCode, keyCodes);
}

ECode PasswordEntryKeyboardHelper::InnerListener::OnPress(
    /* [in] */ Int32 primaryCode)
{
    return  mHost->OnPress(primaryCode);
}

ECode PasswordEntryKeyboardHelper::InnerListener::OnRelease(
    /* [in] */ Int32 primaryCode)
{
    return  mHost->OnRelease(primaryCode);
}

ECode PasswordEntryKeyboardHelper::InnerListener::OnText(
    /* [in] */ ICharSequence* text)
{
    return  mHost->OnText(text);
}

ECode PasswordEntryKeyboardHelper::InnerListener::SwipeDown()
{
    return  mHost->SwipeDown();
}

ECode PasswordEntryKeyboardHelper::InnerListener::SwipeLeft()
{
    return  mHost->SwipeLeft();
}

ECode PasswordEntryKeyboardHelper::InnerListener::SwipeRight()
{
    return  mHost->SwipeRight();
}

ECode PasswordEntryKeyboardHelper::InnerListener::SwipeUp()
{
    return  mHost->SwipeUp();
}

CAR_INTERFACE_IMPL(PasswordEntryKeyboardHelper, Object, IPasswordEntryKeyboardHelper)

PasswordEntryKeyboardHelper::PasswordEntryKeyboardHelper()
    : mKeyboardMode(KEYBOARD_MODE_ALPHA)
    , mKeyboardState(KEYBOARD_STATE_NORMAL)
    , mEnableHaptics(FALSE)
    , mUsingScreenWidth(FALSE)
{
    mLayouts = ArrayOf<Int32>::Alloc(5);
    (*mLayouts)[0] = R::xml::password_kbd_numeric;
    (*mLayouts)[1] = R::xml::password_kbd_qwerty;
    (*mLayouts)[2] = R::xml::password_kbd_qwerty_shifted;
    (*mLayouts)[3] = R::xml::password_kbd_symbols;
    (*mLayouts)[4] = R::xml::password_kbd_symbols_shift;
}

ECode PasswordEntryKeyboardHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IKeyboardView* keyboardView,
    /* [in] */ IView* targetView)
{
    return constructor(context, keyboardView, targetView, TRUE, NULL);
}

ECode PasswordEntryKeyboardHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IKeyboardView* keyboardView,
    /* [in] */ IView* targetView,
    /* [in] */ Boolean useFullScreenWidth)
{
    return constructor(context, keyboardView, targetView, useFullScreenWidth, NULL);
}

ECode PasswordEntryKeyboardHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IKeyboardView* keyboardView,
    /* [in] */ IView* targetView,
    /* [in] */ Boolean useFullScreenWidth,
    /* [in] */ ArrayOf<Int32>* layouts)
{
    mContext = context;
    mTargetView = targetView;
    mKeyboardView = keyboardView;
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mKeyboardView->SetOnKeyboardActionListener(listener);
    mUsingScreenWidth = useFullScreenWidth;
    if (layouts != NULL) {
        if (layouts->GetLength() != mLayouts->GetLength()) {
            return E_RUNTIME_EXCEPTION;
            //throw new RuntimeException("Wrong number of layouts");
        }
        for (Int32 i = 0; i < mLayouts->GetLength(); i++) {
            (*mLayouts)[i] = (*layouts)[i];
        }
    }
    CreateKeyboards();
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::CreateKeyboards()
{
    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(mKeyboardView)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 width = 0, height = 0;
    lp->GetWidth(&width);
    lp->GetHeight(&height);

    if (mUsingScreenWidth || (width == IViewGroupLayoutParams::MATCH_PARENT)) {
        CreateKeyboardsWithDefaultWidth();
    }
    else {
        CreateKeyboardsWithSpecificSize(width, height);
    }
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SetEnableHaptics(
    /* [in] */ Boolean enabled)
{
    mEnableHaptics = enabled;

    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::IsAlpha(
    /* [out] */ Boolean* alpha)
{
    VALIDATE_NOT_NULL(alpha)
    *alpha = mKeyboardMode == KEYBOARD_MODE_ALPHA;
    return NOERROR;
}

void PasswordEntryKeyboardHelper::CreateKeyboardsWithSpecificSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mNumericKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[NUMERIC], 0,
        width, height, (CPasswordEntryKeyboard**)&mNumericKeyboard);
    mQwertyKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[QWERTY], R::id::mode_normal,
        width, height, (CPasswordEntryKeyboard**)&mQwertyKeyboard);
    mQwertyKeyboard->EnableShiftLock();

    mQwertyKeyboardShifted = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[QWERTY_SHIFTED],
        R::id::mode_normal, width, height, (CPasswordEntryKeyboard**)&mQwertyKeyboardShifted);
    mQwertyKeyboardShifted->EnableShiftLock();
    Boolean rst = FALSE;
    mQwertyKeyboardShifted->SetShifted(TRUE, &rst); // always shifted.

    mSymbolsKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[SYMBOLS], 0, width, height, (CPasswordEntryKeyboard**)&mSymbolsKeyboard);
    mSymbolsKeyboard->EnableShiftLock();

    mSymbolsKeyboardShifted = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[SYMBOLS_SHIFTED], 0,
            width, height, (CPasswordEntryKeyboard**)&mSymbolsKeyboardShifted);
    mSymbolsKeyboardShifted->EnableShiftLock();
    mSymbolsKeyboardShifted->SetShifted(TRUE, &rst); // always shifted
}

void PasswordEntryKeyboardHelper::CreateKeyboardsWithDefaultWidth()
{
    mNumericKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[NUMERIC],
        (CPasswordEntryKeyboard**)&mNumericKeyboard);
    mQwertyKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[QWERTY],
        R::id::mode_normal, (CPasswordEntryKeyboard**)&mQwertyKeyboard);
    mQwertyKeyboard->EnableShiftLock();

    mQwertyKeyboardShifted = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[QWERTY_SHIFTED],
        R::id::mode_normal, (CPasswordEntryKeyboard**)&mQwertyKeyboardShifted);
    mQwertyKeyboardShifted->EnableShiftLock();
    Boolean rst;
    mQwertyKeyboardShifted->SetShifted(TRUE, &rst); // always shifted.

    mSymbolsKeyboard = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[SYMBOLS], (CPasswordEntryKeyboard**)&mSymbolsKeyboard);
    mSymbolsKeyboard->EnableShiftLock();

    mSymbolsKeyboardShifted = NULL;
    CPasswordEntryKeyboard::NewByFriend(mContext, (*mLayouts)[SYMBOLS_SHIFTED], (CPasswordEntryKeyboard**)&mSymbolsKeyboardShifted);
    mSymbolsKeyboardShifted->EnableShiftLock();
    mSymbolsKeyboardShifted->SetShifted(TRUE, &rst); // always shifted
}

ECode PasswordEntryKeyboardHelper::SetKeyboardMode(
    /* [in] */ Int32 mode)
{
    switch (mode) {
        case KEYBOARD_MODE_ALPHA: {
            mKeyboardView->SetKeyboard(IKeyboard::Probe(mQwertyKeyboard));
            mKeyboardState = KEYBOARD_STATE_NORMAL;
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            Int32 value = 0;
            FAIL_RETURN(Settings::System::GetInt32(
                    cr, ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &value))
            Boolean visiblePassword = value != 0;
            Boolean enablePreview = FALSE; // TODO: grab from configuration
            mKeyboardView->SetPreviewEnabled(visiblePassword && enablePreview);
            break;
        }
        case KEYBOARD_MODE_NUMERIC:
            mKeyboardView->SetKeyboard((IKeyboard*)(mNumericKeyboard->Probe(EIID_IKeyboard)));
            mKeyboardState = KEYBOARD_STATE_NORMAL;
            mKeyboardView->SetPreviewEnabled(FALSE); // never show popup for numeric keypad
            break;
    }
    mKeyboardMode = mode;
    return NOERROR;
}

void PasswordEntryKeyboardHelper::SendKeyEventsToTarget(
    /* [in] */ Int32 character)
{
    AutoPtr<IViewRootImpl> viewRootImpl;
    ((Elastos::Droid::View::View*)mTargetView.Get())->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
    AutoPtr<ArrayOf<IKeyEvent*> > events;
    AutoPtr<IKeyCharacterMap> kcm;
    CKeyCharacterMap::Load(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IKeyCharacterMap**)&kcm);
    AutoPtr<ArrayOf<Char32> > param = ArrayOf<Char32>::Alloc(1);
    (*param)[0] = character;
    kcm->GetEvents(param, (ArrayOf<IKeyEvent*>**)&events);
    if (events != NULL) {
        Int32 N = events->GetLength();
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IKeyEvent> event = (*events)[i];
            Int32 flags = 0;
            event->GetFlags(&flags);
            AutoPtr<IKeyEvent> newEvent;
            CKeyEvent::ChangeFlags(event, flags
                | IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE, (IKeyEvent**)&newEvent);
            viewRootImpl->DispatchInputEvent(IInputEvent::Probe(newEvent));
        }
    }
}

ECode PasswordEntryKeyboardHelper::SendDownUpKeyEvents(
    /* [in] */ Int32 keyEventCode)
{
    Int64 eventTime = SystemClock::GetUptimeMillis();
    AutoPtr<IViewRootImpl> viewRootImpl;
    ((Elastos::Droid::View::View*)mTargetView.Get())->GetViewRootImpl((IViewRootImpl**)&viewRootImpl);
    AutoPtr<IKeyEvent> keyEvent;
    CKeyEvent::New(eventTime, eventTime, IKeyEvent::ACTION_DOWN, keyEventCode, 0, 0,
                        IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
                    IKeyEvent::FLAG_SOFT_KEYBOARD|IKeyEvent::FLAG_KEEP_TOUCH_MODE, (IKeyEvent**)&keyEvent);
    viewRootImpl->DispatchKeyFromIme(keyEvent);
    AutoPtr<IKeyEvent> keyEvent2;
    CKeyEvent::New(eventTime, eventTime, IKeyEvent::ACTION_UP, keyEventCode, 0, 0,
        IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
        IKeyEvent::FLAG_SOFT_KEYBOARD|IKeyEvent::FLAG_KEEP_TOUCH_MODE, (IKeyEvent**)&keyEvent2);
    viewRootImpl->DispatchKeyFromIme(keyEvent2);
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::OnKey(
    /* [in] */ Int32 primaryCode,
    /* [in] */ ArrayOf<Int32>* keyCodes)
{
    if (primaryCode == IKeyboard::KEYCODE_DELETE) {
        HandleBackspace();
    }
    else if (primaryCode == IKeyboard::KEYCODE_SHIFT) {
        HandleShift();
    }
    else if (primaryCode == IKeyboard::KEYCODE_CANCEL) {
        HandleClose();
        return NOERROR;
    }
    else if (primaryCode == IKeyboard::KEYCODE_MODE_CHANGE && mKeyboardView != NULL) {
        HandleModeChange();
    }
    else {
        HandleCharacter(primaryCode, keyCodes);
        // Switch back to old keyboard if we're not in capslock mode
        if (mKeyboardState == KEYBOARD_STATE_SHIFTED) {
            // skip to the unlocked state
            mKeyboardState = KEYBOARD_STATE_SHIFTED;
            HandleShift();
        }
    }

    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SetVibratePattern(
    /* [in] */ Int32 id)
{
    AutoPtr<ArrayOf<Int32> > tmpArray;
    //try {
        AutoPtr<IResources> rs;
        mContext->GetResources((IResources**)&rs);
        rs->GetInt32Array(id, (ArrayOf<Int32>**)&tmpArray);
    /*} catch (Resources.NotFoundException e) {
        if (id != 0) {
            Log.e(TAG, "Vibrate pattern missing", e);
        }
    }*/
    if (tmpArray == NULL) {
        mVibratePattern = NULL;
        return NOERROR;
    }
    mVibratePattern = ArrayOf<Int64>::Alloc(tmpArray->GetLength());
    for (Int32 i = 0; i < tmpArray->GetLength(); i++) {
        (*mVibratePattern)[i] = (*tmpArray)[i];
    }
    return NOERROR;
}

void PasswordEntryKeyboardHelper::HandleModeChange()
{
    AutoPtr<IKeyboard> current;
    mKeyboardView->GetKeyboard((IKeyboard**)&current);

    AutoPtr<IKeyboard> next = NULL;
    if (IKeyboard::Probe(mQwertyKeyboard) == current ||
            IKeyboard::Probe(mQwertyKeyboardShifted) == current) {
        next = IKeyboard::Probe(mSymbolsKeyboard);
    }
    else if (IKeyboard::Probe(mSymbolsKeyboard) == current ||
            IKeyboard::Probe(mSymbolsKeyboardShifted) == current) {
        next = IKeyboard::Probe(mQwertyKeyboard);
    }
    if (next != NULL) {
        mKeyboardView->SetKeyboard(next);
        mKeyboardState = KEYBOARD_STATE_NORMAL;
    }
}

ECode PasswordEntryKeyboardHelper::HandleBackspace()
{
    SendDownUpKeyEvents(IKeyEvent::KEYCODE_DEL);
    PerformHapticFeedback();
    return NOERROR;
}

void PasswordEntryKeyboardHelper::HandleShift()
{
    if (mKeyboardView == NULL) {
        return;
    }
    AutoPtr<IKeyboard> current;
    mKeyboardView->GetKeyboard((IKeyboard**)&current);
    AutoPtr<CPasswordEntryKeyboard> next;
    Boolean isAlphaMode = IKeyboard::Probe(mQwertyKeyboard) == current
            || IKeyboard::Probe(mQwertyKeyboardShifted) == current;
    if (mKeyboardState == KEYBOARD_STATE_NORMAL) {
        mKeyboardState = isAlphaMode ? KEYBOARD_STATE_SHIFTED : KEYBOARD_STATE_CAPSLOCK;
        next = isAlphaMode ? mQwertyKeyboardShifted : mSymbolsKeyboardShifted;
    }
    else if (mKeyboardState == KEYBOARD_STATE_SHIFTED) {
        mKeyboardState = KEYBOARD_STATE_CAPSLOCK;
        next = isAlphaMode ? mQwertyKeyboardShifted : mSymbolsKeyboardShifted;
    }
    else if (mKeyboardState == KEYBOARD_STATE_CAPSLOCK) {
        mKeyboardState = KEYBOARD_STATE_NORMAL;
        next = isAlphaMode ? mQwertyKeyboard : mSymbolsKeyboard;
    }
    if (next != NULL) {
        if (IKeyboard::Probe(next) != current) {
            mKeyboardView->SetKeyboard(IKeyboard::Probe(next));
        }
        next->SetShiftLocked(mKeyboardState == KEYBOARD_STATE_CAPSLOCK);
        Boolean rst = FALSE;
        mKeyboardView->SetShifted(mKeyboardState != KEYBOARD_STATE_NORMAL, &rst);
    }
}

void PasswordEntryKeyboardHelper::HandleCharacter(
    /* [in] */ Int32 primaryCode,
    /* [in] */ ArrayOf<Int32>* keyCodes)
{
    // Maybe turn off shift if not in capslock mode.
    Boolean isShifted = FALSE;
    mKeyboardView->IsShifted(&isShifted);
    if (isShifted && primaryCode != ' ' && primaryCode != '\n') {
        if(primaryCode >= 97 && primaryCode <= 122)
            primaryCode += 32;
    }
    SendKeyEventsToTarget(primaryCode);
}

void PasswordEntryKeyboardHelper::HandleClose()
{
}

ECode PasswordEntryKeyboardHelper::OnPress(
    /* [in] */ Int32 primaryCode)
{
    PerformHapticFeedback();

    return NOERROR;
}

void PasswordEntryKeyboardHelper::PerformHapticFeedback()
{
    if (mEnableHaptics) {
        Boolean rst = FALSE;
        IView::Probe(mKeyboardView)->PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY,
                IHapticFeedbackConstants::FLAG_IGNORE_VIEW_SETTING
                | IHapticFeedbackConstants::FLAG_IGNORE_GLOBAL_SETTING, &rst);
    }
}

ECode PasswordEntryKeyboardHelper::OnRelease(
    /* [in] */ Int32 primaryCode)
{
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::OnText(
    /* [in] */ ICharSequence* text)
{
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SwipeDown()
{
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SwipeLeft()
{
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SwipeRight()
{
    return NOERROR;
}

ECode PasswordEntryKeyboardHelper::SwipeUp()
{
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
