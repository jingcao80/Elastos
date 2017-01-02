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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_WEIGHTEDLINEARLAYOUT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_WEIGHTEDLINEARLAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Internal::Widget::IWeightedLinearLayout;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * A special layout when measured in AT_MOST will take up a given percentage of
 * the available space.
 */
class WeightedLinearLayout
    : public LinearLayout
    , public IWeightedLinearLayout
{
public:
    CAR_INTERFACE_DECL()

    WeightedLinearLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * <p>Creates a new TableLayout for the given context and with the
     * specified set attributes.</p>
     *
     * @param context the application environment
     * @param attrs a collection of attributes
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    //@Override
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    Float mMajorWeightMin;
    Float mMinorWeightMin;
    Float mMajorWeightMax;
    Float mMinorWeightMax;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_WEIGHTEDLINEARLAYOUT_H__
