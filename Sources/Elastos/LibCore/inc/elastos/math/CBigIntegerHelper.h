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

#ifndef __ELASTOS_MATH_CBIGINTEGERHELPER_H__
#define __ELASTOS_MATH_CBIGINTEGERHELPER_H__

#include "_Elastos_Math_CBigIntegerHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Math {

CarClass(CBigIntegerHelper)
    , public Singleton
    , public IBigIntegerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /** Returns a {@code BigInteger} whose value is equal to {@code value}. */
    CARAPI ValueOf(
        /* [in] */ Int64 value,
        /* [out] */ IBigInteger** result);

    CARAPI ProbablePrime(
        /* [in] */ Int32 bitLength,
        /* [in] */ IRandom* random,
        /* [out] */ IBigInteger** result);

    CARAPI GetMINUSOne(
        /* [out] */ IBigInteger** result);

    CARAPI GetZERO(
        /* [out] */ IBigInteger** result);

    CARAPI GetONE(
        /* [out] */ IBigInteger** result);

    CARAPI GetTEN(
        /* [out] */ IBigInteger** result);
};

} // namespace Math
} // namespace Elastos

#endif // __ELASTOS_MATH_CBIGINTEGERHELPER_H__
