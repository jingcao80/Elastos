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

#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_DISPLAYADJUSTMENTUTILS_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_DISPLAYADJUSTMENTUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

/**
 * Utility methods for performing accessibility display adjustments.
 */
class DisplayAdjustmentUtils
{
public:
    /**
     * Returns whether the specified user with has any display color
     * adjustments.
     */
    static CARAPI_(Boolean) HasAdjustments(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    /**
     * Applies the specified user's display color adjustments.
     */
    static CARAPI ApplyAdjustments(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

private:
    static CARAPI_(AutoPtr< ArrayOf<Float> >) Multiply(
        /* [in] */ ArrayOf<Float>* matrix,
        /* [in] */ ArrayOf<Float>* other);

    /**
     * Sets the surface flinger's Daltonization mode. This adjusts the color
     * space to correct for or simulate various types of color blindness.
     *
     * @param mode new Daltonization mode
     */
    static CARAPI SetDaltonizerMode(
        /* [in] */ Int32 mode);

    /**
     * Sets the surface flinger's color transformation as a 4x4 matrix. If the
     * matrix is null, color transformations are disabled.
     *
     * @param m the float array that holds the transformation matrix, or null to
     *            disable transformation
     */
    static CARAPI SetColorTransform(
        /* [in] */ ArrayOf<Float>* m);

private:
    static const String TAG;//= DisplayAdjustmentUtils.class.getSimpleName();

    /** Matrix and offset used for converting color to gray-scale. */
    static const AutoPtr< ArrayOf<Float> > GRAYSCALE_MATRIX;

    /** Matrix and offset used for value-only display inversion. */
    static const AutoPtr< ArrayOf<Float> > INVERSION_MATRIX_VALUE_ONLY;

    /** Default inversion mode for display color correction. */
    static const Int32 DEFAULT_DISPLAY_DALTONIZER;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_DISPLAYADJUSTMENTUTILS_H__
