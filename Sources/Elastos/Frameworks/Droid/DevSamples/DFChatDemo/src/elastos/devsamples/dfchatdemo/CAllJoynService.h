
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CALLJOYNSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CALLJOYNSERVICE_H__

#include "_Elastos_DevSamples_DFChatDemo_CAllJoynService.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <_Org.Alljoyn.Bus.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/app/Service.h>
#include <org/alljoyn/bus/BusListener.h>
#include <org/alljoyn/bus/SessionListener.h>
#include <org/alljoyn/bus/SessionPortListener.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IComponentName;
using Elastos::Utility::Etl::List;
using Org::Alljoyn::Bus::BusListener;
using Org::Alljoyn::Bus::SessionListener;
using Org::Alljoyn::Bus::SessionPortListener;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::ISessionOpts;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

class CChatApplication;

CarClass(CAllJoynService)
    , public Service
    , public IAllJoynService
    , public IObserver
{
public:
    /**
     * Our chat messages are going to be Bus Signals multicast out onto an
     * associated session.  In order to send signals, we need to define an
     * AllJoyn bus object that will allow us to instantiate a signal emmiter.
     */
    class ChatService
        : public Object
        , public IChatInterface
        , public IBusObject
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        /**
         * Intentionally empty implementation of Chat method.  Since this
         * method is only used as a signal emitter, it will never be called
         * directly.
         */
        CARAPI Chat(
            /* [in] */ const String& str);
    };

private:
    /**
     * This is the Android Service message handler.  It runs in the context of the
     * main Android Service thread, which is also shared with Activities since
     * Android is a fundamentally single-threaded system.
     *
     * The important thing for us is to note that this thread cannot be blocked for
     * a significant amount of time or we risk the dreaded "force close" message.
     * We can run relatively short-lived operations here, but we need to run our
     * distributed system calls in a background thread.
     *
     * This handler serves translates from UI-related events into AllJoyn events
     * and decides whether functions can be handled in the context of the
     * Android main thread or if they must be dispatched to a background thread
     * which can take as much time as needed to accomplish a task.
     */
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAllJoynService::MyHandler")

        MyHandler(
            /* [in] */ CAllJoynService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAllJoynService* mHost;
    };

    /**
     * This is the AllJoyn background thread handler class.  AllJoyn is a
     * distributed system and must therefore make calls to other devices over
     * networks.  These calls may take arbitrary amounts of time.  The Android
     * application framework is fundamentally single-threaded and so the main
     * Service thread that is executing in our component is the same thread as
     * the ones which appear to be executing the user interface code in the
     * other Activities.  We cannot block this thread while waiting for a
     * network to respond, so we need to run our calls in the context of a
     * background thread.  This is the class that provides that background
     * thread implementation.
     *
     * When we need to do some possibly long-lived task, we just pass a message
     * to an object implementing this class telling it what needs to be done.
     * There are two main parts to this class:  an external API and the actual
     * handler.  In order to make life easier for callers, we provide API
     * methods to deal with the actual message passing, and then when the
     * handler thread is executing the desired method, it calls out to an
     * implementation in the enclosing class.  For example, in order to perform
     * a connect() operation in the background, the enclosing class calls
     * BackgroundHandler.connect(); and the result is that the enclosing class
     * method doConnect() will be called in the context of the background
     * thread.
     */
    class BackgroundHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAllJoynService::BackgroundHandler")

        BackgroundHandler();

        CARAPI constructor(
            /* [in] */ CAllJoynService* service,
            /* [in] */ ILooper* looper);

        /**
         * Exit the background handler thread.  This will be the last message
         * executed by an instance of the handler.
         */
        void Exit();

        /**
         * Connect the application to the Alljoyn bus attachment.  We expect
         * this method to be called in the context of the main Service thread.
         * All this method does is to dispatch a corresponding method in the
         * context of the service worker thread.
         */
        void Connect();

        /**
         * Disonnect the application from the Alljoyn bus attachment.  We
         * expect this method to be called in the context of the main Service
         * thread.  All this method does is to dispatch a corresponding method
         * in the context of the service worker thread.
         */
        void Disconnect();

        /**
         * Start discovering remote instances of the application.  We expect
         * this method to be called in the context of the main Service thread.
         * All this method does is to dispatch a corresponding method in the
         * context of the service worker thread.
         */
        void StartDiscovery();

        /**
         * Stop discovering remote instances of the application.  We expect
         * this method to be called in the context of the main Service thread.
         * All this method does is to dispatch a corresponding method in the
         * context of the service worker thread.
         */
        void CancelDiscovery();

        void RequestName();

        void ReleaseName();

        void BindSession();

        void UnbindSession();

        void Advertise();

        void CancelAdvertise();

        void JoinSession();

        void LeaveSession();

        void SendMessages();

        /**
         * The message handler for the worker thread that handles background
         * tasks for the AllJoyn bus.
         */
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        void SendMessageInner(
            /* [in] */ Int32 id);

    private:
        friend class MyHandler;
        CAllJoynService* mHost;
    };

    /**
     * The ChatBusListener is a class that listens to the AllJoyn bus for
     * notifications corresponding to the existence of events happening out on
     * the bus.  We provide one implementation of our listener to the bus
     * attachment during the connect().
     */
    class ChatBusListener
        : public BusListener
    {
    public:
        ChatBusListener(
            /* [in] */ CAllJoynService* host);

        /**
         * This method is called when AllJoyn discovers a remote attachment
         * that is hosting an chat channel.  We expect that since we only
         * do a findAdvertisedName looking for instances of the chat
         * well-known name prefix we will only find names that we know to
         * be interesting.  When we find a remote application that is
         * hosting a channel, we add its channel name it to the list of
         * available channels selectable by the user.
         *
         * In the class documentation for the BusListener note that it is a
         * requirement for this method to be multithread safe.  This is
         * accomplished by the use of a monitor on the ChatApplication as
         * exemplified by the synchronized attribute of the addFoundChannel
         * method there.
         */
        CARAPI FoundAdvertisedName(
            /* [in] */ const String& name,
            /* [in] */ Int16 transport,
            /* [in] */ const String& namePrefix);

        /**
         * This method is called when AllJoyn decides that a remote bus
         * attachment that is hosting an chat channel is no longer available.
         * When we lose a remote application that is hosting a channel, we
         * remote its name from the list of available channels selectable
         * by the user.
         *
         * In the class documentation for the BusListener note that it is a
         * requirement for this method to be multithread safe.  This is
         * accomplished by the use of a monitor on the ChatApplication as
         * exemplified by the synchronized attribute of the removeFoundChannel
         * method there.
         */
        CARAPI LostAdvertisedName(
            /* [in] */ const String& name,
            /* [in] */ Int16 transport,
            /* [in] */ const String& namePrefix);
    private:
        CAllJoynService* mHost;
    };

    class InnerSessionListener : public SessionListener
    {
    public:
        InnerSessionListener(
            /* [in] */ CAllJoynService* host)
            : mHost(host)
        {}

        // @Override
        CARAPI SessionLost(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 reason);

    private:
        CAllJoynService* mHost;
    };

    class InnerSessionPortListener : public SessionPortListener
    {
    public:
        InnerSessionPortListener(
            /* [in] */ CAllJoynService* host)
            : mHost(host)
        {}

        CARAPI AcceptSessionJoiner(
            /* [in] */ Int16 sessionPort,
            /* [in] */ const String& joiner,
            /* [in] */ ISessionOpts* opts,
            /* [out] */ Boolean* accepted);

        CARAPI SessionJoined(
            /* [in] */ Int16 sessionPort,
            /* [in] */ Int32 id,
            /* [in] */ const String& joiner);

    private:
        CAllJoynService* mHost;
    };

public:
    TO_STRING_IMPL("CAllJoynService")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAllJoynService();

    virtual ~CAllJoynService();

    CARAPI constructor();

    /**
     * We don't use the bindery to communiate between any client and this
     * service so we return null.
     */
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    /**
     * Our onCreate() method is called by the Android appliation framework
     * when the service is first created.  We spin up a background thread
     * to handle any long-lived requests (pretty much all AllJoyn calls that
     * involve communication with remote processes) that need to be done and
     * insinuate ourselves into the list of observers of the model so we can
     * get event notifications.
     */
    CARAPI OnCreate();

    /**
     * Our onDestroy() is called by the Android appliation framework when it
     * decides that our Service is no longer needed.  We tell our background
     * thread to exit andremove ourselves from the list of observers of the
     * model.
     */
    CARAPI OnDestroy();

    /**
     * The method onStartCommand() is called by the Android application
     * framework when a client explicitly starts a Service by calling
     * startService().  We expect that the only place this is going to be done
     * is when the Android Application class for our application is created.
     * The Appliation class provides our model in the sense of the MVC
     * application we really are.
     *
     * We return START_STICKY to enable us to be explicity started and stopped
     * which means that our Service will essentially run "forever" (or until
     * Android decides that we should die for resource management issues) since
     * our Application class is left running as long as the process is left running.
     */
     CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    /**
     * This is the event handler for the Observable/Observed design pattern.
     * Whenever an interesting event happens in our appliation, the Model (the
     * source of the event) notifies registered observers, resulting in this
     * method being called since we registered as an Observer in onCreate().
     *
     * This method will be called in the context of the Model, which is, in
     * turn the context of an event source.  This will either be the single
     * Android application framework thread if the source is one of the
     * Activities of the application or the Service.  It could also be in the
     * context of the Service background thread.  Since the Android Application
     * framework is a fundamentally single threaded thing, we avoid multithread
     * issues and deadlocks by immediately getting this event into a separate
     * execution in the context of the Service message pump.
     *
     * We do this by taking the event from the calling component and queueing
     * it onto a "handler" in our Service and returning to the caller.  When
     * the calling componenet finishes what ever caused the event notification,
     * we expect the Android application framework to notice our pending
     * message and run our handler in the context of the single application
     * thread.
     *
     * In reality, both events are executed in the context of the single
     * Android thread.
     */
    CARAPI Update(
        /* [in] */ IObservable* o,
        /* [in] */ IInterface* arg);

    /**
     * The signal handler for messages received from the AllJoyn bus.
     *
     * Since the messages sent on a chat channel will be sent using a bus
     * signal, we need to provide a signal handler to receive those signals.
     * This is it.  Note that the name of the signal handler has the first
     * letter capitalized to conform with the DBus convention for signal
     * handler names.
     */
    CARAPI Chat(
        /* [in] */ const String& str);

private:

    /**
     * Since basically our whole reason for being is to spin up a thread to
     * handle long-lived remote operations, we provide thsi method to do so.
     */
    void StartBusThread();

    /**
     * When Android decides that our Service is no longer needed, we need to
     * tear down the thread that is servicing our long-lived remote operations.
     * This method does so.
     */
    void StopBusThread();

    /**
     * Implementation of the functionality related to connecting our app
     * to the AllJoyn bus.  We expect that this method will only be called in
     * the context of the AllJoyn bus handler thread; and while we are in the
     * DISCONNECTED state.
     */
    void DoConnect();

    /**
     * Implementation of the functionality related to disconnecting our app
     * from the AllJoyn bus.  We expect that this method will only be called
     * in the context of the AllJoyn bus handler thread.  We expect that this
     * method will only be called in the context of the AllJoyn bus handler
     * thread; and while we are in the CONNECTED state.
     */
    Boolean DoDisconnect();

    /**
     * Implementation of the functionality related to discovering remote apps
     * which are hosting chat channels.  We expect that this method will only
     * be called in the context of the AllJoyn bus handler thread; and while
     * we are in the CONNECTED state.  Since this is a core bit of functionalty
     * for the "use" side of the app, we always do this at startup.
     */
    void DoStartDiscovery();

    /**
     * Implementation of the functionality related to stopping discovery of
     * remote apps which are hosting chat channels.
     */
    void DoStopDiscovery();

    /**
     * Implementation of the functionality related to requesting a well-known
     * name from an AllJoyn bus attachment.
     */
    void DoRequestName();

    /**
     * Implementation of the functionality related to releasing a well-known
     * name from an AllJoyn bus attachment.
     */
    void DoReleaseName();

    /**
     * Implementation of the functionality related to binding a session port
     * to an AllJoyn bus attachment.
     */
    void DoBindSession();

    /**
     * Implementation of the functionality related to un-binding a session port
     * from an AllJoyn bus attachment.
     */
    void DoUnbindSession();

    /**
     * Implementation of the functionality related to advertising a service on
     * an AllJoyn bus attachment.
     */
    void DoAdvertise();

    /**
     * Implementation of the functionality related to canceling an advertisement
     * on an AllJoyn bus attachment.
     */
    void DoCancelAdvertise();

    /**
     * Implementation of the functionality related to joining an existing
     * local or remote session.
     */
    void DoJoinSession();

    /**
     * Implementation of the functionality related to joining an existing
     * remote session.
     */
    void DoLeaveSession();

    /**
     * Implementation of the functionality related to sending messages out over
     * an existing remote session.  Note that we always send all of the
     * messages on the outbound queue, so there may be instances where this
     * method is called and we find nothing to send depending on the races.
     */
    void DoSendMessages();

    void SendMessage(
        /* [in] */ Int32 id);

private:
    friend class BackgroundHandler;
    friend class InnerSessionPortListener;

    static const Int32 NOTIFICATION_ID = 0xdefaced;

    /**
     * Value for the HANDLE_APPLICATION_QUIT_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_APPLICATION_QUIT_EVENT = 0;

    /**
     * Value for the HANDLE_USE_JOIN_CHANNEL_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_USE_JOIN_CHANNEL_EVENT = 1;

    /**
     * Value for the HANDLE_USE_LEAVE_CHANNEL_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_USE_LEAVE_CHANNEL_EVENT = 2;

    /**
     * Value for the HANDLE_HOST_INIT_CHANNEL_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_HOST_INIT_CHANNEL_EVENT = 3;

    /**
     * Value for the HANDLE_HOST_START_CHANNEL_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_HOST_START_CHANNEL_EVENT = 4;

    /**
     * Value for the HANDLE_HOST_STOP_CHANNEL_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_HOST_STOP_CHANNEL_EVENT = 5;

    /**
     * Value for the HANDLE_OUTBOUND_CHANGED_EVENT case observer notification handler.
     */
    static const Int32 HANDLE_OUTBOUND_CHANGED_EVENT = 6;

    static const Int32 EXIT = 1;
    static const Int32 CONNECT = 2;
    static const Int32 DISCONNECT = 3;
    static const Int32 START_DISCOVERY = 4;
    static const Int32 CANCEL_DISCOVERY = 5;
    static const Int32 REQUEST_NAME = 6;
    static const Int32 RELEASE_NAME = 7;
    static const Int32 BIND_SESSION = 8;
    static const Int32 UNBIND_SESSION = 9;
    static const Int32 ADVERTISE = 10;
    static const Int32 CANCEL_ADVERTISE = 11;
    static const Int32 JOIN_SESSION = 12;
    static const Int32 LEAVE_SESSION = 13;
    static const Int32 SEND_MESSAGES = 14;

    /**
     * A reference to a descendent of the Android Application class that is
     * acting as the Model of our MVC-based application.
     */
    CChatApplication* mChatApplication;

    /**
     * This is the Android Service message handler.  It runs in the context of the
     * main Android Service thread, which is also shared with Activities since
     * Android is a fundamentally single-threaded system.
     *
     * The important thing for us is to note that this thread cannot be blocked for
     * a significant amount of time or we risk the dreaded "force close" message.
     * We can run relatively short-lived operations here, but we need to run our
     * distributed system calls in a background thread.
     *
     * This handler serves translates from UI-related events into AllJoyn events
     * and decides whether functions can be handled in the context of the
     * Android main thread or if they must be dispatched to a background thread
     * which can take as much time as needed to accomplish a task.
     */
    AutoPtr<MyHandler> mHandler;

    /**
     * The state of the AllJoyn bus attachment.
     */
    BusAttachmentState mBusAttachmentState;// = BusAttachmentState.DISCONNECTED;

    /**
     * The state of the AllJoyn components responsible for hosting an chat channel.
     */
    HostChannelState mHostChannelState;// = HostChannelState.IDLE;

    /**
     * The state of the AllJoyn components responsible for hosting an chat channel.
     */
    UseChannelState mUseChannelState;// = UseChannelState.IDLE;

    /**
     * The instance of the AllJoyn background thread handler.  It is created
     * when Android decides the Service is needed and is called from the
     * onCreate() method.  When Android decides our Service is no longer
     * needed, it will call onDestroy(), which spins down the thread.
     */
    AutoPtr<BackgroundHandler> mBackgroundHandler;

    /**
     * The bus attachment is the object that provides AllJoyn services to Java
     * clients.  Pretty much all communiation with AllJoyn is going to go through
     * this obejct.
     */
    AutoPtr<IBusAttachment> mBus;//  = new BusAttachment(ChatApplication.PACKAGE_NAME, BusAttachment.RemoteMessage.Receive);

    /**
     * The well-known name prefix which all bus attachments hosting a channel
     * will use.  The NAME_PREFIX and the channel name are composed to give
     * the well-known name a hosting bus attachment will request and
     * advertise.
     */
    static const String NAME_PREFIX;// = "org.alljoyn.bus.samples.chat";

    /**
     * The well-known session port used as the contact port for the chat service.
     */
    static const Int16 CONTACT_PORT;// = 27;

    /**
     * The object path used to identify the service "location" in the bus
     * attachment.
     */
    static const String OBJECT_PATH;// = "/chatService";

    /**
     * An instance of an AllJoyn bus listener that knows what to do with
     * foundAdvertisedName and lostAdvertisedName notifications.  Although
     * we often use the anonymous class idiom when talking to AllJoyn, the
     * bus listener works slightly differently and it is better to use an
     * explicitly declared class in this case.
     */
    AutoPtr<ChatBusListener> mBusListener;// = new ChatBusListener();

    /**
     * The session identifier of the "host" session that the application
     * provides for remote devices.  Set to -1 if not connected.
     */
    Int32 mHostSessionId;// = -1;

    /**
     * A flag indicating that the application has joined a chat channel that
     * it is hosting.  See the long comment in doJoinSession() for a
     * description of this rather non-intuitively complicated case.
     */
    Boolean mJoinedToSelf;// = false;

    /**
     * This is the interface over which the chat messages will be sent in
     * the case where the application is joined to a chat channel hosted
     * by the application.  See the long comment in doJoinSession() for a
     * description of this rather non-intuitively complicated case.
     */
    AutoPtr<IChatInterface> mHostChatInterface;// = null;

    /**
     * This is the interface over which the chat messages will be sent.
     */
    AutoPtr<IChatInterface> mChatInterface;// = null;

    /**
     * The session identifier of the "use" session that the application
     * uses to talk to remote instances.  Set to -1 if not connectecd.
     */
    Int32 mUseSessionId;// = -1;

    /**
     * The ChatService is the instance of an AllJoyn interface that is exported
     * on the bus and allows us to send signals implementing messages
     */
    AutoPtr<IChatInterface> mChatService;// = new ChatService();
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CALLJOYNSERVICE_H__
