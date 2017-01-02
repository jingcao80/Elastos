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

#include "CProxy.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Net {

AutoPtr<IProxy> StaticInitProxy()
{
    AutoPtr<CProxy> proxy;
    CProxy::NewByFriend((CProxy**)&proxy);
    return (IProxy*)proxy.Get();
}

AutoPtr<IProxy> CProxy::NO_PROXY = StaticInitProxy();

CAR_INTERFACE_IMPL(CProxy, Object, IProxy)

CAR_OBJECT_IMPL(CProxy)

ECode CProxy::constructor(
    /* [in] */ ProxyType type,
    /* [in] */ ISocketAddress* sa)
{
    /*
     * Don't use DIRECT type to construct a proxy instance directly.
     * SocketAddress must NOT be null.
     */
    if (type == ProxyType_DIRECT || sa == NULL) {
//        throw new IllegalArgumentException("Illegal Proxy.Type or SocketAddress argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = type;
    mAddress = sa;
    return NOERROR;
}

ECode CProxy::constructor()
{
    mType = ProxyType_DIRECT;
    mAddress = NULL;
    return NOERROR;
}

ECode CProxy::GetType(
    /* [out] */ ProxyType* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CProxy::GetAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = mAddress;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode CProxy::GetNO_PROXY(
    /* [out] */ IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy);
    *proxy = NO_PROXY;
    REFCOUNT_ADD(*proxy);
    return NOERROR;
}

ECode CProxy::ToString(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder builder;
    if (mType == ProxyType_DIRECT) {
        builder.Append("DIRECT");
    }
    else if (mType == ProxyType_HTTP) {
        builder.Append("HTTP");
    }
    else if (mType == ProxyType_SOCKS) {
        builder.Append("SOCKS");
    }

    builder.Append("@");
    if (mType != ProxyType_DIRECT && mAddress != NULL) {
        String info = Object::ToString(mAddress);
        builder.Append(info);
    }
    *str = builder.ToString();
    return NOERROR;
}

ECode CProxy::Equals(
    /* [in] */ IInterface * o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    IProxy * other = IProxy::Probe(o);
    if (NULL == other) {
        *result = FALSE;
        return NOERROR;
    }

    if (other == this) {
        *result = TRUE;
        return NOERROR;
    }

    CProxy* another = (CProxy *) other;
    // address is null when and only when it's NO_PROXY.
    *result = (mType == another->mType)
        && Object::Equals(mAddress, another->mAddress);

    return NOERROR;
}

ECode CProxy::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 ret = 0;
    ret += mType;
    if (mAddress != NULL) {
        ret += Object::GetHashCode(mAddress);
    }
    *hash = ret;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
