
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
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IArrayAdapter;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

class CChatApplication;

CarClass(CUseActivity)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler();

        CARAPI constructor(
            /* [in] */ CUseActivity* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CUseActivity* mHost;
    };

public:
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

public:

    static const Int32 DIALOG_JOIN_ID;
    static const Int32 DIALOG_LEAVE_ID;
    static const Int32 DIALOG_ALLJOYN_ERROR_ID;

private:
    static const Int32 HANDLE_APPLICATION_QUIT_EVENT;
    static const Int32 HANDLE_HISTORY_CHANGED_EVENT;
    static const Int32 HANDLE_CHANNEL_STATE_CHANGED_EVENT;
    static const Int32 HANDLE_ALLJOYN_ERROR_EVENT;

    CChatApplication* mChatApplication;

    AutoPtr<IArrayAdapter> mHistoryList;

    AutoPtr<ITextView> mChannelName;
    AutoPtr<ITextView> mChannelStatus;
    AutoPtr<IButton> mJoinButton;
    AutoPtr<IButton> mLeaveButton;

    AutoPtr<IHandler> mHandler;
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CUSEACTIVITY_H__
