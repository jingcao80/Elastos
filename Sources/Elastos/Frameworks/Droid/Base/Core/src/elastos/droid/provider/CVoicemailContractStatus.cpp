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
#include "elastos/droid/provider/CVoicemailContractStatus.h"
#include "elastos/droid/provider/VoicemailContract.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CVoicemailContractStatus)

CAR_INTERFACE_IMPL(CVoicemailContractStatus, Singleton, IVoicemailContractStatus)

ECode CVoicemailContractStatus::BuildSourceUri(
    /* [in] */ const String& packageName,
    /* [out] */ IUri** uri)
{
    return VoicemailContract::Status::BuildSourceUri(packageName, uri);
}

ECode CVoicemailContractStatus::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = VoicemailContract::Status::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
