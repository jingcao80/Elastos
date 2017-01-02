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

#ifndef __ELASTOS_DROID_UTILITY_POOLS_H__
#define __ELASTOS_DROID_UTILITY_POOLS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

class ECO_PUBLIC Pools
{
private:
    Pools() {}

public:
    template<typename T>
    class Pool
        : public Object
    {
    public:
        /**
         * @return An instance from the pool if such, null otherwise.
         */
        virtual AutoPtr<T> AcquireItem() = 0;

        /**
         * Release an instance to the pool.
         *
         * @param instance The instance to release.
         * @return Whether the instance was put in the pool.
         *
         * @throws IllegalStateException If the instance is already in the pool.
         */
        virtual Boolean ReleaseItem(
            /* [in] */ T* instance) = 0;
    };

    /**
     * Simple (non-synchronized) pool of objects.
     *
     * @param <T> The pooled type.
     */
    template<typename T>
    class SimplePool
        : public Pool<T>
    {
    public:
        /**
         * Creates a new instance.
         *
         * @param maxPoolSize The max pool size.
         *
         * @throws IllegalArgumentException If the max pool size is less than zero.
         */
        SimplePool(
            /* [in] */ Int32 maxPoolSize)
            : mPoolSize(maxPoolSize)
        {
            assert(maxPoolSize > 0);
            mPool = ArrayOf<T*>::Alloc(maxPoolSize);
        }

        AutoPtr<T> AcquireItem()
        {
            if (mPoolSize > 0) {
                Int32 lastPooledIndex = mPoolSize - 1;
                assert(mPool != NULL);
                assert(lastPooledIndex >= 0);
                ELA_ASSERT_WITH_BLOCK(lastPooledIndex < mPool->GetLength()) {
                    Logger::E("Pools", "error: lastPooledIndex: %d bigger than mPool->GetLength(): %d, mPoolSize: %d",
                        lastPooledIndex, mPool->GetLength(), mPoolSize);
                }
                AutoPtr<T> instance = (*mPool)[lastPooledIndex];
                mPool->Set(lastPooledIndex, NULL);
                mPoolSize--;
                return instance;
            }
            return NULL;
        }

        Boolean ReleaseItem(
            /* [in] */ T* instance)
        {
            if (IsInPool(instance)) {
                //throw new IllegalStateException("Already in the pool!");
                return FALSE;
            }

            if (mPoolSize < mPool->GetLength()) {
                mPool->Set(mPoolSize, instance);
                mPoolSize++;
                return TRUE;
            }
            return FALSE;
        }

    private:
        Boolean IsInPool(
            /* [in] */ T* instance) {
            for (Int32 i = 0; i < mPoolSize; i++) {
                if ((*mPool)[i] == instance) {
                    return TRUE;
                }
            }
            return FALSE;
        }

    private:
        AutoPtr<ArrayOf<T*> > mPool;
        Int32 mPoolSize;
    };

    /**
     * Synchronized) pool of objects.
     *
     * @param <T> The pooled type.
     */
    template<typename T>
    class SynchronizedPool
        : public SimplePool<T>
    {
    public:
        /**
         * Creates a new instance.
         *
         * @param maxPoolSize The max pool size.
         *
         * @throws IllegalArgumentException If the max pool size is less than zero.
         */
        SynchronizedPool(
            /* [in] */ Int32 maxPoolSize)
            : SimplePool<T>(maxPoolSize)
        {}

        AutoPtr<T> AcquireItem()
        {
            AutoLock lock(mLock);
            return SimplePool<T>::AcquireItem();
        }

        Boolean ReleaseItem(
            /* [in] */ T* instance)
        {
            AutoLock lock(mLock);
            return SimplePool<T>::ReleaseItem(instance);
        }

    private:
        Object mLock;
    };
};

} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_UTILITY_POOLS_H__
