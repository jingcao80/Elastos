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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_COBSERVABLESCROLLVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_COBSERVABLESCROLLVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CObservableScrollView.h"
#include <elastos/droid/widget/ScrollView.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Widget::ScrollView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A scroll view which can be observed for scroll change events.
 */
CarClass(CObservableScrollView)
    , public ScrollView
    , public IObservableScrollView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CObservableScrollView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetListener(
        /* [in] */ IObservableScrollViewListener* listener);

    CARAPI SetTouchEnabled(
        /* [in] */ Boolean touchEnabled);

    CARAPI IsScrolledToBottom(
        /* [out] */ Boolean* result);

    CARAPI IsHandlingTouchEvent(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* state);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* state);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI SetBlockFlinging(
        /* [in] */ Boolean blockFlinging);

    // @Override
    CARAPI Fling(
        /* [in] */ Int32 velocityY);

protected:
    // @Override
    CARAPI OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    // @Override
    CARAPI_(Boolean) OverScrollBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 scrollRangeX,
        /* [in] */ Int32 scrollRangeY,
        /* [in] */ Int32 maxOverScrollX,
        /* [in] */ Int32 maxOverScrollY,
        /* [in] */ Boolean isTouchEvent);

    // @Override
    CARAPI OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

private:
    CARAPI_(Int32) GetMaxScrollY();

private:
    AutoPtr<IObservableScrollViewListener> mListener;
    Int32 mLastOverscrollAmount;
    Boolean mTouchEnabled;
    Boolean mHandlingTouchEvent;
    Float mLastX;
    Float mLastY;
    Boolean mBlockFlinging;
    Boolean mTouchCancelled;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_COBSERVABLESCROLLVIEW_H__
