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

#ifndef __ELASTOS_DROID_TEXT_METHOD_ARROWKEYMOVEMENTMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_ARROWKEYMOVEMENTMETHOD_H__

#include "elastos/droid/text/method/BaseMovementMethod.h"

using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * A movement method that provides cursor movement and selection.
 * Supports displaying the context menu on DPad Center.
 */
class ArrowKeyMovementMethod
    : public BaseMovementMethod
    , public IArrowKeyMovementMethod
{
public:
    CAR_INTERFACE_DECL()

    virtual ~ArrowKeyMovementMethod();

    CARAPI constructor();

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* ret);

    //@Override
    CARAPI CanSelectArbitrarily(
        /* [out] */ Boolean* ret);

    //@Override
    CARAPI Initialize(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text);

    //@Override
    CARAPI OnTakeFocus(
        /* [in] */ ITextView* view,
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 dir);

    static CARAPI GetInstance(
        /* [out] */ IMovementMethod** ret);

protected:
    //@Override
    CARAPI_(Boolean) HandleMovementKey(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 movementMetaState,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) Left(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Right(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Up(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Down(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) PageUp(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) PageDown(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Top(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Bottom(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) LineStart(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) LineEnd(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /** {@hide} */
    //@Override
    CARAPI_(Boolean) LeftWord(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    /** {@hide} */
    //@Override
    CARAPI_(Boolean) RightWord(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Home(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) End(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

private:
    static CARAPI_(Boolean) IsSelecting(
        /* [in] */ ISpannable* buffer);

    static CARAPI_(Int32) GetCurrentLineTop(
        /* [in] */ ISpannable* buffer,
        /* [in] */ ILayout* layout);

    static CARAPI_(Int32) GetPageHeight(
        /* [in] */ ITextView* widget);

    static CARAPI_(Boolean) IsTouchSelecting(
        /* [in] */ Boolean isMouse,
        /* [in] */ ISpannable* buffer);

private:
    static const AutoPtr<IInterface> LAST_TAP_DOWN;
    static AutoPtr<IArrowKeyMovementMethod> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_ARROWKEYMOVEMENTMETHOD_H__
