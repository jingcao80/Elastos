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

#ifndef __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__
#define __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/server/net/DelayedDiskWrite.h"

using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::IDataOutputStream;
using Elastos::Droid::Net::IIpConfiguration;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class IpConfigStore
    : public Object
{
    friend class InnerSub_Writer;

private:
    class InnerSub_Writer
        : public Object
        , public IDelayedDiskWriteWriter
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_Writer(
            /* [in] */ IpConfigStore* host,
            /* [in] */ ISparseArray* networks);

        CARAPI OnWriteCalled(
            /* [in] */ IDataOutputStream* out);

    private:
        IpConfigStore* mHost;
        AutoPtr<ISparseArray> mNetworks;
    };

public:
    CARAPI constructor();

    CARAPI WriteIpAndProxyConfigurations(
        /* [in] */ const String& filePath,
        /* [in] */ ISparseArray* networks);

    CARAPI ReadIpAndProxyConfigurations(
        /* [in] */ const String& filePath,
        /* [out] */ ISparseArray** result);

    CARAPI Loge(
        /* [in] */ const char* fmt, ...);

    CARAPI Log(
        /* [in] */ const char* fmt, ...);

private:
    CARAPI WriteConfig(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ Int32 configKey,
        /* [in] */ IIpConfiguration* config,
        /* [out] */ Boolean* result);

public:
    /* const */ AutoPtr<DelayedDiskWrite> mWriter;

    /* IP and proxy configuration keys */
    static const String ID_KEY;

    static const String IP_ASSIGNMENT_KEY;

    static const String LINK_ADDRESS_KEY;

    static const String GATEWAY_KEY;

    static const String DNS_KEY;

    static const String PROXY_SETTINGS_KEY;

    static const String PROXY_HOST_KEY;

    static const String PROXY_PORT_KEY;

    static const String PROXY_PAC_FILE;

    static const String EXCLUSION_LIST_KEY;

    static const String EOS;

    static const Int32 IPCONFIG_FILE_VERSION;

private:
    static const String TAG;

    static const Boolean DBG;
};

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_IPCONFIGSTORE_H__
