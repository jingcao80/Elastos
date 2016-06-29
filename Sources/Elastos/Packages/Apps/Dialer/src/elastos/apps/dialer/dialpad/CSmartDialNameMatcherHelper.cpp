
#include "elastos/apps/dialer/dialpad/CSmartDialNameMatcherHelper.h"
#include "elastos/apps/dialer/dialpad/SmartDialNameMatcher.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CSmartDialNameMatcherHelper, Singleton, ISmartDialNameMatcherHelper);

CAR_SINGLETON_IMPL(CSmartDialNameMatcherHelper);

ECode CSmartDialNameMatcherHelper::NormalizeNumber(
    /* [in] */ const String& number,
    /* [in] */ ISmartDialMap* map,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmartDialNameMatcher::NormalizeNumber(number, map);
    return NOERROR;
}

ECode CSmartDialNameMatcherHelper::NormalizeNumber(
    /* [in] */ String number,
    /* [in] */ Int32 offset,
    /* [in] */ ISmartDialMap* map,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmartDialNameMatcher::NormalizeNumber(number, offset, map);
    return NOERROR;
}

ECode CSmartDialNameMatcherHelper::GetLATIN_SMART_DIAL_MAP(
    /* [out] */ ISmartDialMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = SmartDialNameMatcher::LATIN_SMART_DIAL_MAP;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
