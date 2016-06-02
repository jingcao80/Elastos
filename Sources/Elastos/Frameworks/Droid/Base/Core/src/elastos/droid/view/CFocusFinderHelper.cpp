
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CFocusFinderHelper.h"
#include "elastos/droid/view/FocusFinder.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CFocusFinderHelper, Singleton, IFocusFinderHelper)

CAR_SINGLETON_IMPL(CFocusFinderHelper)

ECode CFocusFinderHelper::GetInstance(
    /* [out] */ IFocusFinder** finder)
{
    AutoPtr<FocusFinder> result = FocusFinder::GetInstance();
    *finder = result;
    REFCOUNT_ADD(*finder)

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
