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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Animation {

/**
 * Interpolator that builds a lookup table to use. This is a fallback for
 * building a native interpolator from a TimeInterpolator that is not marked
 * with {@link HasNativeInterpolator}
 *
 * This implements TimeInterpolator to allow for easier interop with Animators
 */
// @HasNativeInterpolator
class FallbackLUTInterpolator
    : public Object
    , public IFallbackLUTInterpolator
    , public INativeInterpolatorFactory
    , public ITimeInterpolator
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Used to cache the Float[] LUT for use across multiple native
     * interpolator creation
     */
    CARAPI constructor(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

    // @Override
    CARAPI CreateNativeInterpolator(
        /* [out] */ Int64* result);

    /**
     * Used to create a one-shot Float[] LUT & native interpolator
     */
    static CARAPI_(Int64) CreateNativeInterpolator(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

    // @Override
    CARAPI GetInterpolation(
        /* [in] */ Float input,
        /* [out] */ Float* result);

    CARAPI HasNativeInterpolator(
        /* [out] */ Boolean* res);

private:
    static CARAPI_(AutoPtr<ArrayOf<Float> >) CreateLUT(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

private:
    AutoPtr<ITimeInterpolator> mSourceInterpolator;
    AutoPtr<ArrayOf<Float> > mLut;
};

} //namespace Animation
} //namespace View
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__
