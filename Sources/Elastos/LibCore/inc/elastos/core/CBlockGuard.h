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

#ifndef __ELASTOS_CORE_CBLOCKGUARD_H__
#define __ELASTOS_CORE_CBLOCKGUARD_H__

#include "_Elastos_Core_CBlockGuard.h"
#include "Singleton.h"

namespace Elastos {
namespace Core {

CarClass(CBlockGuard)
    , public Singleton
    , public IBlockGuard
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Get the current thread's policy.
     *
     * @return the current thread's policy.  Never returns null.
     *     Will return the LAX_POLICY instance if nothing else is set.
     */
    CARAPI GetThreadPolicy(
        /* [out] */ IBlockGuardPolicy** policy);

    /**
     * Sets the current thread's block guard policy.
     *
     * @param policy policy to set.  May not be null.  Use the public LAX_POLICY
     *   if you want to unset the active policy.
     */
    CARAPI SetThreadPolicy(
        /* [in] */ IBlockGuardPolicy* policy);

    CARAPI GetLaxPolicy(
        /* [out] */ IBlockGuardPolicy** policy);
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CBLOCKGUARD_H__
