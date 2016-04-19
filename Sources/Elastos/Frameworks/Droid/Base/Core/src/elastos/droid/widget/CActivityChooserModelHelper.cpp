
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ActivityChooserModel.h"
#include "elastos/droid/widget/CActivityChooserModelHelper.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_SINGLETON_IMPL(CActivityChooserModelHelper)

CAR_INTERFACE_IMPL(CActivityChooserModelHelper, Singleton, IActivityChooserModelHelper)

ECode CActivityChooserModelHelper::Get(
    /* [in] */ IContext* context,
    /* [in] */ const String& historyFileName,
    /* [out] */ IActivityChooserModel** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IActivityChooserModel> temp = ActivityChooserModel::Get(context, historyFileName);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
