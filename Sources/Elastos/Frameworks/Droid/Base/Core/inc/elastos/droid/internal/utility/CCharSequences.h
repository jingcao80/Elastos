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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CCHARSEQUENCES_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CCHARSEQUENCES_H__

#include "_Elastos_Droid_Internal_Utility_CCharSequences.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * {@link CharSequence} utility methods.
 */
CarClass(CCharSequences)
    , public Singleton
    , public ICharSequences
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ForAsciiBytes(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ ICharSequence** cs);

    CARAPI ForAsciiBytes(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** cs);

    using Singleton::Equals;

    CARAPI Equals(
        /* [in] */ ICharSequence* a,
        /* [in] */ ICharSequence* b,
        /* [out] */ Boolean* result);

    CARAPI CompareToIgnoreCase(
        /* [in] */ ICharSequence* me,
        /* [in] */ ICharSequence* another,
        /* [out] */ Int32* result);
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CCHARSEQUENCES_H__
