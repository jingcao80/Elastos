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

#include "CLockSupport.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

CAR_INTERFACE_IMPL(CLockSupport, Singleton, ILockSupport)

CAR_SINGLETON_IMPL(CLockSupport)

ECode CLockSupport::Unpark(
    /* [in] */ IThread* thread)
{
    return LockSupport::Unpark(thread);
}

ECode CLockSupport::Park(
    /* [in] */ IInterface* blocker)
{
    return LockSupport::Park(blocker);
}

ECode CLockSupport::ParkNanos(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 nanos)
{
    return LockSupport::ParkNanos(blocker, nanos);
}

ECode CLockSupport::ParkUntil(
    /* [in] */ IInterface* blocker,
    /* [in] */ Int64 deadline)
{
    return LockSupport::ParkUntil(blocker, deadline);
}

ECode CLockSupport::Park()
{
    return LockSupport::Park();
}

ECode CLockSupport::ParkNanos(
    /* [in] */ Int64 nanos)
{
    return LockSupport::ParkNanos(nanos);
}

ECode CLockSupport::ParkUntil(
    /* [in] */ Int64 deadline)
{
    return LockSupport::ParkUntil(deadline);
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
