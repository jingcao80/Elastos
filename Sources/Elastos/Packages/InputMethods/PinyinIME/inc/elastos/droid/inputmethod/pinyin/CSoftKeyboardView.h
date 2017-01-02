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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CSoftKeyboardView.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"
#include "elastos/droid/inputmethod/pinyin/SoundManager.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

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
    friend class CSkbContainer;

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSoftKeyboardView();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(Boolean) SetSoftKeyboard(
        /* [in] */ SoftKeyboard* softSkb);

    CARAPI_(AutoPtr<SoftKeyboard>) GetSoftKeyboard();

    CARAPI_(void) ResizeKeyboard(
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    CARAPI_(void) SetBalloonHint(
        /* [in] */ CBalloonHint* balloonOnKey,
        /* [in] */ CBalloonHint* balloonPopup,
        /* [in] */ Boolean movingNeverHidePopup);

    CARAPI_(void) SetOffsetToSkbContainer(
        /* [in] */ ArrayOf<Int32>* offsetToSkbContainer);

    CARAPI_(void) ResetKeyPress(
        /* [in] */ Int64 balloonDelay);

    // If movePress is true, means that this function is called because user
    // moves his finger to this button. If movePress is false, means that this
    // function is called when user just presses this key.
    CARAPI_(AutoPtr<SoftKey>) OnKeyPress(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ HandlerRunnable* longPressTimer, //SkbContainer.LongPressTimer
        /* [in] */ Boolean movePress);

    CARAPI_(AutoPtr<SoftKey>) OnKeyRelease(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(AutoPtr<SoftKey>) OnKeyMove(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) DimSoftKeyboard(
        /* [in] */ Boolean dimSkb);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) ShowBalloon(
        /* [in] */ CBalloonHint* balloon,
        /* [in] */ ArrayOf<Int32>* balloonLocationToSkb,
        /* [in] */ Boolean movePress);

    CARAPI_(void) TryVibrate();

    CARAPI_(void) TryPlayKeyDown();

    CARAPI_(void) DrawSoftKey(
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
    AutoPtr<CBalloonHint> mBalloonPopup;

    /**
     * The on-key balloon hint for key press/release. If it is NULL, on-key
     * highlight will be drawn on th soft keyboard view directly.
     */
    AutoPtr<CBalloonHint> mBalloonOnKey;

    /** Used to play key sounds. */
    AutoPtr<SoundManager> mSoundManager;

    /** The last key pressed. */
    AutoPtr<SoftKey> mSoftKeyDown;

    /** Used to indicate whether the user is holding on a key. */
    Boolean mKeyPressed;

    /**
     * The location offset of the view to the keyboard container.
     */
    AutoPtr< ArrayOf<Int32> > mOffsetToSkbContainer;

    /**
     * The location of the desired hint view to the keyboard container.
     */
    AutoPtr< ArrayOf<Int32> > mHintLocationToSkbContainer;

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
    AutoPtr<HandlerRunnable> mLongPressTimer; //SkbContainer::LongPressTimer

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
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSOFTKEYBOARDVIEW_H__
