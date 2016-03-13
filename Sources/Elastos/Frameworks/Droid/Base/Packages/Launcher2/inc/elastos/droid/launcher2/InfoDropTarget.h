#ifndef  __ELASTOS_DROID_LAUNCHER2_INFODROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_INFODROPTARGET_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class InfoDropTarget
    : public ButtonDropTarget
    , public IInfoDropTarget
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI AcceptDrop(
        /* [in] */ DragObject* d,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    //@Override
    CARAPI OnDragEnd();

    CARAPI OnDragEnter(
        /* [in] */ DragObject* d);

    CARAPI OnDragExit(
        /* [in] */ DragObject* d);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) IsFromAllApps(
        /* [in] */ IDragSource* source);

private:
    AutoPtr<IColorStateList> mOriginalTextColor;
    AutoPtr<ITransitionDrawable> mDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_INFODROPTARGET_H__