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

#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CCLIPRECTANIMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CCLIPRECTANIMATION_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_View_Animation_CClipRectAnimation.h"
#include "elastos/droid/view/animation/Animation.h"

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

/**
 * An animation that controls the clip of an object. See the
 * {@link android.view.animation full package} description for details and
 * sample code.
 *
 * @hide
 */
CarClass(CClipRectAnimation)
    , public Animation
{
public:
    CAR_OBJECT_DECL()

    CClipRectAnimation();

    /**
     * Constructor to use when building a ClipRectAnimation from code
     *
     * @param fromClip the clip rect to animate from
     * @param toClip the clip rect to animate to
     */
    CARAPI constructor(
        /* [in] */ IRect* fromClip,
        /* [in] */ IRect* toClip);

    // @Override
    CARAPI WillChangeTransformationMatrix(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI_(void) ApplyTransformation(
        /* [in] */ Float it,
        /* [in] */ ITransformation* tr);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [in] */ CClipRectAnimation* object);

private:
    AutoPtr<IRect> mFromRect;
    AutoPtr<IRect> mToRect;
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif // __ELASTOS_DROID_VIEW_ANIMATION_CCLIPRECTANIMATION_H__
