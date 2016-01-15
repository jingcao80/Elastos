#include "CChoiceFormatHelper.h"
#include "ChoiceFormat.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CChoiceFormatHelper, Singleton, IChoiceFormatHelper)

CAR_SINGLETON_IMPL(CChoiceFormatHelper)

ECode CChoiceFormatHelper::NextDouble(
    /* [in] */ Double value,
    /* [out] */ Double* nextValue)
{
    VALIDATE_NOT_NULL(nextValue)
    *nextValue = ChoiceFormat::NextDouble(value);
    return NOERROR;
}

ECode CChoiceFormatHelper::NextDouble(
    /* [in] */ Double value,
    /* [in] */ Boolean increment,
    /* [out] */ Double* nextValue)
{
    VALIDATE_NOT_NULL(nextValue)
    *nextValue = ChoiceFormat::NextDouble(value,increment);
    return NOERROR;
}

ECode CChoiceFormatHelper::PreviousDouble(
    /* [in] */ Double value,
    /* [out] */ Double* previousValue)
{
    VALIDATE_NOT_NULL(previousValue)
    *previousValue = ChoiceFormat::PreviousDouble(value);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos