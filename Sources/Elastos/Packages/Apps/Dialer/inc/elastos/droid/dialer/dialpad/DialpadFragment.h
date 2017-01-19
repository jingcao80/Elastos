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

#ifndef __ELASTOS_DROID_DIALER_DIALPAD_DIALPADFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_DIALPADFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/app/DialogFragment.h>
#include <elastos/droid/app/Fragment.h>
#include <elastos/droid/telephony/PhoneStateListener.h>
#include <elastos/droid/widget/BaseAdapter.h>
#include <elastos/droid/widget/PopupMenu.h>
#include <elastos/droid/widget/RelativeLayout.h>
#include "elastos/droid/contacts/common/widget/FloatingActionButtonController.h"
#include "elastos/droid/phone/common/dialpad/CDialpadView.h"
#include <elastos/droid/dialerbind/analytics/AnalyticsFragment.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::PopupMenu;
using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IHashSet;
using Elastos::Droid::Contacts::Common::Widget::FloatingActionButtonController;
using Elastos::Droid::DialerBind::Analytics::AnalyticsFragment;
using Elastos::Droid::Phone::Common::IHapticFeedback;
using Elastos::Droid::Phone::Common::Dialpad::CDialpadView;
using Elastos::Droid::Phone::Common::Dialpad::IDialpadKeyButtonOnPressedListener;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

/**
 * Fragment that displays a twelve-key phone dialpad.
 */
class DialpadFragment
    : public AnalyticsFragment
    , public IDialpadFragment
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnLongClickListener
        , public IViewOnKeyListener
        , public IAdapterViewOnItemClickListener
        , public IPopupMenuOnMenuItemClickListener
        , public ITextWatcher
        , public IDialpadKeyButtonOnPressedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ DialpadFragment* host);

        virtual CARAPI OnKey(
            /* [in] */ IView* view,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        virtual CARAPI OnPressed(
            /* [in] */ IView* view,
            /* [in] */ Boolean pressed);

        virtual CARAPI OnClick(
            /* [in] */ IView* view);

        virtual CARAPI OnLongClick(
            /* [in] */ IView* view,
            /* [out] */ Boolean* result);

        virtual CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        virtual CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        virtual CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        // @Override
        virtual CARAPI OnTextChanged(
            /* [in] */ ICharSequence* input,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 changeCount);

        // @Override
        virtual CARAPI AfterTextChanged(
            /* [in] */ IEditable* input);

    private:
        DialpadFragment* mHost;
    };

    /**
     * LinearLayout with getter and setter methods for the translationY property using floats,
     * for animation purposes.
     */
    class DialpadSlidingRelativeLayout
        : public RelativeLayout
        , public IDialpadSlidingRelativeLayout
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IContext* context);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyle);

        // @NeededForReflection
        CARAPI GetYFraction(
            /* [out] */ Float* result);

        // @NeededForReflection
        CARAPI SetYFraction(
            /* [in] */ Float yFraction);
    };

    class ErrorDialogFragment
        : public DialogFragment
    {
    public:
        static CARAPI_(AutoPtr<ErrorDialogFragment>) NewInstance(
            /* [in] */ Int32 messageResId);

        static CARAPI_(AutoPtr<ErrorDialogFragment>) NewInstance(
            /* [in] */ Int32 titleResId,
            /* [in] */ Int32 messageResId);

        // @Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

        // @Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** dialog);

    private:
        Int32 mTitleResId;
        Int32 mMessageResId;

        static const String ARG_TITLE_RES_ID;
        static const String ARG_MESSAGE_RES_ID;
    };

private:
    class DialpadPhoneStateListener
        : public PhoneStateListener
    {
    public:
        DialpadPhoneStateListener()
        {}

        CARAPI constructor(
            /* [in] */ DialpadFragment* host);

        /**
         * Listen for phone state changes so that we can take down the
         * "dialpad chooser" if the phone becomes idle while the
         * chooser UI is visible.
         */
        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

    private:
        DialpadFragment* mHost;
    };

    /**
     * Simple list adapter, binding to an icon + text label
     * for each item in the "dialpad chooser" list.
     */
    class DialpadChooserAdapter
        : public BaseAdapter
    {
    public:
        // Simple struct for a single "choice" item.
        class ChoiceItem
            : public Object
        {
        public:
            ChoiceItem(
                /* [in] */ const String& s,
                /* [in] */ IBitmap* b,
                /* [in] */ Int32 i);

        public:
            String mText;
            AutoPtr<IBitmap> mIcon;
            Int32 mId;
        };

    public:
        DialpadChooserAdapter(
            /* [in] */ IContext* context);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    public:
        // IDs for the possible "choices":
        static const Int32 DIALPAD_CHOICE_USE_DTMF_DIALPAD;
        static const Int32 DIALPAD_CHOICE_RETURN_TO_CALL;
        static const Int32 DIALPAD_CHOICE_ADD_NEW_CALL;

    private:
        static const Int32 NUM_ITEMS;
        AutoPtr<ArrayOf<ChoiceItem*> > mChoiceItems;
        AutoPtr<ILayoutInflater> mInflater;
    };

    class AlertDialogClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    };

    class SpacerOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL();

        SpacerOnTouchListener(
            /* [in] */ DialpadFragment* host);

        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [in] */ Boolean* result);

    private:
        DialpadFragment* mHost;
    };

    class DialpadPopupMenu
        : public PopupMenu
    {
    public:
        DialpadPopupMenu(
             /* [in] */ DialpadFragment* host);

        CARAPI Show();

    private:
        DialpadFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DialpadFragment();

    CARAPI constructor();

    // @Override
    virtual CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    // @Override
    virtual CARAPI OnTextChanged(
        /* [in] */ ICharSequence* input,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 changeCount);

    // @Override
    virtual CARAPI AfterTextChanged(
        /* [in] */ IEditable* input);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* state);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedState,
        /* [out] */ IView** view);

    CARAPI GetDigitsWidget(
        /* [out] */ IEditText** text);

    CARAPI SetStartedFromNewIntent(
        /* [in] */ Boolean value);

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    // @Override
    virtual CARAPI OnKey(
        /* [in] */ IView* view,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * When a key is pressed, we start playing DTMF tone, do vibration, and enter the digit
     * immediately. When a key is released, we stop the tone. Note that the "key press" event will
     * be delivered by the system with certain amount of delay, it won't be synced with user's
     * actual "touch-down" behavior.
     */
    // @Override
    virtual CARAPI OnPressed(
        /* [in] */ IView* view,
        /* [in] */ Boolean pressed);

    // @Override
    virtual CARAPI OnClick(
        /* [in] */ IView* view);

    // @Override
    virtual CARAPI OnLongClick(
        /* [in] */ IView* view,
        /* [out] */ Boolean* result);

    CARAPI CallVoicemail();

    CARAPI ClearDialpad();

    /**
     * Handle clicks from the dialpad chooser.
     */
    // @Override
    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    /**
     * @return true if the phone is "in use", meaning that at least one line
     *              is active (ie. off hook or ringing or dialing, or on hold).
     */
    CARAPI IsPhoneInUse(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI OnMenuItemClick(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    /**
     * Returns true of the newDigit parameter can be added at the current selection
     * point, otherwise returns false.
     * Only prevents input of WAIT and PAUSE digits at an unsupported position.
     * Fails early if start == -1 or start is larger than end.
     */
    // @VisibleForTesting
    /* package */ static CARAPI CanAddDigit(
        /* [in] */ ICharSequence* digits,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Char32 newDigit,
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI OnHiddenChanged(
        /* [in] */ Boolean hidden);

    CARAPI SetAnimate(
        /* [in] */ Boolean value);

    CARAPI GetAnimate(
        /* [out] */ Boolean* value);

    CARAPI SetYFraction(
        /* [in] */ Float yFraction);

private:
    /**
     * Return an Intent for launching voicemail screen.
     */
    static CARAPI_(AutoPtr<IIntent>) GetVoicemailIntent();

    CARAPI_(AutoPtr<ITelephonyManager>) GetTelephonyManager();

    CARAPI_(AutoPtr<ITelecomManager>) GetTelecomManager();

    CARAPI_(Boolean) IsLayoutReady();

    /**
     * @return true when {@link #mDigits} is actually filled by the Intent.
     */
    CARAPI_(Boolean) FillDigitsIfNecessary(
        /* [in] */ IIntent* intent);

    /**
     * Determines whether an add call operation is requested.
     *
     * @param intent The intent.
     * @return {@literal true} if add call operation was requested.  {@literal false} otherwise.
     */
    static CARAPI_(Boolean) IsAddCallMode(
        /* [in] */ IIntent* intent);

    /**
     * Checks the given Intent and changes dialpad's UI state. For example, if the Intent requires
     * the screen to enter "Add Call" mode, this method will show correct UI for the mode.
     */
    CARAPI_(void) ConfigureScreenFromIntent(
        /* [in] */ IActivity* parent);

    /**
     * Sets formatted digits to digits field.
     */
    CARAPI_(void) SetFormattedDigits(
        /* [in] */ const String& data,
        /* [in] */ const String& normalizedNumber);

    CARAPI_(void) ConfigureKeypadListeners(
        /* [in] */ IView* fragmentView);

    CARAPI_(void) KeyPressed(
        /* [in] */ Int32 keyCode);

    /**
     * Called by the containing Activity to tell this Fragment to build an overflow options
     * menu for display by the container when appropriate.
     *
     * @param invoker the View that invoked the options menu, to act as an anchor location.
     */
    CARAPI_(AutoPtr<IPopupMenu>) BuildOptionsMenu(
        /* [in] */ IView* invoker);

    /**
     * Remove the digit just before the current position. This can be used if we want to replace
     * the previous digit or cancel previously entered character.
     */
    CARAPI_(void) RemovePreviousDigitIfPossible();

    CARAPI_(void) HideAndClearDialpad(
        /* [in] */ Boolean animate);

    /**
     * In most cases, when the dial button is pressed, there is a
     * number in digits area. Pack it in the intent, start the
     * outgoing call broadcast as a separate task and finish this
     * activity.
     *
     * When there is no digit and the phone is CDMA and off hook,
     * we're sending a blank flash for CDMA. CDMA networks use Flash
     * messages when special processing needs to be done, mainly for
     * 3-way or call waiting scenarios. Presumably, here we're in a
     * special 3-way scenario where the network needs a blank flash
     * before being able to add the new participant.  (This is not the
     * case with all 3-way calls, just certain CDMA infrastructures.)
     *
     * Otherwise, there is no digit, display the last dialed
     * number. Don't finish since the user may want to edit it. The
     * user needs to press the dial button again, to dial it (general
     * case described above).
     */
    CARAPI_(void) HandleDialButtonPressed();

    CARAPI_(void) HandleDialButtonClickWithEmptyDigits();

    /**
     * Plays the specified tone for TONE_LENGTH_MS milliseconds.
     */
    CARAPI_(void) PlayTone(
        /* [in] */ Int32 tone);

    /**
     * Play the specified tone for the specified milliseconds
     *
     * The tone is played locally, using the audio stream for phone calls.
     * Tones are played only if the "Audible touch tones" user preference
     * is checked, and are NOT played if the device is in silent mode.
     *
     * The tone length can be -1, meaning "keep playing the tone." If the caller does so, it should
     * call stopTone() afterward.
     *
     * @param tone a tone code from {@link ToneGenerator}
     * @param durationMs tone length.
     */
    CARAPI_(void) PlayTone(
        /* [in] */ Int32 tone,
        /* [in] */ Int32 durationMs);

    /**
     * Stop the tone if it is played.
     */
    CARAPI_(void) StopTone();

    /**
     * Brings up the "dialpad chooser" UI in place of the usual Dialer
     * elements (the textfield/button and the dialpad underneath).
     *
     * We show this UI if the user brings up the Dialer while a call is
     * already in progress, since there's a good chance we got here
     * accidentally (and the user really wanted the in-call dialpad instead).
     * So in this situation we display an intermediate UI that lets the user
     * explicitly choose between the in-call dialpad ("Use touch tone
     * keypad") and the regular Dialer ("Add call").  (Or, the option "Return
     * to call in progress" just goes back to the in-call UI with no dialpad
     * at all.)
     *
     * @param enabled If true, show the "dialpad chooser" instead
     *                of the regular Dialer UI
     */
    CARAPI_(void) ShowDialpadChooser(
        /* [in] */ Boolean enabled);

    /**
     * @return true if we're currently showing the "dialpad chooser" UI.
     */
    CARAPI_(Boolean) IsDialpadChooserVisible();

    /**
     * Returns to the in-call UI (where there's presumably a call in
     * progress) in response to the user selecting "use touch tone keypad"
     * or "return to call" from the dialpad chooser.
     */
    CARAPI_(void) ReturnToInCallScreen(
        /* [in] */ Boolean showDialpad);

    /**
     * @return true if the phone is a CDMA phone type
     */
    CARAPI_(Boolean) PhoneIsCdma();

    /**
     * Updates the dial string (mDigits) after inserting a Pause character (,)
     * or Wait character (;).
     */
    CARAPI UpdateDialString(
        /* [in] */ Char32 newDigit);

    /**
     * Update the enabledness of the "Dial" and "Backspace" buttons if applicable.
     */
    CARAPI_(void) UpdateDeleteButtonEnabledState();

    /**
     * Handle transitions for the menu button depending on the state of the digits edit text.
     * Transition out when going from digits to no digits and transition in when the first digit
     * is pressed.
     * @param transitionIn True if transitioning in, False if transitioning out
     */
    CARAPI_(void) UpdateMenuOverflowButton(
        /* [in] */ Boolean transitionIn);

    /**
     * Check if voicemail is enabled/accessible.
     *
     * @return true if voicemail is enabled and accessibly. Note that this can be false
     * "temporarily" after the app boot.
     * @see TelephonyManager#getVoiceMailNumber()
     */
    CARAPI_(Boolean) IsVoicemailAvailable();

    /**
     * @return true if the widget with the phone number digits is empty.
     */
    CARAPI_(Boolean) IsDigitsEmpty();

    /**
     * Starts the asyn query to get the last dialed/outgoing
     * number. When the background query finishes, mLastNumberDialed
     * is set to the last dialed number or an empty string if none
     * exists yet.
     */
    CARAPI_(void) QueryLastOutgoingCall();

    CARAPI_(AutoPtr<IIntent>) NewFlashIntent();

private:
    static const String TAG;

    static const Boolean DEBUG;

    // This is the amount of screen the dialpad fragment takes up when fully displayed
    static const Float DIALPAD_SLIDE_FRACTION;

    static const String EMPTY_NUMBER;
    static const Char32 PAUSE;
    static const Char32 WAIT;

    /** The length of DTMF tones in milliseconds */
    static const Int32 TONE_LENGTH_MS;
    static const Int32 TONE_LENGTH_INFINITE;

    /** The DTMF tone volume relative to other sounds in the stream */
    static const Int32 TONE_RELATIVE_VOLUME;

    /** Stream type used to play the DTMF tones off call, and mapped to the volume control keys */
    static const Int32 DIAL_TONE_STREAM_TYPE;

    AutoPtr<IOnDialpadQueryChangedListener> mDialpadQueryListener;

    AutoPtr<CDialpadView> mDialpadView;
    AutoPtr<IEditText> mDigits;
    Int32 mDialpadSlideInDuration;

    /** Remembers if we need to clear digits field when the screen is completely gone. */
    Boolean mClearDigitsOnStop;

    AutoPtr<IView> mOverflowMenuButton;
    AutoPtr<IPopupMenu> mOverflowPopupMenu;
    AutoPtr<IView> mDelete;
    AutoPtr<IToneGenerator> mToneGenerator;
    Object mToneGeneratorLock;
    AutoPtr<IView> mSpacer;

    AutoPtr<FloatingActionButtonController> mFloatingActionButtonController;

    /**
     * Set of dialpad keys that are currently being pressed
     */
    AutoPtr<IHashSet> mPressedDialpadKeys;

    AutoPtr<IListView> mDialpadChooser;
    AutoPtr<DialpadChooserAdapter> mDialpadChooserAdapter;

    /**
     * Regular expression prohibiting manual phone call. Can be empty, which means "no rule".
     */
    String mProhibitedPhoneNumberRegexp;


    // Last number dialed, retrieved asynchronously from the call DB
    // in onCreate. This number is displayed when the user hits the
    // send key and cleared in onPause.
    // AutoPtr<ICallLogAsync> mCallLog; // = new CallLogAsync();
    String mLastNumberDialed;

    // determines if we want to playback local DTMF tones.
    Boolean mDTMFToneEnabled;

    // Vibration (haptic feedback) for dialer key presses.
    AutoPtr<IHapticFeedback> mHaptic;

    /** Identifier for the "Add Call" intent extra. */
    static const String ADD_CALL_MODE_KEY;

    /**
     * Identifier for intent extra for sending an empty Flash message for
     * CDMA networks. This message is used by the network to simulate a
     * press/depress of the "hookswitch" of a landline phone. Aka "empty flash".
     *
     * TODO: Using an intent extra to tell the phone to send this flash is a
     * temporary measure. To be replaced with an Telephony/TelecomManager call in the future.
     * TODO: Keep in sync with the string defined in OutgoingCallBroadcaster.java
     * in Phone app until this is replaced with the Telephony/Telecom API.
     */
    static const String EXTRA_SEND_EMPTY_FLASH;

    String mCurrentCountryIso;

    AutoPtr<DialpadPhoneStateListener> mPhoneStateListener;

    Boolean mWasEmptyBeforeTextChange;

    /**
     * This field is set to true while processing an incoming DIAL intent, in order to make sure
     * that SpecialCharSequenceMgr actions can be triggered by user input but *not* by a
     * tel: URI passed by some other app.  It will be set to false when all digits are cleared.
     */
    Boolean mDigitsFilledByIntent;

    Boolean mStartedFromNewIntent;
    Boolean mFirstLaunch;
    Boolean mAnimate;

    AutoPtr<IComponentName> mSmsPackageComponentName;

    static const String PREF_DIGITS_FILLED_BY_INTENT;
    AutoPtr<InnerListener> mInnerListener;
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Dialer::Dialpad::DialpadFragment::DialpadChooserAdapter::ChoiceItem, IInterface);

#endif //__ELASTOS_DROID_DIALER_DIALPAD_DIALPADFRAGMENT_H__
