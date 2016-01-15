
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtypeHelper.h"
#include "elastos/droid/view/inputmethod/CInputMethodSubtype.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CInputMethodSubtypeHelper::
//========================================================================================
CAR_INTERFACE_IMPL(CInputMethodSubtypeHelper, Singleton, IInputMethodSubtypeHelper)

CAR_SINGLETON_IMPL(CInputMethodSubtypeHelper)

ECode CInputMethodSubtypeHelper::Sort(
    /* [in] */ IContext* context,
    /* [in] */ Int32 flags,
    /* [in] */ IInputMethodInfo* imi,
    /* [in] */ IList* subtypeList,
    /* [out] */ IList** sortedSubtypeList)
{
    VALIDATE_NOT_NULL(sortedSubtypeList)

    AutoPtr<IList> l = CInputMethodSubtype::Sort(context, flags, imi, subtypeList);
    *sortedSubtypeList = l;
    REFCOUNT_ADD(*sortedSubtypeList)
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
