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

#ifndef __ELASTOS_DROID_VIEW_ANIMATION_ALPHAANIMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_ALPHAANIMATION_H__

#include "elastos/droid/view/animation/Animation.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

class AlphaAnimation
    : public Animation
    , public IAlphaAnimation
{
public:
    CAR_INTERFACE_DECL()

    AlphaAnimation();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ Float fromAlpha,
        /* [in] */ Float toAlpha);

    CARAPI WillChangeTransformationMatrix(
        /* [out] */ Boolean* result);

    CARAPI WillChangeBounds(
        /* [out] */ Boolean* result);

    CARAPI HasAlpha(
        /* [out] */ Boolean* has);

protected:
    CARAPI_(void) ApplyTransformation(
        /* [in] */ Float interpolatedTime,
        /* [in] */ ITransformation* t);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [in] */ IAlphaAnimation* object);

private:
    Float mFromAlpha;
    Float mToAlpha;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_ALPHAANIMATION_H__
