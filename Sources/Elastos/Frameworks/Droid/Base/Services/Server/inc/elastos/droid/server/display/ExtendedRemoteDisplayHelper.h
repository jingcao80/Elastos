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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__

#include "_Elastos.Droid.Server.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRemoteDisplayListener;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

class ExtendedRemoteDisplayHelper
{
public:
    /**
     * Starts listening for displays to be connected on the specified interface.
     *
     * @param iface The interface address and port in the form "x.x.x.x:y".
     * @param listener The listener to invoke
     *         when displays are connected or disconnected.
     * @param handler The handler on which to invoke the listener.
     * @param context The current service context
     *  */
    static CARAPI Listen(
        /* [in] */ const String& iface,
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler,
        /* [in] */ IContext* context,
        /* [out] */ IInterface** result);

    /**
     * Disconnects the remote display and stops listening for new connections.
     */
    static CARAPI Dispose(
        /* [in] */ IInterface* extRemoteDisplay);

    /**
     * Checks if ExtendedRemoteDisplay is available
     */
    static CARAPI_(Boolean) IsAvailable();

private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

private:
    static const String TAG;

    // ExtendedRemoteDisplay class
    // ExtendedRemoteDisplay is an enhanced RemoteDisplay. It has
    // similar interface as RemoteDisplay class
    static AutoPtr<IClassInfo> sExtRemoteDisplayClass;

    // Method object for the API ExtendedRemoteDisplay.Listen
    // ExtendedRemoteDisplay.Listen has the same API signature as
    // RemoteDisplay.Listen except for an additional argument to pass the
    // Context
    static AutoPtr<IMethodInfo> sExtRemoteDisplayListen;

    // Method Object for the API ExtendedRemoteDisplay.Dispose
    // ExtendedRemoteDisplay.Dispose follows the same API signature as
    // RemoteDisplay.Dispose
    static AutoPtr<IMethodInfo> sExtRemoteDisplayDispose;
    static const StaticInitializer sInitializer;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__
