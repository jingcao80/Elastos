
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityWindowInfoHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityWindowInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityWindowInfoHelper, Singleton, IAccessibilityWindowInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityWindowInfoHelper)

ECode CAccessibilityWindowInfoHelper::Obtain(
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CAccessibilityWindowInfo::Obtain(info);
}

ECode CAccessibilityWindowInfoHelper::Obtain(
    /* [in] */ IAccessibilityWindowInfo* info,
    /* [out] */ IAccessibilityWindowInfo** cloneInfo)
{
    VALIDATE_NOT_NULL(cloneInfo);
    return CAccessibilityWindowInfo::Obtain(info, cloneInfo);
}

} // Accessibility
} // View
} // Droid
} // Elastos
