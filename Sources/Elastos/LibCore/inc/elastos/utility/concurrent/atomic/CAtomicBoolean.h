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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICBOOLEAN_H__
#define __ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICBOOLEAN_H__

#include "_Elastos_Utility_Concurrent_Atomic_CAtomicBoolean.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Atomic {

CarClass(CAtomicBoolean)
    , public Object
    , public IAtomicBoolean
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new AtomicInteger with the given initial value.
     *
     * @param initialValue the initial value
     */
    CARAPI constructor(
        /* [in] */ Boolean initialValue);

    /**
     * Creates a new AtomicInteger with initial value {@code 0}.
     */
    CARAPI constructor();

    /**
     * Gets the current value.
     *
     * @return the current value
     */
    CARAPI Get(
        /* [out] */ Boolean* value);

    /**
     * Sets to the given value.
     *
     * @param newValue the new value
     */
    CARAPI Set(
        /* [in] */ Boolean newValue);

    /**
     * Eventually sets to the given value.
     *
     * @param newValue the new value
     * @since 1.6
     */
    CARAPI LazySet(
        /* [in] */ Boolean newValue);

    /**
     * Atomically sets to the given value and returns the old value.
     *
     * @param newValue the new value
     * @return the previous value
     */
    CARAPI GetAndSet(
        /* [in] */ Boolean newValue,
        /* [out] */ Boolean* value);

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful. False return indicates that
     * the actual value was not equal to the expected value.
     */
    CARAPI CompareAndSet(
        /* [in] */ Boolean expect,
        /* [in] */ Boolean update,
        /* [out] */ Boolean* result);

    /**
     * Atomically sets the value to the given updated value
     * if the current value {@code ==} the expected value.
     *
     * <p>May <a href="package-summary.html#Spurious">fail spuriously</a>
     * and does not provide ordering guarantees, so is only rarely an
     * appropriate alternative to {@code compareAndSet}.
     *
     * @param expect the expected value
     * @param update the new value
     * @return true if successful.
     */
    CARAPI WeakCompareAndSet(
        /* [in] */ Boolean expect,
        /* [in] */ Boolean update,
        /* [out] */ Boolean* result);

    /**
     * Returns the String representation of the current value.
     * @return the String representation of the current value
     */
    CARAPI ToString(
        /* [out] */ String* str);

private:
    static Int64 mSerialVersionUID;// = 4654671469794556979L;
    // setup to use Unsafe.compareAndSwapInt for updates
//    static Unsafe unsafe = Unsafe.getUnsafe();
    static Int64 mValueOffset;

    // static {
    //     try {
    //         valueOffset = unsafe.objectFieldOffset
    //             (AtomicBoolean.class.getDeclaredField("value"));
    //     } catch (Exception ex) { throw new Error(ex); }
    // }

    volatile Int32 mValue;
};

} // namespace Atomic
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ATOMIC_ELASTOS_UTILITY_CATOMICBOOLEAN_H__
