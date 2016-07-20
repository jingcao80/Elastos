
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSKBCONTAINER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSKBCONTAINER_H__

#include "Elastos.Droid.InputMethodService.h"
#include "_Elastos_Droid_InputMethod_Pinyin_CSkbContainer.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/CSoftKeyboardView.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"
#include "elastos/droid/inputmethod/pinyin/InputModeSwitcher.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include <elastos/droid/os/HandlerRunnable.h>
#include <elastos/droid/widget/RelativeLayout.h>

using Elastos::Droid::InputMethodService::IInputMethodService;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Widget::IViewFlipper;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * The top container to host soft keyboard view(s).
 */
CarClass(CSkbContainer)
    , public RelativeLayout
    , public ISkbContainer
    , public IViewOnTouchListener
{
public:
    class LongPressTimer
        : public HandlerRunnable
    {
    public:
        LongPressTimer(
            /* [in] */ CSkbContainer* skbContainer);

        CARAPI_(void) StartTimer();

        CARAPI_(Boolean) RemoveTimer();

        CARAPI Run();

    private:
        /**
         * When user presses a key for a Int64 time, the timeout interval to
         * generate first {@link #LONG_PRESS_KEYNUM1} key events.
         */
        static const Int32 LONG_PRESS_TIMEOUT1;

        /**
         * When user presses a key for a Int64 time, after the first
         * {@link #LONG_PRESS_KEYNUM1} key events, this timeout interval will be
         * used.
         */
        static const Int32 LONG_PRESS_TIMEOUT2;

        /**
         * When user presses a key for a Int64 time, after the first
         * {@link #LONG_PRESS_KEYNUM2} key events, this timeout interval will be
         * used.
         */
        static const Int32 LONG_PRESS_TIMEOUT3;

        /**
         * When user presses a key for a Int64 time, after the first
         * {@link #LONG_PRESS_KEYNUM1} key events, timeout interval
         * {@link #LONG_PRESS_TIMEOUT2} will be used instead.
         */
        static const Int32 LONG_PRESS_KEYNUM1;

        /**
         * When user presses a key for a Int64 time, after the first
         * {@link #LONG_PRESS_KEYNUM2} key events, timeout interval
         * {@link #LONG_PRESS_TIMEOUT3} will be used instead.
         */
        static const Int32 LONG_PRESS_KEYNUM2;

        CSkbContainer* mSkbContainer;

        Int32 mResponseTimes;
    };

public:
    CSkbContainer();

    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) SetService(
        /* [in] */ IInputMethodService* service);

    CARAPI_(void) SetInputModeSwitcher(
        /* [in] */ InputModeSwitcher* inputModeSwitcher);

    CARAPI_(void) SetGestureDetector(
        /* [in] */ IGestureDetector* gestureDetector);

    CARAPI_(Boolean) IsCurrentSkbSticky();

    CARAPI_(void) ToggleCandidateMode(
        /* [in] */ Boolean candidatesShowing);

    CARAPI_(void) UpdateInputMode();

    CARAPI_(Boolean) HandleBack(
        /* [in] */ Boolean realAction);

    CARAPI_(void) DismissPopups();

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // Function for interface OnTouchListener, it is used to handle touch events
    // which will be delivered to the popup soft keyboard view.
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

protected:
    CARAPI_(void) UpdateSkbLayout();

    CARAPI_(void) ResponseKeyEvent(
        /* [in] */ SoftKey* sKey);

    CARAPI_(AutoPtr<CSoftKeyboardView>) InKeyboardView(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ ArrayOf<Int32>* positionInParent);

    CARAPI_(void) PopupSymbols();

    CARAPI_(void) DimSoftKeyboard(
        /* [in] */ Boolean dimSkb);

    CARAPI_(void) DismissPopupSkb();

    CARAPI_(void) ResetKeyPress(
        /* [in] */ Int64 delay);

protected:
    /**
     * For finger touch, user tends to press the bottom part of the target key,
     * or he/she even presses the area out of it, so it is necessary to make a
     * simple bias correction. If the input method runs on emulator, no bias
     * correction will be used.
     */
    static const Int32 Y_BIAS_CORRECTION;

    /**
     * Used to skip these move events whose position is too close to the
     * previous touch events.
     */
    static const Int32 MOVE_TOLERANCE;

    /**
     * If this member is TRUE, PopupWindow is used to show on-key highlight
     * effect.
     */
    static Boolean POPUPWINDOW_FOR_PRESSED_UI;

    /**
     * The current soft keyboard layout.
     *
     * @see com.android.inputmethod.pinyin.InputModeSwitcher for detailed layout
     *      definitions.
     */
    Int32 mSkbLayout;

    /**
     * The input method service.
     */
    AutoPtr<IInputMethodService> mService;

    /**
     * Input mode switcher used to switch between different modes like Chinese,
     * English, etc.
     */
    AutoPtr<InputModeSwitcher> mInputModeSwitcher;

    /**
     * The gesture detector.
     */
    AutoPtr<IGestureDetector> mGestureDetector;

    AutoPtr<Environment> mEnvironment;

    AutoPtr<IViewFlipper> mSkbFlipper;

    /**
     * The popup balloon hint for key press/release.
     */
    AutoPtr<CBalloonHint> mBalloonPopup;

    /**
     * The on-key balloon hint for key press/release.
     */
    AutoPtr<CBalloonHint> mBalloonOnKey;

    /** The major sub soft keyboard. */
    AutoPtr<CSoftKeyboardView> mMajorView;

    /**
     * The last parameter when function {@link #toggleCandidateMode(Boolean)}
     * was called.
     */
    Boolean mLastCandidatesShowing;

    /** Used to indicate whether a popup soft keyboard is shown. */
    Boolean mPopupSkbShow;

    /**
     * Used to indicate whether a popup soft keyboard is just shown, and waits
     * for the touch event to release. After the release, the popup window can
     * response to touch events.
     **/
    Boolean mPopupSkbNoResponse;

    /** Popup sub keyboard. */
    AutoPtr<IPopupWindow> mPopupSkb;

    /** The view of the popup sub soft keyboard. */
    AutoPtr<CSoftKeyboardView> mPopupSkbView;

    Int32 mPopupX;

    Int32 mPopupY;

    /**
     * When user presses a key, a timer is started, when it times out, it is
     * necessary to detect whether user still holds the key.
     */
    volatile Boolean mWaitForTouchUp;

    /**
     * When user drags on the soft keyboard and the distance is enough, this
     * drag will be recognized as a gesture and a gesture-based action will be
     * taken, in this situation, ignore the consequent events.
     */
    volatile Boolean mDiscardEvent;

    /**
     * For finger touch, user tends to press the bottom part of the target key,
     * or he/she even presses the area out of it, so it is necessary to make a
     * simple bias correction in Y.
     */
    Int32 mYBiasCorrection;

    /**
     * The x coordination of the last touch event.
     */
    Int32 mXLast;

    /**
     * The y coordination of the last touch event.
     */
    Int32 mYLast;

    /**
     * The soft keyboard view.
     */
    AutoPtr<CSoftKeyboardView> mSkv;

    /**
     * The position of the soft keyboard view in the container.
     */
    AutoPtr< ArrayOf<Int32> > mSkvPosInContainer;

    /**
     * The key pressed by user.
     */
    AutoPtr<SoftKey> mSoftKeyDown;

    /**
     * Used to timeout a press if user holds the key for a Int64 time.
     */
    AutoPtr<LongPressTimer> mLongPressTimer;

    /**
     * For temporary use.
     */
    AutoPtr< ArrayOf<Int32> > mXyPosTmp;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSKBCONTAINER_H__
