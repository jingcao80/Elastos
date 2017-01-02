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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULERAC_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULERAC_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/AsyncChannel.h>

using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Net::INetworkRequest;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class DdsSchedulerAc
    : public AsyncChannel
    , public IDdsSchedulerAc
{
public:
    CAR_INTERFACE_DECL()

    DdsSchedulerAc();

    CARAPI constructor();

    CARAPI AllocateDds(
        /* [in] */ INetworkRequest* req);

    CARAPI FreeDds(
        /* [in] */ INetworkRequest* req);

private:
    const String TAG;

    AutoPtr<IDdsScheduler> mScheduler;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULERAC_H__
