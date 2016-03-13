
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
    CAR_INTERFACE_DECL();

    LevelListDrawable();

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
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ LevelListState* state,
        /* [in] */ IResources* res);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

private:
    LevelListDrawable(
        /* [in] */ LevelListState* state,
        /* [in] */ IResources* res);

private:
    AutoPtr<LevelListState> mLevelListState;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_LEVELLISTDRAWABLE_H__
