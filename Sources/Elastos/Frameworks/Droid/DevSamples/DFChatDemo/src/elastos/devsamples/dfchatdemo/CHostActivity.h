
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CHOSTACTIVITY_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CHOSTACTIVITY_H__

#include "_Elastos_DevSamples_DFChatDemo_CHostActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

class CChatApplication;

CarClass(CHostActivity)
    , public Activity
    , public IObserver
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CHostActivity::MyHandler")

        MyHandler(
            /* [in] */ CHostActivity* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CHostActivity* mHost;
    };

    class ShowDialogListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ShowDialogListener(
            /* [in] */ CHostActivity* host,
            /* [in] */ Int32 dialogId);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CHostActivity* mHost;
        Int32 mDialogId;
    };

    class QuitListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        QuitListener(
            /* [in] */ CHostActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CHostActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CHostActivity")

    CHostActivity();

    ~CHostActivity();

    CARAPI constructor();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnDestroy();

public:
    CARAPI Update(
        /* [in] */ IObservable* o,
        /* [in] */ IInterface* arg);

protected:
    virtual CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

private:
    void UpdateChannelState();

    void AlljoynError();

    void SendMessage(
        /* [in] */ Int32 id);

public:
    static const Int32 DIALOG_ALLJOYN_ERROR_ID = 3;

private:

    static const Int32 DIALOG_SET_NAME_ID = 0;
    static const Int32 DIALOG_START_ID = 1;
    static const Int32 DIALOG_STOP_ID = 2;

    static const Int32 HANDLE_APPLICATION_QUIT_EVENT = 0;
    static const Int32 HANDLE_CHANNEL_STATE_CHANGED_EVENT = 1;
    static const Int32 HANDLE_ALLJOYN_ERROR_EVENT = 2;

    CChatApplication* mChatApplication;

    AutoPtr<ITextView> mChannelName;
    AutoPtr<ITextView> mChannelStatus;
    AutoPtr<IButton> mSetNameButton;
    AutoPtr<IButton> mStartButton;
    AutoPtr<IButton> mStopButton;
    AutoPtr<IButton> mQuitButton;

    AutoPtr<MyHandler> mHandler;
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CHOSTACTIVITY_H__
