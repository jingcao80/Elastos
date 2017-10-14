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

#ifndef __ELASTOS_DROID_HARDWARE_CHASHCODEHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CHASHCODEHELPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Utils_CHashCodeHelpers.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CarClass(CHashCodeHelpers)
    , public Singleton
    , public IHashCodeHelpers
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    using Singleton::GetHashCode;

    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of integers
     *
     * @return the numeric hash code
     */
    CARAPI GetHashCode(
        /* [in] */ ArrayOf<Int32>* array,
        /* [out] */ Int32* value);

    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of floats
     *
     * @return the numeric hash code
     */
    CARAPI GetHashCode(
        /* [in] */ ArrayOf<Float>* array,
        /* [out] */ Int32* value);

    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of objects
     *
     * @return the numeric hash code
     */
    CARAPI GetHashCode(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [in] */ IInterface* c,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [in] */ IInterface* c,
        /* [in] */ IInterface* d,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w,
        /* [out] */ Int32* value);

    CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w,
        /* [in] */ Int32 t,
        /* [out] */ Int32* value);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CHASHCODEHELPER_H__
