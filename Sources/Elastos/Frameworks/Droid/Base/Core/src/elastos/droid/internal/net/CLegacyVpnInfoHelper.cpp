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

#include "elastos/droid/internal/net/CLegacyVpnInfoHelper.h"
#include "elastos/droid/internal/net/CLegacyVpnInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CLegacyVpnInfoHelper, Singleton, ILegacyVpnInfoHelper)

CAR_SINGLETON_IMPL(CLegacyVpnInfoHelper)

ECode CLegacyVpnInfoHelper::StateFromNetworkInfo(
        /* [in] */ INetworkInfo* info,
        /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = CLegacyVpnInfo::StateFromNetworkInfo(info);
    return NOERROR;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
