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

#include "elastos/droid/server/CDataActivityObserver.h"
#include "elastos/droid/server/CConnectivityService.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_OBJECT_IMPL(CDataActivityObserver)

ECode CDataActivityObserver::InterfaceClassDataActivityChanged(
    /* [in] */ const String& label,
    /* [in] */ Boolean active,
    /* [in] */ Int64 tsNanos)
{
    Int32 deviceType = StringUtils::ParseInt32(label);
    mHost->SendDataActivityBroadcast(deviceType, active, tsNanos);
    return NOERROR;
}

ECode CDataActivityObserver::constructor(
    /* [in] */ IIConnectivityManager* host)
{
    mHost = (CConnectivityService*)host;
    return NOERROR;
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos
