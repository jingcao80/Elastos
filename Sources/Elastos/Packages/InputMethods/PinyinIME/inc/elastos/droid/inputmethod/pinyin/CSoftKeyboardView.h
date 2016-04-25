
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CSoftKeyboardView.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"

using Elastos::Droid::Os::IVibrator;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class SoftKey;

/**
 * Class used to show a soft keyboard.
 *
 * A soft keyboard view should not handle touch event itself, because we do bias
 * correction, need a global strategy to map an event into a proper view to
 * achieve better user experience.
 */
CarClass(CSoftKeyboardView)
    , public Elastos::Droid::View::View
    , public ISoftKeyboardView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSoftKeyboardView();

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetSoftKeyboard(
        /* [in] */ ISoftKeyboard* softSkb,
        /* [out] */ Boolean* result);

    CARAPI GetSoftKeyboard(
        /* [out] */ ISoftKeyboard** keyboard);

    CARAPI ResizeKeyboard(
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    CARAPI SetBalloonHint(
        /* [in] */ IBalloonHint* balloonOnKey,
        /* [in] */ IBalloonHint* balloonPopup,
        /* [in] */ Boolean movingNeverHidePopup);

    CARAPI SetOffsetToSkbContainer(
        /* [in] */ ArrayOf<Int32>* offsetToSkbContainer);

    CARAPI ResetKeyPress(
        /* [in] */ Int64 balloonDelay);

    // If movePress is true, means that this function is called because user
    // moves his finger to this button. If movePress is false, means that this
    // function is called when user just presses this key.
    CARAPI_(AutoPtr<SoftKey>) OnKeyPress(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] *//* SkbContainer.LongPressTimer*/ IHandler* longPressTimer,
        /* [in] */ Boolean movePress);

    CARAPI OnKeyRelease(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ ISoftKey** key);

    CARAPI OnKeyMove(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ ISoftKey** key);

    CARAPI DimSoftKeyboard(
        /* [in] */ Boolean dimSkb);

protected:
    void OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    void OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    void ShowBalloon(
        /* [in] */ IBalloonHint* balloon,
        /* [in] */ ArrayOf<Int32>* balloonLocationToSkb,
        /* [in] */ Boolean movePress);

    void TryVibrate();

    void TryPlayKeyDown();

    void DrawSoftKey(
        /* [in] */ ICanvas* canvas,
        /* [in] */ SoftKey* softKey,
        /* [in] */ Int32 keyXMargin,
        /* [in] */ Int32 keyYMargin);

protected:
    /**
     * The definition of the soft keyboard for the current this soft keyboard
     * view.
     */
    AutoPtr<SoftKeyboard> mSoftKeyboard;

    /**
     * The popup balloon hint for key press/release.
     */
    AutoPtr<IBalloonHint> mBalloonPopup;

    /**
     * The on-key balloon hint for key press/release. If it is NULL, on-key
     * highlight will be drawn on th soft keyboard view directly.
     */
    AutoPtr<IBalloonHint> mBalloonOnKey;

    /** Used to play key sounds. */
    AutoPtr<ISoundManager> mSoundManager;

    /** The last key pressed. */
    AutoPtr<SoftKey> mSoftKeyDown;

    /** Used to indicate whether the user is holding on a key. */
    Boolean mKeyPressed;

    /**
     * The location offset of the view to the keyboard container.
     */
    Int32 mOffsetToSkbContainer[2];// = new Int32[2];

    /**
     * The location of the desired hint view to the keyboard container.
     */
    Int32 mHintLocationToSkbContainer[2];// = new Int32[2];

    /**
     * Text size for normal key.
     */
    Int32 mNormalKeyTextSize;

    /**
     * Text size for function key.
     */
    Int32 mFunctionKeyTextSize;

    /**
     * Long press timer used to response Int64-press.
     */
    AutoPtr<SkbContainer::LongPressTimer> mLongPressTimer;

    /**
     * Repeated events for Int64 press
     */
    Boolean mRepeatForLongPress;

    /**
     * If this parameter is TRUE, the balloon will never be dismissed even if
     * user moves a lot from the pressed point.
     */
    Boolean mMovingNeverHidePopupBalloon;

    /** Vibration for key press. */
    AutoPtr<IVibrator> mVibrator;

    /** Vibration pattern for key press. */
    // Int64[] mVibratePattern = new Int64[] {1, 20};
    AutoPtr<ArrayOf<Int64> > mVibratePattern;

    /**
     * The dirty rectangle used to mark the area to re-draw during key press and
     * release. Currently, whenever we can invalidate(Rect), view will call
     * onDraw() and we MUST draw the whole view. This dirty information is for
     * future use.
     */
    AutoPtr<IRect> mDirtyRect;

    AutoPtr<IPaint> mPaint;
    AutoPtr<IPaintFontMetricsInt> mFmi;
    Boolean mDimSkb;

    friend class SkbContainer;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__
