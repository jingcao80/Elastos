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

#ifndef __ELASTOS_DROID_WIDGET_TEXTVIEWWITHCIRCULARINDICATOR_H__
#define __ELASTOS_DROID_WIDGET_TEXTVIEWWITHCIRCULARINDICATOR_H__

#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Widget {

class TextViewWithCircularIndicator
    : public TextView
    , public ITextViewWithCircularIndicator
{
public:
    CAR_INTERFACE_DECL()

    TextViewWithCircularIndicator();

    ~TextViewWithCircularIndicator();

    CARAPI constructor(
        /* [in] */ IContext* context);

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

    CARAPI SetCircleColor(
        /* [in] */ Int32 color);

    CARAPI SetDrawIndicator(
        /* [in] */ Boolean drawIndicator);

    //@Override
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    virtual CARAPI GetContentDescription(
        /* [out] */ ICharSequence** text);

private:
    CARAPI_(void) Init();

private:
    static const Int32 SELECTED_CIRCLE_ALPHA;

    AutoPtr<IPaint> mCirclePaint;
    String mItemIsSelectedText;
    Int32 mCircleColor;
    Boolean mDrawIndicator;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
