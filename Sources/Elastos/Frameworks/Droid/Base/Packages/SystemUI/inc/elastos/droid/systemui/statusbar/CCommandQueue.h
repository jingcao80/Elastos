
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CCommandQueue.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Internal::StatusBar::IIStatusBar;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Internal::StatusBar::IStatusBarIconList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CCommandQueue)
    , public Object
    , public ICommandQueue
    , public IIStatusBar
    , public IBinder
{
private:
    class MyHandler : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CCommandQueue* host);

        CARAPI constructor();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCommandQueue* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ ICommandQueueCallbacks* callbacks,
        /* [in] */ IStatusBarIconList* list);

    CARAPI SetIconList(
        /* [in] */ IStatusBarIconList* list);

    CARAPI SetIcon(
        /* [in] */ Int32 index,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI RemoveIcon(
        /* [in] */ Int32 index);

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
        /* [in] */ Int32 backDisposition,
        /* [in] */ Boolean showImeSwitcher);

    CARAPI ShowRecentApps(
        /* [in] */ Boolean triggeredFromAltTab);

    CARAPI HideRecentApps(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    CARAPI ToggleRecentApps();

    CARAPI PreloadRecentApps();

    CARAPI CancelPreloadRecentApps();

    CARAPI SetWindowState(
        /* [in] */ Int32 window,
        /* [in] */ Int32 state);

    CARAPI BuzzBeepBlinked();

    CARAPI NotificationLightOff();

    CARAPI NotificationLightPulse(
        /* [in] */ Int32 argb,
        /* [in] */ Int32 onMillis,
        /* [in] */ Int32 offMillis);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    CARAPI HandleSetIcon(
        /* [in] */ Int32 index,
        /* [in] */ IStatusBarIcon* icon);

    CARAPI HandleRemoveIcon(
        /* [in] */ Int32 index);

private:
    static const Int32 INDEX_MASK; // 0xffff;
    static const Int32 MSG_SHIFT ; // 16;
    static const Int32 MSG_MASK  ; // 0xffff << MSG_SHIFT;

    static const Int32 OP_SET_ICON   ; // 1;
    static const Int32 OP_REMOVE_ICON; // 2;

    static const Int32 MSG_ICON;
    static const Int32 MSG_DISABLE;
    static const Int32 MSG_EXPAND_NOTIFICATIONS;
    static const Int32 MSG_COLLAPSE_PANELS;
    static const Int32 MSG_EXPAND_SETTINGS;
    static const Int32 MSG_SET_SYSTEMUI_VISIBILITY;
    static const Int32 MSG_TOP_APP_WINDOW_CHANGED;
    static const Int32 MSG_SHOW_IME_BUTTON;
    static const Int32 MSG_TOGGLE_RECENT_APPS;
    static const Int32 MSG_PRELOAD_RECENT_APPS;
    static const Int32 MSG_CANCEL_PRELOAD_RECENT_APPS;
    static const Int32 MSG_SET_WINDOW_STATE;
    static const Int32 MSG_SHOW_RECENT_APPS;
    static const Int32 MSG_HIDE_RECENT_APPS;
    static const Int32 MSG_BUZZ_BEEP_BLINKED;
    static const Int32 MSG_NOTIFICATION_LIGHT_OFF;
    static const Int32 MSG_NOTIFICATION_LIGHT_PULSE;

    static const String SHOW_IME_SWITCHER_KEY;
    AutoPtr<IStatusBarIconList> mList;
    AutoPtr<ICommandQueueCallbacks> mCallbacks;
    AutoPtr<IHandler> mHandler;

    Object mListLock;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CCOMMANDQUEUE_H__
