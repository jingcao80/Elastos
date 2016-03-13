#ifndef  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class PagedViewGridLayout
    : public GridLayout
    , public IPage
{
public:
    class LayoutParams
        : public FrameLayout::LayoutParams
    {
    public:
        LayoutParams(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);
    };

public:
    CAR_INTERFACE_DECL();

    PagedViewGridLayout(
        /* [in] */ IContext* context,
        /* [in] */ Int32 cellCountX,
        /* [in] */ Int32 cellCountY);

    CARAPI_(Int32) GetCellCountX();

    CARAPI_(Int32) GetCellCountY();

    /**
     * Clears all the key listeners for the individual widgets.
     */
    CARAPI ResetChildrenOnKeyListeners();

    CARAPI SetOnLayoutListener(
        /* [in] */ IRunnable* r);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RemoveAllViewsOnPage();

    //@Override
    CARAPI RemoveViewOnPageAt(
        /* [in] */ Int32 index);

    //@Override
    CARAPI GetPageChildCount(
        /* [out] */ Int32* count);

    //@Override
    CARAPI GetChildOnPageAt(
        /* [in] */ Int32 i,
        /* [out] */ IView** view);

    //@Override
    CARAPI IndexOfChildOnPage(
        /* [in] */ IView* v,
        /* [out] */ Int32* index);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnDetachedFromWindow();

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

protected:
    static const String TAG;

private:
    Int32 mCellCountX;
    Int32 mCellCountY;
    AutoPtr<IRunnable> mOnLayoutListener;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PAGEDVIEWGRIDLAYOUT_H__