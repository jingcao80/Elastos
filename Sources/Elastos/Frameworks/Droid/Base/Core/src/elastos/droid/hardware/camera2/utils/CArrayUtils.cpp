
#include "elastos/droid/hardware/camera2/utils/CArrayUtils.h"
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CArrayUtils, Singleton, IArrayUtils)

CAR_SINGLETON_IMPL(CArrayUtils)

ECode CArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return ArrayUtils::GetArrayIndex(array, needle, value);
}

ECode CArrayUtils::GetArrayIndex(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 needle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return ArrayUtils::GetArrayIndex(array, needle, value);
}

ECode CArrayUtils::ConvertStringListToIntArray(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    return ArrayUtils::ConvertStringListToIntArray(list, convertFrom, convertTo, outarr);
}

ECode CArrayUtils::ConvertStringListToIntList(
    /* [in] */ IList* list,
    /* [in] */ ArrayOf<String>* convertFrom,
    /* [in] */ ArrayOf<Int32>* convertTo,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);

    return ArrayUtils::ConvertStringListToIntList(list, convertFrom, convertTo, outlist);
}

ECode CArrayUtils::ToIntArray(
    /* [in] */ IList* list,
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    return ArrayUtils::ToIntArray(list, outarr);
}

ECode CArrayUtils::Contains(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return ArrayUtils::Contains(array, elem, result);
}

ECode CArrayUtils::Contains(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* elem,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return ArrayUtils::Contains(array, elem, result);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos