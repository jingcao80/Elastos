#include "CUseActivity.h"
#include "CChatApplication.h"
#include "DialogBuilder.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DFChatDemo.CUseActivity");

//========================================================
// CUseActivity::MyHandler
//========================================================

CUseActivity::MyHandler::MyHandler(
    /* [in] */ CUseActivity* host)
    : mHost(host)
{}

ECode CUseActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
    case CUseActivity::HANDLE_APPLICATION_QUIT_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_APPLICATION_QUIT_EVENT");
            mHost->Finish();
        }
        break;
    case CUseActivity::HANDLE_HISTORY_CHANGED_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_HISTORY_CHANGED_EVENT");
            mHost->UpdateHistory();
            break;
        }
    case CUseActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT:
        {
            Logger::I(TAG, "mHandler.handleMessage(): HANDLE_CHANNEL_STATE_CHANGED_EVENT");
            mHost->UpdateChannelState();
            break;
        }
    case CUseActivity::HANDLE_ALLJOYN_ERROR_EVENT:
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
// CUseActivity::InnerListener
//========================================================
CAR_INTERFACE_IMPL_2(CUseActivity::InnerListener, Object, IOnEditorActionListener, IViewOnClickListener)

CUseActivity::InnerListener::InnerListener(
    /* [in] */ CUseActivity* host)
    : mHost(host)
{}

ECode CUseActivity::InnerListener::OnEditorAction(
    /* [in] */ ITextView* view,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (actionId == IEditorInfo::IME_ACTION_DONE) {
        AutoPtr<ICharSequence> seq;
        view->GetText((ICharSequence**)&seq);
        String message = Object::ToString(seq);
        Logger::I(TAG, "useMessage.onEditorAction(): got message [%s]", message.string());
        mHost->mChatApplication->NewLocalUserMessage(message);
        view->SetText(CoreUtils::Convert(""));
    }

    *result = TRUE;
    return NOERROR;
}

ECode CUseActivity::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);

    switch (id) {
        case R::id::useJoin: {
            mHost->ShowDialog(CUseActivity::DIALOG_JOIN_ID);
        }
        break;

        case R::id::useLeave: {
            mHost->ShowDialog(CUseActivity::DIALOG_LEAVE_ID);
        }
        break;
    }
    return NOERROR;
}

//========================================================
// CUseActivity
//========================================================
const Int32 CUseActivity::DIALOG_JOIN_ID;
const Int32 CUseActivity::DIALOG_LEAVE_ID;
const Int32 CUseActivity::DIALOG_ALLJOYN_ERROR_ID;

const Int32 CUseActivity::HANDLE_APPLICATION_QUIT_EVENT;
const Int32 CUseActivity::HANDLE_HISTORY_CHANGED_EVENT;
const Int32 CUseActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT;
const Int32 CUseActivity::HANDLE_ALLJOYN_ERROR_EVENT;

CAR_INTERFACE_IMPL(CUseActivity, Activity, IObserver)

CAR_OBJECT_IMPL(CUseActivity)

CUseActivity::CUseActivity()
    : mChatApplication(NULL)
{}

CUseActivity::~CUseActivity()
{}

ECode CUseActivity::constructor()
{
    mHandler = new MyHandler(this);
    mHandler->constructor();
    return Activity::constructor();
}

ECode CUseActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, "onCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::use);

    CArrayAdapter::New(this, Elastos::Droid::R::layout::test_list_item,
        (IArrayAdapter**)&mHistoryList);

    AutoPtr<IView> view = FindViewById(R::id::useHistoryList);
    IAdapterView::Probe(view)->SetAdapter(IAdapter::Probe(mHistoryList));

    AutoPtr<InnerListener> listener = new InnerListener(this);

    view = FindViewById(R::id::useMessage);
    AutoPtr<IEditText> messageBox = IEditText::Probe(view);
    ITextView::Probe(messageBox)->SetSingleLine();
    ITextView::Probe(messageBox)->SetOnEditorActionListener(listener.Get());

    view = FindViewById(R::id::useJoin);
    mJoinButton = IButton::Probe(view);
    view->SetOnClickListener(listener.Get());

    view = FindViewById(R::id::useLeave);
    mLeaveButton = IButton::Probe(view);
    view->SetOnClickListener(listener.Get());

    view = FindViewById(R::id::useChannelName);
    mChannelName = ITextView::Probe(view);
    view = FindViewById(R::id::useChannelStatus);
    mChannelStatus = ITextView::Probe(view);

    /*
     * Keep a pointer to the Android Appliation class around.  We use this
     * as the Model for our MVC-based application.    Whenever we are started
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
    UpdateHistory();

    /*
     * Now that we're all ready to go, we are ready to accept notifications
     * from other components.
     */
    mChatApplication->AddObserver(this);
    return NOERROR;
}

ECode CUseActivity::OnDestroy()
{
    Logger::I(TAG, "onDestroy()");

    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    mChatApplication = (CChatApplication*)app.Get();
    mChatApplication->DeleteObserver(this);
    return Activity::OnDestroy();
}

void CUseActivity::SendMessage(
    /* [in] */ Int32 id)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(id, (IMessage**)&message);
    Boolean bval;
    mHandler->SendMessage(message, &bval);
}

ECode CUseActivity::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    AutoLock lock(this);
    String qualifier = Object::ToString(arg);
    Logger::I(TAG, "update(%s)", qualifier.string());

    if (qualifier.Equals(CChatApplication::APPLICATION_QUIT_EVENT)) {
        SendMessage(HANDLE_APPLICATION_QUIT_EVENT);
    }

    if (qualifier.Equals(CChatApplication::HISTORY_CHANGED_EVENT)) {
        SendMessage(HANDLE_HISTORY_CHANGED_EVENT);
    }

    if (qualifier.Equals(CChatApplication::USE_CHANNEL_STATE_CHANGED_EVENT)) {
        SendMessage(HANDLE_CHANNEL_STATE_CHANGED_EVENT);
    }

    if (qualifier.Equals(CChatApplication::ALLJOYN_ERROR_EVENT)) {
        SendMessage(HANDLE_ALLJOYN_ERROR_EVENT);
    }
    return NOERROR;
}

AutoPtr<IDialog> CUseActivity::OnCreateDialog(
    /* [in] */ Int32 id)
{
    Logger::I(TAG, "onCreateDialog()");

    AutoPtr<IDialog> result;
    switch(id) {
    case DIALOG_JOIN_ID:
        {
            result = DialogBuilder::CreateUseJoinDialog(this, mChatApplication);
        }
        break;
    case DIALOG_LEAVE_ID:
        {
            result = DialogBuilder::CreateUseLeaveDialog(this, mChatApplication);
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

void CUseActivity::UpdateHistory()
{
    Logger::I(TAG, "updateHistory()");
    mHistoryList->Clear();
    AutoPtr< List<String> > messages = mChatApplication->GetHistory();
    List<String>::Iterator it;
    for (it = messages->Begin(); it != messages->End(); ++it) {
        mHistoryList->Add(CoreUtils::Convert(*it));
    }
    IBaseAdapter::Probe(mHistoryList)->NotifyDataSetChanged();
}

void CUseActivity::UpdateChannelState()
{
    Logger::I(TAG, "updateHistory()");
    UseChannelState channelState = mChatApplication->UseGetChannelState();
    String name = mChatApplication->UseGetChannelName();
    if (name.IsNull()) {
        name = "Not set";
    }
    mChannelName->SetText(CoreUtils::Convert(name));

    switch (channelState) {
    case UseChannelState_IDLE:
        mChannelStatus->SetText(CoreUtils::Convert("Idle"));
        IView::Probe(mJoinButton)->SetEnabled(TRUE);
        IView::Probe(mLeaveButton)->SetEnabled(FALSE);
        break;

    case UseChannelState_JOINED:
        mChannelStatus->SetText(CoreUtils::Convert("Joined"));
        IView::Probe(mJoinButton)->SetEnabled(FALSE);
        IView::Probe(mLeaveButton)->SetEnabled(TRUE);
        break;
    }
}

void CUseActivity::AlljoynError()
{
    if (mChatApplication->GetErrorModule() == Module_GENERAL ||
        mChatApplication->GetErrorModule() == Module_USE) {
        ShowDialog(DIALOG_ALLJOYN_ERROR_ID);
    }
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos