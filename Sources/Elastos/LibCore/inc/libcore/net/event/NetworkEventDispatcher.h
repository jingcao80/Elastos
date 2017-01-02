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

#ifndef __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__
#define __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;

namespace Libcore {
namespace Net {
namespace Event {

/**
 * A singleton used to dispatch network events to registered listeners.
 */
class NetworkEventDispatcher
    : public Object
    , public INetworkEventDispatcher
{
public:
    CAR_INTERFACE_DECL()

    /**
    * Returns the shared {@link NetworkEventDispatcher} instance.
    */
    static CARAPI GetInstance(
        /* [out] */ INetworkEventDispatcher** result);

    /**
    * Registers a listener to be notified when network events occur.
    * It can be deregistered using {@link #removeListener(NetworkEventListener)}
    */
    CARAPI AddListener(
        /* [in] */ INetworkEventListener* toAdd);

    /**
    * De-registers a listener previously added with {@link #addListener(NetworkEventListener)}. If
    * the listener was not previously registered this is a no-op.
    */
    CARAPI RemoveListener(
        /* [in] */ INetworkEventListener* toRemove);

    /**
    * Notifies registered listeners of a network configuration change.
    */
    CARAPI OnNetworkConfigurationChanged();

protected:
    /** Visible for testing. Use {@link #getInstance()} instead. */
    NetworkEventDispatcher();

private:
    static AutoPtr<INetworkEventDispatcher> sInstance;

    // private final List<NetworkEventListener> listeners =
    //   new CopyOnWriteArrayList<NetworkEventListener>();
    AutoPtr<ICopyOnWriteArrayList> mListeners;
};

} // namespace Event
} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_EVENT_NETWORKEVENTDISPATCHER_H__