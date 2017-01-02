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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::IPreferenceImageView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PreferenceImageView
    : public ImageView
    , public IPreferenceImageView
{
public:
    CAR_INTERFACE_DECL()

    PreferenceImageView();

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

protected:
    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__

