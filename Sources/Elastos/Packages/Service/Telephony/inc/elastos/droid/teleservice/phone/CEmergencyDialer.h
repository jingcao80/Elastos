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

#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYDIALER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYDIALER_H__

#include "_Elastos_Droid_TeleService_Phone_CEmergencyDialer.h"
#include "Elastos.Droid.Phone.Common.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Media::IToneGenerator;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Phone::Common::IHapticFeedback;
using Elastos::Droid::Phone::Common::Dialpad::IDialpadKeyButtonOnPressedListener;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Top-level Application class for the Phone app.
 */
CarClass(CEmergencyDialer)
    , public Activity
    , public IEmergencyDialer
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnLongClickListener
        , public IViewOnKeyListener
        , public ITextWatcher
        , public IDialpadKeyButtonOnPressedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CEmergencyDialer* host);

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* input,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 changeCount);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* input);

        CARAPI OnKey(
            /* [in] */ IView* view,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnClick(
            /* [in] */ IView* view);

        CARAPI OnPressed(
            /* [in] */ IView* view,
            /* [in] */ Boolean pressed);

        CARAPI OnLongClick(
            /* [in] */ IView* view,
            /* [out] */ Boolean* result);

    private:
        CEmergencyDialer* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CEmergencyDialer::MyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IEmergencyDialer* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CEmergencyDialer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEmergencyDialer();

    CARAPI constructor();

    //@Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* input,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 changeCount);

    //@Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* input);

    /**
     * handle key events
     */
    // /@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKey(
        /* [in] */ IView* view,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnPressed(
        /* [in] */ IView* view,
        /* [in] */ Boolean pressed);

    /**
     * called for long touch events
     */
    //@Override
    CARAPI OnLongClick(
        /* [in] */ IView* view,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPause();

    /**
     * Plays the specified tone for TONE_LENGTH_MS milliseconds.
     *
     * The tone is played locally, using the audio stream for phone calls.
     * Tones are played only if the "Audible touch tones" user preference
     * is checked, and are NOT played if the device is in silent mode.
     *
     * @param tone a tone code from {@link ToneGenerator}
     */
    CARAPI PlayTone(
        /* [in] */ Int32 tone);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    /**
     * Explicitly turn off number formatting, since it gets in the way of the emergency
     * number detector
     */
    CARAPI MaybeAddNumberFormatting();

    //@Override
    CARAPI OnPostCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

    //@Override
    CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog);

private:
    CARAPI_(void) SetupKeypad();

    CARAPI_(void) KeyPressed(
        /* [in] */ Int32 keyCode);

    /**
     * place the call, but check to make sure it is a viable number.
     */
    CARAPI_(void) PlaceCall();

    CARAPI_(AutoPtr<ICharSequence>) CreateErrorMessage(
        /* [in] */ const String& number);

    /**
     * Update the enabledness of the "Dial" and "Backspace" buttons if applicable.
     */
    CARAPI UpdateDialAndDeleteButtonStateEnabledAttr();

private:
    // Keys used with onSaveInstanceState().
    static const String LAST_NUMBER;

    // List of dialer button IDs.
    static const AutoPtr<ArrayOf<Int32> > DIALER_KEYS;

    // Debug constants.
    static const Boolean DBG;
    static const String TAG;

    AutoPtr<IStatusBarManager> mStatusBarManager;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    /** The length of DTMF tones in milliseconds */
    static const Int32 TONE_LENGTH_MS;

    /** The DTMF tone volume relative to other sounds in the stream */
    static const Int32 TONE_RELATIVE_VOLUME;

    /** Stream type used to play the DTMF tones off call, and mapped to the volume control keys */
    static const Int32 DIAL_TONE_STREAM_TYPE;

    static const Int32 BAD_EMERGENCY_NUMBER_DIALOG;

    // private static final int USER_ACTIVITY_TIMEOUT_WHEN_NO_PROX_SENSOR = 15000; // millis

    AutoPtr<IEditText> mDigits;
    AutoPtr<IView> mDialButton;
    AutoPtr<IView> mDelete;

    AutoPtr<IToneGenerator> mToneGenerator;
    Object mToneGeneratorLock;

    // determines if we want to playback local DTMF tones.
    Boolean mDTMFToneEnabled;

    // Haptic feedback (vibration) for dialer key presses.
    AutoPtr<IHapticFeedback> mHaptic;

    // close activity when screen turns off
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    String mLastNumber; // last number we tried to dial. Used to restore error dialog.
    AutoPtr<InnerListener> mListener;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYDIALER_H__