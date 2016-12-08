
#ifndef __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSERVICE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "_Elastos_DevSamples_DFSecureDemo_SecureService_CService.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <_Org.Alljoyn.Bus.h>
#include <org/alljoyn/bus/SessionPortListener.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;
using Elastos::Utility::IRandom;
using Elastos::Utility::Concurrent::ICountDownLatch;

using Org::Alljoyn::Bus::IAuthListener;
using Org::Alljoyn::Bus::IAuthRequest;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::SessionPortListener;

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace SecureService {

CarClass(CService)
    , public Activity
{
public:
    /*
     * The main differences between a secure application and a plain application, besides the
     * @Secure annotations of the interfaces, are encapsulated in the AuthListener.  The
     * BusAttachment calls the listener with various authentication requests in the process of
     * authenticating a peer.  The requests made are dependent on the specific authentication
     * mechanism negotiated between the peers.
     *
     * This class, registered with the BusAttachment, supports all the available authentication
     * mechanisms.
     */
    class AuthListeners
        : public Object
        , public IAuthListener
    {
    public:
        AuthListeners(
            /* [in] */ CService* host);

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
         * A TRUE return value tells the BusAttachment that the requests have been handled.
         *
         * This simply defers to the specific listener based on the mechanism in use.
         */
        CARAPI Requested(
            /* [in] */ const String& mechanism,
            /* [in] */ const String& peer,
            /* [in] */ Int32 count,
            /* [in] */ const String& userName,
            /* [in] */ ArrayOf<IAuthRequest*>* requests,
            /* [out] */ Boolean* res);

        /*
         * An authentication attempt has completed, either successfully or unsuccessfully.
         *
         * This simply defers to the specific listener based on the mechanism in use.
         */
        CARAPI Completed(
            /* [in] */ const String& mechanism,
            /* [in] */ const String& authPeer,
            /* [in] */ Boolean authenticated);

    private:
        /* Map from authentication mechanism to the specific listener. */
        HashMap<String, AutoPtr<IAuthListener> > mAuthListeners;
        CService* mHost;
    };

    /*
     * This Secure Remote Password Key eXchange listener creates a random 6-digit pincode for each
     * authentication attempt.
     */
    class SrpKeyXListener
        : public Object
        , public IAuthListener
    {
    public:
        SrpKeyXListener(
            /* [in] */ CService* host);

        CAR_INTERFACE_DECL()

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
        AutoPtr<IRandom> mPasswordGenerator;
        Boolean mGeneratePassword;
        CService* mHost;
    };

    /*
     * This Secure Remote Password Logon listener responds to authentication requests from a table
     * of username, password pairs similar to /etc/passwd.
     */
    class SrpLogonListener
        : public Object
        , public IAuthListener
    {
    public:
        /* Populate the username, password table used by this listener. */
        SrpLogonListener();

        CAR_INTERFACE_DECL()

        /*
         * Given the user name, lookup the password.  Returning TRUE without setting the password
         * tells the authentication engine to ask the peer for the username again.
         *
         * A pre-computed token called the logon entry may also be supplied instead of the given
         * password.  See LogonEntryRequest.
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
        HashMap<String, AutoPtr<ArrayOf<Char32> > > mUserNamePassword;
    };

    /* the ECDHEKeyXListener supports the Elliptic Curve Cryptography Diffie-Hellman Ephemeral
     * key exchanges.
     */
    class ECDHEKeyXListener
        : public Object
        , public IAuthListener
    {
    public:
        ECDHEKeyXListener(
            /* [in] */ CService* host);

        CAR_INTERFACE_DECL()

        /*
         * Authentication requests are being made.  Contained in this call are the mechanism in use,
         * the number of attempts made so far, the desired user name for the requests, and the
         * specific credentials being requested in the form of AuthRequests.
         *
         * The ECDHE key exchange can request for private key, public key, and to verify the peer
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
        Boolean mSendBackKeys;  /* toggle the send back keys */
        /* the followings are sample data to try out the ECDHE_ECDSA key exchange */
        static const String SERVER_PK_PEM;
        static const String SERVER_ECC_X509_PEM;
        CService* mHost;
    };

    class SecureService
        : public Object
        , public ISecureInterface
        , public IBusObject
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IObject* host);

        CARAPI Ping(
            /* [in] */ const String& inStr,
            /* [out] */ String* result);

    private:
        CService* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CService::MyHandler")

        MyHandler(
            /* [in] */ CService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CService* mHost;
    };

    class OnEditorActionListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        OnEditorActionListener(
            /* [in] */ CService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnEditorAction(
            /* [in] */ ITextView* view,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        TO_STRING_IMPL("CService::OnEditorActionListener")

    private:
        CService* mHost;
    };

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class BusHandler
        : public Handler
    {
    private:
        class InnerSessionPortListener : public SessionPortListener
        {
        public:
            InnerSessionPortListener(
                /* [in] */ BusHandler* host)
                : mHost(host)
            {}

            CARAPI AcceptSessionJoiner(
                /* [in] */ Int16 sessionPort,
                /* [in] */ const String& joiner,
                /* [in] */ ISessionOpts* opts,
                /* [out] */ Boolean* accepted);

        private:
            BusHandler* mHost;
        };

    public:
        TO_STRING_IMPL("CService::BusHandler")

        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ CService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 CONNECT;
        static const Int32 DISCONNECT;

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
        CService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

    // @Override
    CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog);

protected:
    // @Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) SendUiMessage(
        /* [in] */ Int32 what,
        /* [in] */ const String& str);

    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

private:
    static const String TAG;

    static const Int32 DIALOG_ONE_TIME_PASSWORD = 1;
    static const Int32 DIALOG_CREATE_PASSPHRASE = 2;
    static const Int32 DIALOG_ENTER_PASSPHRASE = 3;

    static const Int32 MESSAGE_PING = 1;
    static const Int32 MESSAGE_PING_REPLY = 2;
    static const Int32 MESSAGE_SHOW_ONE_TIME_PASSWORD_DIALOG = 3;
    static const Int32 MESSAGE_SHOW_PASSPHRASE_DIALOG = 4;
    static const Int32 MESSAGE_AUTH_COMPLETE = 5;
    static const Int32 MESSAGE_POST_TOAST = 6;

    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;
    Int32 mCredentialsDialog;
    AutoPtr<ICountDownLatch> mLatch;
    String mPassword;

    AutoPtr<IHandler> mHandler;

    /* The authentication listener for our bus attachment. */
    AutoPtr<AuthListeners> mAuthListener;

    AutoPtr<SecureService> mSecureService;

    AutoPtr<BusHandler> mBusHandler;
};

} // namespace SecureService
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSECUREDEMO_SECURESERVICE_CSERVICE_H__
