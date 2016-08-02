
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CPinyinIME.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/EnglishInputProcessor.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"
#include "elastos/droid/inputmethod/pinyin/InputModeSwitcher.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include <elastos/droid/inputmethodservice/InputMethodService.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/HandlerRunnable.h>
#include <elastos/droid/view/GestureDetector.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Core::Object;
using Elastos::Core::StringBuffer;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Main class of the Pinyin input method.
 */
CarClass(CPinyinIME)
    , public Elastos::Droid::InputMethodService::InputMethodService
    , public IPinyinIME
{
public:
    /**
     * Used to notify IME that the user selects a candidate or performs an
     * gesture.
     */
    class ChoiceNotifier
        : public Handler
        , public ICandidateViewListener
    {
    public:
        TO_STRING_IMPL("CPinyinIME::ChoiceNotifier")

        CAR_INTERFACE_DECL();

        ChoiceNotifier(
            /* [in] */ CPinyinIME* ime);

        CARAPI OnClickChoice(
            /* [in] */ Int32 choiceId);

        CARAPI OnToLeftGesture();

        CARAPI OnToRightGesture();

        CARAPI OnToTopGesture();

        CARAPI OnToBottomGesture();

    private:
        CPinyinIME* mIme;
    };

    class OnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        OnGestureListener(
            /* [in] */ Boolean reponseGestures,
            /* [in] */ CPinyinIME* ime);

        CARAPI OnDown(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        CARAPI OnFling(
            /* [in] */ IMotionEvent* me1,
            /* [in] */ IMotionEvent* me2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

        CARAPI_(void) OnDirectionGesture(
            /* [in] */ Int32 gravity);

    private:
        /**
         * When user presses and drags, the minimum x-distance to make a
         * response to the drag event.
         */
        static const Int32 MIN_X_FOR_DRAG;

        /**
         * When user presses and drags, the minimum y-distance to make a
         * response to the drag event.
         */
        static const Int32 MIN_Y_FOR_DRAG;

        /**
         * Velocity threshold for a screen-move gesture. If the minimum
         * x-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_X1;

        /**
         * Velocity threshold for a screen-move gesture. If the maximum
         * x-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_X2;

        /**
         * Velocity threshold for a screen-move gesture. If the minimum
         * y-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_Y1;

        /**
         * Velocity threshold for a screen-move gesture. If the maximum
         * y-velocity is less than it, no gesture.
         */
        static const Float VELOCITY_THRESHOLD_Y2;

        /** If it FALSE, we will not response detected gestures. */
        Boolean mReponseGestures;

        /** The minimum X velocity observed in the gesture. */
        Float mMinVelocityX;

        /** The minimum Y velocity observed in the gesture. */
        Float mMinVelocityY;

        /** The first down time for the series of touch events for an action. */
        Int64 mTimeDown;

        /** The last time when onScroll() is called. */
        Int64 mTimeLastOnScroll;

        /** This flag used to indicate that this gesture is not a gesture. */
        Boolean mNotGesture;

        /** This flag used to indicate that this gesture has been recognized. */
        Boolean mGestureRecognized;

        CPinyinIME* mIme;
    };

    /**
     * Connection used for binding to the Pinyin decoding service.
     */
    class PinyinDecoderServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        PinyinDecoderServiceConnection(
            /* [in] */ CPinyinIME* ime);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CPinyinIME* mIme;
    };

    enum ImeState
    {
        STATE_BYPASS,
        STATE_IDLE,
        STATE_INPUT,
        STATE_COMPOSING,
        STATE_PREDICT,
        STATE_APP_COMPLETION
    };

    class DecodingInfo
        : public Object
    {
        friend class CPinyinIME;

    public:
        DecodingInfo(
            /* [in] */ CPinyinIME* ime);

        CARAPI_(void) Reset();

        CARAPI_(Boolean) IsCandidatesListEmpty();

        CARAPI_(Boolean) IsSplStrFull();

        CARAPI_(void) AddSplChar(
            /* [in] */ Char32 ch,
            /* [in] */ Boolean reset);

        // Prepare to delete before cursor. We may delete a spelling char if
        // the cursor is in the range of unfixed part, delete a whole spelling
        // if the cursor in inside the range of the fixed part.
        // This function only marks the position used to delete.
        CARAPI_(void) PrepareDeleteBeforeCursor();

        CARAPI_(Int32) GetLength();

        CARAPI_(Char32) GetCharAt(
            /* [in] */ Int32 index);

        CARAPI_(AutoPtr<StringBuffer>) GetOrigianlSplStr();

        CARAPI_(Int32) GetSplStrDecodedLen();

        CARAPI_(AutoPtr< ArrayOf<Int32> >) GetSplStart();

        CARAPI_(String) GetComposingStr();

        CARAPI_(String) GetComposingStrActivePart();

        CARAPI_(Int32) GetActiveCmpsLen();

        CARAPI_(String) GetComposingStrForDisplay();

        CARAPI_(Int32) GetActiveCmpsDisplayLen();

        CARAPI_(String) GetFullSent();

        CARAPI_(String) GetCurrentFullSent(
            /* [in] */ Int32 activeCandPos);

        CARAPI_(void) ResetCandidates();

        CARAPI_(Boolean) CandidatesFromApp();

        CARAPI_(Boolean) CanDoPrediction();

        CARAPI_(Boolean) SelectionFinished();

        CARAPI_(String) GetCandidate(
            /* [in] */ Int32 candId);

        CARAPI_(Boolean) PageReady(
            /* [in] */ Int32 pageNo);

        CARAPI_(Boolean) PreparePage(
            /* [in] */ Int32 pageNo);

        CARAPI_(void) PreparePredicts(
            /* [in] */ ICharSequence* history);

        CARAPI_(Int32) GetCurrentPageSize(
            /* [in] */ Int32 currentPage);

        CARAPI_(Int32) GetCurrentPageStart(
            /* [in] */ Int32 currentPage);

        CARAPI_(Boolean) PageForwardable(
            /* [in] */ Int32 currentPage);

        CARAPI_(Boolean) PageBackwardable(
            /* [in] */ Int32 currentPage);

        CARAPI_(Boolean) CharBeforeCursorIsSeparator();

        CARAPI_(Int32) GetCursorPos();

        CARAPI_(Int32) GetCursorPosInCmps();

        CARAPI_(Int32) GetCursorPosInCmpsDisplay();

        CARAPI_(void) MoveCursorToEdge(
            /* [in] */ Boolean left);

        CARAPI_(void) MoveCursor(
            /* [in] */ Int32 offset);

        CARAPI_(Int32) GetSplNum();

        CARAPI_(Int32) GetFixedLen();

    private:
        // After the user chooses a candidate, input method will do a
        // re-decoding and give the new candidate list.
        // If candidate id is less than 0, means user is inputting Pinyin,
        // not selecting any choice.
        CARAPI_(void) ChooseDecodingCandidate(
            /* [in] */ Int32 candId);

        CARAPI_(void) UpdateDecInfoForSearch(
            /* [in] */ Int32 totalChoicesNum);

        CARAPI_(void) ChoosePredictChoice(
            /* [in] */ Int32 choiceId);

        CARAPI_(void) GetCandiagtesForCache();

        CARAPI_(void) PrepareAppCompletions(
            /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    public:
        /**
         * The total number of choices for display. The list may only contains
         * the first part. If user tries to navigate to next page which is not
         * in the result list, we need to get these items.
         **/
        Int32 mTotalChoicesNum;

        /**
         * Candidate list. The first one is the full-sentence candidate.
         */
        Vector<String> mCandidatesList;

        /**
         * Element i stores the starting position of page i.
         */
        Vector<Int32> mPageStart;

        /**
         * Element i stores the number of characters to page i.
         */
        Vector<Int32> mCnToPage;

        /**
         * The position to delete in Pinyin string. If it is less than 0, IME
         * will do an incremental search, otherwise IME will do a deletion
         * operation. if {@link #mIsPosInSpl} is TRUE, IME will delete the whole
         * string for mPosDelSpl-th spelling, otherwise it will only delete
         * mPosDelSpl-th character in the Pinyin string.
         */
        Int32 mPosDelSpl;

        /**
         * If {@link #mPosDelSpl} is big than or equal to 0, this member is used
         * to indicate that whether the postion is counted in spelling id or
         * character.
         */
        Boolean mIsPosInSpl;

    private:
        /**
         * Maximum length of the Pinyin string
         */
        static const Int32 PY_STRING_MAX;

        /**
         * Maximum number of candidates to display in one page.
         */
        static const Int32 MAX_PAGE_SIZE_DISPLAY;

        /**
         * Spelling (Pinyin) string.
         */
        AutoPtr<StringBuffer> mSurface;

        /**
         * Byte buffer used as the Pinyin string parameter for native function
         * call.
         */
        AutoPtr<ArrayOf<Byte> > mPyBuf;

        /**
         * The length of surface string successfully decoded by engine.
         */
        Int32 mSurfaceDecodedLen;

        /**
         * Composing string.
         */
        String mComposingStr;

        /**
         * Length of the active composing string.
         */
        Int32 mActiveCmpsLen;

        /**
         * Composing string for display, it is copied from mComposingStr, and
         * add spaces between spellings.
         **/
        String mComposingStrDisplay;

        /**
         * Length of the active composing string for display.
         */
        Int32 mActiveCmpsDisplayLen;

        /**
         * The first full sentence choice.
         */
        String mFullSent;

        /**
         * Number of characters which have been fixed.
         */
        Int32 mFixedLen;

        /**
         * If this flag is TRUE, selection is finished.
         */
        Boolean mFinishSelection;

        /**
         * The starting position for each spelling. The first one is the number
         * of the real starting position elements.
         */
        AutoPtr<ArrayOf<Int32> > mSplStart;

        /**
         * Editing cursor in mSurface.
         */
        Int32 mCursorPos;

        /**
         * Remote Pinyin-to-Hanzi decoding engine service.
         */
        AutoPtr<IIPinyinDecoderService> mIPinyinDecoderService;

        /**
         * The complication information suggested by application.
         */
        AutoPtr< ArrayOf<ICompletionInfo*> > mAppCompletions;

        CPinyinIME* mHost;
    };

private:
    class PopupTimer
        : public HandlerRunnable
    {
    public:
        PopupTimer(
            /* [in] */ CPinyinIME* host);

        CARAPI_(void) PostShowFloatingWindow();

        CARAPI_(void) CancelShowing();

        CARAPI Run();

    private:
        AutoPtr< ArrayOf<Int32> > mParentLocation;
        CPinyinIME* mHost;
    };

    class MyReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CPinyinIME::MyReceiver")

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    };

    class BuilderListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        BuilderListener(
            /* [in] */ CPinyinIME* ime);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* di,
            /* [in] */ Int32 position);

    private:
        CPinyinIME* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CPinyinIME();

    ~CPinyinIME();

    CARAPI constructor();

    CARAPI OnCreate();

    CARAPI OnEvaluateFullscreenMode(
        /* [out] */ Boolean* screenMode);

    CARAPI OnDestroy();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnCreateCandidatesView(
        /* [out] */ IView** view);

    CARAPI ResponseSoftKeyEvent(
        /* [in] */ SoftKey* key);

    CARAPI OnCreateInputView(
        /* [out] */ IView** view);

    CARAPI OnStartInput(
        /* [in] */ IEditorInfo* editorInfo,
        /* [in] */ Boolean restarting);

    CARAPI OnStartInputView(
        /* [in] */ IEditorInfo* editorInfo,
        /* [in] */ Boolean restarting);

    CARAPI OnFinishInputView(
        /* [in] */ Boolean finishingInput);

    CARAPI OnFinishInput();

    CARAPI OnFinishCandidatesView(
        /* [in] */ Boolean finishingInput);

    CARAPI OnDisplayCompletions(
        /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    CARAPI RequestHideSelf(
        /* [in] */ Int32 flags);

    CARAPI ShowOptionsMenu();

private:
    CARAPI_(Boolean) ProcessKey(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    // keyCode can be from both hard key or soft key.
    CARAPI_(Boolean) ProcessFunctionKeys(
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean realAction);

    CARAPI_(Boolean) ProcessStateIdle(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    CARAPI_(Boolean) ProcessStateInput(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    CARAPI_(Boolean) ProcessStatePredict(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    CARAPI_(Boolean) ProcessStateEditComposing(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean realAction);

    CARAPI_(Boolean) TryInputRawUnicode(
        /* [in] */ const String& str);

    CARAPI_(Boolean) ProcessSurfaceChange(
        /* [in] */ Int32 keyChar,
        /* [in] */ Int32 keyCode);

    CARAPI_(void) ChangeToStateComposing(
        /* [in] */ Boolean updateUi);

    CARAPI_(void) ChangeToStateInput(
        /* [in] */ Boolean updateUi);

    CARAPI_(void) SimulateKeyEventDownUp(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) CommitResultText(
        /* [in] */ const String& resultText);

    CARAPI_(void) UpdateComposingText(
        /* [in] */ Boolean visible);

    CARAPI_(void) InputCommaPeriod(
        /* [in] */ const String& preEdit,
        /* [in] */ Int32 keyChar,
        /* [in] */ Boolean dismissCandWindow,
        /* [in] */ ImeState nextState);

    CARAPI_(void) ResetToIdleState(
        /* [in] */ Boolean resetInlineText);

    CARAPI_(void) ChooseAndUpdate(
        /* [in] */ Int32 candId);

    // If activeCandNo is less than 0, get the current active candidate number
    // from candidate view, otherwise use activeCandNo.
    CARAPI_(void) ChooseCandidate(
        /* [in] */ Int32 activeCandNo);

    CARAPI_(Boolean) StartPinyinDecoderService();

    CARAPI_(void) ShowCandidateWindow(
        /* [in] */ Boolean showComposingView);

    CARAPI_(void) DismissCandidateWindow();

    CARAPI_(void) ResetCandidateWindow();

    CARAPI_(void) UpdateIcon(
        /* [in] */ Int32 iconId);

    CARAPI_(void) OnChoiceTouched(
        /* [in] */ Int32 activeCandNo);

    CARAPI_(void) LaunchSettings();

private:
    /**
     * TAG for debug.
     */
    static const String TAG;

    /**
     * If is is TRUE, IME will simulate key events for delete key, and send the
     * events back to the application.
     */
    static const Boolean SIMULATE_KEY_DELETE;

    /**
     * Necessary environment configurations like screen size for this IME.
     */
    AutoPtr<Environment> mEnvironment;

    /**
     * Used to switch input mode.
     */
    AutoPtr<InputModeSwitcher> mInputModeSwitcher;

    /**
     * Soft keyboard container view to host real soft keyboard view.
     */
    AutoPtr<ISkbContainer> mSkbContainer;

    /**
     * The floating container which contains the composing view. If necessary,
     * some other view like candiates container can also be put here.
     */
    AutoPtr<ILinearLayout> mFloatingContainer;

    /**
     * View to show the composing string.
     */
    AutoPtr<IComposingView> mComposingView;

    /**
     * Window to show the composing string.
     */
    AutoPtr<IPopupWindow> mFloatingWindow;

    /**
     * Used to show the floating window.
     */
    AutoPtr<PopupTimer> mFloatingWindowTimer;

    /**
     * View to show candidates list.
     */
    AutoPtr<ICandidatesContainer> mCandidatesContainer;

    /**
     * Balloon used when user presses a candidate.
     */
    AutoPtr<CBalloonHint> mCandidatesBalloon;

    /**
     * Used to notify the input method when the user touch a candidate.
     */
    AutoPtr<ChoiceNotifier> mChoiceNotifier;

    /**
     * Used to notify gestures from soft keyboard.
     */
    AutoPtr<IGestureDetectorOnGestureListener> mGestureListenerSkb;

    /**
     * Used to notify gestures from candidates view.
     */
    AutoPtr<IGestureDetectorOnGestureListener> mGestureListenerCandidates;

    /**
     * The on-screen movement gesture detector for soft keyboard.
     */
    AutoPtr<IGestureDetector> mGestureDetectorSkb;

    /**
     * The on-screen movement gesture detector for candidates view.
     */
    AutoPtr<IGestureDetector> mGestureDetectorCandidates;

    /**
     * Option dialog to choose settings and other IMEs.
     */
    AutoPtr<IAlertDialog> mOptionsDialog;

    /**
     * Connection used to bind the decoding service.
     */
    AutoPtr<PinyinDecoderServiceConnection> mPinyinDecoderServiceConnection;

    /**
     * The current IME status.
     *
     * @see com.android.inputmethod.pinyin.PinyinIME.ImeState
     */
    ImeState mImeState;

    /**
     * The decoding information, include spelling(Pinyin) string, decoding
     * result, etc.
     */
    AutoPtr<DecodingInfo> mDecInfo;

    /**
     * For English input.
     */
    AutoPtr<EnglishInputProcessor> mImEn;

    // receive ringer mode changes
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINIME_H__
