
#include "elastos/droid/incallui/CCallButtonFragment.h"
#include "elastos/droid/incallui/CallButtonPresenter.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Dialer::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const Int32 CCallButtonFragment::HIDDEN = 0;
const Int32 CCallButtonFragment::VISIBLE = 255;

CAR_INTERFACE_IMPL_7(CCallButtonFragment, BaseFragment, IPopupMenuOnMenuItemClickListener, IPopupMenuOnDismissListener,
        IViewOnClickListener, ICompoundButtonOnCheckedChangeListener, IUi, ICallButtonUi, ICallButtonFragment);

CAR_OBJECT_IMPL(CCallButtonFragment);

CCallButtonFragment::CCallButtonFragment()
    : mAudioModePopupVisible(FALSE)
    , mPrevAudioMode(0)
    , mIsEnabled(FALSE)
{}

ECode CCallButtonFragment::constructor()
{
    return NOERROR;
}

AutoPtr<IPresenter> CCallButtonFragment::CreatePresenter()
{
    // TODO: find a cleaner way to include audio mode provider than having a singleton instance.
    AutoPtr<IPresenter> presenter = new CallButtonPresenter();
    return presenter;
}

AutoPtr<IUi> CCallButtonFragment::GetUi()
{
    return this;
}

ECode CCallButtonFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    return BaseFragment::OnCreate(savedInstanceState);
}

ECode CCallButtonFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> parent;
    inflater->Inflate(R::layout::call_button_fragment, container, FALSE, (IView**)&parent);

    AutoPtr<IView> v;
    parent->FindViewById(R::id::audioButton, (IView**)&v);
    mAudioButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::changeToVoiceButton, (IView**)&v);
    mChangeToVoiceButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::muteButton, (IView**)&v);
    mMuteButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::dialpadButton, (IView**)&v);
    mShowDialpadButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::holdButton, (IView**)&v);
    mHoldButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::swapButton, (IView**)&v);
    mSwapButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::changeToVideoButton, (IView**)&v);
    mChangeToVideoButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::switchCameraButton, (IView**)&v);
    mSwitchCameraButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::addButton, (IView**)&v);
    mAddCallButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::mergeButton, (IView**)&v);
    mMergeButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::pauseVideoButton, (IView**)&v);
    mPauseVideoButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);
    v = NULL;
    parent->FindViewById(R::id::overflowButton, (IView**)&v);
    mOverflowButton = IImageButton::Probe(v);
    v->SetOnClickListener(this);

    *view = parent;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CCallButtonFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    BaseFragment::OnActivityCreated(savedInstanceState);

    // set the buttons
    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    UpdateAudioButtons(presenter->GetSupportedAudio());
    return NOERROR;
}

ECode CCallButtonFragment::OnResume()
{
    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    if (presenter != NULL) {
        presenter->RefreshMuteState();
    }
    BaseFragment::OnResume();
    return NOERROR;
}

ECode CCallButtonFragment::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return NOERROR;
}

ECode CCallButtonFragment::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    Logger::D("CCallButtonFragment", "onClick(View %s, id %d)...", TO_CSTR(view), id);

    switch(id) {
        case R::id::audioButton:
            OnAudioButtonClicked();
            break;
        case R::id::addButton: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->AddCallClicked();
            break;
        }
        case R::id::changeToVoiceButton: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->ChangeToVoiceClicked();
            break;
        }
        case R::id::muteButton: {
            Boolean selected;
            view->IsSelected(&selected);
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->MuteClicked(!selected);
            break;
        }
        case R::id::mergeButton: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->MergeClicked();
            break;
        }
        case R::id::holdButton: {
            Boolean selected;
            view->IsSelected(&selected);
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->HoldClicked(!selected);
            break;
        }
        case R::id::swapButton: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->SwapClicked();
            break;
        }
        case R::id::dialpadButton: {
            Boolean selected;
            IView::Probe(mShowDialpadButton)->IsSelected(&selected);
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->ShowDialpadClicked(!selected);
            break;
        }
        case R::id::changeToVideoButton: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->ChangeToVideoClicked();
            break;
        }
        case R::id::switchCameraButton: {
            Boolean selected;
            IView::Probe(mSwitchCameraButton)->IsSelected(&selected);
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->SwitchCameraClicked(
                    selected /* useFrontFacingCamera */);
            break;
        }
        case R::id::pauseVideoButton: {
            Boolean selected;
            IView::Probe(mPauseVideoButton)->IsSelected(&selected);
            CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
            presenter->PauseVideoClicked(
                    !selected /* pause */);
            break;
        }
        case R::id::overflowButton:
            mOverflowPopup->Show();
            break;
        default:
            Logger::E("CCallButtonFragment", "onClick: unexpected");
            break;
    }
    return NOERROR;
}

ECode CCallButtonFragment::SetEnabled(
    /* [in] */ Boolean isEnabled)
{
    mIsEnabled = isEnabled;
    AutoPtr<IView> view;
    GetView((IView**)&view);
    Int32 visibility;
    view->GetVisibility(&visibility);
    if (visibility != IView::VISIBLE) {
        view->SetVisibility(IView::VISIBLE);
    }

    IView::Probe(mAudioButton)->SetEnabled(isEnabled);
    IView::Probe(mChangeToVoiceButton)->SetEnabled(isEnabled);
    IView::Probe(mMuteButton)->SetEnabled(isEnabled);
    IView::Probe(mShowDialpadButton)->SetEnabled(isEnabled);
    IView::Probe(mHoldButton)->SetEnabled(isEnabled);
    IView::Probe(mSwapButton)->SetEnabled(isEnabled);
    IView::Probe(mChangeToVideoButton)->SetEnabled(isEnabled);
    IView::Probe(mSwitchCameraButton)->SetEnabled(isEnabled);
    IView::Probe(mAddCallButton)->SetEnabled(isEnabled);
    IView::Probe(mMergeButton)->SetEnabled(isEnabled);
    IView::Probe(mPauseVideoButton)->SetEnabled(isEnabled);
    IView::Probe(mOverflowButton)->SetEnabled(isEnabled);
    return NOERROR;
}

ECode CCallButtonFragment::SetMute(
    /* [in] */ Boolean value)
{
    Boolean selected;
    IView::Probe(mMuteButton)->IsSelected(&selected);
    if (selected != value) {
        IView::Probe(mMuteButton)->SetSelected(value);
        MaybeSendAccessibilityEvent(mMuteButton, value ? R::string::accessibility_call_muted
                : R::string::accessibility_call_unmuted);
    }
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
