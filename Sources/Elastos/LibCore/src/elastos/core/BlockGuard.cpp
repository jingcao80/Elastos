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

#include "BlockGuard.h"
#include "Thread.h"
#include "StringUtils.h"
#include <cutils/log.h>
#include "AutoLock.h"

using Elastos::Core::StringUtils;
using Elastos::Core::Thread;

namespace Elastos {
namespace Core {

AutoPtr<IBlockGuardPolicy> BlockGuard::LAX_POLICY = new BlockGuard::LAXPOLICY();

AutoPtr<IBlockGuardPolicy> BlockGuard::sPolicy;
Object BlockGuard::sLock;
pthread_key_t BlockGuard::sTlsKey;
pthread_once_t BlockGuard::sKeyOnce = PTHREAD_ONCE_INIT;

static void ThreadDestructor(void* st)
{
    IBlockGuardPolicy* policy = static_cast<IBlockGuardPolicy*>(st);
    if (policy) {
        policy->Release();
    }
}

static void MakeKey()
{
    ASSERT_TRUE(pthread_key_create(&BlockGuard::sTlsKey, ThreadDestructor) == 0);
}

CAR_INTERFACE_IMPL(BlockGuard::LAXPOLICY, Object, IBlockGuardPolicy)

BlockGuard::LAXPOLICY::~LAXPOLICY()
{
}

ECode BlockGuard::LAXPOLICY::OnWriteToDisk()
{
    return NOERROR;
}

ECode BlockGuard::LAXPOLICY::OnReadFromDisk()
{
    return NOERROR;
}

ECode BlockGuard::LAXPOLICY::OnNetwork()
{
    return NOERROR;
}

ECode BlockGuard::LAXPOLICY::GetPolicyMask(
    /* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask);

    *mask = 0;
    return NOERROR;
}

AutoPtr<IBlockGuardPolicy> BlockGuard::GetThreadPolicy()
{
    pthread_once(&sKeyOnce, MakeKey);

    AutoPtr<IBlockGuardPolicy> policy = (IBlockGuardPolicy*)pthread_getspecific(sTlsKey);
    if (policy == NULL) {
        if (sPolicy == NULL) {
            sPolicy = LAX_POLICY;
        }

        ASSERT_TRUE(pthread_setspecific(sTlsKey, sPolicy.Get()) == 0);
        sPolicy->AddRef();
        policy = (IBlockGuardPolicy*)pthread_getspecific(sTlsKey);
    }
    assert(policy.Get() != NULL && "check BlockGuardPolicy failed!");
    return policy;
}

ECode BlockGuard::GetThreadPolicy(
    /* [out] */ IBlockGuardPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    AutoPtr<IBlockGuardPolicy> _policy = GetThreadPolicy();
    *policy = _policy;
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

ECode BlockGuard::SetThreadPolicy(
    /* [in] */ IBlockGuardPolicy* policy)
{
    if (policy == NULL) {
        //throw new NullPointerException("policy == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IBlockGuardPolicy> _policy = GetThreadPolicy();
    assert(_policy.Get() != NULL);
    if (_policy.Get() != policy) {
        _policy->Release();

        AutoLock locK(sLock);
        ASSERT_TRUE(pthread_setspecific(BlockGuard::sTlsKey, policy) == 0);
        REFCOUNT_ADD(policy);
    }

    return NOERROR;
}

CARAPI BlockGuard::GetLaxPolicy(
    /* [out] */ IBlockGuardPolicy** policy)
{
    VALIDATE_NOT_NULL(policy);

    GetThreadPolicy();
    assert(sPolicy.Get() != NULL);

    *policy = sPolicy;
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
