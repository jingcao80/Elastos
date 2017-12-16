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

#ifndef __ELASTOS_DROID_MEDIA_CREMOTEDISPLAY_H__
#define __ELASTOS_DROID_MEDIA_CREMOTEDISPLAY_H__

#include "_Elastos_Droid_Media_CRemoteDisplay.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::View::ISurface;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRemoteDisplay)
    , public Object
    , public IRemoteDisplay
{
public:
    class NotifyDisplayConnectedRun
         : public Runnable
    {
    public:
        NotifyDisplayConnectedRun(
            /* [in] */ CRemoteDisplay* owner,
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 flags);

        CARAPI Run();

    private:
        CRemoteDisplay* mOwner;

        AutoPtr<ISurface> mSurface;

        Int32 mWidth, mHeight, mFlags, mSession;
    };

    class NotifyDisplayDisconnectedRun
         : public Runnable
    {
    public:
        NotifyDisplayDisconnectedRun(
            /* [in] */ CRemoteDisplay* owner);

        CARAPI Run();

    private:
        CRemoteDisplay* mOwner;
    };

    class NotifyDisplayErrorRun
         : public Runnable
    {
    public:
        NotifyDisplayErrorRun(
            /* [in] */ CRemoteDisplay* owner,
            /* [in] */ Int32 error);

        CARAPI Run();

    private:
        CRemoteDisplay* mOwner;

        Int32 mError;
    };

public:
    CRemoteDisplay();

    virtual ~CRemoteDisplay();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Disconnects the remote display and stops listening for new connections.
     */
    CARAPI Dispose();

    CARAPI Pause();

    CARAPI Resume();

    /**
     * Starts listening for displays to be connected on the specified interface.
     *
     * @param iface The interface address and port in the form "x.x.x.x:y".
     * @param listener The listener to invoke when displays are connected or disconnected.
     * @param handler The handler on which to invoke the listener.
     */
    static CARAPI Listen(
        /* [in] */ const String& iface,
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ IRemoteDisplay** result);

    // Called from native.
    CARAPI_(void) NotifyDisplayConnected(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 session);

    // Called from native.
    CARAPI_(void) NotifyDisplayDisconnected();

    // Called from native.
    CARAPI_(void) NotifyDisplayError(
        /* [in] */ Int32 error);

protected:
    //@Override
    CARAPI_(void) Finalize();

private:
    CARAPI_(Int64) NativeListen(
        /* [in] */ const String& iface);

    CARAPI_(void) NativeDispose(
        /* [in] */ Int64 ptr);

    CARAPI_(void) NativePause(
        /* [in] */ Int64 ptr);

    CARAPI_(void) NativeResume(
        /* [in] */ Int64 ptr);

    CARAPI_(void) Dispose(
        /* [in] */ Boolean finalized);

    CARAPI_(void) StartListening(
        /* [in] */ const String& iface);

private:
    AutoPtr<ICloseGuard> mGuard; // = CloseGuard.get();

    AutoPtr<IRemoteDisplayListener> mListener;

    AutoPtr<IHandler> mHandler;

    Int64 mPtr;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CREMOTEDISPLAY_H__
