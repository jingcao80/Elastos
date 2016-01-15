
#ifndef __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__

#include "_Elastos_Droid_Internal_Net_CNetworkStatsFactoryHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::INetworkStats;
using Elastos::IO::IFile;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CNetworkStatsFactoryHelper)
    , public Singleton
    , public INetworkStatsFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NoteStackedIface(
        /* [in] */ const String& stackedIface,
        /* [in] */ const String& baseIface);

    /**
     * Parse and return {@link NetworkStats} with UID-level details. Values are
     * expected to monotonically increase since device boot.
     */
    // @VisibleForTesting
    CARAPI JavaReadNetworkStatsDetail(
        /* [in] */ IFile* detailPath,
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [out] */ INetworkStats** stats);

    /**
     * Parse statistics from file into given {@link NetworkStats} object. Values
     * are expected to monotonically increase since device boot.
     */
    // @VisibleForTesting
    CARAPI NativeReadNetworkStatsDetail(
        /* [in] */ INetworkStats* stats,
        /* [in] */ const String& path,
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [out] */ Int32* result);
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORYHELPER_H__
