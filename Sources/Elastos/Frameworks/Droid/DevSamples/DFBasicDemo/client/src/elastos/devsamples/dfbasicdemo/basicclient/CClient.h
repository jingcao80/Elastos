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

#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICCLIENT_CCLIENT_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICCLIENT_CCLIENT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_DFBasicDemo_BasicClient_CClient.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <_Org.Alljoyn.Bus.h>
#include <org/alljoyn/bus/BusListener.h>
#include <org/alljoyn/bus/SessionListener.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;
using Elastos::Utility::IMap;

using Org::Alljoyn::Bus::BusListener;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IProxyBusObject;
using Org::Alljoyn::Bus::SessionListener;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicClient {

CarClass(CClient)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CClient* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CClient* mHost;
    };

    class EditorListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        EditorListener(
            /* [in] */ CClient* host);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        CClient* mHost;
    };

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class BusHandler
        : public Handler
    {
    private:
        class InnerBusListener : public BusListener
        {
        public:
            InnerBusListener(
                /* [in] */ BusHandler* host)
                : mHost(host)
            {}

            // @Override
            CARAPI FoundAdvertisedName(
                /* [in] */ const String& name,
                /* [in] */ Int16 transport,
                /* [in] */ const String& namePrefix);

        private:
            BusHandler* mHost;
        };

        class InnerSessionListener : public SessionListener
        {
        public:
            InnerSessionListener(
                /* [in] */ BusHandler* host)
                : mHost(host)
            {}

            // @Override
            CARAPI SessionLost(
                /* [in] */ Int32 sessionId,
                /* [in] */ Int32 reason);

        private:
            BusHandler* mHost;
        };

    public:
        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ CClient* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        /* Helper function to send a message to the UI thread. */
        CARAPI_(void) SendUiMessage(
            /* [in] */ Int32 what,
            /* [in] */ const String& str);

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 CONNECT;
        static const Int32 JOIN_SESSION;
        static const Int32 DISCONNECT;
        static const Int32 CAT;

    private:
        /*
         * Name used as the well-known name and the advertised name of the service this client is
         * interested in.  This name must be a unique name both to the bus and to the network as a
         * whole.
         *
         * The name uses reverse URL style of naming, and matches the name used by the service.
         */
        static const String SERVICE_NAME;
        static const Int16 CONTACT_PORT;

        AutoPtr<IBusAttachment> mBus;
        AutoPtr<IProxyBusObject> mProxyObj;
        AutoPtr<IBasicInterface> mBasicInterface;

        Int32 mSessionId;
        Boolean mIsInASession;
        Boolean mIsConnected;
        Boolean mIsStoppingDiscovery;
        CClient* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnDestroy();

private:
    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

    CARAPI_(void) LogException(
        /* [in] */ const String& msg,
        /* [in] */ ECode ex);

private:
    static const Int32 MESSAGE_PING = 1;
    static const Int32 MESSAGE_PING_REPLY = 2;
    static const Int32 MESSAGE_POST_TOAST = 3;
    static const Int32 MESSAGE_START_PROGRESS_DIALOG = 4;
    static const Int32 MESSAGE_STOP_PROGRESS_DIALOG = 5;

    static const String TAG;

    AutoPtr<IEditText> mEditText;
    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;

    /* Handler used to make calls to AllJoyn methods. See onCreate(). */
    AutoPtr<BusHandler> mBusHandler;

    AutoPtr<IProgressDialog> mDialog;

    AutoPtr<IHandler> mHandler;
};

} // namespace BasicClient
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_CCLIENT_H__
