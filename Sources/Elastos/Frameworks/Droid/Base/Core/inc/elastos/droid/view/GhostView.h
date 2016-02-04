#ifndef __ELASTOS_DROID_VIEW_GHOSTVIEW_H__
#define __ELASTOS_DROID_VIEW_GHOSTVIEW_H__

#include "elastos/droid/view/View.h"

using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace View {

class GhostView
    : public View
    , public IGhostView
{
public:
    GhostView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IView* view);

    CARAPI SetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    static CARAPI_(void) CalculateMatrix(
        /* [in] */ IView* view,
        /* [in] */ IViewGroup* host,
        /* [in] */ IMatrix* matrix);

    static CARAPI AddGhost(
        /* [in] */ IView* view,
        /* [in] */ IViewGroup* viewGroup,
        /* [in] */ IMatrix* matrix,
        /* [out] */ IGhostView** res);

    static CARAPI_(AutoPtr<IGhostView>) AddGhost(
        /* [in] */ IView* view,
        /* [in] */ IViewGroup* viewGroup);

    static CARAPI_(void) RemoveGhost(
        /* [in] */ IView* view);

    static CARAPI_(AutoPtr<IGhostView>) GetGhost(
        /* [in] */ IView* view);

protected:
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) SetGhostedVisibility(
        /* [in] */ Int32 visibility);

    static CARAPI_(void) CopySize(
        /* [in] */ IView* from,
        /* [in] */ IView* to);

    static CARAPI_(Int32) MoveGhostViewsToTop(
        /* [in] */ IViewGroup* viewGroup,
        /* [in] */ IArrayList* tempViews);

    static CARAPI_(void) InsertIntoOverlay(
        /* [in] */ IViewGroup* viewGroup,
        /* [in] */ IViewGroup* wrapper,
        /* [in] */ IGhostView* ghostView,
        /* [in] */ IArrayList* tempParents,
        /* [in] */ Int32 firstGhost);

    static CARAPI_(Int32) GetInsertIndex(
        /* [in] */ IViewGroup* overlayViewGroup,
        /* [in] */ IArrayList* viewParents,
        /* [in] */ IArrayList* tempParents,
        /* [in] */ Int32 firstGhost);

    static CARAPI_(Boolean) IsGhostWrapper(
        /* [in] */ IView* view);

    static CARAPI_(Boolean) IsOnTop(
        /* [in] */ IArrayList* viewParents,
        /* [in] */ IArrayList* comparedWith);

    static CARAPI_(void) GetParents(
        /* [in] */ IView* view,
        /* [in] */ IArrayList* parents);

    static CARAPI_(Boolean) IsOnTop(
        /* [in] */ IView* view,
        /* [in] */ IView* comparedWith);

private:
    IView* mView; //TODO prevent circular references

    Int32 mReferences;

    Boolean mBeingMoved;
};

} // namespace View
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_VIEW_GHOSTVIEW_H__