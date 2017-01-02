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

#include "CPasswordAuthentication.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CPasswordAuthentication, Object, IPasswordAuthentication)

CAR_OBJECT_IMPL(CPasswordAuthentication)

CPasswordAuthentication::~CPasswordAuthentication()
{
}

ECode CPasswordAuthentication::constructor(
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<Char32> * password)
{
    VALIDATE_NOT_NULL(password)
    mUserName = userName;
    mPassword = password->Clone();

    return NOERROR;
}

ECode CPasswordAuthentication::GetPassword(
    /* [out, callee] */ ArrayOf<Char32>** password)
{
    VALIDATE_NOT_NULL(password);
    AutoPtr<ArrayOf<Char32> > pwd = mPassword->Clone();
    *password = pwd;
    REFCOUNT_ADD(*password)
    return NOERROR;
}

ECode CPasswordAuthentication::GetUserName(
    /* [out] */ String* userName)
{
    VALIDATE_NOT_NULL(userName);
    *userName = mUserName;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
