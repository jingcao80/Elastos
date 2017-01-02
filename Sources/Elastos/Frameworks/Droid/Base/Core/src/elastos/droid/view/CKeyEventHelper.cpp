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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/view/KeyEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CKeyEventHelper, Singleton, IKeyEventHelper);

CAR_SINGLETON_IMPL(CKeyEventHelper);

ECode CKeyEventHelper::GetMaxKeyCode(
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode);
    *keyCode = KeyEvent::GetMaxKeyCode();

    return NOERROR;
}

ECode CKeyEventHelper::GetDeadChar(
    /* [in] */ Int32 accent,
    /* [in] */ Int32 c,
    /* [out] */ Int32* deadChar)
{
    VALIDATE_NOT_NULL(deadChar);
    *deadChar = KeyEvent::GetDeadChar(accent, c);

    return NOERROR;
}

ECode CKeyEventHelper::Obtain(
    /* [in] */ Int64 downTime,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 action,
    /* [in] */ Int32 code,
    /* [in] */ Int32 repeat,
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 scancode,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 source,
    /* [in] */ const String& characters,
    /* [out] */ IKeyEvent** keyEvent)
{
    VALIDATE_NOT_NULL(keyEvent);
    AutoPtr<IKeyEvent> temp = KeyEvent::Obtain(
            downTime, eventTime, action, code, repeat, metaState,
            deviceId, scancode, flags, source, characters);
    *keyEvent = temp;
    REFCOUNT_ADD(*keyEvent);

    return NOERROR;
}

ECode CKeyEventHelper::Obtain(
    /* [in] */ IKeyEvent* otherEvent,
    /* [out] */ IKeyEvent** keyEvent)
{
    VALIDATE_NOT_NULL(keyEvent);
    AutoPtr<IKeyEvent> temp = KeyEvent::Obtain(otherEvent);
    *keyEvent = temp;
    REFCOUNT_ADD(*keyEvent);

    return NOERROR;
}

ECode CKeyEventHelper::ChangeTimeRepeat(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 newRepeat,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return KeyEvent::ChangeTimeRepeat(event, eventTime, newRepeat, newEvent);
}

ECode CKeyEventHelper::ChangeTimeRepeat(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 newRepeat,
    /* [in] */ Int32 newFlags,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return KeyEvent::ChangeTimeRepeat(event, eventTime, newRepeat, newFlags, newEvent);
}

ECode CKeyEventHelper::ChangeAction(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 action,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return KeyEvent::ChangeAction(event, action, newEvent);
}

ECode CKeyEventHelper::ChangeFlags(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(event);
    return KeyEvent::ChangeFlags(event, flags, newEvent);
}

ECode CKeyEventHelper::IsGamepadButton(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = KeyEvent::IsGamepadButton(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::IsConfirmKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = KeyEvent::IsConfirmKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::IsMediaKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = KeyEvent::IsMediaKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::IsSystemKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = KeyEvent::IsSystemKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::IsWakeKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = KeyEvent::IsWakeKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::IsMetaKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = KeyEvent::IsMetaKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::GetModifierMetaStateMask(
    /* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask);
    *mask = KeyEvent::GetModifierMetaStateMask();

    return NOERROR;
}

ECode CKeyEventHelper::IsModifierKey(
    /* [in] */ Int32 keyCode,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = KeyEvent::IsModifierKey(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::NormalizeMetaState(
    /* [in] */ Int32 metaState,
    /* [out] */ Int32* normalizeMetaState)
{
    VALIDATE_NOT_NULL(normalizeMetaState);
    *normalizeMetaState = KeyEvent::NormalizeMetaState(metaState);

    return NOERROR;
}

ECode CKeyEventHelper::MetaStateHasNoModifiers(
    /* [in] */ Int32 metaState,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = KeyEvent::MetaStateHasNoModifiers(metaState);

    return NOERROR;
}

ECode CKeyEventHelper::MetaStateHasModifiers(
    /* [in] */ Int32 metaState,
    /* [in] */ Int32 modifiers,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return KeyEvent::MetaStateHasModifiers(metaState, modifiers, res);
}

ECode CKeyEventHelper::ActionToString(
    /* [in] */ Int32 action,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = KeyEvent::ActionToString(action);

    return NOERROR;
}

ECode CKeyEventHelper::KeyCodeToString(
    /* [in] */ Int32 keyCode,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = KeyEvent::KeyCodeToString(keyCode);

    return NOERROR;
}

ECode CKeyEventHelper::KeyCodeFromString(
    /* [in] */ const String& symbolicName,
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode);
    return KeyEvent::KeyCodeFromString(symbolicName, keyCode);
}

ECode CKeyEventHelper::MetaStateToString(
    /* [in] */ Int32 metaState,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = KeyEvent::MetaStateToString(metaState);

    return NOERROR;
}

ECode CKeyEventHelper::CreateFromParcelBody(
    /* [in] */ IParcel* in,
    /* [out] */ IKeyEvent** newEvent)
{
    VALIDATE_NOT_NULL(newEvent);
    return KeyEvent::CreateFromParcelBody(in, newEvent);
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
