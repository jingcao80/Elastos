
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCOMPOSINGVIEW_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCOMPOSINGVIEW_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CComposingView.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * View used to show composing string (The Pinyin string for the unselected
 * syllables and the Chinese string for the selected syllables.)
 */
CarClass(CComposingView)
    , public View
    , public IComposingView
{
public:
    CComposingView();

    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Reset();

    /**
     * Set the composing string to show. If the IME status is
     * {@link PinyinIME.ImeState#STATE_INPUT}, the composing view's status will
     * be set to {@link ComposingStatus#SHOW_PINYIN}, otherwise the composing
     * view will set its status to {@link ComposingStatus#SHOW_STRING_LOWERCASE}
     * or {@link ComposingStatus#EDIT_PINYIN} automatically.
     */
    CARAPI SetDecodingInfo(
        /* [in] */ IDecodingInfo* decInfo,
        /* [in] */ ImeState imeStatus);

    CARAPI MoveCursor(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

    CARAPI GetComposingStatus(
        /* [out] */ ComposingStatus* status);

protected:
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) DrawCursor(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float x);

    CARAPI_(void) DrawForPinyin(
        /* [in] */ ICanvas* canvas);

private:
    static const Int32 LEFT_RIGHT_MARGIN;

    /**
     * Used to draw composing string. When drawing the active and idle part of
     * the spelling(Pinyin) string, the color may be changed.
     */
    AutoPtr<IPaint> mPaint;

    /**
     * Drawable used to draw highlight effect.
     */
    AutoPtr<IDrawable> mHlDrawable;

    /**
     * Drawable used to draw cursor for editing mode.
     */
    AutoPtr<IDrawable> mCursor;

    /**
     * Used to estimate dimensions to show the string .
     */
    AutoPtr<IPaintFontMetricsInt> mFmi;

    Int32 mStrColor;
    Int32 mStrColorHl;
    Int32 mStrColorIdle;

    Int32 mFontSize;

    ComposingStatus mComposingStatus;

    AutoPtr<IDecodingInfo> mDecInfo;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCOMPOSINGVIEW_H__
