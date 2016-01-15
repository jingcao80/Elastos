
#ifndef __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__

#include "_Elastos_Droid_Internal_Os_CBatteryStatsHelperHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Core::Singleton;
using Elastos::IO::IFileInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CBatteryStatsHelperHelper)
    , public Singleton
    , public IBatteryStatsHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CheckWifiOnly(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI StatsFromFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [out] */ IBatteryStats** stats);

    CARAPI DropFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname);

    CARAPI MakemAh(
        /* [in] */ Double power,
        /* [out] */ String* result);

    CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [in] */ Int32 avail,
        /* [out, callee] */ ArrayOf<Byte>** bytes);
};

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CBATTERYSTATSHELPERHELPER_H__
