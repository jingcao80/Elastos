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

#ifndef __ELASTOS_DROID_TEXT_METHOD_LINKMOVEMENTMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_LINKMOVEMENTMETHOD_H__

#include "elastos/droid/text/method/ScrollingMovementMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * A movement method that traverses links in the text buffer and scrolls if necessary.
 * Supports clicking on links with DPad Center or Enter.
 */
class LinkMovementMethod
    : public ScrollingMovementMethod
    , public ILinkMovementMethod
{
public:
    LinkMovementMethod();

    virtual ~LinkMovementMethod();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    //@Override
    CARAPI CanSelectArbitrarily(
        /* [out] */ Boolean* ret);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ IMotionEvent* event,
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
    CARAPI_(Boolean) Up(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Down(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Left(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

    //@Override
    CARAPI_(Boolean) Right(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

private:
    CARAPI_(Boolean) Action(Int32 what,
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer);

private:
    static const Int32 CLICK;// = 1;
    static const Int32 UP;// = 2;
    static const Int32 DOWN;// = 3;

private:
    static AutoPtr<ILinkMovementMethod> sInstance;

    static AutoPtr<IInterface> FROM_BELOW;// = new NoCopySpan.Concrete();
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_LINKMOVEMENTMETHOD_H__
