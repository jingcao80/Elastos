#ifndef  __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__
#define  __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class ShortcutAndWidgetContainer
    : public ViewGroup
    , public IShortcutAndWidgetContainer
{
public:
    CAR_INTERFACE_DECL();

    ShortcutAndWidgetContainer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetCellDimensions(
        /* [in] */ Int32 cellWidth,
        /* [in] */ Int32 cellHeight,
        /* [in] */ Int32 widthGap,
        /* [in] */ Int32 heightGap,
        /* [in] */ Int32 countX);

    CARAPI_(AutoPtr<IView>) GetChildAt(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI SetupLp(
        /* [in] */ CellLayout::LayoutParams* lp);

    // Set whether or not to invert the layout horizontally if the layout is in RTL mode.
    CARAPI SetInvertIfRtl(
        /* [in] */ Boolean invert);

    CARAPI MeasureChild(
        /* [in] */ IView* child);

    CARAPI_(Boolean) IsLayoutRtl();

    //@Override
    CARAPI_(Boolean) ShouldDelayChildPressedState();

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    //@Override
    CARAPI CancelLongPress();

protected:
     //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

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

    //@Override
    CARAPI SetChildrenDrawnWithCacheEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(Boolean) InvertLayoutHorizontally();

public:
    static const String TAG;

private:
    AutoPtr<IWallpaperManager> mWallpaperManager;

    Int32 mCellWidth;
    Int32 mCellHeight;

    Int32 mWidthGap;
    Int32 mHeightGap;

    Int32 mCountX;

    Boolean mInvertIfRtl;

    // These are temporary variables to prevent having to allocate a new object just to
    // return an (x, y) value from helper functions. Do NOT use them to maintain other state.
    AutoPtr<ArrayOf<Int32> > mTmpCellXY;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SHORTCUTANDWIDGETCONTAINER_H__