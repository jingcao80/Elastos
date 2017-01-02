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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_LEVELLISTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_LEVELLISTDRAWABLE_H__

#include "elastos/droid/graphics/drawable/DrawableContainer.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class LevelListDrawable
    : public DrawableContainer
    , public ILevelListDrawable
{
protected:
    class LevelListState : public DrawableContainer::DrawableContainerState
    {
    public:
        LevelListState(
            /* [in] */ LevelListState* orig,
            /* [in] */ LevelListDrawable* owner,
            /* [in] */ IResources* res);

        CARAPI_(void) AddLevel(
            /* [in] */ Int32 low,
            /* [in] */ Int32 high,
            /* [in] */ IDrawable* drawable);

        CARAPI_(Int32) IndexOfLevel(
            /* [in] */ Int32 level);

        //@Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI_(void) GrowArray(
            /* [in] */ Int32 oldSize,
            /* [in] */ Int32 newSize);

    public:
        AutoPtr< ArrayOf<Int32> > mLows;
        AutoPtr< ArrayOf<Int32> > mHighs;
    };

public:
    CAR_INTERFACE_DECL()

    LevelListDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    virtual CARAPI AddLevel(
        /* [in] */ Int32 low,
        /* [in] */ Int32 high,
        /* [in] */ IDrawable* drawable);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    //@Override
    CARAPI Mutate();

protected:
    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

private:
    AutoPtr<LevelListState> mLevelListState;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_LEVELLISTDRAWABLE_H__
