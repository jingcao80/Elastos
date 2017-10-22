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

#ifndef __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__
#define __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessenger;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Net {

class BaseNetworkStateTracker
    : public Object
    , public INetworkStateTracker
    , public IBaseNetworkStateTracker
{
    // TODO: better document threading expectations
    // TODO: migrate to make NetworkStateTracker abstract class

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 networkType);

    // @Override
    virtual CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    virtual CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    virtual CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** result);

    // @Override
    CARAPI GetNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    virtual CARAPI GetLinkQualityInfo(
        /* [out] */ ILinkQualityInfo** result);

    virtual CARAPI CaptivePortalCheckCompleted(
        /* [in] */ Boolean isCaptive);

    virtual CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    virtual CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    virtual CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    virtual CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* result);

    virtual CARAPI PrivateDnsRouteSet(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* result);

    virtual CARAPI DefaultRouteSet(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsTeardownRequested(
        /* [out] */ Boolean* result);

    virtual CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    virtual CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

    // @Override
    virtual CARAPI AddStackedLink(
        /* [in] */ ILinkProperties* link);

    // @Override
    virtual CARAPI RemoveStackedLink(
        /* [in] */ ILinkProperties* link);

    // @Override
    virtual CARAPI SupplyMessenger(
        /* [in] */ IMessenger* messenger);

    // @Override
    virtual CARAPI GetNetworkInterfaceName(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI StartSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    // @Override
    virtual CARAPI StopSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    // @Override
    virtual CARAPI SetNetId(
        /* [in] */ Int32 netId);

    // @Override
    virtual CARAPI GetNetwork(
        /* [out] */ INetwork** result);

protected:
    BaseNetworkStateTracker();

    CARAPI constructor();

    virtual CARAPI GetTargetHandler(
        /* [out] */ IHandler** handler);

    virtual CARAPI DispatchStateChanged();

    virtual CARAPI DispatchConfigurationChanged();

    virtual CARAPI StartMonitoringInternal();

protected:
    AutoPtr<IContext> mContext;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<INetwork> mNetwork;
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

private:
    AutoPtr<IHandler> mTarget;
    AutoPtr<IAtomicBoolean> mTeardownRequested;
    AutoPtr<IAtomicBoolean> mPrivateDnsRouteSet;
    AutoPtr<IAtomicBoolean> mDefaultRouteSet;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__
