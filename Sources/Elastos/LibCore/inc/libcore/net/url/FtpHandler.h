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

#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "URLStreamHandler.h"

using Elastos::Net::URLStreamHandler;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;

namespace Libcore {
namespace Net {
namespace Url {

class FtpHandler
    : public URLStreamHandler
    , public IFtpHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

protected:
    using URLStreamHandler::OpenConnection;

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [in] */ IProxy* proxy,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI GetDefaultPort(
        /* [out] */ Int32* portNum);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
