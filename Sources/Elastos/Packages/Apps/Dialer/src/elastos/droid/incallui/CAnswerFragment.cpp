
#include "elastos/droid/incallui/CAnswerFragment.h"
#include "elastos/droid/incallui/AnswerPresenter.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::CListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CAnswerFragment::RespondViaSmsItemClickListener
//================================================================
CAR_INTERFACE_IMPL(CAnswerFragment::RespondViaSmsItemClickListener, Object, IAdapterViewOnItemClickListener);

ECode CAnswerFragment::RespondViaSmsItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Logger::D("CAnswerFragment", "RespondViaSmsItemClickListener.onItemClick(%d)...", position);
    AutoPtr<IInterface> item;
    parent->GetItemAtPosition(position, (IInterface**)&item);
    String message;
    ICharSequence::Probe(item)->ToString(&message);
    Logger::V("CAnswerFragment", "- message: '%s'", message.string());
    mHost->DismissCannedResponsePopup();

    // The "Custom" choice is a special case.
    // (For now, it's guaranteed to be the last item.)
    Int32 size;
    parent->GetCount(&size);
    if (position == (size - 1)) {
        // Show the custom message dialog
        mHost->ShowCustomMessageDialog();
    }
    else {
        AnswerPresenter* present = (AnswerPresenter*)mHost->GetPresenter().Get();
        present->RejectCallWithMessage(message);
    }
    return NOERROR;
}


//================================================================
// CAnswerFragment::DialogCancelListener
//================================================================
CAR_INTERFACE_IMPL(CAnswerFragment::DialogCancelListener, Object, IDialogInterfaceOnCancelListener);

ECode CAnswerFragment::DialogCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    if (mHost->mGlowpad != NULL) {
        mHost->mGlowpad->StartPing();
    }
    mHost->DismissCannedResponsePopup();
    AnswerPresenter* present = (AnswerPresenter*)mHost->GetPresenter().Get();
    present->OnDismissDialog();
    return NOERROR;
}


//================================================================
// CAnswerFragment::PositiveButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(CAnswerFragment::PositiveButtonClickListener, Object, IDialogInterfaceOnClickListener);

ECode CAnswerFragment::PositiveButtonClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // The order is arranged in a way that the popup will be destroyed when the
    // InCallActivity is about to finish.
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mEditText)->GetText((ICharSequence**)&text);
    String textMessage = CoreUtils::Unbox(text).Trim();
    mHost->DismissCustomMessagePopup();
    AnswerPresenter* present = (AnswerPresenter*)mHost->GetPresenter().Get();
    present->RejectCallWithMessage(textMessage);
    return NOERROR;
}


//================================================================
// CAnswerFragment::NegativeButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(CAnswerFragment::NegativeButtonClickListener, Object, IDialogInterfaceOnClickListener);

ECode CAnswerFragment::NegativeButtonClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->DismissCustomMessagePopup();
    AnswerPresenter* present = (AnswerPresenter*)mHost->GetPresenter().Get();
    present->OnDismissDialog();
    return NOERROR;
}


//================================================================
// CAnswerFragment::MyTextWatcher
//================================================================
CAR_INTERFACE_IMPL_2(CAnswerFragment::MyTextWatcher, Object, ITextWatcher, INoCopySpan);

ECode CAnswerFragment::MyTextWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CAnswerFragment::MyTextWatcher::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return NOERROR;
}

ECode CAnswerFragment::MyTextWatcher::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    AutoPtr<IButton> sendButton;
    mHost->mCustomMessagePopup->GetButton(
            IDialogInterface::BUTTON_POSITIVE, (IButton**)&sendButton);
    String text;
    IView::Probe(sendButton)->SetEnabled(s != NULL &&
            (IObject::Probe(s)->ToString(&text), text.Trim().GetLength() != 0));
    return NOERROR;
}


CAR_INTERFACE_IMPL(CAnswerFragment::InnerListener, Object, IAnswerListener);

ECode CAnswerFragment::InnerListener::OnAnswer(
    /* [in] */ Int32 videoState,
    /* [in] */ IContext* context)
{
    return mHost->OnAnswer(videoState, context);
}

ECode CAnswerFragment::InnerListener::OnDecline()
{
    return mHost->OnDecline();
}

ECode CAnswerFragment::InnerListener::OnText()
{
    return mHost->OnText();
}

//================================================================
// CAnswerFragment
//================================================================
const Int32 CAnswerFragment::TARGET_SET_FOR_AUDIO_WITHOUT_SMS;
const Int32 CAnswerFragment::TARGET_SET_FOR_AUDIO_WITH_SMS;
const Int32 CAnswerFragment::TARGET_SET_FOR_VIDEO_WITHOUT_SMS;
const Int32 CAnswerFragment::TARGET_SET_FOR_VIDEO_WITH_SMS;
const Int32 CAnswerFragment::TARGET_SET_FOR_VIDEO_UPGRADE_REQUEST;

CAR_INTERFACE_IMPL_3(CAnswerFragment, BaseFragment, IAnswerFragment, IAnswerUi, IUi);

CAR_OBJECT_IMPL(CAnswerFragment);

CAnswerFragment::CAnswerFragment()
{
    CArrayList::New((IList**)&mSmsResponses);
}

ECode CAnswerFragment::constructor()
{
    return BaseFragment::constructor();
}

AutoPtr<IPresenter> CAnswerFragment::CreatePresenter()
{
    AutoPtr<IPresenter> presenter = new AnswerPresenter();
    return presenter;
}

AutoPtr<IUi> CAnswerFragment::GetUi()
{
    return this;
}

ECode CAnswerFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<IView> v;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::answer_fragment, container, FALSE, (IView**)&v);
    mGlowpad = (CGlowPadWrapper*)IGlowPadWrapper::Probe(v);

    Logger::D("CAnswerFragment", "Creating view for answer fragment %s", TO_CSTR(this));
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Logger::D("CAnswerFragment", "Created from activity %s", TO_CSTR(activity));
    mGlowpad->SetAnswerListener(listener);

    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CAnswerFragment::OnDestroyView()
{
    Logger::D("CAnswerFragment", "onDestroyView");
    if (mGlowpad != NULL) {
        mGlowpad->StopPing();
        mGlowpad = NULL;
    }
    BaseFragment::OnDestroyView();
    return NOERROR;
}

ECode CAnswerFragment::ShowAnswerUi(
    /* [in] */ Boolean show)
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    v->SetVisibility(show ? IView::VISIBLE : IView::GONE);

    Logger::D("CAnswerFragment", "Show answer UI: %d", show);
    if (show) {
        mGlowpad->StartPing();
    }
    else {
        mGlowpad->StopPing();
    }
    return NOERROR;
}

ECode CAnswerFragment::ShowTargets(
    /* [in] */ Int32 targetSet)
{
    using Elastos::Droid::Dialer::R;

    Int32 targetResourceId;
    Int32 targetDescriptionsResourceId;
    Int32 directionDescriptionsResourceId;
    Int32 handleDrawableResourceId;

    switch (targetSet) {
        case TARGET_SET_FOR_AUDIO_WITH_SMS:
            targetResourceId = R::array::incoming_call_widget_audio_with_sms_targets;
            targetDescriptionsResourceId =
                    R::array::incoming_call_widget_audio_with_sms_target_descriptions;
            directionDescriptionsResourceId =
                    R::array::incoming_call_widget_audio_with_sms_direction_descriptions;
            handleDrawableResourceId = R::drawable::ic_incall_audio_handle;
            break;
        case TARGET_SET_FOR_VIDEO_WITHOUT_SMS:
            targetResourceId = R::array::incoming_call_widget_video_without_sms_targets;
            targetDescriptionsResourceId =
                    R::array::incoming_call_widget_video_without_sms_target_descriptions;
            directionDescriptionsResourceId =
                    R::array::incoming_call_widget_video_without_sms_direction_descriptions;
            handleDrawableResourceId = R::drawable::ic_incall_video_handle;
            break;
        case TARGET_SET_FOR_VIDEO_WITH_SMS:
            targetResourceId = R::array::incoming_call_widget_video_with_sms_targets;
            targetDescriptionsResourceId =
                    R::array::incoming_call_widget_video_with_sms_target_descriptions;
            directionDescriptionsResourceId =
                    R::array::incoming_call_widget_video_with_sms_direction_descriptions;
            handleDrawableResourceId = R::drawable::ic_incall_video_handle;
            break;
        case TARGET_SET_FOR_VIDEO_UPGRADE_REQUEST:
            targetResourceId = R::array::incoming_call_widget_video_upgrade_request_targets;
            targetDescriptionsResourceId =
                    R::array::incoming_call_widget_video_upgrade_request_target_descriptions;
            directionDescriptionsResourceId =
                    R::array::incoming_call_widget_video_upgrade_request_target_direction_descriptions;
            handleDrawableResourceId = R::drawable::ic_incall_video_handle;
            break;
        case TARGET_SET_FOR_AUDIO_WITHOUT_SMS:
        default:
            targetResourceId = R::array::incoming_call_widget_audio_without_sms_targets;
            targetDescriptionsResourceId =
                    R::array::incoming_call_widget_audio_without_sms_target_descriptions;
            directionDescriptionsResourceId =
                    R::array::incoming_call_widget_audio_without_sms_direction_descriptions;
            handleDrawableResourceId = R::drawable::ic_incall_audio_handle;
            break;
    }

    if (targetResourceId != mGlowpad->GetTargetResourceId()) {
        mGlowpad->SetTargetResources(targetResourceId);
        mGlowpad->SetTargetDescriptionsResourceId(targetDescriptionsResourceId);
        mGlowpad->SetDirectionDescriptionsResourceId(directionDescriptionsResourceId);
        mGlowpad->SetHandleDrawable(handleDrawableResourceId);
        mGlowpad->Reset(FALSE);
    }
    return NOERROR;
}

ECode CAnswerFragment::ShowMessageDialog()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);

    AutoPtr<IContext> context;
    builder->GetContext((IContext**)&context);
    AutoPtr< ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, mSmsResponses);
    mSmsResponsesAdapter = NULL;
    CArrayAdapter::New(context, Elastos::Droid::R::layout::simple_list_item_1,
            Elastos::Droid::R::id::text1, mSmsResponses, (IArrayAdapter**)&mSmsResponsesAdapter);

    AutoPtr<IListView> lv;
    CListView::New(IContext::Probe(activity), (IListView**)&lv);
    IAdapterView::Probe(lv)->SetAdapter(IAdapter::Probe(mSmsResponsesAdapter));
    IAdapterView::Probe(lv)->SetOnItemClickListener(new RespondViaSmsItemClickListener(this));

    builder->SetCancelable(TRUE);
    builder->SetView(IView::Probe(lv));
    builder->SetOnCancelListener(new DialogCancelListener(this));
    AutoPtr<IAlertDialog> alert;
    builder->Create((IAlertDialog**)&alert);
    mCannedResponsePopup = IDialog::Probe(alert);
    AutoPtr<IWindow> window;
    mCannedResponsePopup->GetWindow((IWindow**)&window);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
    mCannedResponsePopup->Show();
    return NOERROR;
}

Boolean CAnswerFragment::IsCannedResponsePopupShowing()
{
    if (mCannedResponsePopup != NULL) {
        Boolean isShowing;
        mCannedResponsePopup->IsShowing(&isShowing);
        return isShowing;
    }
    return FALSE;
}

Boolean CAnswerFragment::IsCustomMessagePopupShowing()
{
    if (mCustomMessagePopup != NULL) {
        Boolean isShowing;
        IDialog::Probe(mCustomMessagePopup)->IsShowing(&isShowing);
        return isShowing;
    }
    return FALSE;
}

void CAnswerFragment::DismissCannedResponsePopup()
{
    if (mCannedResponsePopup != NULL) {
        IDialogInterface::Probe(mCannedResponsePopup)->Dismiss();  // safe even if already dismissed
        mCannedResponsePopup = NULL;
    }
}

void CAnswerFragment::DismissCustomMessagePopup()
{
    if (mCustomMessagePopup != NULL) {
        IDialogInterface::Probe(mCustomMessagePopup)->Dismiss();
        mCustomMessagePopup = NULL;
    }
}

void CAnswerFragment::DismissPendingDialogues()
{
    if (IsCannedResponsePopupShowing()) {
        DismissCannedResponsePopup();
    }

    if (IsCustomMessagePopupShowing()) {
        DismissCustomMessagePopup();
    }
}

Boolean CAnswerFragment::HasPendingDialogs()
{
    return !(mCannedResponsePopup == NULL && mCustomMessagePopup == NULL);
}

void CAnswerFragment::ShowCustomMessageDialog()
{
    using Elastos::Droid::Dialer::R;

    // Create an alert dialog containing an EditText
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    AutoPtr<IContext> context;
    builder->GetContext((IContext**)&context);
    AutoPtr<IEditText> et;
    CEditText::New(context, (IEditText**)&et);
    builder->SetCancelable(TRUE);
    builder->SetView(IView::Probe(et));
    builder->SetPositiveButton(R::string::custom_message_send,
            new PositiveButtonClickListener(et, this));
    builder->SetNegativeButton(R::string::custom_message_cancel,
            new NegativeButtonClickListener(this));
    builder->SetTitle(R::string::respond_via_sms_custom_message);
    mCustomMessagePopup = NULL;
    builder->Create((IAlertDialog**)&mCustomMessagePopup);

    // Enable/disable the send button based on whether there is a message in the EditText
    ITextView::Probe(et)->AddTextChangedListener(new MyTextWatcher(this));

    // Keyboard up, show the dialog
    AutoPtr<IWindow> window;
    IDialog::Probe(mCustomMessagePopup)->GetWindow((IWindow**)&window);
    window->SetSoftInputMode(
            IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_VISIBLE);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED);
    IDialog::Probe(mCustomMessagePopup)->Show();

    // Send button starts out disabled
    AutoPtr<IButton> sendButton;
    mCustomMessagePopup->GetButton(IDialogInterface::BUTTON_POSITIVE, (IButton**)&sendButton);
    IView::Probe(sendButton)->SetEnabled(FALSE);
}

ECode CAnswerFragment::ConfigureMessageDialog(
    /* [in] */ IList* textResponses)
{
    mSmsResponses->Clear();
    mSmsResponses->AddAll(ICollection::Probe(textResponses));
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String str;
    res->GetString(
            Elastos::Droid::Dialer::R::string::respond_via_sms_custom_message, &str);
    mSmsResponses->Add(CoreUtils::Convert(str));
    if (mSmsResponsesAdapter != NULL) {
        IBaseAdapter::Probe(mSmsResponsesAdapter)->NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode CAnswerFragment::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    *context = IContext::Probe(activity);
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CAnswerFragment::OnAnswer(
    /* [in] */ Int32 videoState,
    /* [in] */ IContext* context)
{
    AnswerPresenter* present = (AnswerPresenter*)GetPresenter().Get();
    present->OnAnswer(videoState, context);
    return NOERROR;
}

ECode CAnswerFragment::OnDecline()
{
    AnswerPresenter* present = (AnswerPresenter*)GetPresenter().Get();
    present->OnDecline();
    return NOERROR;
}

ECode CAnswerFragment::OnText()
{
    AnswerPresenter* present = (AnswerPresenter*)GetPresenter().Get();
    present->OnText();
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
