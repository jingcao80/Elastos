
#include "elastos/droid/widget/CNumberPickerHelper.h"
#include "elastos/droid/widget/NumberPicker.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CNumberPickerHelper, Singleton, INumberPickerHelper);

CAR_SINGLETON_IMPL(CNumberPickerHelper);

ECode CNumberPickerHelper::GetTwoDigitFormatter(
    /* [out] */ INumberPickerFormatter** format)
{
    VALIDATE_NOT_NULL(format);
    AutoPtr<INumberPickerFormatter> formatter = NumberPicker::GetTwoDigitFormatter();
    *format = formatter;
    REFCOUNT_ADD(*format);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
