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

#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__

#include "_Libcore_Net_Url_CJarHandler.h"
#include "URLStreamHandler.h"

using Elastos::Net::URLStreamHandler;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::IInetAddress;

namespace Libcore {
namespace Net {
namespace Url {

CarClass(CJarHandler)
    , public URLStreamHandler
    , public IJarHandler
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    virtual CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI ParseURL(
        /* [in] */ IURL* url,
        /* [in] */ const String& spec,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI ToExternalForm(
        /* [in] */ IURL* url,
        /* [out] */ String* s);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__
