
#ifndef __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORY_H__
#define __ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORY_H__

#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Net_CNetworkStatsFactory.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CNetworkStatsFactory)
    , public Object
    , public INetworkStatsFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFile* procRoot);

    /**
     * Parse and return interface-level summary {@link NetworkStats} measured
     * using {@code /proc/net/dev} style hooks, which may include non IP layer
     * traffic. Values monotonically increase since device boot, and may include
     * details about inactive interfaces.
     *
     * @throws IllegalStateException when problem parsing stats.
     */
    CARAPI ReadNetworkStatsSummaryDev(
        /* [out] */ INetworkStats** stats);

    /**
     * Parse and return interface-level summary {@link NetworkStats}. Designed
     * to return only IP layer traffic. Values monotonically increase since
     * device boot, and may include details about inactive interfaces.
     *
     * @throws IllegalStateException when problem parsing stats.
     */
    CARAPI ReadNetworkStatsSummaryXt(
        /* [out] */ INetworkStats** stats);

    CARAPI ReadNetworkStatsDetail(
        /* [out] */ INetworkStats** stats);

    /**
     * Parse and return {@link NetworkStats} with UID-level details. Values
     * monotonically increase since device boot.
     *
     * @throws IllegalStateException when problem parsing stats.
     */
    CARAPI ReadNetworkStatsDetail(
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [in] */ INetworkStats* lastStats,
        /* [out] */ INetworkStats** stats);

    CARAPI AssertEquals(
        /* [in] */ INetworkStats* expected,
        /* [in] */ INetworkStats* actual);

    static CARAPI NoteStackedIface(
        /* [in] */ const String& stackedIface,
        /* [in] */ const String& baseIface);

    /**
     * Parse and return {@link NetworkStats} with UID-level details. Values are
     * expected to monotonically increase since device boot.
     */
    // @VisibleForTesting
    static CARAPI JavaReadNetworkStatsDetail(
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
    static CARAPI_(Int32) NativeReadNetworkStatsDetail(
        /* [in] */ INetworkStats* stats,
        /* [in] */ const String& path,
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag);

private:
    CARAPI ReadNetworkStatsDetailInternal(
        /* [in] */ Int32 limitUid,
        /* [in] */ ArrayOf<String>* limitIfaces,
        /* [in] */ Int32 limitTag,
        /* [in] */ INetworkStats* lastStats,
        /* [out] */ INetworkStats** stats);

private:
    static const String TAG;
    static const Boolean USE_NATIVE_PARSING;
    static const Boolean SANITY_CHECK_NATIVE;

    /** Path to {@code /proc/net/xt_qtaguid/iface_stat_all}. */
    AutoPtr<IFile> mStatsXtIfaceAll;
    /** Path to {@code /proc/net/xt_qtaguid/iface_stat_fmt}. */
    AutoPtr<IFile> mStatsXtIfaceFmt;
    /** Path to {@code /proc/net/xt_qtaguid/stats}. */
    AutoPtr<IFile> mStatsXtUid;

    // @GuardedBy("sStackedIfaces")
    static AutoPtr<IArrayMap> sStackedIfaces;
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CNETWORKSTATSFACTORY_H__
