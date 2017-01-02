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

#ifndef __ELASTOS_CORE_BLOCKGUARD_H__
#define __ELASTOS_CORE_BLOCKGUARD_H__

#include "Singleton.h"

namespace Elastos {
namespace Core {

class BlockGuard
{
public:

    /**
     * The default, permissive policy that doesn't prevent any operations.
     */
    class LAXPOLICY
        : public Object
        , public IBlockGuardPolicy
    {
    public:
        CAR_INTERFACE_DECL()

        ~LAXPOLICY();

        CARAPI OnWriteToDisk();

        CARAPI OnReadFromDisk();

        CARAPI OnNetwork();

        CARAPI GetPolicyMask(
            /* [out] */ Int32* mask);
    };

public:
    /**
     * Get the current thread's policy.
     *
     * @return the current thread's policy.  Never returns null.
     *     Will return the LAX_POLICY instance if nothing else is set.
     */
    static CARAPI GetThreadPolicy(
        /* [out] */ IBlockGuardPolicy** policy);

    static CARAPI_(AutoPtr<IBlockGuardPolicy>) GetThreadPolicy();

    /**
     * Sets the current thread's block guard policy.
     *
     * @param policy policy to set.  May not be null.  Use the public LAX_POLICY
     *   if you want to unset the active policy.
     */
    static CARAPI SetThreadPolicy(
        /* [in] */ IBlockGuardPolicy* policy);

    static CARAPI GetLaxPolicy(
        /* [out] */ IBlockGuardPolicy** policy);

public:
    /**
     * The default, permissive policy that doesn't prevent any operations.
     */
    static AutoPtr<IBlockGuardPolicy> LAX_POLICY;

    static AutoPtr<IBlockGuardPolicy> sPolicy;
    static pthread_once_t sKeyOnce;
    static pthread_key_t sTlsKey;
    static Object sLock;

private:
    BlockGuard();
    BlockGuard(const BlockGuard&);
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_BLOCKGUARD_H__
