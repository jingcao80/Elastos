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

#include "elastos/droid/net/EthernetManager.h"
#include "elastos/droid/net/CIpConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(EthernetManager, Object, IEthernetManager)

const String EthernetManager::TAG("EthernetManager");

ECode EthernetManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIEthernetManager* service)
{
    mContext = context;
    mService = service;
    return NOERROR;
}

ECode EthernetManager::GetConfiguration(
    /* [out] */ IIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result)

    if (mService == NULL) {
        return CIpConfiguration::New(result);
    }
        // try {
    ECode ec = mService->GetConfiguration(result);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return CIpConfiguration::New(result);
    }
        // }
    return ec;
}

ECode EthernetManager::SetConfiguration(
    /* [in] */ IIpConfiguration* config)
{
    if (mService == NULL) {
        return NOERROR;
    }
        // try {
    ECode ec = mService->SetConfiguration(config);
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) return NOERROR;
        // }
    return ec;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
