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
