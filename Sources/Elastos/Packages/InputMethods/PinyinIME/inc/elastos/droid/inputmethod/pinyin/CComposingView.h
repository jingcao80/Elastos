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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCOMPOSINGVIEW_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCOMPOSINGVIEW_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CComposingView.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinIME.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * View used to show composing string (The Pinyin string for the unselected
 * syllables and the Chinese string for the selected syllables.)
 */
CarClass(CComposingView)
    , public Elastos::Droid::View::View
    , public IComposingView
{
public:
    /**
     * <p>
     * There are three statuses for the composing view.
     * </p>
     *
     * <p>
     * {@link #SHOW_PINYIN} is used to show the current Pinyin string without
     * highlighted effect. When user inputs Pinyin characters one by one, the
     * Pinyin string will be shown in this mode.
     * </p>
     * <p>
     * {@link #SHOW_STRING_LOWERCASE} is used to show the Pinyin string in
     * lowercase with highlighted effect. When user presses UP key and there is
     * no fixed Chinese characters, composing view will switch from
     * {@link #SHOW_PINYIN} to this mode, and in this mode, user can press
     * confirm key to input the lower-case string, so that user can input
     * English letter in Chinese mode.
     * </p>
     * <p>
     * {@link #EDIT_PINYIN} is used to edit the Pinyin string (shown with
     * highlighted effect). When current status is {@link #SHOW_PINYIN} and user
     * presses UP key, if there are fixed Characters, the input method will
     * switch to {@link #EDIT_PINYIN} thus user can modify some characters in
     * the middle of the Pinyin string. If the current status is
     * {@link #SHOW_STRING_LOWERCASE} and user presses LEFT and RIGHT key, it
     * will also switch to {@link #EDIT_PINYIN}.
     * </p>
     * <p>
     * Whenever user presses down key, the status switches to
     * {@link #SHOW_PINYIN}.
     * </p>
     * <p>
     * When composing view's status is {@link #SHOW_PINYIN}, the IME's status is
     * {@link PinyinIME.ImeState#STATE_INPUT}, otherwise, the IME's status
     * should be {@link PinyinIME.ImeState#STATE_COMPOSING}.
     * </p>
     */
    enum ComposingStatus {
        SHOW_PINYIN,
        SHOW_STRING_LOWERCASE,
        EDIT_PINYIN
    };

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
        /* [in] */ CPinyinIME::DecodingInfo* decInfo,
        /* [in] */ CPinyinIME::ImeState imeStatus);

    CARAPI_(Boolean) MoveCursor(
        /* [in] */ Int32 keyCode);

    CARAPI_(ComposingStatus) GetComposingStatus();

protected:
    CARAPI OnMeasure(
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

    AutoPtr<CPinyinIME::DecodingInfo> mDecInfo;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCOMPOSINGVIEW_H__
