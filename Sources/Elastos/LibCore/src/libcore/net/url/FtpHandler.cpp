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

#include "FtpHandler.h"

using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FtpHandler, URLStreamHandler, IFtpHandler)

ECode FtpHandler::constructor()
{
    return NOERROR;
}
ECode FtpHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [out] */ IURLConnection** urlConnection)
{
    // return new FtpURLConnection(u);
    return NOERROR;
}

ECode FtpHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [in] */ IProxy* proxy,
    /* [out] */ IURLConnection** urlConnection)
{
    // if (url == null || proxy == null) {
    //     throw new IllegalArgumentException("url == null || proxy == null");
    // }
    // return new FtpURLConnection(url, proxy);
    return NOERROR;
}

ECode FtpHandler::GetDefaultPort(
    /* [out] */ Int32* portNum)
{
    VALIDATE_NOT_NULL(portNum)

    *portNum = 21;
    return NOERROR;
}

} // namespace Url
} // namespace Net
} // namespace Libcore
