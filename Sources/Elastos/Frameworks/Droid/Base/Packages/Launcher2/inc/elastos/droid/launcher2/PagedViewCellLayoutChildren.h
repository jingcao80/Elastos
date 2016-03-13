#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * An abstraction of the original CellLayout which supports laying out items
 * which span multiple cells into a grid-like layout.  Also supports dimming
 * to give a preview of its contents.
 */
class PagedViewCellLayoutChildren
    : public ViewGroup
    , public IPagedViewCellLayoutChildren
{
public:
    CAR_INTERFACE_DECL();

    PagedViewCellLayoutChildren(
        /* [in] */ IContext* context);

    //@Override
    CARAPI CancelLongPress();

    CARAPI SetGap(
        /* [in] */ Int32 widthGap,
        /* [in] */ Int32 heightGap);

    CARAPI SetCellDimensions(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI EnableCenteredContent(
        /* [in] */ Boolean enabled);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI SetChildrenDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

protected:
    static const String TAG;

private:
    Boolean mCenterContent;

    Int32 mCellWidth;
    Int32 mCellHeight;
    Int32 mWidthGap;
    Int32 mHeightGap;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWCELLLAYOUTCHILDREN_H__