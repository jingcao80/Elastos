
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/os/CBatteryStatsHelperHelper.h"
#include "elastos/droid/internal/os/BatteryStatsHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CBatteryStatsHelperHelper, Singleton, IBatteryStatsHelperHelper)

CAR_SINGLETON_IMPL(CBatteryStatsHelperHelper)

ECode CBatteryStatsHelperHelper::CheckWifiOnly(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = BatteryStatsHelper::CheckWifiOnly(context);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::StatsFromFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname,
    /* [out] */ IBatteryStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    AutoPtr<IBatteryStats> bs = BatteryStatsHelper::StatsFromFile(context, fname);
    *stats = bs;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::DropFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname)
{
    BatteryStatsHelper::DropFile(context, fname);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::MakemAh(
    /* [in] */ Double power,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = BatteryStatsHelper::MakemAh(power);
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = BatteryStatsHelper::ReadFully(stream);
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CBatteryStatsHelperHelper::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [in] */ Int32 avail,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = BatteryStatsHelper::ReadFully(stream, avail);
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
