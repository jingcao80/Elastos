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

#ifndef __ELASTOS_DROID_TEXT_METHOD_SCROLLINGMOVEMENTMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_SCROLLINGMOVEMENTMETHOD_H__

#include "elastos/droid/text/method/BaseMovementMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * A movement method that interprets movement keys by scrolling the text buffer.
 */
class ScrollingMovementMethod
    : public BaseMovementMethod
    , public IScrollingMovementMethod
{
public:
    ScrollingMovementMethod();

    virtual ~ScrollingMovementMethod();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* ret);

    //@Override
    CARAPI OnTakeFocus(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 dir);

    static CARAPI GetInstance(
        /* [out] */ IMovementMethod** ret);

protected:
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

    //@Override
    CARAPI_(Boolean) Home(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) End(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

private:
    static AutoPtr<IScrollingMovementMethod> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_ScrollingMovementMethod_H__
