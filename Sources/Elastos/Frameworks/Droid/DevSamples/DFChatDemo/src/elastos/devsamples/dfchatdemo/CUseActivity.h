
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CUSEACTIVITY_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CUSEACTIVITY_H__

#include "_Elastos_DevSamples_DFChatDemo_CUseActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IArrayAdapter;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

class CChatApplication;

CarClass(CUseActivity)
    , public Activity
    , public IObserver
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CUseActivity::MyHandler")

        MyHandler(
            /* [in] */ CUseActivity* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CUseActivity* mHost;
    };

    class InnerListener
        : public Object
        , public IOnEditorActionListener
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CUseActivity* host);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CUseActivity* mHost;
    };
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CUseActivity")

    CUseActivity();

    ~CUseActivity();

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
    void UpdateHistory();

    void UpdateChannelState();

    void AlljoynError();

    void SendMessage(
        /* [in] */ Int32 id);

public:

    static const Int32 DIALOG_JOIN_ID = 0;
    static const Int32 DIALOG_LEAVE_ID = 1;
    static const Int32 DIALOG_ALLJOYN_ERROR_ID = 2;

private:
    friend class MyHandler;

    static const Int32 HANDLE_APPLICATION_QUIT_EVENT = 0;
    static const Int32 HANDLE_HISTORY_CHANGED_EVENT = 1;
    static const Int32 HANDLE_CHANNEL_STATE_CHANGED_EVENT = 2;
    static const Int32 HANDLE_ALLJOYN_ERROR_EVENT = 3;

    CChatApplication* mChatApplication;

    AutoPtr<IArrayAdapter> mHistoryList;

    AutoPtr<ITextView> mChannelName;
    AutoPtr<ITextView> mChannelStatus;
    AutoPtr<IButton> mJoinButton;
    AutoPtr<IButton> mLeaveButton;

    AutoPtr<MyHandler> mHandler;
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CUSEACTIVITY_H__
