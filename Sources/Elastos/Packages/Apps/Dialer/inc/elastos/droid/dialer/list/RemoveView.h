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

#ifndef __ELASTOS_DROID_DIALER_LIST_REMOVEVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_REMOVEVIEW_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/widget/FrameLayout.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

class RemoveView
    : public FrameLayout
    , public IRemoveView
{
public:
    CAR_INTERFACE_DECL();

    RemoveView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetDragDropController(
        /* [in] */ IDragDropController* controller);

    // @Override
    CARAPI OnDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) SetAppearanceNormal();

    CARAPI_(void) SetAppearanceHighlighted();

public:
    AutoPtr<IDragDropController> mDragDropController;
    AutoPtr<ITextView> mRemoveText;
    AutoPtr<IImageView> mRemoveIcon;
    Int32 mUnhighlightedColor;
    Int32 mHighlightedColor;
    AutoPtr<IDrawable> mRemoveDrawable;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_REMOVEVIEW_H__
