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

#ifndef __ELASTOS_DROID_NET_ETHERNETMANAGER_H__
#define __ELASTOS_DROID_NET_ETHERNETMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing the IP configuration of the Ethernet network.
 *
 * @hide
 */
class EthernetManager
    : public Object
    , public IEthernetManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new EthernetManager instance.
     * Applications will almost always want to use
     * {@link android.content.Context#getSystemService Context.getSystemService()} to retrieve
     * the standard {@link android.content.Context#ETHERNET_SERVICE Context.ETHERNET_SERVICE}.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIEthernetManager* service);

    /**
     * Get Ethernet configuration.
     * @return the Ethernet Configuration, contained in {@link IpConfiguration}.
     */
    CARAPI GetConfiguration(
        /* [out] */ IIpConfiguration** result);

    /**
     * Set Ethernet configuration.
     */
    CARAPI SetConfiguration(
        /* [in] */ IIpConfiguration* config);

private:
    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IIEthernetManager> mService;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_ETHERNETMANAGER_H__
