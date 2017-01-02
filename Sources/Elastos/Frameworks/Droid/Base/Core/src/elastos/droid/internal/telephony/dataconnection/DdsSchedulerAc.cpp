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

#include "elastos/droid/internal/telephony/dataconnection/DdsSchedulerAc.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/provider/Telephony.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::Telephony;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DdsSchedulerAc, AsyncChannel, IDdsSchedulerAc)

DdsSchedulerAc::DdsSchedulerAc()
    : TAG("DdsSchedulerAc")
{}

ECode DdsSchedulerAc::constructor()
{
    return NOERROR;
}

ECode DdsSchedulerAc::AllocateDds(
    /* [in] */ INetworkRequest* req)
{
    Logger::D(TAG, "EVENT_ADD_REQUEST = %s", TO_CSTR(req));
    SendMessage(EVENT_ADD_REQUEST, req);
    return NOERROR;
}

ECode DdsSchedulerAc::FreeDds(
    /* [in] */ INetworkRequest* req)
{
    Logger::D(TAG, "EVENT_REMOVE_REQUEST = %s", TO_CSTR(req));
    SendMessage(EVENT_REMOVE_REQUEST, req);
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
