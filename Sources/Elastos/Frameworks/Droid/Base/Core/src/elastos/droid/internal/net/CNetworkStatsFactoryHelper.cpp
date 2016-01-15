
#include "elastos/droid/internal/net/CNetworkStatsFactoryHelper.h"
#include "elastos/droid/internal/net/CNetworkStatsFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkStatsFactoryHelper, Singleton, INetworkStatsFactoryHelper)

CAR_SINGLETON_IMPL(CNetworkStatsFactoryHelper)

ECode  CNetworkStatsFactoryHelper::NoteStackedIface(
        /* [in] */ const String& stackedIface,
        /* [in] */ const String& baseIface)
{
    return CNetworkStatsFactory::NoteStackedIface(stackedIface, baseIface);
}

ECode  CNetworkStatsFactoryHelper::JavaReadNetworkStatsDetail(
    /* [in] */ IFile* detailPath,
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag,
    /* [out] */ INetworkStats** stats)
{
    return CNetworkStatsFactory::JavaReadNetworkStatsDetail(
        detailPath, limitUid, limitIfaces, limitTag, stats);
}

ECode  CNetworkStatsFactoryHelper::NativeReadNetworkStatsDetail(
    /* [in] */ INetworkStats* stats,
    /* [in] */ const String& path,
    /* [in] */ Int32 limitUid,
    /* [in] */ ArrayOf<String>* limitIfaces,
    /* [in] */ Int32 limitTag,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CNetworkStatsFactory::NativeReadNetworkStatsDetail(
        stats, path, limitUid, limitIfaces, limitTag);
    return NOERROR;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
