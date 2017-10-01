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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSEMAPHORE_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSEMAPHORE_H__

#include "_Elastos_Utility_Concurrent_CSemaphore.h"
#include "AbstractQueuedSynchronizer.h"
#include "Object.h"

using Elastos::IO::ISerializable;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CSemaphore)
    , public Object
    , public ISemaphore
    , public ISerializable
{
public:
    enum SyncID {
        SyncID_Sync,
        SyncID_NonfairSync,
        SyncID_FairSync,
    };

    class Sync
        : public Locks::AbstractQueuedSynchronizer
    {
    public:
        Sync(
            /* [in] */ Int32 permits);

        CARAPI GetPermits(
            /* [out] */ Int32* out);

        CARAPI NonfairTryAcquireShared(
            /* [in] */ Int32 acquires,
            /* [out] */ Int32* out);

        virtual CARAPI_(Boolean) TryReleaseShared(
            /* [in] */ Int32 arg);

        CARAPI ReducePermits(
            /* [in] */ Int32 reductions);

        CARAPI DrainPermits(
            /* [out] */ Int32* out);

        virtual SyncID GetSyncID()
        {
            return SyncID_Sync;
        }

    private:
        static const Int64 serialVersionUID = 1192457210091910933L;
    };

    class NonfairSync : public Sync
    {
    public:
        NonfairSync(
            /* [in] */ Int32 permits);

        virtual CARAPI_(Int32) TryAcquireShared(
            /* [in] */ Int32 arg);

        virtual SyncID GetSyncID()
        {
            return SyncID_NonfairSync;
        }
    private:
        static const Int64 serialVersionUID = -2694183684443567898L;
    };

    class FairSync : public Sync
    {
    public:
        FairSync(
            /* [in] */ Int32 permits);

        virtual CARAPI_(Int32) TryAcquireShared(
            /* [in] */ Int32 arg);

        virtual SyncID GetSyncID()
        {
            return SyncID_FairSync;
        }
    private:
        static const Int64 serialVersionUID = 2014338818796000944L;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 permits);

    CARAPI constructor(
        /* [in] */ Int32 permits,
        /* [in] */ Boolean fair);

    CARAPI Acquire();

    CARAPI AcquireUninterruptibly();

    CARAPI TryAcquire(
        /* [out] */ Boolean* value);

    CARAPI TryAcquire(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    CARAPI ReleasePermit();

    CARAPI Acquire(
        /* [in] */ Int32 permits);

    CARAPI AcquireUninterruptibly(
        /* [in] */ Int32 permits);

    CARAPI TryAcquire(
        /* [in] */ Int32 permits,
        /* [out] */ Boolean* value);

    CARAPI TryAcquire(
        /* [in] */ Int32 permits,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* value);

    CARAPI ReleasePermit(
        /* [in] */ Int32 permits);

    CARAPI AvailablePermits(
        /* [out] */ Int32* value);

    CARAPI DrainPermits(
        /* [out] */ Int32* value);

    CARAPI IsFair(
        /* [out] */ Boolean* value);

    CARAPI HasQueuedThreads(
        /* [out] */ Boolean* value);

    CARAPI GetQueueLength(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI ReducePermits(
        /* [in] */ Int32 reduction);

    CARAPI GetQueuedThreads(
        /* [out] */ ICollection** out);

private:
    static const Int64 serialVersionUID = -3222578661600680210L;
    AutoPtr<Sync> mSync;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CSEMAPHORE_H__
