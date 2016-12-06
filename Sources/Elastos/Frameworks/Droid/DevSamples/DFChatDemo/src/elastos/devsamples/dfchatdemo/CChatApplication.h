
#ifndef __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCAHTAPPLICATION_H__
#define __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCAHTAPPLICATION_H__

#include "_Elastos_DevSamples_DFChatDemo_CChatApplication.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/app/Application.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::Application;
using Elastos::Droid::Content::IComponentName;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

/**
 * The ChatAppliation class serves as the Model (in the sense of the common
 * user interface design pattern known as Model-View-Controller) for the chat
 * application.
 *
 * The ChatApplication inherits from the relatively little-known Android
 * application framework class Application.  From the Android developers
 * reference on class Application:
 *
 *   Base class for those who need to maintain global application state.
 *   You can provide your own implementation by specifying its name in your
 *   AndroidManifest.xml's <application> tag, which will cause that class to
 *   be instantiated for you when the process for your application/package is
 *   created.
 *
 * The important property of class Application is that its lifetime coincides
 * with the lifetime of the application, not its activities.  Since we have
 * persistent state in our connections to the outside world via our AllJoyn
 * objects, and that state cannot be serialized, saved and restored; we need
 * a persistent object to ensure that state is held if transient objects like
 * Activities are destroyed and recreated by the Android application framework
 * during its normal operation.
 *
 * This object holds the global state for our chat application, and starts the
 * Android Service that handles the background processing relating to our
 * AllJoyn connections.
 *
 * Additionally, this class provides the Model for an MVC framework.  It
 * provides a relatively abstract idea of what it is the application is doing.
 * For example, we provide methods oriented to conceptual actions (like our
 * user has typed a message) instead of methods oriented to the implementation
 * (like, create an AllJoyn bus object and register it).  This allows the
 * user interface to be relatively independent of the channel implementation.
 *
 * Android Activities can come and go in sometimes surprising ways during the
 * operation of an application.  For example, when a phone is rotated from
 * portrait to landscape orientation, the displayed Activities are deleted
 * and recreated in the new orientation.  This class holds the persistent
 * state that is required to correctly display Activities when they are
 * recreated.
 */
CarClass(CChatApplication)
    , public Application
{
public:
    CChatApplication();

    ~CChatApplication();

    CARAPI constructor();

    /**
     * When created, the application fires an intent to create the AllJoyn
     * service.  This acts as sort of a combined view/controller in the
     * overall architecture.
     */
    CARAPI OnCreate();

    /**
     * Since our application is "rooted" in this class derived from Appliation
     * and we have a long-running service, we can't just call finish in one of
     * the Activities.  We have to orchestrate it from here.  We send an event
     * notification out to all of our obsservers which tells them to exit.
     *
     * Note that as a result of the notification, all of the observers will
     * stop -- as they should.  One of the things that will stop is the AllJoyn
     * Service.  Notice that it is started in the onCreate() method of the
     * Application.  As noted in the Android documentation, the Application
     * class never gets torn down, nor does it provide a way to tear itself
     * down.  Thus, if the Chat application is ever run again, we need to have
     * a way of detecting the case where it is "re-run" and then "re-start"
     * the service.
     */
    CARAPI Quit();

    /**
     * Application components call this method to indicate that they are alive
     * and may have need of the AllJoyn Service.  This is required because the
     * Android Application class doesn't have an end to its lifecycle other
     * than through "kill -9".  See quit().
     */
    void Checkin();

    /**
     * This is the method that AllJoyn Service calls to tell us that an error
     * has happened.  We are provided a module, which corresponds to the high-
     * level "hunk" of code where the error happened, and a descriptive string
     * that we do not interpret.
     *
     * We expect the user interface code to sort out the best activity to tell
     * the user about the error (by calling getErrorModule) and then to call in
     * to get the string.
     */
    void AlljoynError(
        /* [in] */ Module m,
        /* [in] */ const String& s);

    /**
     * Return the high-level module that caught the last AllJoyn error.
     */
    Module GetErrorModule();

    /**
     * Return the error string stored when the last AllJoyn error happened.
     */
    String GetErrorString();

    /**
     * Called from the AllJoyn Service when it gets a FoundAdvertisedName.  We
     * know by construction that the advertised name will correspond to an chat
     * channel.  Note that the channel here is the complete well-known name of
     * the bus attachment advertising the channel.  In most other places it is
     * simply the channel name, which is the final segment of the well-known
     * name.
     */
    void AddFoundChannel(
        /* [in] */ const String& channel);

    /**
     * Called from the AllJoyn Service when it gets a LostAdvertisedName.  We
     * know by construction that the advertised name will correspond to an chat
     * channel.
     */
    void RemoveFoundChannel(
        /* [in] */ const String& channel);

    /**
     * Whenever the user is asked for a channel to join, it nees the list of
     * channels found via FoundAdvertisedName.  This method provides that
     * list.  Since we have no idea how or when the caller is going to access
     * or change the list, and we are deeply paranoid, we provide a deep copy.
     */
    AutoPtr< List<String> > GetFoundChannels();

    /**
     * Set the status of the "host" channel.  The AllJoyn Service part of the
     * appliciation is expected to make this call to set the status to reflect
     * the status of the underlying AllJoyn session.
     */
    void HostSetChannelState(
        /* [in] */ HostChannelState state);

    /**
     * Get the state of the "use" channel.
     */
    HostChannelState HostGetChannelState();

    /**
     * Set the name part of the "host" channel.  Since we are going to "use" a
     * channel that is implemented remotely and discovered through an AllJoyn
     * FoundAdvertisedName, this must come from a list of advertised names.
     * These names are our channels, and so we expect the GUI to choose from
     * among the list of channels it retrieves from getFoundChannels().
     *
     * Since we are talking about user-level interactions here, we are talking
     * about the final segment of a well-known name representing a channel at
     * this point.
     */
    void HostSetChannelName(
        /* [in] */ const String& name);

    /**
     * Get the name part of the "use" channel.
     */
    String HostGetChannelName();

    /**
     * Set the status of the "use" channel.  The AllJoyn Service part of the
     * appliciation is expected to make this call to set the status to reflect
     * the status of the underlying AllJoyn session.
     */
    void UseSetChannelState(
        /* [in] */ UseChannelState state);

    /**
     * Get the state of the "use" channel.
     */
    UseChannelState UseGetChannelState();

    /**
     * Set the name part of the "use" channel.  Since we are going to "use" a
     * channel that is implemented remotely and discovered through an AllJoyn
     * FoundAdvertisedName, this must come from a list of advertised names.
     * These names are our channels, and so we expect the GUI to choose from
     * among the list of channels it retrieves from getFoundChannels().
     *
     * Since we are talking about user-level interactions here, we are talking
     * about the final segment of a well-known name representing a channel at
     * this point.
     */
    void UseSetChannelName(
        /* [in] */ const String& name);

    /**
     * Get the name part of the "use" channel.
     */
    String UseGetChannelName();

    /**
     * This is the method that the "use" tab user interface calls when the
     * user indicates that she wants to join a channel.  The channel name
     * must have been previously set with a call to setUseChannelName().
     * The "use" channel is the channel that we talk about in the "Use" tab.
     * Since it's a remote channel in a remote bus attachment, we need to tell
     * the AllJoyn Service to go join the corresponding session.
     */
    void UseJoinChannel();

    /**
     * This is the method that the "use" tab user interface calls when the
     * user indicates that she wants to leave a channel.  Since we're
     * talking about a remote channel corresponding to a session with a
     * remote bus attachment, we needto tell the AllJoyn Service to leave
     * the corresponding session.
     */
    void UseLeaveChannel();

    /**
     * This is the method that the "host" tab user interface calls when the
     * user has completed providing her preferences for hosting a channel.
     */
    void HostInitChannel();

    /**
     * This is the method that the "host" tab user interface calls when the
     * user indicates that she wants to start hosting a channel.
     */
    void HostStartChannel();

    /**
     * This is the method that the "host" tab user interface calls when the
     * user indicates that she wants to stop hosting a channel.
     */
    void HostStopChannel();

    /**
     * Whenever our local user types a message, we need to send it out on the
     * channel, which we do by calling addOutboundItem.  This will eventually
     * result in an AllJoyn Bus Signal being sent to the other participants on
     * the channel.  Since the sessions that implement the channel don't "echo"
     * back to the source, we need to echo the message into our history.
     */
    void NewLocalUserMessage(
        /* [in] */ const String& message);

    /**
     * Whenever a user types a message into the channel, we expect the AllJoyn
     * Service local to that user to send the message to everyone participating
     * on the channel.  At each participant, the messages arrive in the AllJoyn
     * Service as a Bus Signal.  The Service handles the signals and passes the
     * associated messages on to us here.  We expect the nickname to be the
     * unique ID of the sending bus attachment.  This is not very user friendly,
     * but is convenient and guaranteed to be unique.
     */
    void NewRemoteUserMessage(
        /* [in] */ const String& nickname,
        /* [in] */ const String& message);

    /**
     * Whenever the local user types a message for distribution to the channel
     * it is queued to a list of outbound messages.  The AllJoyn Service is
     * notified and calls in here to get the outbound messages that need to be
     * sent.
     */
    String GetOutboundItem();

    /**
     * Whenever a new message is added to the history list, an update
     * notification is sent to all of the observers registered to this object
     * that indicates that the history list has changed.  When the observer
     * hears that the list has changed, it calls in here to get the new
     * contents.  Since we have no idea how or when the caller is going to
     * access or change the list, and we are deeply paranoid, we provide a
     * deep copy.
     */
    AutoPtr< List<String> > GetHistory();

    /**
     * This object is really the model of a model-view-controller architecture.
     * The observer/observed design pattern is used to notify view-controller
     * objects when the model has changed.  The observed object is this object,
     * the model.  Observers correspond to the view-controllers which in this
     * case are the Android Activities (corresponding to the use tab and the
     * hsot tab) and the Android Service that does all of the AllJoyn work.
     * When an observer wants to register for change notifications, it calls
     * here.
     */
    CARAPI AddObserver(
        /* [in] */ IObserver* obs);

    /**
     * When an observer wants to unregister to stop receiving change
     * notifications, it calls here.
     */
    CARAPI DeleteObserver(
        /* [in] */ IObserver* obs);

private:
    /**
     * Whenever the local user types a message for distribution to the channel
     * it calls newLocalMessage.  We are called to queue up the message and
     * send a notification to all of our observers indicating that the we have
     * something ready to go out.  We expect that the AllJoyn Service will
     * eventually respond by calling back in here to get items off of the queue
     * and send them down the session corresponding to the channel.
     */
    void AddOutboundItem(
        /* [in] */ const String& message);

    /**
     * Whenever a message comes in from the AllJoyn Service over its channel
     * session, it calls in here.  We just add the message item to the history
     * list, with the "nickname" provided by Service.  This is currently
     * expected to be the unique name of the bus attachment originating the
     * message.  Once the message is saved in the history, a change notification
     * will be sent to all observers indicating that the history has changed.
     * The user interface part of the application is then expected to wake up
     * and syncrhonize itself to the new history.
     */
     void AddInboundItem(
        /* [in] */ const String& nickname,
        /* [in] */ const String& message);

    /**
     * Whenever a user in the channel types a message, it needs to result in
     * the history being updated with the nickname of the user originating the
     * message and the message itself.  We keep a history list of a given
     * maximum size just for general principles.  This history list contains
     * the local time at which the message was recived, the nickname of the
     * user who originated the message and the message itself.  We send a
     * change notification to all observers indicating that the history has
     * changed when we modify it.
     */
     void AddHistoryItem(
        /* [in] */ const String& nickname,
        /* [in] */ const String& message);

    /**
     * Clear the history list.  Whenever a user joins a new channel, we want
     * to get rid of any existing history to avoid confusion.
     */
    void ClearHistory();

    /**
     * This object is really the model of a model-view-controller architecture.
     * The observer/observed design pattern is used to notify view-controller
     * objects when the model has changed.  The observed object is this object,
     * the model.  Observers correspond to the view-controllers which in this
     * case are the Android Activities (corresponding to the use tab and the
     * hsot tab) and the Android Service that does all of the AllJoyn work.
     * When the model (this object) wants to notify its observers that some
     * interesting event has happened, it calles here and provides an object
     * that identifies what has happened.  To keep things obvious, we pass a
     * descriptive string which is then sent to all observers.  They can decide
     * to act or not based on the content of the string.
     */
    void NotifyObservers(
        /* [in] */ IInterface* arg);

public:
    static String PACKAGE_NAME;

    static const String APPLICATION_QUIT_EVENT; // = "APPLICATION_QUIT_EVENT";

    /**
     * The object we use in notifications to indicate that an AllJoyn error has
     * happened.
     */
    static const String ALLJOYN_ERROR_EVENT; // = "ALLJOYN_ERROR_EVENT";

    /**
     * The object we use in notifications to indicate that the state of the
     * "host" channel or its name has changed.
     */
    static const String HOST_CHANNEL_STATE_CHANGED_EVENT; // = "HOST_CHANNEL_STATE_CHANGED_EVENT";

    /**
     * The object we use in notifications to indicate that the state of the
     * "use" channel or its name has changed.
     */
    static const String USE_CHANNEL_STATE_CHANGED_EVENT; // = "USE_CHANNEL_STATE_CHANGED_EVENT";

    /**
     * The object we use in notifications to indicate that user has requested
     * that we join a channel in the "use" tab.
     */
    static const String USE_JOIN_CHANNEL_EVENT; // = "USE_JOIN_CHANNEL_EVENT";

    /**
     * The object we use in notifications to indicate that user has requested
     * that we leave a channel in the "use" tab.
     */
    static const String USE_LEAVE_CHANNEL_EVENT; // = "USE_LEAVE_CHANNEL_EVENT";

    /**
     * The object we use in notifications to indicate that user has requested
     * that we initialize the host channel parameters in the "use" tab.
     */
    static const String HOST_INIT_CHANNEL_EVENT; // = "HOST_INIT_CHANNEL_EVENT";

    /**
     * The object we use in notifications to indicate that user has requested
     * that we initialize the host channel parameters in the "use" tab.
     */
    static const String HOST_START_CHANNEL_EVENT; // = "HOST_START_CHANNEL_EVENT";

    /**
     * The object we use in notifications to indicate that user has requested
     * that we initialize the host channel parameters in the "use" tab.
     */
    static const String HOST_STOP_CHANNEL_EVENT; // = "HOST_STOP_CHANNEL_EVENT";

    /**
     * The object we use in notifications to indicate that the the user has
     * entered a message and it is queued to be sent to the outside world.
     */
    static const String OUTBOUND_CHANGED_EVENT; // = "OUTBOUND_CHANGED_EVENT";

    /**
     * The object we use in notifications to indicate that the history state of
     * the model has changed and observers need to synchronize with it.
     */
    static const String HISTORY_CHANGED_EVENT; // = "HISTORY_CHANGED_EVENT";


private:
    /**
     * The high-level module that caught the last AllJoyn error.
     */
    Module mModule;// = Module_NONE;

    AutoPtr<IComponentName> mRunningService;

    /**
     * The string representing the last AllJoyn error that happened in the
     * AllJoyn Service.
     */
    String mErrorString;// = "ER_OK";

    /**
     * The channels list is the list of all well-known names that correspond to
     * channels we might conceivably be interested in.  We expect that the
     * "use" GUID will allow the local user to have this list displayed in a
     * "join channel" dialog, whereupon she will choose one.  This will
     * eventually result in a joinSession call out from the AllJoyn Service
     */
    List<String> mChannels;

    /**
     * The application has three ideas about the state of its channels.  This
     * is very detailed for a real application, but since this is an AllJoyn
     * sample, we think it is important to convey the detailed state back to
     * our user, whom we assume knows what it all means.
     *
     * We have a basic bus attachment state, which reflects the fact that we
     * can't do anything wihtout a bus attachment.  When the service comes up
     * it automatically connects and starts discovering other instances of the
     * application, so this isn't terribly interesting.
     */
    BusAttachmentState mBusAttachmentState;

    /**
     * The "host" state which reflects the state of the part of the system
     * related to hosting an chat channel.  In a "real" application this kind
     * of detail probably isn't appropriate, but we want to do so for this
     * sample.
     */
    HostChannelState mHostChannelState;// = HostChannelState_IDLE;

    /**
     * The name of the "host" channel which the user has selected.
     */
    String mHostChannelName;

    /**
     * The "use" state which reflects the state of the part of the system
     * related to using a remotely hosted chat channel.  In a "real" application
     * this kind of detail probably isn't appropriate, but we want to do so for
     * this sample.
     */
    UseChannelState mUseChannelState;// = UseChannelState_IDLE;;

    /**
     * The name of the "use" channel which the user has selected.
     */
    String mUseChannelName;

    static const Int32 OUTBOUND_MAX;

    /**
     * The outbound list is the list of all messages that have been originated
     * by our local user and are designed for the outside world.
     */
    List<String> mOutbound;

    /**
     * Don't keep an infinite amount of history.  Although we don't want to
     * admit it, this is a toy application, so we just keep a little history.
     */
    static const Int32 HISTORY_MAX;

    /**
     * The history list is the list of all messages that have been originated
     * or recieved by the "use" channel.
     */
    List<String> mHistory;

    /**
     * The observers list is the list of all objects that have registered with
     * us as observers in order to get notifications of interesting events.
     */
    List< AutoPtr<IObserver> > mObservers;
};

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFCHATDEMO_CCAHTAPPLICATION_H__
