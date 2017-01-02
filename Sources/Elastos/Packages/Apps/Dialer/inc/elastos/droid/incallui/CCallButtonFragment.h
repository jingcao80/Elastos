//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INCALLUI_CCALLBUTTONFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_CCALLBUTTONFRAGMENT_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_InCallUI_CCallButtonFragment.h"
#include "elastos/droid/incallui/BaseFragment.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CCallButtonFragment)
    , public BaseFragment
    , public IViewOnClickListener
    , public ICompoundButtonOnCheckedChangeListener
    , public IUi
    , public ICallButtonUi
    , public ICallButtonFragment
{
private:
    class InnerListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerListener(
            /* [in] */ CCallButtonFragment* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        CCallButtonFragment* mHost;
    };

    class OverflowPopupMenuListener
        : public Object
        , public IPopupMenuOnMenuItemClickListener
        , public IPopupMenuOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL();

        OverflowPopupMenuListener(
            /* [in] */ CCallButtonFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IPopupMenu* popupMenu);

    private:
        CCallButtonFragment* mHost;
    };

    class AudioModePopupMenuListener
        : public Object
        , public IPopupMenuOnMenuItemClickListener
        , public IPopupMenuOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL();

        AudioModePopupMenuListener(
            /* [in] */ CCallButtonFragment* host);

        // @Override
        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IPopupMenu* popupMenu);

    private:
        CCallButtonFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCallButtonFragment();

    CARAPI constructor();

    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* view);

    // @Override
    CARAPI SetEnabled(
        /* [in] */ Boolean isEnabled);

    // @Override
    CARAPI SetMute(
        /* [in] */ Boolean value);

    // @Override
    CARAPI ShowAudioButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ShowChangeToVoiceButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI EnableMute(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI ShowDialpadButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI SetHold(
        /* [in] */ Boolean value);

    // @Override
    CARAPI ShowHoldButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI EnableHold(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI ShowSwapButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ShowChangeToVideoButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ShowSwitchCameraButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI SetSwitchCameraButton(
        /* [in] */ Boolean isBackFacingCamera);

    // @Override
    CARAPI ShowAddCallButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ShowMergeButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ShowPauseVideoButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI SetPauseVideoButton(
        /* [in] */ Boolean isPaused);

    // @Override
    CARAPI ShowOverflowButton(
        /* [in] */ Boolean show);

    // @Override
    CARAPI ConfigureOverflowMenu(
        /* [in] */ Boolean showMergeMenuOption,
        /* [in] */ Boolean showAddMenuOption,
        /* [in] */ Boolean showHoldMenuOption,
        /* [in] */ Boolean showSwapMenuOption);

    // @Override
    CARAPI SetAudio(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI SetSupportedAudio(
        /* [in] */ Int32 modeMask);

    // @Override
    CARAPI OnMenuItemClick(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // PopupMenu.OnDismissListener implementation; see showAudioModePopup().
    // This gets called when the PopupMenu gets dismissed for *any* reason, like
    // the user tapping outside its bounds, or pressing Back, or selecting one
    // of the menu items.
    // @Override
    CARAPI OnDismiss(
        /* [in] */ IPopupMenu* menu);

    /**
     * Refreshes the "Audio mode" popup if it's visible.  This is useful
     * (for example) when a wired headset is plugged or unplugged,
     * since we need to switch back and forth between the "earpiece"
     * and "wired headset" items.
     *
     * This is safe to call even if the popup is already dismissed, or even if
     * you never called showAudioModePopup() in the first place.
     */
    CARAPI_(void) RefreshAudioModePopup();

    // @Override
    CARAPI DisplayDialpad(
        /* [in] */ Boolean value,
        /* [in] */ Boolean animate);

    // @Override
    CARAPI IsDialpadVisible(
        /* [out] */ Boolean* visible);

    // @Override
    CARAPI GetContext(
        /* [out] */ IContext** context);

private:
    /**
     * Checks for supporting modes.  If bluetooth is supported, it uses the audio
     * pop up menu.  Otherwise, it toggles the speakerphone.
     */
    CARAPI_(void) OnAudioButtonClicked();

    /**
     * Updates the audio button so that the appriopriate visual layers
     * are visible based on the supported audio formats.
     */
    CARAPI_(void) UpdateAudioButtons(
        /* [in] */ Int32 supportedModes);

    CARAPI_(void) ShowAudioModePopup();

    CARAPI_(Boolean) IsSupported(
        /* [in] */ Int32 mode);

    CARAPI_(Boolean) IsAudio(
        /* [in] */ Int32 mode);

    CARAPI_(void) MaybeSendAccessibilityEvent(
        /* [in] */ IView* view,
        /* [in] */ Int32 stringId);

private:
    AutoPtr<IImageButton> mAudioButton;
    AutoPtr<IImageButton> mChangeToVoiceButton;
    AutoPtr<IImageButton> mMuteButton;
    AutoPtr<IImageButton> mShowDialpadButton;
    AutoPtr<IImageButton> mHoldButton;
    AutoPtr<IImageButton> mSwapButton;
    AutoPtr<IImageButton> mChangeToVideoButton;
    AutoPtr<IImageButton> mSwitchCameraButton;
    AutoPtr<IImageButton> mAddCallButton;
    AutoPtr<IImageButton> mMergeButton;
    AutoPtr<IImageButton> mPauseVideoButton;
    AutoPtr<IImageButton> mOverflowButton;

    AutoPtr<IPopupMenu> mAudioModePopup;
    Boolean mAudioModePopupVisible;
    AutoPtr<IPopupMenu> mOverflowPopup;

    Int32 mPrevAudioMode;

    // Constants for Drawable.setAlpha()
    static const Int32 HIDDEN;
    static const Int32 VISIBLE;

    Boolean mIsEnabled;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CCALLBUTTONFRAGMENT_H__
