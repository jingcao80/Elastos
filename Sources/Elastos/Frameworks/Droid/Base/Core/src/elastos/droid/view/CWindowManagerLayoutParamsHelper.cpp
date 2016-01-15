
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CWindowManagerLayoutParamsHelper.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CWindowManagerLayoutParamsHelper);
CAR_INTERFACE_IMPL(CWindowManagerLayoutParamsHelper, Singleton, IWindowManagerLayoutParamsHelper);
CWindowManagerLayoutParamsHelper::MayUseInputMethod(
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* mayUse)
{
    VALIDATE_NOT_NULL(mayUse);
    *mayUse = CWindowManagerLayoutParams::MayUseInputMethod(flags);
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
