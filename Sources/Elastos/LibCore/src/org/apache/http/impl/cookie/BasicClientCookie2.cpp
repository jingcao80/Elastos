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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/cookie/BasicClientCookie2.h"

using Org::Apache::Http::Cookie::EIID_ISetCookie2;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

BasicClientCookie2::BasicClientCookie2()
    : mDiscard(FALSE)
{}

BasicClientCookie2::BasicClientCookie2(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
    : BasicClientCookie(name, value)
    , mDiscard(FALSE)
{}

CAR_INTERFACE_IMPL(BasicClientCookie2, BasicClientCookie, ISetCookie2)

ECode BasicClientCookie2::GetPorts(
    /* [out, callee] */ ArrayOf<Int32>** ports)
{
    VALIDATE_NOT_NULL(ports)
    *ports = mPorts;
    REFCOUNT_ADD(*ports)
    return NOERROR;
}

ECode BasicClientCookie2::SetPorts(
    /* [in] */ ArrayOf<Int32>* ports)
{
    mPorts = ports;
    return NOERROR;
}

ECode BasicClientCookie2::GetCommentURL(
    /* [out] */ String* commentURL)
{
    VALIDATE_NOT_NULL(commentURL)
    *commentURL = mCommentURL;
    return NOERROR;
}

ECode BasicClientCookie2::SetCommentURL(
    /* [in] */ const String& commentURL)
{
    mCommentURL = commentURL;
    return NOERROR;
}

ECode BasicClientCookie2::SetDiscard(
    /* [in] */ Boolean discard)
{
    mDiscard = discard;
    return NOERROR;
}

ECode BasicClientCookie2::IsPersistent(
    /* [out] */ Boolean* isPersistent)
{
    VALIDATE_NOT_NULL(isPersistent)
    Boolean result;
    *isPersistent = !mDiscard && (BasicClientCookie::IsPersistent(&result), result);
    return NOERROR;
}

ECode BasicClientCookie2::IsExpired(
    /* [in] */ IDate* date,
    /* [out] */ Boolean* isExpired)
{
    VALIDATE_NOT_NULL(isExpired)
    Boolean result;
    *isExpired = mDiscard && (BasicClientCookie::IsExpired(date, &result), result);
    return NOERROR;
}

ECode BasicClientCookie2::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<BasicClientCookie2> cookie = new BasicClientCookie2();
    CloneImpl(cookie);
    *object = cookie->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode BasicClientCookie2::CloneImpl(
    /* [in] */ BasicClientCookie2* cookie)
{
    BasicClientCookie::CloneImpl((BasicClientCookie*)cookie);
    cookie->mCommentURL = mCommentURL;
    cookie->mPorts = mPorts->Clone();
    cookie->mDiscard = mDiscard;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org