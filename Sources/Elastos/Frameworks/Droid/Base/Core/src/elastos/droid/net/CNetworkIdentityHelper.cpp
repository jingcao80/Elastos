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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CNetworkIdentity.h"
#include "elastos/droid/net/CNetworkIdentityHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkIdentityHelper, Singleton, INetworkIdentityHelper)

CAR_SINGLETON_IMPL(CNetworkIdentityHelper)

ECode CNetworkIdentityHelper::ScrubSubscriberId(
    /* [in] */ const String& subscriberId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkIdentity::ScrubSubscriberId(subscriberId, result);
}

ECode CNetworkIdentityHelper::BuildNetworkIdentity(
    /* [in] */ IContext* context,
    /* [in] */ INetworkState* state,
    /* [out] */ INetworkIdentity** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkIdentity::BuildNetworkIdentity(context, state, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
