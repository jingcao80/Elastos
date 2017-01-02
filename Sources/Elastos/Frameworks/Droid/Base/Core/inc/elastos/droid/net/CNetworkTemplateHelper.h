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

#ifndef __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__

#include "_Elastos_Droid_Net_CNetworkTemplateHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Template definition used to generically match {@link NetworkIdentity},
 * usually when collecting statistics.
 *
 * @hide
 */
CarClass(CNetworkTemplateHelper)
    , public Singleton
    , public INetworkTemplateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ForceAllNetworkTypes();

    CARAPI BuildTemplateMobileAll(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobile3gLower(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobile4g(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobileWildcard(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifiWildcard(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifi(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifi(
        /* [in] */ const String& networkId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateEthernet(
        /* [out] */ INetworkTemplate** result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__
