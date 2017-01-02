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

#ifndef  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * The ListAdapter which presents all the valid widgets that can be created for a given drop.
 */
class HolographicViewHelper
    : public Object
{
public:
    HolographicViewHelper(
        /* [in] */ IContext* context);

    /**
     * Generate the pressed/focused states if necessary.
     */
    CARAPI_(void) GeneratePressedFocusedStates(
        /* [in] */ IImageView* v);

    /**
     * Invalidates the pressed/focused states.
     */
    CARAPI_(void) InvalidatePressedFocusedStates(
        /* [in] */ IImageView* v);

    /**
     * Creates a copy of the original image.
     */
private:
    CARAPI_(AutoPtr<IBitmap>) CreateOriginalImage(
        /* [in] */ IImageView* v,
        /* [in] */ ICanvas* canvas);

    /**
     * Creates a new press state image which is the old image with a blue overlay.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI_(AutoPtr<IBitmap>) CreatePressImage(
        /* [in] */ IImageView* v,
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<ICanvas> mTempCanvas;

    Boolean mStatesUpdated;
    Int32 mHighlightColor;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__