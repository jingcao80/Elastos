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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_WIREDHEADSETMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_WIREDHEADSETMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/** Listens for and caches headset state. */
class WiredHeadsetManager
    : public Object
{
public:
    /** Receiver for wired headset plugged and unplugged events. */
    class WiredHeadsetBroadcastReceiver
        : public BroadcastReceiver
        , public IWiredHeadsetManagerWiredHeadsetBroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WiredHeadsetManager::WiredHeadsetBroadcastReceiver")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WiredHeadsetManager* mHost;
    };

public:
    WiredHeadsetManager();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI AddListener(
        /* [in] */ IWiredHeadsetManagerListener* listener);

    CARAPI RemoveListener(
        /* [in] */ IWiredHeadsetManagerListener* listener);

    CARAPI IsPluggedIn(
        /* [out] */ Boolean* result);

private:
    CARAPI OnHeadsetPluggedInChanged(
        /* [in] */ Boolean isPluggedIn);

private:
    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mListeners;

    AutoPtr<IWiredHeadsetManagerWiredHeadsetBroadcastReceiver> mReceiver;

    Boolean mIsPluggedIn;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_WIREDHEADSETMANAGER_H__
