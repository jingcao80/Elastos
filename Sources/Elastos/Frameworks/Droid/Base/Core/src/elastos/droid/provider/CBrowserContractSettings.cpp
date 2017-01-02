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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractSettings.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractSettings)

CAR_INTERFACE_IMPL(CBrowserContractSettings, Singleton, IBrowserContractSettings)

ECode CBrowserContractSettings::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Settings::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractSettings::IsSyncEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{
    return BrowserContract::Settings::IsSyncEnabled(context, res);
}

ECode CBrowserContractSettings::SetSyncEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    return BrowserContract::Settings::SetSyncEnabled(context, enabled);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
