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

#include "CKeyStorePasswordProtection.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStorePasswordProtection)
CAR_INTERFACE_IMPL_3(CKeyStorePasswordProtection, Object, IKeyStorePasswordProtection, IKeyStoreProtectionParameter, IDestroyable)
CKeyStorePasswordProtection::CKeyStorePasswordProtection()
    : mIsDestroyed(FALSE)
{}

ECode CKeyStorePasswordProtection::GetPassword(
    /* [out, callee] */ ArrayOf<Char32> **pwd)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(pwd)
    if (mIsDestroyed) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *pwd = mPassword;
    REFCOUNT_ADD(*pwd)
    return NOERROR;
}

ECode CKeyStorePasswordProtection::Destroy()
{
    AutoLock lock(mLock);
    if (mPassword) {
        mPassword = NULL;
    }
    return NOERROR;
}

ECode CKeyStorePasswordProtection::IsDestroyed(
    /* [out] */ Boolean *isDestroyed)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(isDestroyed)
    *isDestroyed = mIsDestroyed;
    return NOERROR;
}

ECode CKeyStorePasswordProtection::constructor(
    /* [in] */ ArrayOf<Char32> *password)
{
    if (password){
        mPassword = password->Clone();
    }
    return NOERROR;
}

}
}
