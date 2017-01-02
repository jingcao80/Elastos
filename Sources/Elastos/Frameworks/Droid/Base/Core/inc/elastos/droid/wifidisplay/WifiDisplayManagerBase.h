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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace WifiDisplay {

class WifiDisplayManagerBase
{
private:
    class H
        : public HandlerBase
    {
    public:
        H(
            /* [in] */ WifiDisplayManagerBase* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        static String CodeToString(
            /* [in] */ Int32 code);
    public:
        static const Int32 EXIT_WIFIDISPLAY_RECEIVE         ;//= 100;
        static const Int32 EXIT_WIFIDISPLAY_SEND            ;//= 101;
        static const Int32 START_WIFIDISPLAY_SEND           ;//= 102;
        static const Int32 DISPATCH_WIFIDISPLAY_EVEVT       ;//= 103;
    private:
        WifiDisplayManagerBase* mHost;
    };

    class WifiDisplayThread
        : public ElRefBase
        , public IWifiDisplayThread
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        WifiDisplayThread(
            /* [in] */ WifiDisplayManagerBase* host);

        CARAPI ExitWifiDisplayReceive();

        CARAPI ExitWifiDisplaySend();

        CARAPI StartWifiDisplaySend();

        CARAPI DispatchMotionEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI GetHashCode(
             /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* info);
    private:
        WifiDisplayManagerBase* mHost;
    };

public:
    WifiDisplayManagerBase();

    virtual ~WifiDisplayManagerBase();

    virtual CARAPI StartWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI EndWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI StartWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI EndWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

protected:
    virtual CARAPI OnExitWifiDisplayReceive() = 0;

    virtual CARAPI OnExitWifiDisplaySend() = 0;

    virtual CARAPI OnStartWifiDisplaySend() = 0;

    virtual CARAPI OnDispatchMotionEvent(
        /* [in] */ IMotionEvent* motionevent) = 0;

private:
    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1);

    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

public:
    static const Int32 EXIT_WIFIDISPLAY_RECEIVE         = 100;
    static const Int32 EXIT_WIFIDISPLAY_SEND            = 101;
    static const Int32 START_WIFIDISPLAY_SEND           = 102;
    static const Int32 DISPATCH_WIFIDISPLAY_EVEVT       = 103;

    static const String TAG;
private:
    static const Boolean DEBUG_MESSAGES;// = false;
    static const Boolean DEBUG_REMOTE;// = true;

    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IWifiDisplayManager> mWifiDisplayManager;
    AutoPtr<IWifiDisplayThread> mWifiDisplayThread;

    AutoPtr<IHandler> mH;
    Object mThisLock;
};

} // namespace WifiDisplay
} // namespace Droid
} // namespace Elastos
