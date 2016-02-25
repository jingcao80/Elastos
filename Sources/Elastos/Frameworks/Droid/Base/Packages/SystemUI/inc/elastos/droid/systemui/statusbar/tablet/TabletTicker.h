#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETTICKER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETTICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ImageView.h"


using Elastos::Droid::Animation::ITransitionListener;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::StatusBar::IStatusBarNotification;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

class TabletStatusBar;

class TabletTicker
    : public ElRefBase
    , public ITransitionListener
    , public IHandler
    , public Handler
{
private:
    class TickerClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TickerClickListener(
            /* [in] */ IViewOnClickListener* clicker,
            /* [in] */ TabletTicker* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        AutoPtr<IViewOnClickListener> mClicker;
        TabletTicker* mHost;
    };

public:
    CAR_INTERFACE_DECL()
    IHANDLER_METHODS_DECL()

    TabletTicker(
        /* [in] */ TabletStatusBar* bar);

    void Add(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    void Remove(
        /* [in] */ IBinder* key);

    void Remove(
        /* [in] */ IBinder* key,
        /* [in] */ Boolean advance);

    void Halt();

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    void Advance();

    CARAPI StartTransition(
        /* [in] */ ILayoutTransition* transition,
        /* [in] */ IViewGroup* container,
        /* [in] */ IView* view,
        /* [in] */ Int32 transitionType);

    CARAPI EndTransition(
        /* [in] */ ILayoutTransition* transition,
        /* [in] */ IViewGroup* container,
        /* [in] */ IView* view,
        /* [in] */ Int32 transitionType);

private:
    void Dequeue();

    AutoPtr<IViewGroup> MakeWindow();

    AutoPtr<IView> MakeTickerView(
        /* [in] */ IStatusBarNotification* notification);

public:
    static const Int32 MSG_ADVANCE;// = 1;
    static const Int32 ADVANCE_DELAY;// = 5000; // 5 seconds

private:
    static const String TAG;
    static const Boolean DBG;

    // 3 is enough to let us see most cases, but not get so far behind that it's too annoying.
    static const Int32 QUEUE_LENGTH;// = 3;

    static const Boolean CLICKABLE_TICKER;// = true;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManager> mWindowManager;

    AutoPtr<IViewGroup> mWindow;
    AutoPtr<IBinder> mCurrentKey;
    AutoPtr<IStatusBarNotification> mCurrentNotification;
    AutoPtr<IStatusBarNotification> mPreNotification;
    AutoPtr<IView> mCurrentView;

    AutoPtr<ArrayOf<IBinder*> > mKeys;// = new IBinder[QUEUE_LENGTH];
    AutoPtr<ArrayOf<IStatusBarNotification*> > mNotificaitonQueue;// = new StatusBarNotification[QUEUE_LENGTH];
    Int32 mQueuePos;

    Int32 mLargeIconHeight;

    TabletStatusBar* mBar;

    AutoPtr<ILayoutTransition> mLayoutTransition;
    Boolean mWindowShouldClose;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETTICKER_H__
