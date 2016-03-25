
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARDVIEW_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARDVIEW_H__

#include "elastos/droid/inputmethodservice/Keyboard.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::InputMethodService::IKeyboardKey;
using Elastos::Droid::InputMethodService::IOnKeyboardActionListener;
using Elastos::Droid::View::View;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Utility::IAttributeSet;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::InputMethodService::IKeyboardKey);

namespace Elastos {
namespace Droid {
namespace InputMethodService {

class KeyboardView
    : public Elastos::Droid::View::View
    , public IKeyboardView
{
public:
    class MyHandler : public Handler
    {
    public:
        MyHandler(
            /* [in] */ KeyboardView* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        KeyboardView* mHost;
    };

    class _OnKeyboardActionListener
        : public Object
        , public IOnKeyboardActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        _OnKeyboardActionListener(
            /* [in] */ KeyboardView* host);

        ~_OnKeyboardActionListener();

        CARAPI OnPress(
            /* [in] */ Int32 primaryCode);

        CARAPI OnRelease(
            /* [in] */ Int32 primaryCode);

        CARAPI OnKey(
            /* [in] */ Int32 primaryCode,
            /* [in] */ ArrayOf<Int32>* keyCodes);

        CARAPI OnText(
            /* [in] */ ICharSequence* text);

        CARAPI SwipeLeft();

        CARAPI SwipeRight();

        CARAPI SwipeDown();

        CARAPI SwipeUp();

    private:
        KeyboardView* mHost;
    };

private:
    class SwipeTracker : public Object
    {
    public:
        SwipeTracker();

        ~SwipeTracker();

        CARAPI_(void) Clear();

        CARAPI_(void) AddMovement(
            /* [in] */ IMotionEvent* ev);

        CARAPI_(void) ComputeCurrentVelocity(
            /* [in] */ Int32 units);

        CARAPI_(void) ComputeCurrentVelocity(
            /* [in] */ Int32 units,
            /* [in] */ Float maxVelocity);

        CARAPI_(Float) GetXVelocity();

        CARAPI_(Float) GetYVelocity();

    private:
        CARAPI_(void) AddPoint(
            /* [in] */ Float x,
            /* [in] */ Float y,
            /* [in] */ Int64 time);

    private:
        static const Int32 NUM_PAST;
        static const Int32 LONGEST_PAST_TIME;

        AutoPtr<ArrayOf<Float> > mPastX;
        AutoPtr<ArrayOf<Float> > mPastY;
        AutoPtr<ArrayOf<Int64> > mPastTime;
        Float mYVelocity;
        Float mXVelocity;
    };

    class _SimpleOnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        _SimpleOnGestureListener(
            /* [in] */ KeyboardView* host);

        ~_SimpleOnGestureListener();

        virtual CARAPI OnFling(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

    private:
        KeyboardView* mHost;
    };

    class CloseButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CloseButtonClickListener(
            /* [in] */ KeyboardView* host);

        CARAPI OnClick(
            /* [in] */ IView* view);
    private:
        KeyboardView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    KeyboardView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI SetOnKeyboardActionListener(
        /* [in] */ IOnKeyboardActionListener* listener);

    /**
     * Returns the {@link IOnKeyboardActionListener} object.
     * @return the listener attached to this keyboard
     */
    virtual CARAPI GetOnKeyboardActionListener(
        /* [out] */ IOnKeyboardActionListener** listener);

    /**
     * Attaches a keyboard to this view. The keyboard can be switched at any time and the
     * view will re-layout itself to accommodate the keyboard.
     * @see IKeyboard
     * @see #getKeyboard()
     * @param keyboard the keyboard to display in this view
     */
    virtual CARAPI SetKeyboard(
        /* [in] */ IKeyboard* keyboard);

    /**
     * Returns the current keyboard being displayed by this view.
     * @return the currently attached keyboard
     * @see #setKeyboard(IKeyboard)
     */
    virtual CARAPI GetKeyboard(
        /* [out] */ IKeyboard** keyboard);

    /**
     * Sets the state of the shift key of the keyboard, if any.
     * @param shifted whether or not to enable the state of the shift key
     * @return TRUE if the shift key state changed, FALSE if there was no change
     * @see KeyboardView#isShifted()
     */
    virtual CARAPI SetShifted(
        /* [in] */ Boolean shifted,
        /* [out] */ Boolean* res);

    /**
     * Returns the state of the shift key of the keyboard, if any.
     * @return TRUE if the shift is in a pressed state, FALSE otherwise. If there is
     * no shift key on the keyboard or there is no keyboard attached, it returns FALSE.
     * @see KeyboardView#setShifted(Boolean)
     */
    virtual CARAPI IsShifted(
        /* [out] */ Boolean* shifted);

    /**
     * Enables or disables the key feedback popup. This is a popup that shows a magnified
     * version of the depressed key. By default the preview is enabled.
     * @param previewEnabled whether or not to enable the key feedback popup
     * @see #isPreviewEnabled()
     */
    virtual CARAPI SetPreviewEnabled(
        /* [in] */ Boolean previewEnabled);

    /**
     * Returns the enabled state of the key feedback popup.
     * @return whether or not the key feedback popup is enabled
     * @see #setPreviewEnabled(Boolean)
     */
    virtual CARAPI IsPreviewEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetVerticalCorrection(
        /* [in] */ Int32 verticalOffset);

    virtual CARAPI SetPopupParent(
        /* [in] */ IView* v);

    virtual CARAPI SetPopupOffset(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * When enabled, calls to {@link IOnKeyboardActionListener#onKey} will include key
     * codes for adjacent keys.  When disabled, only the primary key code will be
     * reported.
     * @param enabled whether or not the proximity correction is enabled
     */
    virtual CARAPI SetProximityCorrectionEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Returns TRUE if proximity correction is enabled.
     */
    virtual CARAPI IsProximityCorrectionEnabled(
        /* [out] */ Boolean* res);

    /**
     * Popup keyboard close button clicked.
     * @hide
     */
    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    /**
     * Requests a redraw of the entire keyboard. Calling {@link #invalidate} is not sufficient
     * because the keyboard renders the keys to an off-screen buffer and an invalidate() only
     * draws the cached buffer.
     * @see #invalidateKey(Int32)
     */
    virtual CARAPI InvalidateAllKeys();

    /**
     * Invalidates a key so that it will be redrawn on the next repaint. Use this method if only
     * one key is changing it's content. Any changes that affect the position or size of the key
     * may not be honored.
     * @param keyIndex the index of the key in the attached {@link IKeyboard}.
     * @see #invalidateAllKeys
     */
    virtual CARAPI InvalidateKey(
        /* [in] */ Int32 keyIndex);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* me,
        /* [out] */ Boolean* res);

    virtual CARAPI Closing();

    //@Override
    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI HandleBack(
        /* [out] */ Boolean* res);

    virtual CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    /**
     * Called when a key is Int64 pressed. By default this will open any popup keyboard associated
     * with this key through the attributes popupLayout and popupCharacters.
     * @param popupKey the key that was Int64 pressed
     * @return TRUE if the Int64 press is handled, FALSE otherwise. Subclasses should call the
     * method on the base class if the subclass doesn't wish to handle the call.
     */
    virtual CARAPI OnLongPress(
        /* [in] */ IKeyboardKey* popupKey,
        /* [out] */ Boolean* res);

    virtual CARAPI_(void) SwipeRight();

    virtual CARAPI_(void) SwipeLeft();

    virtual CARAPI_(void) SwipeUp();

    virtual CARAPI_(void) SwipeDown();

private:
    CARAPI_(void) InitGestureDetector();

    CARAPI_(AutoPtr<ICharSequence>) AdjustCase(
        /* [in] */ ICharSequence* label);

    /**
     * Compute the average distance between adjacent keys (horizontally and vertically)
     * and square it to get the proximity threshold. We use a square here and in computing
     * the touch distance from a key's center to avoid taking a square root.
     * @param keyboard
     */
    CARAPI_(void) ComputeProximityThreshold(
        /* [in] */ IKeyboard* keyboard);

    CARAPI_(void) OnBufferDraw();

    CARAPI_(Int32) GetKeyIndices(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ ArrayOf<Int32>* allKeys);

    CARAPI_(void) DetectAndSendKey(
        /* [in] */ Int32 index,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int64 eventTime);

    /**
     * Handle multi-tap keys by producing the key label for the current multi-tap state.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetPreviewText(
        /* [in] */ IKeyboardKey* key);

    CARAPI_(void) ShowPreview(
        Int32 keyIndex);

    CARAPI_(void) ShowKey(
        /* [in] */ Int32 keyIndex);

    CARAPI_(Boolean) OpenPopupIfRequired(
        /* [in] */ IMotionEvent* me);

    CARAPI_(Boolean) OnModifiedTouchEvent(
        /* [in] */ IMotionEvent* me,
        /* [in] */ Boolean possiblePoly);

    CARAPI_(Boolean) RepeatKey();

    CARAPI_(void) RemoveMessages();

    CARAPI_(void) DismissPopupKeyboard();

    CARAPI_(void) ResetMultiTap();

    CARAPI_(void) CheckMultiTap(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 keyIndex);

    CARAPI_(void) HandleShowPreview(
        /* [in] */ Int32 keyIndex);

    CARAPI_(void) HandleRemovePreview();

    CARAPI_(void) HandleRepeat();

    CARAPI_(void) HandleLongPress(
        /* [in] */ IMotionEvent* obj);

    CARAPI_(void) SendAccessibilityEventForUnicodeCharacter(
        /* [in] */ Int32 eventType,
        /* [in] */ Int32 code);

protected:
    static const Boolean DEBUG;
    static const Int32 NOT_A_KEY;
    static Int32 KEY_DELETE[];
    static Int32 LONG_PRESSABLE_STATE_SET[];

    AutoPtr<IKeyboard> mKeyboard;
    Int32 mCurrentKeyIndex;
    Int32 mLabelTextSize;
    Int32 mKeyTextSize;
    Int32 mKeyTextColor;
    Float mShadowRadius;
    Int32 mShadowColor;
    Float mBackgroundDimAmount;

    AutoPtr<ITextView> mPreviewText;
    AutoPtr<IPopupWindow> mPreviewPopup;
    Int32 mPreviewTextSizeLarge;
    Int32 mPreviewOffset;
    Int32 mPreviewHeight;
    // Working variable
    AutoPtr<ArrayOf<Int32> > mCoordinates;

    AutoPtr<IPopupWindow> mPopupKeyboard;
    AutoPtr<IView> mMiniKeyboardContainer;
    AutoPtr<IKeyboardView> mMiniKeyboard;
    Boolean mMiniKeyboardOnScreen;
    AutoPtr<IView> mPopupParent;
    Int32 mMiniKeyboardOffsetX;
    Int32 mMiniKeyboardOffsetY;
    AutoPtr< HashMap<AutoPtr<IKeyboardKey>, AutoPtr<IView> > > mMiniKeyboardCache;
    AutoPtr< ArrayOf<IKeyboardKey*> > mKeys;

    /** Listener for {@link IOnKeyboardActionListener}. */
    AutoPtr<IOnKeyboardActionListener> mKeyboardActionListener;

    static const Int32 MSG_SHOW_PREVIEW;
    static const Int32 MSG_REMOVE_PREVIEW;
    static const Int32 MSG_REPEAT;
    static const Int32 MSG_LONGPRESS;

    static const Int32 DELAY_BEFORE_PREVIEW;
    static const Int32 DELAY_AFTER_PREVIEW;
    static const Int32 DEBOUNCE_TIME;

    Int32 mVerticalCorrection;
    Int32 mProximityThreshold;

    Boolean mPreviewCentered;
    Boolean mShowPreview;
    Boolean mShowTouchPoints;
    Int32 mPopupPreviewX;
    Int32 mPopupPreviewY;

    Int32 mLastX;
    Int32 mLastY;
    Int32 mStartX;
    Int32 mStartY;

    Boolean mProximityCorrectOn;

    AutoPtr<IPaint> mPaint;
    AutoPtr<IRect> mPadding;

    Int64 mDownTime;
    Int64 mLastMoveTime;
    Int32 mLastKey;
    Int32 mLastCodeX;
    Int32 mLastCodeY;
    Int32 mCurrentKey;
    Int32 mDownKey;
    Int64 mLastKeyTime;
    Int64 mCurrentKeyTime;
    AutoPtr< ArrayOf<Int32> > mKeyIndices;
    AutoPtr<IGestureDetector> mGestureDetector;
    Int32 mPopupX;
    Int32 mPopupY;
    Int32 mRepeatKeyIndex;
    Int32 mPopupLayout;
    Boolean mAbortKey;
    AutoPtr<IKeyboardKey> mInvalidatedKey;
    AutoPtr<IRect> mClipRegion;
    Boolean mPossiblePoly;
    AutoPtr<SwipeTracker> mSwipeTracker;;
    Int32 mSwipeThreshold;
    Boolean mDisambiguateSwipe;

    // Variables for dealing with multiple pointers
    Int32 mOldPointerCount;
    Float mOldPointerX;
    Float mOldPointerY;

    AutoPtr<IDrawable> mKeyBackground;

    static const Int32 REPEAT_INTERVAL; // ~20 keys per second
    static const Int32 REPEAT_START_DELAY;
    static const Int32 LONGPRESS_TIMEOUT;

    static const Int32 MAX_NEARBY_KEYS;
    AutoPtr< ArrayOf<Int32> > mDistances;

    // For multi-tap
    Int32 mLastSentIndex;
    Int32 mTapCount;
    Int64 mLastTapTime;
    Boolean mInMultiTap;
    static const Int32 MULTITAP_INTERVAL; // milliseconds

    StringBuilder mPreviewLabel;

    /** Whether the keyboard bitmap needs to be redrawn before it's blitted. **/
    Boolean mDrawPending;
    /** The dirty region in the keyboard bitmap */
    AutoPtr<IRectF> mDirtyRect;
    /** The keyboard bitmap for faster updates */
    AutoPtr<IBitmap> mBuffer;
    /** Notes if the keyboard just changed, so that we could possibly reallocate the mBuffer. */
    Boolean mKeyboardChanged;
    /** The canvas for the above mutable keyboard bitmap */
    AutoPtr<ICanvas> mCanvas;

    /** The accessibility manager for accessibility support */
    AutoPtr<IAccessibilityManager> mAccessibilityManager;
    /** The audio manager for accessibility support */
    AutoPtr<IAudioManager> mAudioManager;
    /** Whether the requirement of a headset to hear passwords if accessibility is enabled is announced. */
    Boolean mHeadsetRequiredToHearPasswordsAnnounced;

    AutoPtr<IHandler> mHandler;

    Int64 mOldEventTime;
    Boolean mUsedVelocity;

    friend class _OnKeyboardActionListener;
    friend class _SimpleOnGestureListener;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARDVIEW_H__
