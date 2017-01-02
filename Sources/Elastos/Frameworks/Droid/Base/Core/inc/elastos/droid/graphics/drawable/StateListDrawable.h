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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_STATELISTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_STATELISTDRAWABLE_H__

#include "elastos/droid/graphics/drawable/DrawableContainer.h"
#include "elastos/droid/utility/StateSet.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class StateListDrawable
    : public DrawableContainer
    , public IStateListDrawable
{
public:
    class StateListState : public DrawableContainer::DrawableContainerState
    {
    public:
        StateListState(
            /* [in] */ StateListState* orig,
            /* [in] */ StateListDrawable* owner,
            /* [in] */ IResources* res);

        ~StateListState();

        CARAPI_(Int32) AddStateSet(
            /* [in] */ ArrayOf<Int32>* stateSet,
            /* [in] */ IDrawable* drawable);

        CARAPI_(Int32) IndexOfStateSet(
            /* [in] */ ArrayOf<Int32>* stateSet);

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
        AutoPtr<ArrayOf< AutoPtr<ArrayOf<Int32> > > > mStateSets;
    };

public:
    CAR_INTERFACE_DECL()

    StateListDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    /**
     * This constructor exists so subclasses can avoid calling the default
     * constructor and setting up a StateListDrawable-specific constant state.
     */
    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state);

    /**
     * Add a new image/string ID to the set of images.
     *
     * @param stateSet - An array of resource Ids to associate with the image.
     *                 Switch to this image by calling setState().
     * @param drawable -The image to show.
     */
    virtual CARAPI AddState(
        /* [in] */ ArrayOf<Int32>* stateSet,
        /* [in] */ IDrawable* drawable);

    //@Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    virtual CARAPI_(AutoPtr<StateListState>) GetStateListState();

    /**
     * Gets the number of states contained in this drawable.
     *
     * @return The number of states contained in this drawable.
     * @hide pending API council
     * @see #getStateSet(int)
     * @see #getStateDrawable(int)
     */
    virtual CARAPI GetStateCount(
        /* [out] */ Int32* count);

    /**
     * Gets the state set at an index.
     *
     * @param index The index of the state set.
     * @return The state set at the index.
     * @hide pending API council
     * @see #getStateCount()
     * @see #getStateDrawable(int)
     */
    virtual CARAPI GetStateSet(
        /* [in] */ Int32 index,
        /* [out, callee] */ ArrayOf<Int32>** stateSet);

    /**
     * Gets the drawable at an index.
     *
     * @param index The index of the drawable.
     * @return The drawable at the index.
     * @hide pending API council
     * @see #getStateCount()
     * @see #getStateSet(int)
     */
    virtual CARAPI GetStateDrawable(
        /* [in] */ Int32 index,
        /* [out] */ IDrawable** drawable);

    /**
     * Gets the index of the drawable with the provided state set.
     *
     * @param stateSet the state set to look up
     * @return the index of the provided state set, or -1 if not found
     * @hide pending API council
     * @see #getStateDrawable(int)
     * @see #getStateSet(int)
     */
    virtual CARAPI GetStateDrawableIndex(
        /* [in] */ ArrayOf<Int32>* stateSet,
        /* [out] */ Int32* index);

    //@Override
    CARAPI Mutate();

    /** @hide */
    //@Override
    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    CARAPI_(void) SetConstantState(
        /* [in] */ StateListState* state);

protected:
    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

private:
    /**
     * To be proper, we should have a getter for dither (and alpha, etc.)
     * so that proxy classes like this can save/restore their delegates'
     * values, but we don't have getters. Since we do have setters
     * (e.g. setDither), which this proxy forwards on, we have to have some
     * default/initial setting.
     *
     * The initial setting for dither is now true, since it almost always seems
     * to improve the quality at negligible cost.
     */
    static const Boolean DEFAULT_DITHER = TRUE;
    static Boolean DEBUG;
    static String TAG;
    AutoPtr<StateListState> mStateListState;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_STATELISTDRAWABLE_H__

