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

#include "elastos/droid/settings/wifi/p2p/CWifiP2pPersistentGroup.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

CAR_OBJECT_IMPL(CWifiP2pPersistentGroup)

CAR_INTERFACE_IMPL(CWifiP2pPersistentGroup, Preference, IWifiP2pPersistentGroup)

CWifiP2pPersistentGroup::CWifiP2pPersistentGroup()
{}

CWifiP2pPersistentGroup::~CWifiP2pPersistentGroup()
{}

ECode CWifiP2pPersistentGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWifiP2pGroup* group)
{
    Preference::constructor(context);
    mGroup = group;
    return NOERROR;
}

ECode CWifiP2pPersistentGroup::OnBindView(
    /* [in] */ IView* view)
{
    String networkName;
    mGroup->GetNetworkName(&networkName);
    SetTitle(CoreUtils::Convert(networkName));
    return Preference::OnBindView(view);
}

Int32 CWifiP2pPersistentGroup::GetNetworkId()
{
    Int32 netId;
    mGroup->GetNetworkId(&netId);
    return netId;
}

String CWifiP2pPersistentGroup::GetGroupName()
{
    String networkName;
    mGroup->GetNetworkName(&networkName);
    return networkName;
}

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos