
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CCommandQueue.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::StatusBar::IStatusBarIcon;
using Elastos::Droid::StatusBar::IStatusBarNotification;
using Elastos::Droid::StatusBar::IStatusBarIconList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CCommandQueue)
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ CCommandQueue* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        CCommandQueue* mHost;
    };

    class NotificationQueueEntry
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        AutoPtr<IBinder> mKey;
        AutoPtr<IStatusBarNotification> mNotification;
    };

public:
    CARAPI constructor(
        /* [in] */ ICommandQueueCallbacks* callbacks);

    CARAPI SetIconList(
        /* [in] */ IStatusBarIconList* list);

    CARAPI SetIcon(
        /* [in] */ Int32 index,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI RemoveIcon(
        /* [in] */ Int32 index);

    CARAPI AddNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI UpdateNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI RemoveNotification(
        /* [in] */ IBinder* key);

    CARAPI Disable(
        /* [in] */ Int32 state);

    CARAPI AnimateExpandNotificationsPanel();

    CARAPI AnimateExpandSettingsPanel();

    CARAPI AnimateCollapsePanels();

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    CARAPI TopAppWindowChanged(
        /* [in] */ Boolean menuVisible);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

    CARAPI ToggleRecentApps();

    CARAPI PreloadRecentApps();

    CARAPI CancelPreloadRecentApps();

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    CARAPI HandleSetIcon(
        /* [in] */ Int32 index,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI HandleRemoveIcon(
        /* [in] */ Int32 index);
private:
    const static Int32 INDEX_MASK; // 0xffff;
    const static Int32 MSG_SHIFT ; // 16;
    const static Int32 MSG_MASK  ; // 0xffff << MSG_SHIFT;

    const static Int32 OP_SET_ICON   ; // 1;
    const static Int32 OP_REMOVE_ICON; // 2;

    const static Int32 MSG_ICON                      ; // 1 << MSG_SHIFT;
    const static Int32 MSG_ADD_NOTIFICATION          ; // 2 << MSG_SHIFT;
    const static Int32 MSG_UPDATE_NOTIFICATION       ; // 3 << MSG_SHIFT;
    const static Int32 MSG_REMOVE_NOTIFICATION       ; // 4 << MSG_SHIFT;
    const static Int32 MSG_DISABLE                   ; // 5 << MSG_SHIFT;
    const static Int32 MSG_EXPAND_NOTIFICATIONS      ; // 6 << MSG_SHIFT;
    const static Int32 MSG_COLLAPSE_PANELS           ; // 7 << MSG_SHIFT;
    const static Int32 MSG_EXPAND_SETTINGS           ; // 8 << MSG_SHIFT;
    const static Int32 MSG_SET_SYSTEMUI_VISIBILITY   ; // 9 << MSG_SHIFT;
    const static Int32 MSG_TOP_APP_WINDOW_CHANGED    ; // 10 << MSG_SHIFT;
    const static Int32 MSG_SHOW_IME_BUTTON           ; // 11 << MSG_SHIFT;
    const static Int32 MSG_SET_HARD_KEYBOARD_STATUS  ; // 12 << MSG_SHIFT;
    const static Int32 MSG_TOGGLE_RECENT_APPS        ; // 13 << MSG_SHIFT;
    const static Int32 MSG_PRELOAD_RECENT_APPS       ; // 14 << MSG_SHIFT;
    const static Int32 MSG_CANCEL_PRELOAD_RECENT_APPS; // 15 << MSG_SHIFT;
    const static Int32 MSG_SET_NAVIGATION_ICON_HINTS ; // 16 << MSG_SHIFT;

private:
    AutoPtr<IStatusBarIconList> mList;
    AutoPtr<ICommandQueueCallbacks> mCallbacks;
    AutoPtr<IHandler> mHandler;

    Object mListLock;
};

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__
