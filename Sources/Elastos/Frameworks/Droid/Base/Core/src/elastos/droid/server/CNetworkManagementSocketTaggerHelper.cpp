#include "elastos/droid/server/CNetworkManagementSocketTaggerHelper.h"
#include "elastos/droid/server/NetworkManagementSocketTagger.h"

using Elastos::Droid::Server::NetworkManagementSocketTagger;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_SINGLETON_IMPL(CNetworkManagementSocketTaggerHelper)

CAR_INTERFACE_IMPL(CNetworkManagementSocketTaggerHelper, Singleton, INetworkManagementSocketTagger)

ECode CNetworkManagementSocketTaggerHelper::Install()
{
    return NetworkManagementSocketTagger::Install();
}

ECode CNetworkManagementSocketTaggerHelper::SetThreadSocketStatsTag(
    /* [in] */ Int32 tag)
{
    return NetworkManagementSocketTagger::SetThreadSocketStatsTag(tag);
}

ECode CNetworkManagementSocketTaggerHelper::GetThreadSocketStatsTag(
    /* [out] */ Int32* tag)
{
    return NetworkManagementSocketTagger::GetThreadSocketStatsTag(tag);
}

ECode CNetworkManagementSocketTaggerHelper::SetThreadSocketStatsUid(
    /* [in] */ Int32 uid)
{
    return NetworkManagementSocketTagger::SetThreadSocketStatsUid(uid);
}

ECode CNetworkManagementSocketTaggerHelper::SetKernelCounterSet(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 counterSet)
{
    return NetworkManagementSocketTagger::SetKernelCounterSet(uid, counterSet);
}

ECode CNetworkManagementSocketTaggerHelper::ResetKernelUidStats(
    /* [in] */ Int32 uid)
{
    return NetworkManagementSocketTagger::ResetKernelUidStats(uid);
}

ECode CNetworkManagementSocketTaggerHelper::KernelToTag(
    /* [in] */ const String& string,
    /* [out] */ Int32* tag)
{
    return NetworkManagementSocketTagger::KernelToTag(string, tag);
}


} // namespace Server
} // namespace Droid
} // namespace Elastos