#ifndef  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class HideFromAccessibilityHelper
    : public Object
    , public IViewGroupOnHierarchyChangeListener
{
public:
    CAR_INTERFACE_DECL();

    HideFromAccessibilityHelper();

    CARAPI SetImportantForAccessibilityToNo(
        /* [in] */ IView* v,
        /* [in] */ Boolean onlyAllApps);

    CARAPI RestoreImportantForAccessibility(
        /* [in] */ IView* v);

    CARAPI OnChildViewAdded(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    CARAPI OnChildViewRemoved(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

private:
    CARAPI_(void) SetImportantForAccessibilityToNoHelper(
        /* [in] */ IView* v);

    CARAPI_(void) RestoreImportantForAccessibilityHelper(
        /* [in] */ IView* v);

    CARAPI_(Boolean) IncludeView(
        /* [in] */ IView* v);

    CARAPI_(Boolean) HasAncestorOfType(
        /* [in] */ IView* v,
        /* [in] */ ClassID c);

protected:
    Boolean mHide;
    Boolean mOnlyAllApps;

private:
    AutoPtr<IHashMap> mPreviousValues;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__