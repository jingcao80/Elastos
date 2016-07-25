
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CCallButtonFragment.h"
#include "elastos/droid/incallui/CallButtonPresenter.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::Telecomm::Telecom::IAudioState;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Dialer::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL(CCallButtonFragment::OverflowPopupOnMenuItemClickListener, Object, IPopupMenuOnMenuItemClickListener);

ECode CCallButtonFragment::OverflowPopupOnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case R::id::overflow_merge_menu_item: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)mHost->GetPresenter().Get();
            presenter->MergeClicked();
            break;
        }
        case R::id::overflow_add_menu_item: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)mHost->GetPresenter().Get();
            presenter->AddCallClicked();
            break;
        }
        case R::id::overflow_hold_menu_item: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)mHost->GetPresenter().Get();
            presenter->HoldClicked(TRUE /* checked */);
            break;
        }
        case R::id::overflow_resume_menu_item: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)mHost->GetPresenter().Get();
            presenter->HoldClicked(FALSE /* checked */);
            break;
        }
        case R::id::overflow_swap_menu_item: {
            CallButtonPresenter* presenter = (CallButtonPresenter*)mHost->GetPresenter().Get();
            presenter->AddCallClicked();
            break;
        }
        default:
            Logger::E("CCallButtonFragment", "onMenuItemClick: unexpected overflow menu click");
            break;
    }
    *result = TRUE;
    return NOERROR;
}


CAR_INTERFACE_IMPL(CCallButtonFragment::OverflowPopupOnDismissListener, Object, IPopupMenuOnDismissListener);

ECode CCallButtonFragment::OverflowPopupOnDismissListener::OnDismiss(
    /* [in] */ IPopupMenu* popupMenu)
{
    popupMenu->Dismiss();
    return NOERROR;
}


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
        MaybeSendAccessibilityEvent(IView::Probe(mMuteButton), value ? R::string::accessibility_call_muted
                : R::string::accessibility_call_unmuted);
    }
    return NOERROR;
}

ECode CCallButtonFragment::ShowAudioButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mAudioButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ShowChangeToVoiceButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mChangeToVoiceButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::EnableMute(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mMuteButton)->SetEnabled(enabled);
    return NOERROR;
}

ECode CCallButtonFragment::ShowDialpadButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mShowDialpadButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::SetHold(
    /* [in] */ Boolean value)
{
    IView* v = IView::Probe(mHoldButton);
    Boolean selected;
    if (v->IsSelected(&selected), selected != value) {
        v->SetSelected(value);
        MaybeSendAccessibilityEvent(v,
                value ? R::string::accessibility_call_put_on_hold :
                        R::string::accessibility_call_removed_from_hold);
    }
    return NOERROR;
}

ECode CCallButtonFragment::ShowHoldButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mHoldButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::EnableHold(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mHoldButton)->SetEnabled(enabled);
    return NOERROR;
}

ECode CCallButtonFragment::ShowSwapButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mSwapButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ShowChangeToVideoButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mChangeToVideoButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ShowSwitchCameraButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mSwitchCameraButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::SetSwitchCameraButton(
    /* [in] */ Boolean isBackFacingCamera)
{
    IView::Probe(mSwitchCameraButton)->SetSelected(isBackFacingCamera);
    return NOERROR;
}

ECode CCallButtonFragment::ShowAddCallButton(
    /* [in] */ Boolean show)
{
    Logger::D("CCallButtonFragment", "show Add call button: %d", show);
    IView::Probe(mAddCallButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ShowMergeButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mMergeButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ShowPauseVideoButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mPauseVideoButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::SetPauseVideoButton(
    /* [in] */ Boolean isPaused)
{
    IView::Probe(mPauseVideoButton)->SetSelected(isPaused);
    return NOERROR;
}

ECode CCallButtonFragment::ShowOverflowButton(
    /* [in] */ Boolean show)
{
    IView::Probe(mOverflowButton)->SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallButtonFragment::ConfigureOverflowMenu(
    /* [in] */ Boolean showMergeMenuOption,
    /* [in] */ Boolean showAddMenuOption,
    /* [in] */ Boolean showHoldMenuOption,
    /* [in] */ Boolean showSwapMenuOption)
{
    if (mOverflowPopup == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContext> contextWrapper;
        CContextThemeWrapper::New(IContext::Probe(activity),
                R::style::InCallPopupMenuStyle, (IContext**)&contextWrapper);
        CPopupMenu::New(contextWrapper, IView::Probe(mOverflowButton), (IPopupMenu**)&mOverflowPopup);
        AutoPtr<IMenuInflater> inflater;
        mOverflowPopup->GetMenuInflater((IMenuInflater**)&inflater);
        AutoPtr<IMenu> menu;
        mOverflowPopup->GetMenu((IMenu**)&menu);
        inflater->Inflate(R::menu::incall_overflow_menu, menu);
        mOverflowPopup->SetOnMenuItemClickListener(new OverflowPopupOnMenuItemClickListener(this));
        mOverflowPopup->SetOnDismissListener(new OverflowPopupOnDismissListener(this));
    }

    AutoPtr<IMenu> menu;
    mOverflowPopup->GetMenu((IMenu**)&menu);
    AutoPtr<IMenuItem> item;
    menu->FindItem(R::id::overflow_merge_menu_item, (IMenuItem**)&item);
    item->SetVisible(showMergeMenuOption);
    item = NULL;
    menu->FindItem(R::id::overflow_add_menu_item, (IMenuItem**)&item);
    item->SetVisible(showAddMenuOption);
    item = NULL;
    menu->FindItem(R::id::overflow_hold_menu_item, (IMenuItem**)&item);
    Boolean selected;
    IView::Probe(mHoldButton)->IsSelected(&selected);
    item->SetVisible(showHoldMenuOption && !selected);
    item = NULL;
    menu->FindItem(R::id::overflow_resume_menu_item, (IMenuItem**)&item);
    item->SetVisible(showHoldMenuOption && selected);
    item = NULL;
    menu->FindItem(R::id::overflow_swap_menu_item, (IMenuItem**)&item);
    item->SetVisible(showSwapMenuOption);

    Boolean hasItems;
    menu->HasVisibleItems(&hasItems);
    IView::Probe(mOverflowButton)->SetEnabled(hasItems);
    return NOERROR;
}

ECode CCallButtonFragment::SetAudio(
    /* [in] */ Int32 mode)
{
    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    UpdateAudioButtons(presenter->GetSupportedAudio());
    RefreshAudioModePopup();

    if (mPrevAudioMode != mode) {
        if (mPrevAudioMode != 0) {
            Int32 stringId = 0;
            switch (mode) {
                case IAudioState::ROUTE_EARPIECE:
                    stringId = R::string::accessibility_earpiece_selected;
                    break;
                case IAudioState::ROUTE_BLUETOOTH:
                    stringId = R::string::accessibility_bluetooth_headset_selected;
                    break;
                case IAudioState::ROUTE_WIRED_HEADSET:
                    stringId = R::string::accessibility_wired_headset_selected;
                    break;
                case IAudioState::ROUTE_SPEAKER:
                    stringId = R::string::accessibility_speakerphone_selected;
                    break;
            }
            if (stringId != 0) {
                MaybeSendAccessibilityEvent(IView::Probe(mAudioButton), stringId);
            }
        }
        mPrevAudioMode = mode;
    }
    return NOERROR;
}

ECode CCallButtonFragment::SetSupportedAudio(
    /* [in] */ Int32 modeMask)
{
    UpdateAudioButtons(modeMask);
    RefreshAudioModePopup();
    return NOERROR;
}

ECode CCallButtonFragment::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    item->GetItemId(&id);
    AutoPtr<ICharSequence> title;
    item->GetTitle((ICharSequence**)&title);
    Logger::D("CCallButtonFragment", "- onMenuItemClick: %s", TO_CSTR(item));
    Logger::D("CCallButtonFragment", "  id: %d", id);
    Logger::D("CCallButtonFragment", "  title: '%s'", CoreUtils::Unbox(title).string());

    Int32 mode = IAudioState::ROUTE_WIRED_OR_EARPIECE;

    switch (id) {
        case R::id::audio_mode_speaker:
            mode = IAudioState::ROUTE_SPEAKER;
            break;
        case R::id::audio_mode_earpiece:
        case R::id::audio_mode_wired_headset:
            // InCallAudioState.ROUTE_EARPIECE means either the handset earpiece,
            // or the wired headset (if connected.)
            mode = IAudioState::ROUTE_WIRED_OR_EARPIECE;
            break;
        case R::id::audio_mode_bluetooth:
            mode = IAudioState::ROUTE_BLUETOOTH;
            break;
        default:
            Logger::E("CCallButtonFragment", "onMenuItemClick:  unexpected View ID %d (MenuItem = '%s')",
                    id, TO_CSTR(item));
            break;
    }

    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    presenter->SetAudioMode(mode);

    *result = TRUE;
    return NOERROR;
}

ECode CCallButtonFragment::OnDismiss(
    /* [in] */ IPopupMenu* menu)
{
    Logger::D("CCallButtonFragment", "- onDismiss: %s", TO_CSTR(menu));
    mAudioModePopupVisible = FALSE;
    return NOERROR;
}

void CCallButtonFragment::OnAudioButtonClicked()
{
    // Log.d(this, "onAudioButtonClicked: " +
    //         AudioState.audioRouteToString(getPresenter().getSupportedAudio()));

    if (IsSupported(IAudioState::ROUTE_BLUETOOTH)) {
        ShowAudioModePopup();
    }
    else {
        CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
        presenter->ToggleSpeakerphone();
    }
}

void CCallButtonFragment::RefreshAudioModePopup()
{
    if (mAudioModePopup != NULL && mAudioModePopupVisible) {
        // Dismiss the previous one
        mAudioModePopup->Dismiss();  // safe even if already dismissed
        // And bring up a fresh PopupMenu
        ShowAudioModePopup();
    }
}

void CCallButtonFragment::UpdateAudioButtons(
    /* [in] */ Int32 supportedModes)
{
    Boolean bluetoothSupported = IsSupported(IAudioState::ROUTE_BLUETOOTH);
    Boolean speakerSupported = IsSupported(IAudioState::ROUTE_SPEAKER);

    Boolean audioButtonEnabled = FALSE;
    Boolean audioButtonChecked = FALSE;
    Boolean showMoreIndicator = FALSE;

    Boolean showBluetoothIcon = FALSE;
    Boolean showSpeakerphoneIcon = FALSE;
    Boolean showHandsetIcon = FALSE;

    Boolean showToggleIndicator = FALSE;

    if (bluetoothSupported) {
        Logger::D("CCallButtonFragment", "updateAudioButtons - popup menu mode");

        audioButtonEnabled = TRUE;
        showMoreIndicator = TRUE;
        // The audio button is NOT a toggle in this state.  (And its
        // setChecked() state is irrelevant since we completely hide the
        // btn_compound_background layer anyway.)

        // Update desired layers:
        if (IsAudio(IAudioState::ROUTE_BLUETOOTH)) {
            showBluetoothIcon = TRUE;
        }
        else if (IsAudio(IAudioState::ROUTE_SPEAKER)) {
            showSpeakerphoneIcon = TRUE;
        }
        else {
            showHandsetIcon = TRUE;
            // TODO: if a wired headset is plugged in, that takes precedence
            // over the handset earpiece.  If so, maybe we should show some
            // sort of "wired headset" icon here instead of the "handset
            // earpiece" icon.  (Still need an asset for that, though.)
        }
    }
    else if (speakerSupported) {
        Logger::D("CCallButtonFragment", "updateAudioButtons - speaker toggle mode");

        audioButtonEnabled = TRUE;

        // The audio button *is* a toggle in this state, and indicated the
        // current state of the speakerphone.
        audioButtonChecked = IsAudio(IAudioState::ROUTE_SPEAKER);

        // update desired layers:
        showToggleIndicator = TRUE;
        showSpeakerphoneIcon = TRUE;
    }
    else {
        Logger::D("CCallButtonFragment", "updateAudioButtons - disabled...");

        // The audio button is a toggle in this state, but that's mostly
        // irrelevant since it's always disabled and unchecked.
        audioButtonEnabled = FALSE;
        audioButtonChecked = FALSE;

        // update desired layers:
        showToggleIndicator = TRUE;
        showSpeakerphoneIcon = TRUE;
    }

    // Finally, update it all!

    Logger::V("CCallButtonFragment", "audioButtonEnabled: %d", audioButtonEnabled);
    Logger::V("CCallButtonFragment", "audioButtonChecked: %d", audioButtonChecked);
    Logger::V("CCallButtonFragment", "showMoreIndicator: %d", showMoreIndicator);
    Logger::V("CCallButtonFragment", "showBluetoothIcon: %d", showBluetoothIcon);
    Logger::V("CCallButtonFragment", "showSpeakerphoneIcon: %d", showSpeakerphoneIcon);
    Logger::V("CCallButtonFragment", "showHandsetIcon: %d", showHandsetIcon);

    // Only enable the audio button if the fragment is enabled.
    IView::Probe(mAudioButton)->SetEnabled(audioButtonEnabled && mIsEnabled);
    IView::Probe(mAudioButton)->SetSelected(audioButtonChecked);

    AutoPtr<IDrawable> background;
    IView::Probe(mAudioButton)->GetBackground((IDrawable**)&background);
    ILayerDrawable* layers = ILayerDrawable::Probe(background);
    Logger::D("CCallButtonFragment", "'layers' drawable: %s", TO_CSTR(layers));

    AutoPtr<IDrawable> drawable;
    layers->FindDrawableByLayerId(R::id::compoundBackgroundItem, (IDrawable**)&drawable);
    drawable->SetAlpha(showToggleIndicator ? VISIBLE : HIDDEN);

    drawable = NULL;
    layers->FindDrawableByLayerId(R::id::moreIndicatorItem, (IDrawable**)&drawable);
    drawable->SetAlpha(showMoreIndicator ? VISIBLE : HIDDEN);

    drawable = NULL;
    layers->FindDrawableByLayerId(R::id::bluetoothItem, (IDrawable**)&drawable);
    drawable->SetAlpha(showBluetoothIcon ? VISIBLE : HIDDEN);

    drawable = NULL;
    layers->FindDrawableByLayerId(R::id::handsetItem, (IDrawable**)&drawable);
    drawable->SetAlpha(showHandsetIcon ? VISIBLE : HIDDEN);

    drawable = NULL;
    layers->FindDrawableByLayerId(R::id::speakerphoneItem, (IDrawable**)&drawable);
    drawable->SetAlpha(showSpeakerphoneIcon ? VISIBLE : HIDDEN);
}

void CCallButtonFragment::ShowAudioModePopup()
{
    Logger::D("CCallButtonFragment", "showAudioPopup()...");

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContext> contextWrapper;
    CContextThemeWrapper::New(IContext::Probe(activity),
            R::style::InCallPopupMenuStyle, (IContext**)&contextWrapper);
    mAudioModePopup = NULL;
    CPopupMenu::New(contextWrapper, IView::Probe(mAudioButton) /* anchorView */, (IPopupMenu**)&mAudioModePopup);
    AutoPtr<IMenuInflater> inflater;
    mAudioModePopup->GetMenuInflater((IMenuInflater**)&inflater);
    AutoPtr<IMenu> menu;
    mAudioModePopup->GetMenu((IMenu**)&menu);
    inflater->Inflate(R::menu::incall_audio_mode_menu, menu);
    mAudioModePopup->SetOnMenuItemClickListener(this);
    mAudioModePopup->SetOnDismissListener(this);

    // TODO: Still need to have the "currently active" audio mode come
    // up pre-selected (or focused?) with a blue highlight.  Still
    // need exact visual design, and possibly framework support for this.
    // See comments below for the exact logic.

    AutoPtr<IMenuItem> speakerItem;
    menu->FindItem(R::id::audio_mode_speaker, (IMenuItem**)&speakerItem);
    speakerItem->SetEnabled(IsSupported(IAudioState::ROUTE_SPEAKER));
    // TODO: Show speakerItem as initially "selected" if
    // speaker is on.

    // We display *either* "earpiece" or "wired headset", never both,
    // depending on whether a wired headset is physically plugged in.
    AutoPtr<IMenuItem> earpieceItem;
    menu->FindItem(R::id::audio_mode_earpiece, (IMenuItem**)&earpieceItem);
    AutoPtr<IMenuItem> wiredHeadsetItem;
    menu->FindItem(R::id::audio_mode_wired_headset, (IMenuItem**)&wiredHeadsetItem);

    Boolean usingHeadset = IsSupported(IAudioState::ROUTE_WIRED_HEADSET);
    earpieceItem->SetVisible(!usingHeadset);
    earpieceItem->SetEnabled(!usingHeadset);
    wiredHeadsetItem->SetVisible(usingHeadset);
    wiredHeadsetItem->SetEnabled(usingHeadset);
    // TODO: Show the above item (either earpieceItem or wiredHeadsetItem)
    // as initially "selected" if speakerOn and
    // bluetoothIndicatorOn are both false.

    AutoPtr<IMenuItem> bluetoothItem;
    menu->FindItem(R::id::audio_mode_bluetooth, (IMenuItem**)&bluetoothItem);
    bluetoothItem->SetEnabled(IsSupported(IAudioState::ROUTE_BLUETOOTH));
    // TODO: Show bluetoothItem as initially "selected" if
    // bluetoothIndicatorOn is true.

    mAudioModePopup->Show();

    // Unfortunately we need to manually keep track of the popup menu's
    // visiblity, since PopupMenu doesn't have an isShowing() method like
    // Dialogs do.
    mAudioModePopupVisible = TRUE;
}

Boolean CCallButtonFragment::IsSupported(
    /* [in] */ Int32 mode)
{
    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    return (mode == (presenter->GetSupportedAudio() & mode));
}

Boolean CCallButtonFragment::IsAudio(
    /* [in] */ Int32 mode)
{
    CallButtonPresenter* presenter = (CallButtonPresenter*)GetPresenter().Get();
    return (mode == presenter->GetAudioMode());
}

ECode CCallButtonFragment::DisplayDialpad(
    /* [in] */ Boolean value,
    /* [in] */ Boolean animate)
{
    IView::Probe(mShowDialpadButton)->SetSelected(value);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity != NULL && IInCallActivity::Probe(activity) != NULL) {
        ((CInCallActivity*)IInCallActivity::Probe(activity))->DisplayDialpad(value, animate);
    }
    return NOERROR;
}

ECode CCallButtonFragment::IsDialpadVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity != NULL && IInCallActivity::Probe(activity) != NULL) {
        *visible = ((CInCallActivity*)IInCallActivity::Probe(activity))->IsDialpadVisible();
        return NOERROR;
    }
    *visible = FALSE;
    return NOERROR;
}

ECode CCallButtonFragment::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    *context = IContext::Probe(activity);
    REFCOUNT_ADD(*context)
    return NOERROR;
}

void CCallButtonFragment::MaybeSendAccessibilityEvent(
    /* [in] */ IView* view,
    /* [in] */ Int32 stringId)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    IAccessibilityManager* manager = IAccessibilityManager::Probe(obj);
    Boolean enabled;
    if (manager != NULL && (manager->IsEnabled(&enabled), enabled)) {
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> e;
        helper->Obtain((IAccessibilityEvent**)&e);
        IAccessibilityRecord* er = IAccessibilityRecord::Probe(e);
        er->SetSource(view);
        e->SetEventType(IAccessibilityEvent::TYPE_ANNOUNCEMENT);
        er->SetClassName(CoreUtils::Convert(String("CCallButtonFragment")));
        String pkgName;
        context->GetPackageName(&pkgName);
        e->SetPackageName(CoreUtils::Convert(pkgName));
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        String str;
        res->GetString(stringId, &str);
        AutoPtr<IList> text;
        er->GetText((IList**)&text);
        text->Add(CoreUtils::Convert(str));
        manager->SendAccessibilityEvent(e);
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
