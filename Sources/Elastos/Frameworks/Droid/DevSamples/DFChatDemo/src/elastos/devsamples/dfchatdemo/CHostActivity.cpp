#include "CHostActivity.h"
#include "CChatApplication.h"
#include "DialogBuilder.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IApplication;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DFChatDemo.CHostActivity");

//========================================================
// CHostActivity::MyHandler
//========================================================
CHostActivity::MyHandler::MyHandler()
    : mHost(NULL)
{}

ECode CHostActivity::MyHandler::constructor(
    /* [in] */ CHostActivity* host)
{
    mHost = host;
    return NOERROR;
}

ECode CHostActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
    case CHostActivity::HANDLE_APPLICATION_QUIT_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_APPLICATION_QUIT_EVENT");
            mHost->Finish();
        }
        break;
    case CHostActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_CHANNEL_STATE_CHANGED_EVENT");
            mHost->UpdateChannelState();
            break;
        }
    case CHostActivity::HANDLE_ALLJOYN_ERROR_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_ALLJOYN_ERROR_EVENT");
            mHost->AlljoynError();
            break;
        }
    default:
        break;
    }

    return NOERROR;
}

//========================================================
// CHostActivity::ShowDialogListener
//========================================================
CAR_INTERFACE_IMPL(CHostActivity::ShowDialogListener, Object, IViewOnClickListener)

CHostActivity::ShowDialogListener::ShowDialogListener(
    /* [in] */ CHostActivity* host,
    /* [in] */ Int32 dialogId)
    : mHost(host)
    , mDialogId(dialogId)
{}

ECode CHostActivity::ShowDialogListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->ShowDialog(mDialogId);
    return NOERROR;
}


//========================================================
// CHostActivity::QuitListener
//========================================================
CAR_INTERFACE_IMPL(CHostActivity::QuitListener, Object, IViewOnClickListener)

CHostActivity::QuitListener::QuitListener(
    /* [in] */ CHostActivity* host)
    : mHost(host)
{}

ECode CHostActivity::QuitListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->mChatApplication->Quit();
    return NOERROR;
}


//========================================================
// CHostActivity
//========================================================
const Int32 CHostActivity::DIALOG_ALLJOYN_ERROR_ID;
const Int32 CHostActivity::DIALOG_SET_NAME_ID;
const Int32 CHostActivity::DIALOG_START_ID;
const Int32 CHostActivity::DIALOG_STOP_ID;
const Int32 CHostActivity::HANDLE_APPLICATION_QUIT_EVENT;
const Int32 CHostActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT;
const Int32 CHostActivity::HANDLE_ALLJOYN_ERROR_EVENT;

CAR_INTERFACE_IMPL(CHostActivity, Activity, IObserver)

CHostActivity::CHostActivity()
    : mChatApplication(NULL)
{}

CHostActivity::~CHostActivity()
{}

ECode CHostActivity::constructor()
{
    mHandler = new MyHandler();
    mHandler->constructor(this);
    return Activity::constructor();
}

ECode CHostActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, "onCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::host);

    AutoPtr<IView> view = FindViewById(R::id::hostChannelName);
    mChannelName = ITextView::Probe(view);
    mChannelName->SetText(CoreUtils::Convert(""));

    view = FindViewById(R::id::hostChannelStatus);
    mChannelStatus = ITextView::Probe(view);
    mChannelStatus->SetText(CoreUtils::Convert("Idle"));

    view = FindViewById(R::id::hostSetName);
    mSetNameButton = IButton::Probe(view);
    view->SetEnabled(TRUE);
    AutoPtr<ShowDialogListener> snl = new ShowDialogListener(this, DIALOG_SET_NAME_ID);
    view->SetOnClickListener(snl.Get());

    view = FindViewById(R::id::hostStart);
    mStartButton = IButton::Probe(view);
    view->SetEnabled(FALSE);
    AutoPtr<ShowDialogListener> hs = new ShowDialogListener(this, DIALOG_START_ID);
    view->SetOnClickListener(hs.Get());

    view = FindViewById(R::id::hostStop);
    mStopButton = IButton::Probe(view);
    view->SetEnabled(FALSE);
    AutoPtr<ShowDialogListener> sl = new ShowDialogListener(this, DIALOG_STOP_ID);
    view->SetOnClickListener(sl.Get());

    /*
     * Keep a pointer to the Android Appliation class around.  We use this
     * as the Model for our MVC-based application.  Whenever we are started
     * we need to "check in" with the application so it can ensure that our
     * required services are running.
     */
    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    mChatApplication = (CChatApplication*)app.Get();
    mChatApplication->Checkin();

    /*
     * Call down into the model to get its current state.  Since the model
     * outlives its Activities, this may actually be a lot of state and not
     * just empty.
     */
    UpdateChannelState();

    /*
     * Now that we're all ready to go, we are ready to accept notifications
     * from other components.
     */
    mChatApplication->AddObserver(this);

    view = FindViewById(R::id::hostQuit);
    mQuitButton = IButton::Probe(view);
    view->SetEnabled(TRUE);
    AutoPtr<QuitListener> ql = new QuitListener(this);
    view->SetOnClickListener(ql.Get());

    return NOERROR;
}

ECode CHostActivity::OnDestroy()
{
    Logger::I(TAG, "onDestroy()");
    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    mChatApplication = (CChatApplication*)app.Get();
    mChatApplication->DeleteObserver(this);
    return Activity::OnDestroy();
}

void CHostActivity::SendMessage(
    /* [in] */ Int32 id)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(id, (IMessage**)&message);
    Boolean bval;
    mHandler->SendMessage(message, &bval);
}

ECode CHostActivity::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    AutoLock lock(this);
    String qualifier = Object::ToString(arg);
    Logger::I(TAG, "update(%s)", qualifier.string());

    if (qualifier.Equals(CChatApplication::APPLICATION_QUIT_EVENT)) {
        SendMessage(HANDLE_APPLICATION_QUIT_EVENT);
    }

    if (qualifier.Equals(CChatApplication::HOST_CHANNEL_STATE_CHANGED_EVENT)) {
        SendMessage(HANDLE_CHANNEL_STATE_CHANGED_EVENT);
    }

    if (qualifier.Equals(CChatApplication::ALLJOYN_ERROR_EVENT)) {
        SendMessage(HANDLE_ALLJOYN_ERROR_EVENT);
    }
    return NOERROR;
}

AutoPtr<IDialog> CHostActivity::OnCreateDialog(
    /* [in] */ Int32 id)
{
    Logger::I(TAG, "onCreateDialog()");
    AutoPtr<IDialog> result;
    switch(id) {
    case DIALOG_SET_NAME_ID:
        {
            result = DialogBuilder::CreateHostNameDialog(this, mChatApplication);
        }
        break;
    case DIALOG_START_ID:
        {
            result = DialogBuilder::CreateHostStartDialog(this, mChatApplication);
        }
        break;
    case DIALOG_STOP_ID:
        {
            result = DialogBuilder::CreateHostStopDialog(this, mChatApplication);
        }
        break;
    case DIALOG_ALLJOYN_ERROR_ID:
        {
            result = DialogBuilder::CreateAllJoynErrorDialog(this, mChatApplication);
        }
        break;
    }
    return result;
}

void CHostActivity::UpdateChannelState()
{
    HostChannelState channelState = mChatApplication->HostGetChannelState();
    String name = mChatApplication->HostGetChannelName();
    Boolean haveName = TRUE;
    if (name.IsNull()) {
        haveName = FALSE;
        name = "Not set";
    }
    mChannelName->SetText(CoreUtils::Convert(name));
    switch (channelState) {
    case HostChannelState_IDLE:
        mChannelStatus->SetText(CoreUtils::Convert("Idle"));
        break;
    case HostChannelState_NAMED:
        mChannelStatus->SetText(CoreUtils::Convert("Named"));
        break;
    case HostChannelState_BOUND:
        mChannelStatus->SetText(CoreUtils::Convert("Bound"));
        break;
    case HostChannelState_ADVERTISED:
        mChannelStatus->SetText(CoreUtils::Convert("Advertised"));
        break;
    case HostChannelState_CONNECTED:
        mChannelStatus->SetText(CoreUtils::Convert("Connected"));
        break;
    default:
        mChannelStatus->SetText(CoreUtils::Convert("Unknown"));
        break;
    }

    if (channelState == HostChannelState_IDLE) {
        IView::Probe(mSetNameButton)->SetEnabled(TRUE);
        if (haveName) {
            IView::Probe(mStartButton)->SetEnabled(TRUE);
        } else {
            IView::Probe(mStartButton)->SetEnabled(FALSE);
        }
        IView::Probe(mStopButton)->SetEnabled(FALSE);
    }
    else {
        IView::Probe(mSetNameButton)->SetEnabled(FALSE);
        IView::Probe(mStartButton)->SetEnabled(FALSE);
        IView::Probe(mStopButton)->SetEnabled(TRUE);
    }
}

void CHostActivity::AlljoynError()
{
    if (mChatApplication->GetErrorModule() == Module_GENERAL ||
        mChatApplication->GetErrorModule() == Module_USE) {
        ShowDialog(DIALOG_ALLJOYN_ERROR_ID);
    }
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos