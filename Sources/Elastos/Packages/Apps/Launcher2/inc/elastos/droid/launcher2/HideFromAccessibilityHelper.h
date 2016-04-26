#ifndef  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

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