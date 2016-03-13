#ifndef  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Implements a DropTarget.
 */
class ButtonDropTarget
    : public TextView
    , public IDropTarget
    , public IDragControllerDragListener
{
public:
    ButtonDropTarget(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ButtonDropTarget(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI_(void) SetLauncher(
        /* [in] */ ILauncher* launcher);

    CARAPI_(Boolean) AcceptDrop(
        /* [in] */ DragObject* d);

    CARAPI_(void) SetSearchDropTargetBar(
        /* [in] */ ISearchDropTargetBar* searchDropTargetBar);

    CARAPI_(void) OnDrop(
        /* [in] */ DragObject* d);

    CARAPI_(void) OnFlingToDelete(
        /* [in] */ DragObject* d,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPointF* vec);

    CARAPI_(void) OnDragEnter(
        /* [in] */ DragObject* d);

    CARAPI_(void) OnDragOver(
        /* [in] */ DragObject* d);

    CARAPI_(void) OnDragExit(
        /* [in] */ DragObject* d);

    CARAPI_(void) OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    CARAPI_(Boolean) IsDropEnabled();

    CARAPI_(void) OnDragEnd();

    //@Override
    CARAPI_(void) GetHitRect(
        /* [in] */ IRect* outRect);

    CARAPI_(AutoPtr<IRect>) GetIconRect(
        /* [in] */ Int32 viewWidth,
        /* [in] */ Int32 viewHeight,
        /* [in] */ Int32 drawableWidth,
        /* [in] */ Int32 drawableHeight);

    //@Override
    CARAPI_(AutoPtr<DropTarget>) GetDropTargetDelegate(
        /* [in] */ DragObject* d);

    CARAPI_(void) GetLocationInDragLayer(
        /* [in] */ ArrayOf<Int32>* loc);

protected:
    CARAPI_(AutoPtr<IDrawable>) GetCurrentDrawable();

private:
    CARAPI_(Boolean) IsRtl();

protected:
    Int32 mTransitionDuration;

    AutoPtr<ILauncher> mLauncher;

    AutoPtr<ITextView> mText;
    AutoPtr<ISearchDropTargetBar> mSearchDropTargetBar;

    /** Whether this drop target is active for the current drag */
    Boolean mActive;

    /** The paint applied to the drag view on hover */
    Int32 mHoverColor;

private:
    Int32 mBottomDragPadding;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_APPLICATIOMINFO_H__