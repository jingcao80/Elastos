//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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