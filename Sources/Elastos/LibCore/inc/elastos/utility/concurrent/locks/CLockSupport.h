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

#ifndef __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CLOCKSUPPORT_H__
#define __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CLOCKSUPPORT_H__

#include "_Elastos_Utility_Concurrent_Locks_CLockSupport.h"
#include "LockSupport.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

CarClass(CLockSupport)
    , public Singleton
    , public ILockSupport
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Unpark(
        /* [in] */ IThread* thread);

    CARAPI Park(
        /* [in] */ IInterface* blocker);

    CARAPI ParkNanos(
        /* [in] */ IInterface* blocker,
        /* [in] */ Int64 nanos);

    CARAPI ParkUntil(
        /* [in] */ IInterface* blocker,
        /* [in] */ Int64 deadline);

    CARAPI Park();

    CARAPI ParkNanos(
        /* [in] */ Int64 nanos);

    CARAPI ParkUntil(
        /* [in] */ Int64 deadline);
};

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CLOCKSUPPORT_H__
