#ifndef  __ELASTOS_DROID_LAUNCHER2_BUTTONDROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_BUTTONDROPTARGET_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TextView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Implements a DropTarget.
 */
class ButtonDropTarget
    : public TextView
    , public IButtonDropTarget
    , public IDropTarget
    , public IDragControllerDragListener
{
public:
    CAR_INTERFACE_DECL();

    ButtonDropTarget();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetLauncher(
        /* [in] */ ILauncher* launcher);

    CARAPI AcceptDrop(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ Boolean* result);

    CARAPI SetSearchDropTargetBar(
        /* [in] */ ISearchDropTargetBar* searchDropTargetBar);

    CARAPI OnDrop(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnFlingToDelete(
        /* [in] */ IDropTargetDragObject* d,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPointF* vec);

    CARAPI OnDragEnter(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnDragOver(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnDragExit(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    CARAPI IsDropEnabled(
        /* [out] */ Boolean* result);

    CARAPI OnDragEnd();

    //@Override
    CARAPI GetHitRect(
        /* [in] */ IRect* outRect);

    CARAPI GetIconRect(
        /* [in] */ Int32 viewWidth,
        /* [in] */ Int32 viewHeight,
        /* [in] */ Int32 drawableWidth,
        /* [in] */ Int32 drawableHeight,
        /* [out] */ IRect** rect);

    //@Override
    virtual CARAPI GetDropTargetDelegate(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ IDropTarget** target);

    CARAPI GetLocationInDragLayer(
        /* [in] */ ArrayOf<Int32>* loc);

protected:
    CARAPI GetCurrentDrawable(
        /* [out] */ IDrawable** drawable);

private:
    CARAPI_(Boolean) IsRtl();

protected:
    Int32 mTransitionDuration;

    ILauncher* mLauncher;

    AutoPtr<ITextView> mText;
    ISearchDropTargetBar* mSearchDropTargetBar;     // weak-ref

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

#endif // __ELASTOS_DROID_LAUNCHER2_BUTTONDROPTARGET_H__