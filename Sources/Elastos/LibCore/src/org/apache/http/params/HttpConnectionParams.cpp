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

#include "org/apache/http/params/HttpConnectionParams.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

ECode HttpConnectionParams::GetSoTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    *timeout = 0;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt32Parameter(ICoreConnectionPNames::SO_TIMEOUT, 0, timeout);
}

ECode HttpConnectionParams::SetSoTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 timeout)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetInt32Parameter(ICoreConnectionPNames::SO_TIMEOUT, timeout, (IHttpParams**)&p);
}

ECode HttpConnectionParams::GetTcpNoDelay(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetBooleanParameter(ICoreConnectionPNames::TCP_NODELAY, TRUE, result);
}

ECode HttpConnectionParams::SetTcpNoDelay(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetBooleanParameter(ICoreConnectionPNames::TCP_NODELAY, value, (IHttpParams**)&p);
}

ECode HttpConnectionParams::GetSocketBufferSize(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = -1;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt32Parameter(ICoreConnectionPNames::SOCKET_BUFFER_SIZE, -1, size);
}

ECode HttpConnectionParams::SetSocketBufferSize(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 size)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetInt32Parameter(ICoreConnectionPNames::SOCKET_BUFFER_SIZE, size, (IHttpParams**)&p);
}

ECode HttpConnectionParams::GetLinger(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* linger)
{
    VALIDATE_NOT_NULL(linger)
    *linger = -1;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt32Parameter(ICoreConnectionPNames::SO_LINGER_HTTP, -1, linger);
}

ECode HttpConnectionParams::SetLinger(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 value)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetInt32Parameter(ICoreConnectionPNames::SO_LINGER_HTTP, value, (IHttpParams**)&p);
}

ECode HttpConnectionParams::GetConnectionTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    *timeout = 0;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetInt32Parameter(ICoreConnectionPNames::CONNECTION_TIMEOUT, 0, timeout);
}

ECode HttpConnectionParams::SetConnectionTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 timeout)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetInt32Parameter(ICoreConnectionPNames::CONNECTION_TIMEOUT, timeout, (IHttpParams**)&p);
}

ECode HttpConnectionParams::IsStaleCheckingEnabled(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return params->GetBooleanParameter(ICoreConnectionPNames::STALE_CONNECTION_CHECK, TRUE, result);
}

ECode HttpConnectionParams::SetStaleCheckingEnabled(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    if (params == NULL) {
        Logger::E("HttpConnectionParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetBooleanParameter(ICoreConnectionPNames::STALE_CONNECTION_CHECK, value, (IHttpParams**)&p);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
