
#include "elastos/droid/hardware/camera2/utils/CListUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CListUtils, Singleton, IListUtils)

CAR_SINGLETON_IMPL(CListUtils)

ECode CListUtils::ListContains(
    /* [in] */ IList* list,
    /* [in] */ IInterface* needle,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return ListUtils::ListContains(list, needle, value);
}

ECode CListUtils::ListElementsEqualTo(
    /* [in] */ IList* list,
    /* [in] */ IInterface* single,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return ListUtils::ListElementsEqualTo(list, single, value);
}

ECode CListUtils::ListToString(
    /* [in] */ IList* list,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return ListUtils::ListToString(list, str);
}

ECode CListUtils::ListSelectFirstFrom(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<IInterface*>* choices,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    return ListUtils::ListSelectFirstFrom(list, choices, outface);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos