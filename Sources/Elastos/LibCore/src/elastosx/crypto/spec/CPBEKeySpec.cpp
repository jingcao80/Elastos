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

#include "CPBEKeySpec.h"
#include "EmptyArray.h"
#include "Arrays.h"

using Libcore::Utility::EmptyArray;
using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CPBEKeySpec)
CAR_INTERFACE_IMPL_2(CPBEKeySpec, Object, IPBEKeySpec, IKeySpec)

CPBEKeySpec::CPBEKeySpec()
    : mIterationCount(0)
    , mKeyLength(0)
{
}

ECode CPBEKeySpec::constructor(
    /* [in] */ ArrayOf<Char32> * password)
{
    if (password == NULL) {
        mPassword = EmptyArray::CHAR32;
    } else {
        mPassword = ArrayOf<Char32>::Alloc(password->GetLength());
        // System.arraycopy(password, 0, this.password, 0, password.length);
        mPassword->Copy(0, password, 0, password->GetLength());
    }
    mSalt = NULL;
    return NOERROR;
}

ECode CPBEKeySpec::constructor(
    /* [in] */ ArrayOf<Char32> * password,
    /* [in] */ ArrayOf<Byte> * salt,
    /* [in] */ Int32 iterationCount,
    /* [in] */ Int32 keyLength)
{
    if (salt == NULL) {
        // throw new NullPointerException("salt == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (salt->GetLength() == 0) {
        // throw new IllegalArgumentException("salt.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (iterationCount <= 0) {
        // throw new IllegalArgumentException("iterationCount <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (keyLength <= 0) {
        // throw new IllegalArgumentException("keyLength <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (password == NULL) {
        mPassword = EmptyArray::CHAR32;
    } else {
        mPassword = ArrayOf<Char32>::Alloc(password->GetLength());
        // System.arraycopy(password, 0, this.password, 0, password.length);
        mPassword->Copy(0, password, 0, password->GetLength());
    }
    mSalt = ArrayOf<Byte>::Alloc(salt->GetLength());
    // System.arraycopy(salt, 0, this.salt, 0, salt.length);
    mSalt->Copy(0, salt, 0, salt->GetLength());
    mIterationCount = iterationCount;
    mKeyLength = keyLength;
    return NOERROR;
}

ECode CPBEKeySpec::constructor(
    /* [in] */ ArrayOf<Char32> * password,
    /* [in] */ ArrayOf<Byte> * salt,
    /* [in] */ Int32 iterationCount)
{
    if (salt == NULL) {
        // throw new NullPointerException("salt == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (salt->GetLength() == 0) {
        // throw new IllegalArgumentException("salt.length == 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (iterationCount <= 0) {
        // throw new IllegalArgumentException("iterationCount <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (password == NULL) {
        mPassword = EmptyArray::CHAR32;
    } else {
        mPassword = ArrayOf<Char32>::Alloc(password->GetLength());
        // System.arraycopy(password, 0, mPassword, 0, password.length);
        mPassword->Copy(0, password, 0, password->GetLength());
    }
    mSalt = ArrayOf<Byte>::Alloc(salt->GetLength());
    // System.arraycopy(salt, 0, mSalt, 0, salt.length);
    mSalt->Copy(0, salt, 0, salt->GetLength());
    mIterationCount = iterationCount;
    mKeyLength = 0;
    return NOERROR;
}

ECode CPBEKeySpec::ClearPassword()
{
    Char32 c = '?';
    Arrays::Fill(mPassword, c);
    mPassword = NULL;
    return NOERROR;
}

ECode CPBEKeySpec::GetPassword(
    /* [out, callee] */ ArrayOf<Char32> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mPassword == NULL) {
        // throw new IllegalStateException("The password has been cleared");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = ArrayOf<Char32>::Alloc(mPassword->GetLength());
    // System.arraycopy(mPassword, 0, result, 0, mPassword.length);
    (*result)->Copy(0, mPassword, 0, mPassword->GetLength());
    return NOERROR;
}

ECode CPBEKeySpec::GetSalt(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mSalt == NULL) {
        return NOERROR;
    }
    *result = ArrayOf<Byte>::Alloc(mSalt->GetLength());
    // System.arraycopy(mSalt, 0, result, 0, mSalt.length);
    (*result)->Copy(0, mSalt, 0, mSalt->GetLength());
    return NOERROR;
}

ECode CPBEKeySpec::GetIterationCount(
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = mIterationCount;
    return NOERROR;
}

ECode CPBEKeySpec::GetKeyLength(
    /* [out] */ Int32 * length)
{
    VALIDATE_NOT_NULL(length)
    *length = mKeyLength;
    return NOERROR;
}


} // Spec
} // Crypto
} // Elastosx