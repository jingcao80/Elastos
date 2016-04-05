
#include "elastos/droid/widget/CTimePickerClockDelegateHelper.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_SINGLETON_IMPL(CTimePickerClockDelegateHelper)

CAR_INTERFACE_IMPL(CTimePickerClockDelegateHelper, Singleton, ITimePickerClockDelegateHelper)

ECode CTimePickerClockDelegateHelper::GetAmPmStrings(
    /* [in] */ IContext* context,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = TimePickerClockDelegate::GetAmPmStrings(context);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
