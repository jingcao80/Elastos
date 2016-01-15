#ifndef __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__
#define __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__

#include "_Elastos_Droid_Server_CNetworkManagementSocketTaggerHelper.h"
#include "Elastos.Droid.Core.Server.h"
#include "elastos/droid/server/NetworkManagementSocketTagger.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Server::INetworkManagementSocketTagger;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CNetworkManagementSocketTaggerHelper)
    , public Singleton
    , public INetworkManagementSocketTagger
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Install();

    CARAPI SetThreadSocketStatsTag(
        /* [in] */ Int32 tag);

    CARAPI GetThreadSocketStatsTag(
        /* [out] */ Int32* tag);

    CARAPI SetThreadSocketStatsUid(
        /* [in] */ Int32 uid);

    CARAPI SetKernelCounterSet(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 counterSet);

    CARAPI ResetKernelUidStats(
        /* [in] */ Int32 uid);

    /**
     * Convert {@code /proc/} tag format to {@link Integer}. Assumes incoming
     * format like {@code 0x7fffffff00000000}.
     */
    CARAPI KernelToTag(
        /* [in] */ const String& string,
        /* [out] */ Int32* tag);
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGERHELPER_H__
