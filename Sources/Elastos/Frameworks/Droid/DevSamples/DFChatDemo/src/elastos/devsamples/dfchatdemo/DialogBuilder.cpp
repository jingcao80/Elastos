
#include "DialogBuilder.h"
#include "CChatApplication.h"
#include "CUseActivity.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DialogBuilder");

//============================================================================
// DialogBuilder::HostNameOnEditorActionListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::HostNameOnEditorActionListener, Object, IOnEditorActionListener)

DialogBuilder::HostNameOnEditorActionListener::HostNameOnEditorActionListener(
    /* [in] */ CChatApplication* application,
    /* [in] */ IDialog* dialog)
    : mApplication(application)
    , mDialog(dialog)
{
}

ECode DialogBuilder::HostNameOnEditorActionListener::OnEditorAction(
    /* [in] */ ITextView* view,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    event->GetAction(&action);
    if (actionId == IEditorInfo::IME_NULL && action == IKeyEvent::ACTION_UP) {
        AutoPtr<ICharSequence> seq;
        view->GetText((ICharSequence**)&seq);
        String message = Object::ToString(seq);
        mApplication->HostSetChannelName(message);
        mApplication->HostInitChannel();

        IDialogInterface::Probe(mDialog)->Cancel();
    }

    *result = TRUE;
    return NOERROR;
}

//============================================================================
// DialogBuilder::HostNameOkOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::HostNameOkOnClickListener, Object, IViewOnClickListener)

DialogBuilder::HostNameOkOnClickListener::HostNameOkOnClickListener(
    /* [in] */ CChatApplication* application,
    /* [in] */ IDialog* dialog,
    /* [in] */ ITextView* textView)
    : mApplication(application)
    , mDialog(dialog)
    , mTextView(textView)
{
}

ECode DialogBuilder::HostNameOkOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<ICharSequence> csq;
    mTextView->GetText((ICharSequence**)&csq);
    String name = Object::ToString(csq);
    mApplication->HostSetChannelName(name);
    mApplication->HostInitChannel();

    IDialogInterface::Probe(mDialog)->Cancel();
    return NOERROR;
}

//============================================================================
// DialogBuilder::HostStopOkOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::HostStopOkOnClickListener, Object, IViewOnClickListener)

DialogBuilder::HostStopOkOnClickListener::HostStopOkOnClickListener(
    /* [in] */ CChatApplication* application,
    /* [in] */ IDialog* dialog)
    : mApplication(application)
    , mDialog(dialog)
{
}

ECode DialogBuilder::HostStopOkOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    mApplication->HostStopChannel();
    IDialogInterface::Probe(mDialog)->Cancel();
    return NOERROR;
}

//============================================================================
// DialogBuilder::HostStartOkOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::HostStartOkOnClickListener, Object, IViewOnClickListener)

DialogBuilder::HostStartOkOnClickListener::HostStartOkOnClickListener(
    /* [in] */ CChatApplication* application,
    /* [in] */ IDialog* dialog)
    : mApplication(application)
    , mDialog(dialog)
{
}

ECode DialogBuilder::HostStartOkOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    mApplication->HostStartChannel();
    IDialogInterface::Probe(mDialog)->Cancel();
    return NOERROR;
}

//============================================================================
// DialogBuilder::UseLeaveOKOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::UseLeaveOKOnClickListener, Object, IViewOnClickListener)

DialogBuilder::UseLeaveOKOnClickListener::UseLeaveOKOnClickListener(
    /* [in] */ CChatApplication* application,
    /* [in] */ IDialog* dialog)
    : mApplication(application)
    , mDialog(dialog)
{
}

ECode DialogBuilder::UseLeaveOKOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    mApplication->UseLeaveChannel();
    mApplication->UseSetChannelName(String("Not set"));
    IDialogInterface::Probe(mDialog)->Cancel();
    return NOERROR;
}

//============================================================================
// DialogBuilder::DialogCancelOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::DialogCancelOnClickListener, Object, IViewOnClickListener)

DialogBuilder::DialogCancelOnClickListener::DialogCancelOnClickListener(
    /* [in] */ IDialog* dialog)
    : mDialog(dialog)
{
}

ECode DialogBuilder::DialogCancelOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    IDialogInterface::Probe(mDialog)->Cancel();
    return NOERROR;
}

//============================================================================
// DialogBuilder::UseJoinCancelOnClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::UseJoinCancelOnClickListener, Object, IViewOnClickListener)

DialogBuilder::UseJoinCancelOnClickListener::UseJoinCancelOnClickListener(
    /* [in] */ IActivity* activity)
    : mActivity(activity)
{
}

ECode DialogBuilder::UseJoinCancelOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    return mActivity->RemoveDialog(CUseActivity::DIALOG_JOIN_ID);
}

//============================================================================
// DialogBuilder::UseJoinCancelOnItemClickListener
//============================================================================
CAR_INTERFACE_IMPL(DialogBuilder::UseJoinCancelOnItemClickListener, \
    Object, IAdapterViewOnItemClickListener)

DialogBuilder::UseJoinCancelOnItemClickListener::UseJoinCancelOnItemClickListener(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application,
    /* [in] */ IAdapterView* listView)
    : mActivity(activity)
    , mApplication(application)
    , mChannelList(listView)
{}

ECode DialogBuilder::UseJoinCancelOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
     /* [in] */ Int64 id)
{
    AutoPtr<IInterface> obj;
    mChannelList->GetItemAtPosition(position, (IInterface**)&obj);
    String name = Object::ToString(obj);
    mApplication->UseSetChannelName(name);
    mApplication->UseJoinChannel();

    /*
     * Android likes to reuse dialogs for performance reasons.  If
     * we reuse this one, the list of channels will eventually be
     * wrong since it can change.  We have to tell the Android
     * application framework to forget about this dialog completely.
     */
    mActivity->RemoveDialog(CUseActivity::DIALOG_JOIN_ID);
    return NOERROR;
}

AutoPtr<IDialog> DialogBuilder::CreateUseJoinDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createUseJoinDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::usejoindialog);

    AutoPtr<IArrayAdapter> channelListAdapter;
    CArrayAdapter::New(IContext::Probe(activity), Elastos::Droid::R::layout::test_list_item,
        (IArrayAdapter**)&channelListAdapter);
    AutoPtr<IView> view;
    dialog->FindViewById(R::id::useJoinChannelList, (IView**)&view);
    AutoPtr<IListView> channelList = IListView::Probe(view);
    IAdapterView* adapterView = IAdapterView::Probe(channelList);
    adapterView->SetAdapter(IAdapter::Probe(channelListAdapter));

    AutoPtr< List<String> > channels = application->GetFoundChannels();
    List<String>::Iterator it;
    for (it = channels->Begin(); it != channels->End(); ++it) {
        String channel = (*it);
        int lastDot = channel.LastIndexOf('.');
        if (lastDot < 0) {
            continue;
        }
        channelListAdapter->Add(CoreUtils::Convert(channel.Substring(lastDot + 1)));
    }
    IBaseAdapter::Probe(channelListAdapter)->NotifyDataSetChanged();

    AutoPtr<UseJoinCancelOnItemClickListener> icl = new UseJoinCancelOnItemClickListener(
        activity, application, adapterView);
    adapterView->SetOnItemClickListener(icl.Get());

    view = NULL;
    dialog->FindViewById(R::id::useJoinCancel, (IView**)&view);
    AutoPtr<IViewOnClickListener> ujcl = new UseJoinCancelOnClickListener(activity);
    view->SetOnClickListener(ujcl);

    return dialog;
}

AutoPtr<IDialog> DialogBuilder::CreateUseLeaveDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createUseLeaveDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::useleavedialog);

    AutoPtr<IView> view;
    dialog->FindViewById(R::id::useLeaveOk, (IView**)&view);
    AutoPtr<UseLeaveOKOnClickListener> uocl = new UseLeaveOKOnClickListener(application, dialog);
    view->SetOnClickListener(uocl.Get());

    view = NULL;
    dialog->FindViewById(R::id::useLeaveCancel, (IView**)&view);
    AutoPtr<DialogCancelOnClickListener> cl = new DialogCancelOnClickListener(dialog);
    view->SetOnClickListener(cl.Get());

    return dialog;
}

AutoPtr<IDialog> DialogBuilder::CreateHostNameDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createHostNameDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::hostnamedialog);

    AutoPtr<IView> view;
    dialog->FindViewById(R::id::hostNameChannel, (IView**)&view);
    ITextView* channel = ITextView::Probe(view);
    AutoPtr<HostNameOnEditorActionListener> eal = new HostNameOnEditorActionListener(application, dialog);
    channel->SetOnEditorActionListener(eal.Get());

    dialog->FindViewById(R::id::hostNameOk, (IView**)&view);
    AutoPtr<HostNameOkOnClickListener> uocl = new HostNameOkOnClickListener(
        application, dialog, channel);
    view->SetOnClickListener(uocl.Get());

    view = NULL;
    dialog->FindViewById(R::id::hostNameCancel, (IView**)&view);
    AutoPtr<DialogCancelOnClickListener> cl = new DialogCancelOnClickListener(dialog);
    view->SetOnClickListener(cl.Get());

    return dialog;
}

AutoPtr<IDialog> DialogBuilder::CreateHostStartDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createHostStartDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::hoststartdialog);

    AutoPtr<IView> view;
    dialog->FindViewById(R::id::hostStartOk, (IView**)&view);
    AutoPtr<HostStartOkOnClickListener> uocl = new HostStartOkOnClickListener(application, dialog);
    view->SetOnClickListener(uocl.Get());

    view = NULL;
    dialog->FindViewById(R::id::hostStartCancel, (IView**)&view);
    AutoPtr<DialogCancelOnClickListener> cl = new DialogCancelOnClickListener(dialog);
    view->SetOnClickListener(cl.Get());

    return dialog;
}

AutoPtr<IDialog> DialogBuilder::CreateHostStopDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createHostStopDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::hoststopdialog);

    AutoPtr<IView> view;
    dialog->FindViewById(R::id::hostStopOk, (IView**)&view);
    AutoPtr<HostStopOkOnClickListener> uocl = new HostStopOkOnClickListener(application, dialog);
    view->SetOnClickListener(uocl.Get());

    view = NULL;
    dialog->FindViewById(R::id::hostStopCancel, (IView**)&view);
    AutoPtr<DialogCancelOnClickListener> cl = new DialogCancelOnClickListener(dialog);
    view->SetOnClickListener(cl.Get());


    return dialog;
}

AutoPtr<IDialog> DialogBuilder::CreateAllJoynErrorDialog(
    /* [in] */ IActivity* activity,
    /* [in] */ CChatApplication* application)
{
    Logger::I(TAG, "createAllJoynErrorDialog()");
    AutoPtr<IDialog> dialog;
    CDialog::New(IContext::Probe(activity), (IDialog**)&dialog);
    Boolean bval;
    dialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &bval);
    dialog->SetContentView(R::layout::alljoynerrordialog);

    AutoPtr<IView> view;
    dialog->FindViewById(R::id::errorDescription, (IView**)&view);
    String str = application->GetErrorString();
    ITextView::Probe(view)->SetText(CoreUtils::Convert(str));

    view = NULL;
    dialog->FindViewById(R::id::errorOk, (IView**)&view);
    AutoPtr<DialogCancelOnClickListener> cl = new DialogCancelOnClickListener(dialog);
    view->SetOnClickListener(cl.Get());

    return dialog;
}



} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos