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

#ifndef __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__

#include "_Elastos_Droid_Net_Http_CHttpsConnectionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * A Connection connecting to a secure http server or tunneling through
 * a http proxy server to a https server.
 *
 * @hide
 */
CarClass(CHttpsConnectionHelper)
    , public Singleton
    , public IHttpsConnectionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     *
     * @param sessionDir directory to cache SSL sessions
     */
    CARAPI InitializeEngine(
        /* [in] */ IFile* sessionDir);
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CHTTPSCONNECTIONHELPER_H__
