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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOM_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOM_H__

#include "_Elastos_Utility_Concurrent_CThreadLocalRandom.h"
#include "Random.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CThreadLocalRandom)
    , public Random
{
public:
    CAR_OBJECT_DECL()

    CThreadLocalRandom();

    /**
     * Constructor called only by localRandom.initialValue.
     */
    CARAPI constructor();

    /**
     * Returns the current thread's {@code ThreadLocalRandom}.
     *
     * @return the current thread's {@code ThreadLocalRandom}
     */
    static CARAPI_(AutoPtr<IThreadLocalRandom>) GetCurrent();

    CARAPI SetSeed(
        /* [in] */ Int64 seed);

    using Random::NextInt32;

    CARAPI NextInt32(
        /* [in] */ Int32 least,
        /* [in] */ Int32 bound,
        /* [out] */ Int32* value);

    using Random::NextInt64;

    CARAPI NextInt64(
        /* [in] */ Int64 n,
        /* [out] */ Int64* value);

    CARAPI NextInt64(
        /* [in] */ Int64 least,
        /* [in] */ Int64 bound,
        /* [out] */ Int64* value);

    using Random::NextDouble;

    CARAPI NextDouble(
        /* [in] */ Double n,
        /* [out] */ Double* value);

    CARAPI NextDouble(
        /* [in] */ Double least,
        /* [in] */ Double bound,
        /* [out] */ Double* value);

    CARAPI_(Int32) Next(
        /* [in] */ Int32 bits);

public:
    /**
     * Initialization flag to permit calls to setSeed to succeed only
     * while executing the Random constructor.  We can't allow others
     * since it would cause setting seed in one part of a program to
     * unintentionally impact other usages by the thread.
     */
    Boolean mInitialized;

private:
    static const Int64 mSerialVersionUID;

    // same constants as Random, but must be redeclared because private
    static const Int64 mMultiplier;
    static const Int64 mAddend;
    static const Int64 mMask;

    /**
     * The random seed. We can't use super.seed.
     */
    Int64 mRnd;

    // Padding to help avoid memory contention among seed updates in
    // different TLRs in the common case that they are located near
    // each other.
    Int64 mPad0, mPad1, mPad2, mPad3, mPad4, mPad5, mPad6, mPad7;

    /**
     * The actual ThreadLocal
     */
//    static AutoPtr<IThreadLocal> mLocalRandom;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOM_H__
