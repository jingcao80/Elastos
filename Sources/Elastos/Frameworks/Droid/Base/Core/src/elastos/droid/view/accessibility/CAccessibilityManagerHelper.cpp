
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityManagerHelper, Singleton, IAccessibilityManagerHelper)

CAR_SINGLETON_IMPL(CAccessibilityManagerHelper)

ECode CAccessibilityManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IAccessibilityManager** manager)
{
    VALIDATE_NOT_NULL(manager);
    return CAccessibilityManager::GetInstance(context, manager);
}

} // Accessibility
} // View
} // Droid
} // Elastos
