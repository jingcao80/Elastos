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

#include "DefaultSSLSocketFactory.h"
#include "EmptyArray.h"

using Libcore::Utility::EmptyArray;

namespace Elastosx {
namespace Net {
namespace Ssl {

ECode DefaultSSLSocketFactory::constructor(
    /* [in] */ const String& mes)
{
    mErrMessage = mes;
    return NOERROR;
}

ECode DefaultSSLSocketFactory::GetDefaultCipherSuites(
    /* [out, callee] */ ArrayOf<String>** suites)
{
    VALIDATE_NOT_NULL(suites)

    *suites = EmptyArray::STRING;
    REFCOUNT_ADD(*suites);
    return NOERROR;
}

ECode DefaultSSLSocketFactory::GetSupportedCipherSuites(
    /* [out, callee] */ ArrayOf<String>** suites)
{
    VALIDATE_NOT_NULL(suites)

    *suites = EmptyArray::STRING;
    REFCOUNT_ADD(*suites);
    return NOERROR;
}

ECode DefaultSSLSocketFactory::CreateSocket(
    /* [in] */ ISocket* s,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ Boolean autoClose,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localHost,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

ECode DefaultSSLSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    //throw new SocketException(errMessage);
    return E_SOCKET_EXCEPTION;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
