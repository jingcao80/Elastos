#include "COffsetIntervalHelper.h"
#include "ZoneInfo.h"

namespace Libcore {
namespace Utility {

CAR_SINGLETON_IMPL(COffsetIntervalHelper)

CAR_INTERFACE_IMPL(COffsetIntervalHelper, Singleton, IOffsetIntervalHelper)

ECode COffsetIntervalHelper::Create(
    /* [in] */ IZoneInfo* timeZone,
    /* [in] */ Int32 transitionIndex,
    /* [out] */ IOffsetInterval** result)
{
    return ZoneInfo::OffsetInterval::Create(timeZone, transitionIndex, result);
}


} // namespace Utility
} // namespace Libcore