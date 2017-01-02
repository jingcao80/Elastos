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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class LinearLayoutWithDefaultTouchRecepient
    : public LinearLayout
    , public ILinearLayoutWithDefaultTouchRecepient
{
public:
    CAR_INTERFACE_DECL()

    LinearLayoutWithDefaultTouchRecepient();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetDefaultTouchRecepient(
        /* [in] */ IView* defaultTouchRecepient);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IRect> mTempRect;
    AutoPtr<IView> mDefaultTouchRecepient;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__
