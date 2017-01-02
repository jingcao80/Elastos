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

#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_SrpCLIENT_CCLIENT_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_SrpCLIENT_CCLIENT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "_Elastos_DevSamples_DFSecureDemo_SrpClient_CClient.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <_Org.Alljoyn.Bus.h>
#include <org/alljoyn/bus/BusListener.h>
#include <org/alljoyn/bus/SessionListener.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
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
using Elastos::Utility::Concurrent::ICountDownLatch;

using Org::Alljoyn::Bus::IAuthListener;
using Org::Alljoyn::Bus::IAuthRequest;
using Org::Alljoyn::Bus::BusListener;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IProxyBusObject;
using Org::Alljoyn::Bus::SessionListener;

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace SrpClient {

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

    class OnEditorActionListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        OnEditorActionListener(
            /* [in] */ CClient* host);

        CAR_INTERFACE_DECL()

        CARAPI OnEditorAction(
            /* [in] */ ITextView* view,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        TO_STRING_IMPL("CClient::OnEditorActionListener")

    private:
        CClient* mHost;
    };

    /*
     * The main differences between a secure application and a plain application, besides the
     * @Secure annotations of the interfaces, are encapsulated in the AuthListener.  The
     * BusAttachment calls the listener with various authentication requests in the process of
     * authenticating a peer.  The requests made are dependent on the specific authentication
     * mechanism negotiated between the peers.
     */
    class SrpKeyXListener
        : public Object
        , public IAuthListener
    {
    public:
        SrpKeyXListener(
            /* [in] */ CClient* host);

        CAR_INTERFACE_DECL()

        /* Returns the list of supported mechanisms. */
        CARAPI_(String) GetMechanisms();

        /*
         * Persistent authentication and encryption data is stored at this location.
         *
         * This uses the private file area associated with the application package.
         */
        CARAPI_(String) GetKeyStoreFileName();

        /*
         * Authentication requests are being made.  Contained in this call are the mechanism in use,
         * the number of attempts made so far, the desired user name for the requests, and the
         * specific credentials being requested in the form of AuthRequests.
         *
         * The ALLJOYN_SRP_KEYX mechanism will request the password.
         */
        CARAPI Requested(
            /* [in] */ const String& mechanism,
            /* [in] */ const String& peer,
            /* [in] */ Int32 count,
            /* [in] */ const String& userName,
            /* [in] */ ArrayOf<IAuthRequest*>* requests,
            /* [out] */ Boolean* res);

        CARAPI Completed(
            /* [in] */ const String& mechanism,
            /* [in] */ const String& authPeer,
            /* [in] */ Boolean authenticated);

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

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 CONNECT;
        static const Int32 JOIN_SESSION;
        static const Int32 DISCONNECT;
        static const Int32 PING;

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
        AutoPtr<ISecureInterface> mSecureInterface;

        Int32 mSessionId;
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

    // @Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

private:
    CARAPI_(void) SendUiMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

    CARAPI_(void) LogException(
        /* [in] */ const String& msg,
        /* [in] */ ECode ex);

private:
    static const Int32 DIALOG_GET_CREDENTIALS = 1;

    static const Int32 MESSAGE_PING = 1;
    static const Int32 MESSAGE_PING_REPLY = 2;
    static const Int32 MESSAGE_GET_CREDENTIALS = 3;
    static const Int32 MESSAGE_AUTH_COMPLETE = 4;
    static const Int32 MESSAGE_POST_TOAST = 5;
    static const Int32 MESSAGE_START_PROGRESS_DIALOG = 6;
    static const Int32 MESSAGE_STOP_PROGRESS_DIALOG = 7;

    static const String TAG;

    AutoPtr<IEditText> mEditText;
    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;
    AutoPtr<ICountDownLatch> mLatch;
    String mPassword;

    AutoPtr<IProgressDialog> mDialog;
    AutoPtr<IHandler> mHandler;
    AutoPtr<SrpKeyXListener> mAuthListener;
    AutoPtr<BusHandler> mBusHandler;
};

} // namespace SrpClient
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_SrpCLIENT_CCLIENT_H__
