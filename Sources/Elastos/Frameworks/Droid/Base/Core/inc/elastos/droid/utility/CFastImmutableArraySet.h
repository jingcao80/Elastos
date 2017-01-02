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

#ifndef __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__
#define __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__

#include "_Elastos_Droid_Utility_CFastImmutableArraySet.h"
#include "elastos/droid/utility/FastImmutableArraySet.h"

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * A fast immutable set wrapper for an array that is optimized for non-concurrent iteration.
 * The same iterator instance is reused each time to avoid creating lots of garbage.
 * Iterating over an array in this fashion is 2.5x faster than iterating over a {@link HashSet}
 * so it is worth copying the contents of the set to an array when iterating over it
 * hundreds of times.
 * @hide
 */
CarClass(CFastImmutableArraySet)
    , public FastImmutableArraySet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__
