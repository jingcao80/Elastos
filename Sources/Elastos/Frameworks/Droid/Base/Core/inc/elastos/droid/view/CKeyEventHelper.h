
#ifndef __ELASTOS_DROID_VIEW_CKEYEVENTHELPER_H__
#define __ELASTOS_DROID_VIEW_CKEYEVENTHELPER_H__

#include "_Elastos_Droid_View_CKeyEventHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CKeyEventHelper)
    , public Singleton
    , public IKeyEventHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetMaxKeyCode(
        /* [out] */ Int32* maxKeyCode);

    CARAPI GetDeadChar(
        /* [in] */ Int32 accent,
        /* [in] */ Int32 c,
        /* [out] */ Int32* deadChar);

    CARAPI Obtain(
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
        /* [out] */ IKeyEvent** keyEvent);

    CARAPI Obtain(
        /* [in] */ IKeyEvent* otherEvent,
        /* [out] */ IKeyEvent** keyEvent);

    CARAPI ChangeTimeRepeat(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 newRepeat,
        /* [out] */ IKeyEvent** newEvent);

    CARAPI ChangeTimeRepeat(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 newRepeat,
        /* [in] */ Int32 newFlags,
        /* [out] */ IKeyEvent** newEvent);

    CARAPI ChangeAction(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 action,
        /* [out] */ IKeyEvent** newEvent);

    CARAPI ChangeFlags(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ IKeyEvent** newEvent);

    CARAPI IsGamepadButton(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* res);

    CARAPI IsConfirmKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI IsMediaKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI IsSystemKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI IsWakeKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI IsMetaKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI GetModifierMetaStateMask(
        /* [out] */ Int32* mask);

    CARAPI IsModifierKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* res);

    CARAPI NormalizeMetaState(
        /* [in] */ Int32 metaState,
        /* [out] */ Int32* normalizeMetaState);

    CARAPI MetaStateHasNoModifiers(
        /* [in] */ Int32 metaState,
        /* [out] */ Boolean* res);

    CARAPI MetaStateHasModifiers(
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 modifiers,
        /* [out] */ Boolean* res);

    CARAPI ActionToString(
        /* [in] */ Int32 action,
        /* [out] */ String* str);

    CARAPI KeyCodeToString(
        /* [in] */ Int32 keyCode,
        /* [out] */ String* str);

    CARAPI KeyCodeFromString(
        /* [in] */ const String& symbolicName,
        /* [out] */ Int32* keyCode);

    CARAPI MetaStateToString(
        /* [in] */ Int32 metaState,
        /* [out] */ String* str);

    CARAPI CreateFromParcelBody(
        /* [in] */ IParcel* in,
        /* [out] */ IKeyEvent** newEvent);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
#endif //__ELASTOS_DROID_VIEW_CKEYEVENTHELPER_H__
