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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__

#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * Helper class for drawing a fallback background in framework decor layouts.
 * Useful for when an app has not set a window background but we're asked to draw
 * an uncovered area.
 */
class BackgroundFallback
    : public Object
    , public IBackgroundFallback
{
public:
    CAR_INTERFACE_DECL()

    BackgroundFallback();

    ~BackgroundFallback();

    CARAPI constructor();

    CARAPI SetDrawable(
        /* [in] */ IDrawable* d);

    CARAPI HasFallback(
        /* [out] */ Boolean* hasFallback);

    CARAPI Draw(
        /* [in] */ IViewGroup* root,
        /* [in] */ ICanvas* c,
        /* [in] */ IView* content);

private:
    AutoPtr<IDrawable> mBackgroundFallback;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__
