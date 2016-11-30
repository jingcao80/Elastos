
#ifndef __ORG_ALLJOYN_BUS_CBUSATTACHMENT_H__
#define __ORG_ALLJOYN_BUS_CBUSATTACHMENT_H__

#include "_Org_Alljoyn_Bus_CBusAttachment.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "org/alljoyn/bus/ProxyBusObject.h"
#include <elastos/core/Object.h>

using Org::Alljoyn::Bus::Ifaces::IDBusProxyObj;
using Elastos::Core::Object;
using Elastos::Utility::IHashSet;
using Elastos::Utility::ISet;
using Elastos::Utility::Concurrent::IExecutorService;

namespace Org {
namespace Alljoyn {
namespace Bus {

typedef enum {
    BA_HSL,     // BusAttachment hosted session listener index
    BA_JSL,     // BusAttachment joined session listener index
    BA_SL,     // BusAttachment session listener index
    BA_LAST     // indicates the size of the enum
} BusAttachmentSessionListenerIndex;

CarClass(CBusAttachment)
    , public Object
    , public IBusAttachment
{
private:
    class AuthListenerInternal
        : public Object
        , public IAuthListenerInternal
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI SetAuthListener(
            /* [in] */ IAuthListener* authListener);

        CARAPI AuthListenerSet(
            /* [out] */ Boolean* result);

        CARAPI SetSecurityViolationListener(
            /* [in] */ ISecurityViolationListener* violationListener);

        CARAPI RequestCredentials(
            /* [in] */ const String& authMechanism,
            /* [in] */ const String& authPeer,
            /* [in] */ Int32 authCount,
            /* [in] */ const String& userName,
            /* [in] */ Int32 credMask,
            /* [out] */ ICredentials** credentials);

        CARAPI VerifyCredentials(
            /* [in] */ const String& authMechanism,
            /* [in] */ const String& peerName,
            /* [in] */ const String& userName,
            /* [in] */ const String& cert,
            /* [out] */ Boolean* result);

        CARAPI SecurityViolation(
            /* [in] */ ECode status);

        CARAPI AuthenticationComplete(
            /* [in] */ const String& authMechanism,
            /* [in] */ const String& peerName,
            /* [in] */ Boolean success);

    private:
        static const Int32 PASSWORD       = 0x0001;
        static const Int32 USER_NAME      = 0x0002;
        static const Int32 CERT_CHAIN     = 0x0004;
        static const Int32 PRIVATE_KEY    = 0x0008;
        static const Int32 LOGON_ENTRY    = 0x0010;
        static const Int32 EXPIRATION     = 0x0020;
        static const Int32 NEW_PASSWORD   = 0x1001;
        static const Int32 ONE_TIME_PWD   = 0x2001;

        AutoPtr<IAuthListener> mAuthListener;
        AutoPtr<ISecurityViolationListener> mViolationListener;
    };

public:
    CBusAttachment();

    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    /**
     * Constructs a BusAttachment.
     *
     * @param applicationName the name of the application
     * @param policy if this attachment is allowed to receive messages from
     *               remote devices
     * @param concurrency The maximum number of concurrent method and signal
     *                    handlers locally executing.
     */
    CARAPI constructor(
        /* [in] */ const String& applicationName,
        /* [in] */ RemoteMessage policy,
        /* [in] */ Int32 concurrency);

    /**
     * Constructs a BusAttachment.
     *
     * @param applicationName the name of the application
     * @param policy if this attachment is allowed to receive messages
     *               from remote devices
     */
    CARAPI constructor(
        /* [in] */ const String& applicationName,
        /* [in] */ RemoteMessage policy);

    /**
     * Construct a BusAttachment that will only communicate on the local device.
     *
     * @param applicationName the name of the application
     */
    CARAPI constructor(
        /* [in] */ const String& applicationName);

    /**
     * @deprecated
     * Emit PropertiesChanged to signal the bus that this property has been updated
     *
     * @param busObject The BusObject that is the source of this signal
     * @param ifcName   The name of the interface
     * @param propName  The name of the property being changed
     * @param val       The new value of the property
     * @param sessionId Id of the session we broadcast to (0 for all)
     */
    // @Deprecated
    CARAPI EmitChangedSignal(
        /* [in] */ IBusObject* busObject,
        /* [in] */ const String& ifcName,
        /* [in] */ const String& propName,
        /* [in] */ IInterface* val,
        /* [in] */ Int32 sessionId);

    /**
     * Request a well-known name.
     * This method is a shortcut/helper that issues an org.freedesktop.DBus.RequestName method call to the local router
     * and interprets the response.
     *
     * @param name    Well-known name being requested.
     * @param flags   Bitmask name flag (see DBusStd.h)
     *                  <ul>
     *                  <li>ALLJOYN_NAME_FLAG_ALLOW_REPLACEMENT</li>
     *                  <li>ALLJOYN_REQUESTNAME_FLAG_REPLACE_EXISTING</li>
     *                  <li>ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE</li>
     *                  </ul>
     *
     * @return
     * <ul>
     * <li>OK if request completed successfully and primary ownership was granted.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI RequestName(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags);

    /**
     * Release a previously requeted well-known name.
     * This method is a shortcut/helper that issues an org.freedesktop.DBus.ReleaseName method call to the local router
     * and interprets the response.
     *
     * @param name  Well-known name being released.
     *
     * @return
     * <ul>
     * <li>OK if the name was released.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI ReleaseName(
        /* [in] */ const String& name);

    /**
     * Add a DBus match rule.
     * This method is a shortcut/helper that issues an org.freedesktop.DBus.AddMatch method call to the local router.
     *
     * @param rule  Match rule to be added (see the DBus specification for the
     *              format of this string).
     *
     * @return
     * <ul>
     * <li>OK if the match rule was added.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure</li>
     * </ul>
     */
    CARAPI AddMatch(
        /* [in] */ const String& rule);

    /**
     * Remove a DBus match rule.
     * This method is a shortcut/helper that issues an org.freedesktop.DBus.RemoveMatch method call to the local router.
     *
     * @param rule  Match rule to be removed (see the DBus specification for the
     *              format of this string).
     *
     * @return
     * <ul>
     * <li>OK if the match rule was removed.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure</li>
     * </ul>
     */
    CARAPI RemoveMatch(
        /* [in] */ const String& rule);

    /**
     * Advertise the existence of a well-known name to other (possibly disconnected) AllJoyn routers.
     *
     * This method is a shortcut/helper that issues an org.allJoyn.Bus.AdvertisedName method call to the local router
     * and interprets the response.
     *
     * @param name        The well-known name to advertise. (Must be owned by the caller via RequestName).
     * @param transports  Set of transports to use for sending advertisment.
     *
     * @return
     * <ul>
     * <li>OK if the name was advertised.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure </li>
     * </ul>
     */
    CARAPI AdvertiseName(
        /* [in] */ const String& name,
        /* [in] */ Int16 transports);

    /**
     * Stop advertising the existence of a well-known name to other AllJoyn routers.
     *
     * This method is a shortcut/helper that issues an
     * org.allJoyn.Bus.CancelAdvertiseName method call to the local
     * router and interprets the response.
     *
     * @param name        A well-known name that was previously advertised via AdvertiseName.
     * @param transports  Set of transports whose name advertisment will be cancelled.
     *
     * @return
     * <ul>
     * <li>OK if the name advertisements were stopped.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>BUS_MATCH_RULE_NOT_FOUND if interfaces added using the WhoImplements method were not found.</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI CancelAdvertiseName(
        /* [in] */ const String& name,
        /* [in] */ Int16 transports);

    /**
     * Register interest in a well-known name prefix for the purpose of discovery over transports included in TRANSPORT_ANY.
     * This method is a shortcut/helper that issues an org.allJoyn.Bus.FindAdvertisedName method call to the local router
     * and interprets the response.
     *
     * @param namePrefix  Well-known name prefix that application is interested in receiving BusListener::FoundAdvertisedName
     *                    notifications about.
     *
     * @return
     * <ul>
     * <li>OK if discovery was started.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI FindAdvertisedName(
        /* [in] */ const String& namePrefix);

    /**
     * Register interest in a well-known name prefix for the purpose of discovery over specified transports.
     * This method is a shortcut/helper that issues an org.allJoyn.Bus.FindAdvertisedName method call to the local router
     * and interprets the response.
     *
     * @param namePrefix  Well-known name prefix that application is interested in receiving BusListener::FoundAdvertisedName
     *                    notifications about.
     * @param transports  Set of transports over which discovery will be enabled.
     *
     * @return
     * <ul>
     * <li>OK if discovery was started.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI FindAdvertisedNameByTransport(
        /* [in] */ const String& namePrefix,
        /* [in] */ Int16 transports);

    /**
     * Cancel interest in a well-known name prefix that was previously
     * registered with FindAdvertisedName. This cancels well-known name discovery
     * over transports included in TRANSPORT_ANY.  This method is a shortcut/helper
     * that issues an org.allJoyn.Bus.CancelFindAdvertisedName method
     * call to the local router and interprets the response.
     *
     * @param namePrefix  Well-known name prefix that application is no longer interested in receiving
     *                    BusListener::FoundAdvertisedName notifications about.
     *
     * @return
     * <ul>
     * <li>OK if discovery was cancelled.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a general failure condition.</li>
     * </ul>
     */
    CARAPI CancelFindAdvertisedName(
        /* [in] */ const String& namePrefix);

    /**
     * Cancel interest in a well-known name prefix that was previously
     * registered with FindAdvertisedName. This cancels well-known name discovery
     * over the specified transports.  This method is a shortcut/helper
     * that issues an org.allJoyn.Bus.CancelFindAdvertisedName method
     * call to the local router and interprets the response.
     *
     * @param namePrefix  Well-known name prefix that application is no longer interested in receiving
     *                    BusListener::FoundAdvertisedName notifications about.
     * @param transports  Set of transports over which discovery will be cancelled.
     *
     * @return
     * <ul>
     * <li>OK if discovery was cancelled.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a general failure condition.</li>
     * </ul>
     */
    CARAPI CancelFindAdvertisedNameByTransport(
        /* [in] */ const String& namePrefix,
        /* [in] */ Int16 transports);

    /**
     * Make a SessionPort available for external BusAttachments to join.
     *
     * Each BusAttachment binds its own set of SessionPorts. Session joiners use
     * the bound session port along with the name of the attachement to create a
     * persistent logical connection (called a Session) with the original
     * BusAttachment.
     *
     * A SessionPort and bus name form a unique identifier that BusAttachments
     * use when joining a session.  SessionPort values can be pre-arranged
     * between AllJoyn services and their clients (well-known SessionPorts).
     *
     * Once a session is joined using one of the service's well-known
     * SessionPorts, the service may bind additional SessionPorts (dyanamically)
     * and share these SessionPorts with the joiner over the original
     * session. The joiner can then create additional sessions with the service
     * by calling JoinSession with these dynamic SessionPort ids.
     *
     * @param sessionPort SessionPort value to bind or SESSION_PORT_ANY to allow
     *                    this method to choose an available port. On successful
     *                    return, this value contains the chosen SessionPort.
     *
     * @param opts        Session options that joiners must agree to in order to
     *                    successfully join the session.
     *
     * @param listener    SessionPortListener that will be notified via callback
     *                    when a join attempt is made on the bound session port.
     *
     * @return
     * <ul>
     * <li>OK if the new session port was bound.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI BindSessionPort(
        /* [in] */ IMutableInteger16Value* sessionPort,
        /* [in] */ ISessionOpts* opts,
        /* [in] */ ISessionPortListener* listener);

    /**
     * Set a Translator for all BusObjects and InterfaceDescriptions. This Translator is used for
     * descriptions appearing in introspection. Note that any Translators set on a specific
     * InterfaceDescription or BusObject will be used for those specific elements - this Translator
     * is used only for BusObjects and InterfaceDescriptions that do not have Translators set for them.
     *
     * @param translator       The Translator instance
     */
    CARAPI SetDescriptionTranslator(
        /* [in] */ ITranslator* translator);

    /**
     * Cancel an existing port binding.
     *
     * @param   sessionPort    Existing session port to be un-bound.
     *
     * @return
     * <ul>
     * <li>OK if the session port was unbound.</li>
     * <li>BUS_NOT_CONNECTED if connection has not been made with the local router.</li>
     * <li>other error status codes indicating a failure</li>
     * </ul>
     */
    CARAPI UnbindSessionPort(
        /* [in] */ Int16 sessionPort);

    /**
     * Join a session.
     *
     * This method is a shortcut/helper that issues an
     * org.allJoyn.Bus.JoinSession method call to the local router and
     * interprets the response.
     *
     * @param sessionHost   Bus name of attachment that is hosting the session to be joined.
     * @param sessionPort   SessionPort of sessionHost to be joined.
     * @param sessionId     Set to the unique identifier for session.
     * @param opts          Set to the actual session options of the joined session.
     * @param listener      Listener to be called when session related asynchronous
     *                      events occur.
     *
     * @return
     * <ul>
     * <li>OK if the session was joined.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI JoinSession(
        /* [in] */ const String& sessionHost,
        /* [in] */ Int16 sessionPort,
        /* [in] */ IMutableInteger32Value* sessionId,
        /* [in] */ ISessionOpts* opts,
        /* [in] */ ISessionListener* listener);

    /**
     * Asynchronous version of {@link #joinSession(String, short, Mutable.IntegerValue, SessionOpts,
     * SessionListener)}.
     *
     * @param sessionHost   Bus name of attachment that is hosting the session to be joined.
     * @param sessionPort   SessionPort of sessionHost to be joined.
     * @param opts          The requested session options of the session to be joined.
     * @param listener      Listener to be called when session related asynchronous
     *                      events occur.
     * @param onJoinSession Listener to be called when joinSession completes.
     * @param context       User-defined context object.  Passed through to {@link
     *                      OnJoinSessionListener#onJoinSession(Status, int, SessionOpts, Object)}.
     *
     * @return
     * <ul>
     * <li>OK if method call to local router response was was successful.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus.</li>
     * <li>Other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI JoinSession(
        /* [in] */ const String& sessionHost,
        /* [in] */ Int16 sessionPort,
        /* [in] */ ISessionOpts* opts,
        /* [in] */ ISessionListener* listener,
        /* [in] */ IOnJoinSessionListener* onJoinSession,
        /* [in] */ IInterface* context);

    /**
     * Leave an existing session.
     * This method is a shortcut/helper that issues an org.alljoyn.Bus.LeaveSession method call to the local router
     * and interprets the response.
     * This method cannot be called on self-joined session.
     *
     * @param sessionId     Session id.
     *
     * @return
     * <ul>
     * <li>OK if router response was left.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>ER_BUS_NO_SESSION if session did not exist.</li>
     * <li>other error status codes indicating failures.</li>
     * </ul>
     */
    CARAPI LeaveSession(
        /* [in] */ Int32 sessionId);

    /**
     * Leave an existing session as host. This function will fail if you were not the host. This method is a
     * shortcut/helper that issues an org.alljoyn.Bus.LeaveHostedSession method call to the local router and interprets
     * the response.
     *
     * @param  sessionId     Session id.
     *
     * @return
     * <ul>
     * <li>ER_OK if router response was received and the leave operation was successfully completed.</li>
     * <li>ER_BUS_NOT_CONNECTED if a connection has not been made with a local bus.</li>
     * <li>ER_BUS_NO_SESSION if session did not exist or if not host of the session.</li>
     * <li>Other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI LeaveHostedSession(
        /* [in] */ Int32 sessionId);

    /**
     * Leave an existing session as joiner. This function will fail if you were not the joiner.
     * This method is a shortcut/helper that issues an org.alljoyn.Bus.LeaveJoinedSession method call to the local router
     * and interprets the response.
     *
     * @param sessionId Session id.
     *
     * @return
     * <ul>
     * <li>ER_OK if router response was received and the leave operation was successfully completed.</li>
     * <li>ER_BUS_NOT_CONNECTED if a connection has not been made with a local bus.</li>
     * <li>ER_BUS_NO_SESSION if session did not exist or if not joiner of the session.</li>
     * <li>Other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI LeaveJoinedSession(
        /* [in] */ Int32 sessionId);

    /**
     * Remove a session member from an existing multipoint session.
     *
     * This method is a shortcut/helper that issues an
     * org.alljoyn.Bus.RemoveSessionMember method call to the local router
     * and interprets the response.
     *
     * @param sessionId             Session id.
     * @param sessionMemberName     Name of member to remove from session.
     *
     * @return
     * <ul>
     * <li>OK if router response was left.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating failures.</li>
     * </ul>
     */
    CARAPI RemoveSessionMember(
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& sessionMemberName);

    /**
     * Set the SessionListener for an existing session on both host and joiner side.
     *
     * Calling this method will override (replace) the listener set by a previoius call to
     * setSessionListener, SetHostedSessionListener, SetJoinedSessionListener or a listener specified in joinSession.
     *
     * @param sessionId    The session id of an existing session.
     * @param listener     The SessionListener to associate with the session. May be null to clear previous listener.
     * @return  ER_OK if successful.
     * @return  ER_BUS_NO_SESSION if session did not exist
     */
    CARAPI SetSessionListener(
        /* [in] */ Int32 sessionId,
        /* [in] */ ISessionListener* listener);

    /**
     * Set the SessionListener for an existing sessionId on the joiner side.
     *
     * Calling this method will override the listener set by a previous call to SetSessionListener, SetJoinedSessionListener
     * or any listener specified in JoinSession.
     *
     * @param sessionId    The session id of an existing session.
     * @param listener     The SessionListener to associate with the session. May be NULL to clear previous listener.
     * @return  ER_OK if successful.
     * @return  ER_BUS_NO_SESSION if session did not exist or if not joiner side of the session
     */
    CARAPI SetJoinedSessionListener(
        /* [in] */ Int32 sessionId,
        /* [in] */ ISessionListener* listener);

    /**
     * Set the SessionListener for an existing sessionId on the host side.
     *
     * Calling this method will override the listener set by a previous call to SetSessionListener or SetHostedSessionListener.
     *
     * @param sessionId    The session id of an existing session.
     * @param listener     The SessionListener to associate with the session. May be NULL to clear previous listener.
     * @return  ER_OK if successful.
     * @return  ER_BUS_NO_SESSION if session did not exist or if not host side of the session
     */
    CARAPI SetHostedSessionListener(
        /* [in] */ Int32 sessionId,
        /* [in] */ ISessionListener* listener);

    /**
     * Get the file descriptor for a raw (non-message based) session.
     *
     * @param sessionId  Id of an existing streamming session.
     * @param sockFd     Socket file descriptor for session.
     *
     * @return
     * <ul>
     * <li>Status.OK if the socket FD was returned.</li>
     * <li>ER_BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * <li>other error status codes indicating a failure.</li>
     * </ul>
     *
     */
    CARAPI GetSessionFd(
        /* [in] */ Int32 sessionId,
        /* [in] */ IMutableInteger32Value* sockFd);

    /**
     * Set the link timeout for a session.
     *
     * Link timeout is the maximum number of seconds that an unresponsive
     * router-to-router connection will be monitored before delcaring the
     * session lost (via SessionLost callback). Link timeout defaults to 0 which
     * indicates that AllJoyn link monitoring is disabled.
     *
     * Each transport type defines a lower bound on link timeout to avoid
     * defeating transport specific power management algorithms.
     *
     * @param sessionId   Id of session whose link timeout will be modified.
     * @param linkTimeout [IN/OUT] Max number of seconds that a link can be
     *                    unresponsive before being delcared lost. 0 indicates
     *                    that AllJoyn link monitoring will be disabled. On
     *                    return, this value will be the resulting (possibly
     *                    upward) adjusted linkTimeout value that acceptible
     *                    to the underlying transport.
     * @return
     * <ul>
     * <li>Status.OK if the linkTimeout was successfully modified</li>
     * <li>ER_BUS_NOT_CONNECTED if a connection has not been made with a local bus</li>
     * </ul>
     */
    CARAPI SetLinkTimeout(
        /* [in] */ Int32 sessionId,
        /* [in] */ IMutableInteger32Value* linkTimeout);

    /**
     * Get the peer GUID for this peer or an authenticated remote peer. Peer
     * GUIDs are used by the authentication mechanisms to uniquely and identify
     * a remote application instance. The peer GUID for a remote peer is only
     * available if the remote peer has been authenticated.
     *
     * @param name  Name of a remote peer or NULL to get the local (our) peer
     *              GUID.
     * @param guid  Mutable value that contains a reference to the returned
     *              GUID string (think C++ [out] parameter.
     *
     * @return
     * <ul>
     * <li>OK if the requested GUID was obtained</li>
     * <li>other error status codes indicating a failure</li>
     * </ul>
     */
    CARAPI GetPeerGUID(
        /* [in] */ const String& name,
        /* [in] */ IMutableStringValue* guid);

    /**
     * Determine if a name is present and responding by pinging it. The name can
     * be the unique or well-known name.
     *
     * @param name     The unique or well-known name to ping
     * @param timeout  Timeout specified in milliseconds to wait for reply
     *
     * @return
     * <ul>
     * <li>OK the name is present and responding</li>
     * <li>ALLJOYN_PING_REPLY_UNREACHABLE the name is no longer present</li>
     * </ul>
     * The following return values indicate that the router cannot determine if the
     * remote name is present and responding:
     * <ul>
     * <li>ALLJOYN_PING_REPLY_TIMEOUT Ping call timed out</li>
     * <li>ALLJOYN_PING_REPLY_UNKNOWN_NAME name not found currently or not part of any known session</li>
     * <li>ALLJOYN_PING_REPLY_INCOMPATIBLE_REMOTE_ROUTING_NODE the remote routing node does not implement Ping</li>
     * </ul>
     * The following return values indicate an error with the ping call itself:
     * <ul>
     * <li>ALLJOYN_PING_FAILED Ping failed</li>
     * <li>BUS_UNEXPECTED_DISPOSITION An unexpected disposition was returned and has been treated as an error</li>
     * <li>BUS_NOT_CONNECTED the BusAttachment is not connected to the bus</li>
     * <li>BUS_BAD_BUS_NAME the name parameter is not a valid bus name</li>
     * <li>An error status otherwise</li>
     * </ul>
     */
    CARAPI Ping(
        /* [in] */ const String& name,
        /* [in] */ Int32 timeout);

    /**
     * The JNI loader can't resolve the overloaded ping if both the sync and async versions
     * are native.  This is the workaround.
     */
    CARAPI PingAsync(
        /* [in] */ const String& name,
        /* [in] */ Int32 timeout,
        /* [in] */ IOnPingListener* onPing,
        /* [in] */ IInterface* context);

    /**
     * Asynchronous version of {@link #ping(String, int)}.
     *
     * @param name     The unique or well-known name to ping
     * @param timeout  Timeout specified in milliseconds to wait for reply
     * @param onPing   Listener to be called when ping completes.
     * @param context  User-defined context object.  Passed through to {@link
     *                 OnPingListener#onPing(Status, Object)}.
     *
     * @return
     * <ul>
     * <li>OK iff method call to local router response was was successful.</li>
     * <li>BUS_NOT_CONNECTED if a connection has not been made with a local bus.</li>
     * <li>BUS_BAD_BUS_NAME if the name passed in is an invalid bus name.</li>
     * <li>Other error status codes indicating a failure.</li>
     * </ul>
     */
    CARAPI Ping(
        /* [in] */ const String& name,
        /* [in] */ Int32 timeout,
        /* [in] */ IOnPingListener* onPing,
        /* [in] */ IInterface* context);

    /**
     * This sets the debug level of the local AllJoyn router if that router
     * was built in debug mode.
     *
     * The debug level can be set for individual subsystems or for "ALL"
     * subsystems.  Common subsystems are "ALLJOYN" for core AllJoyn code,
     * "ALLJOYN_OBJ" for the sessions management code and "ALLJOYN_NS" for the
     * TCP name services.  Debug levels for specific subsystems override the
     * setting for "ALL" subsystems.  For example if "ALL" is set to 7, but
     * "ALLJOYN_OBJ" is set to 1, then detailed debug output will be generated
     * for all subsystems expcept for "ALLJOYN_OBJ" which will only generate
     * high level debug output.  "ALL" defaults to 0 which is off, or no debug
     * output.
     *
     * The debug output levels are actually a bit field that controls what
     * output is generated.  Those bit fields are described below:
     *<ul>
     *     <li>0x1: High level debug prints (these debug prints are not common)</li>
     *     <li>0x2: Normal debug prints (these debug prints are common)</li>
     *     <li>0x4: Function call tracing (these debug prints are used
     *            sporadically)</li>
     *     <li>0x8: Data dump (really only used in the "SOCKET" module - can
     *            generate a <strong>lot</strong> of output)</li>
     * </ul>
     *
     * Typically, when enabling debug for a subsystem, the level would be set
     * to 7 which enables High level debug, normal debug, and function call
     * tracing.  Setting the level 0, forces debug output to be off for the
     * specified subsystem.
     *
     * @param module  the name of the module to generate debug output from.
     * @param level   the debug level to set for the module.
     *
     * @return
     * <ul>
     * <li>OK if debug request was successfully sent to the AllJoyn router</li>
     * <li>BUS_NO_SUCH_OBJECT if router was not built in debug mode.</li>
     * </ul>
     */
    CARAPI SetDaemonDebug(
        /* [in] */ const String& moduleName,
        /* [in] */ Int32 level);

    /**
     * Set AllJoyn logging levels.
     *
     * @param logEnv    A semicolon separated list of KEY=VALUE entries used
     *                  to set the log levels for internal AllJoyn modules.
     *                  (i.e. ALLJOYN=7;ALL=1)
     */
    CARAPI SetLogLevels(
        /* [in] */ const String& logEnv);

    /**
     * Set AllJoyn debug levels.
     *
     * @param module    name of the module to generate debug output
     * @param level     debug level to set for the module
     */
    CARAPI SetDebugLevel(
        /* [in] */ const String& moduleName,
        /* [in] */ Int32 level);

    /**
     * Indicate whether AllJoyn logging goes to OS logger or stdout
     *
     * @param  useOSLog   true iff OS specific logging should be used rather than print for AllJoyn debug messages.
     */
    CARAPI UseOSLogging(
        /* [in] */ Boolean useOSLog);

    /**
     * Register an object that will receive About Interface event notifications.
     *
     * @param listener  Object instance that will receive bus event notifications.
     */
    CARAPI RegisterAboutListener(
        /* [in] */ IAboutListener* listener);

    /**
     * unregister an object that was previously registered with registerAboutListener.
     *
     * @param listener  Object instance to un-register as a listener.
     */
    CARAPI UnregisterAboutListener(
        /* [in] */ IAboutListener* listener);

    /**
     * Signal handler used to process announce signals from the bus. It will
     * forward the signal to the registered AboutListener
     *
     * @param version - version of the announce signal received
     * @param port - Session Port used by the remote device
     * @param objectDescriptions - list of object paths any interfaces found at
     *                             that object path
     * @param aboutData - A dictionary containing information about the remote
     *                    device.
     */
    // @BusSignalHandler(iface = "org.alljoyn.About", signal = "Announce")
    // public void announce(short version, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> aboutData);

    /**
     * Change the announce flag for an already added interface. Changes in the
     * announce flag are not visible to other devices till Announce is called.
     *
     * @param busObject   The BusObject that the interface is registered with
     * @param intf        The interface to change the announce flag on.
     * @param isAnnounced If "true" the interface will be part of the next
     *                    Announce signal if "false" the interface will not be
     *                    part of the next Announce signal.
     *
     * @return
     *  <ul>
     *  <li>OK if successful</li>
     *  <li>BUS_OBJECT_NO_SUCH_INTERFACE if the interface is not part of the BusObject.</li>
     *  </ul>
     */
    // public Status setAnnounceFlag(BusObject busObject, Class<?> intf, boolean isAnnounced);

    /**
     * Change the announce flag for an already added interface. Changes in the
     * announce flag are not visible to other devices till Announce is called.
     *
     * @param busObject   The BusObject that the interface is registered with
     * @param ifcName     The name of the interface
     * @param isAnnounced if "true" the interface will be part of the next
     *                    Announce signal if "false" the interface will not be
     *                    part of the next Announce signal.
     *
     * @return
     *  <ul>
     *  <li>OK if successful</li>
     *  <li>BUS_OBJECT_NO_SUCH_INTERFACE if the interface is not part of the BusObject.</li>
     *  </ul>
     */
    CARAPI SetAnnounceFlag(
        /* [in] */ IBusObject* busObject,
        /* [in] */ const String& ifcName,
        /* [in] */ Boolean isAnnounced);

    /**
     * TODO cleanup the documentation make sure it is accurate remove doxygen
     * style code blocks.
     *
     * List the interfaces your application is interested in.  If a remote device
     * is announcing that interface then the all Registered AboutListeners will
     * be called.
     *
     * For example, if you need both "com.example.Audio" <em>and</em>
     * "com.example.Video" interfaces then do the following.
     * registerAboutListener once:
     * <pre>{@code
     * String interfaces[] = {"com.example.Audio", "com.example.Video"};
     * registerAboutListener(aboutListener);
     * whoImplements(interfaces);
     * }</pre>
     *
     * If the handler should be called if "com.example.Audio" <em>or</em>
     * "com.example.Video" interfaces are implemented then call
     * RegisterAboutListener multiple times:
     * <pre>{@code
     * registerAboutListener(aboutListener);
     * String audioInterface[] = {"com.example.Audio"};
     * whoImplements(interfaces);
     * whoImplements(audioInterface);
     * String videoInterface[] = {"com.example.Video"};
     * whoImplements(videoInterface);
     * }</pre>
     *
     * The interface name may be a prefix followed by a *.  Using
     * this, the example where we are interested in "com.example.Audio" <em>or</em>
     * "com.example.Video" interfaces could be written as:
     * <pre>{@code
     * String exampleInterface[] = {"com.example.*"};
     * registerAboutListener(aboutListener);
     * whoImplements(exampleInterface);
     * }</pre>
     *
     * The AboutListener will receive any announcement that implements an interface
     * beginning with the "com.example." name.
     *
     * If the same AboutListener is used for for multiple interfaces then it is
     * the listeners responsibility to parse through the reported interfaces to
     * figure out what should be done in response to the Announce signal.
     *
     * Note: specifying null for the interfaces parameter could have significant
     * impact on network performance and should be avoided unless its known that
     * all announcements are needed.
     *
     * @param interfaces a list of interfaces that the Announce
     *               signal reports as implemented. NULL to receive all Announce
     *               signals regardless of interfaces
     * @return Status.OK on success. An error status otherwise.
     */
    CARAPI WhoImplements(
        /* [in] */ ArrayOf<String>* interfaces);

    /**
     * Stop showing interest in the listed interfaces. Stop receiving announce
     * signals from the devices with the listed interfaces.
     *
     * Note if WhoImplements has been called multiple times the announce signal
     * will still be received for any interfaces that still remain.
     *
     * @param interfaces a list of interfaces list must match a list previously
     *                   passed to the whoImplements method.
     *
     * @return Status.OK on success. An error status otherwise.
     */
    CARAPI CancelWhoImplements(
        /* [in] */ ArrayOf<String>* interfaces);

    /**
     * Register an object that will receive bus event notifications.
     *
     * @param listener  Object instance that will receive bus event notifications.
     */
    CARAPI RegisterBusListener(
        /* [in] */ IBusListener* listener);

    /**
     * unregister an object that was previously registered with registerBusListener.
     *
     * @param listener  Object instance to un-register as a listener.
     */
    CARAPI UnregisterBusListener(
        /* [in] */ IBusListener* listener);

    /**
     * Release resources immediately.
     *
     * Normally, when all references are removed to a given object, the Java
     * garbage collector notices the fact that the object is no longer required
     * and will destroy it.  This can happen at the garbage collector's leisure
     * an so any resources held by the object will not be released until "some
     * time later" after the object is no longer needed.
     *
     * Often, in test programs, we cycle through many BusAttachments in a very
     * short time, and if we rely on the garbage collector to clean up, we can
     * fairly quickly run out of scarce underlying resources -- especially file
     * descriptors.
     *
     * We provide an explicity release() method to allow test programs to release
     * the underlying resources immediately.  The garbage collector will still
     * call finalize, but the resources held by the underlying C++ objects will
     * go away immediately.
     *
     * It is a programming error to call another method on the BusAttachment
     * after the release() method has been called.
     */
    CARAPI ReleaseResources();

    /**
     * Starts the message bus and connects to the local router.
     * This method blocks until the connection attempt succeeds or fails.
     * <p>
     * {@link BusObjectListener#registered()} is called by the bus when the bus
     * is connected.
     *
     * @return OK if successful
     */
    CARAPI Connect();

    /**
     * Indicate whether bus is currently connected.
     *
     * @return true if the bus is connected
     */
    CARAPI IsConnected(
        /* [out] */ Boolean* connected);

    /**
     * Disconnects from the local router and stops the message bus.
     */
    CARAPI Disconnect();

    /**
     * Registers a bus object.
     * Once registered, the bus object may communicate to and from other
     * objects via its implemented bus interfaces.
     * <p>
     * The same object may not be registered on multiple bus connections.
     *
     * @param busObj the BusObject to register
     * @param objPath the object path of the BusObject
     * @return <ul>
     *         <li>OK if succesful
     *         <li>BUS_BAD_OBJ_PATH for a bad object path
     *         <li>BUS_OBJ_ALREADY_EXISTS if an object is already registered at this path
     *         </ul>
     * @see org.alljoyn.bus.annotation.BusInterface
     */
    CARAPI RegisterBusObject(
        /* [in] */ IBusObject* busObj,
        /* [in] */ const String& objPath);

    /**
     * Registers a bus object.
     * Once registered, the bus object may communicate to and from other
     * objects via its implemented bus interfaces.
     * <p>
     * The same object may not be registered on multiple bus connections.
     *
     * @param busObj the BusObject to register
     * @param objPath the object path of the BusObject
     * @param secure true if authentication is required to access this object
     * @return <ul>
     *         <li>OK if succesful
     *         <li>BUS_BAD_OBJ_PATH for a bad object path
     *         <li>BUS_OBJ_ALREADY_EXISTS if an object is already registered at this path
     *         </ul>
     * @see org.alljoyn.bus.annotation.BusInterface
     */
    CARAPI RegisterBusObject(
        /* [in] */ IBusObject* busObj,
        /* [in] */ const String& objPath,
        /* [in] */ Boolean secure);

    /**
     * Registers a bus object.
     * Once registered, the bus object may communicate to and from other
     * objects via its implemented bus interfaces.
     * <p>
     * The same object may not be registered on multiple bus connections.
     *
     * @param busObj the BusObject to register
     * @param objPath the object path of the BusObject
     * @param secure true if authentication is required to access this object
     * @param languageTag a language tag describing the language of the description of this BusObject
     * @param description a textual description of this BusObject
     * @return <ul>
     *         <li>OK if succesful
     *         <li>BUS_BAD_OBJ_PATH for a bad object path
     *         <li>BUS_OBJ_ALREADY_EXISTS if an object is already registered at this path
     *         </ul>
     * @see org.alljoyn.bus.annotation.BusInterface
     */
    CARAPI RegisterBusObject(
        /* [in] */ IBusObject* busObj,
        /* [in] */ const String& objPath,
        /* [in] */ Boolean secure,
        /* [in] */ const String& languageTag,
        /* [in] */ const String& description);

    /**
     * Registers a bus object.
     * Once registered, the bus object may communicate to and from other
     * objects via its implemented bus interfaces.
     * <p>
     * The same object may not be registered on multiple bus connections.
     *
     * @param busObj the BusObject to register
     * @param objPath the object path of the BusObject
     * @param secure true if authentication is required to access this object
     * @param languageTag a language tag describing the language of the description of this BusObject
     * @param description a textual description of this BusObject
     * @param dt a Translator instance to translate descriptions of this object
     * @return <ul>
     *         <li>OK if succesful
     *         <li>BUS_BAD_OBJ_PATH for a bad object path
     *         <li>BUS_OBJ_ALREADY_EXISTS if an object is already registered at this path
     *         </ul>
     * @see org.alljoyn.bus.annotation.BusInterface
     */
    CARAPI RegisterBusObject(
        /* [in] */ IBusObject* busObj,
        /* [in] */ const String& objPath,
        /* [in] */ Boolean secure,
        /* [in] */ const String& languageTag,
        /* [in] */ const String& description,
        /* [in] */ ITranslator* dt);

    /**
     * Indicates if the BusObject is secure.
     *
     * @param busObj BusObject to check if it is secure
     * @return Return true if authentication is required to emit signals or call
     *         methods on this object.
     */
    CARAPI IsBusObjectSecure(
        /* [in] */ IBusObject* busObj,
        /* [out] */ Boolean* secure);

    /**
     * Unregisters a bus object.
     *
     * @param obj the BusObject to unregister
     */
    CARAPI UnregisterBusObject(
        /* [in] */ IBusObject* obj);

    /**
     * Creates a proxy bus object for a remote bus object.
     * Methods on the remote object can be invoked through the proxy object.
     * <p>
     * There is no guarantee that the remote object referred to by the proxy
     * acutally exists.  If the remote object does not exist, proxy method
     * calls will fail.
     * <p>
     * Java proxy classes do not allow methods from two different interfaces to
     * have the same name and calling parameters. If two AllJoyn methods from two
     * different interfaces are implemented by the same remote object, one (or
     * both) of the method names must be modified. You may then use an
     * annotation for the renamed method to cause AllJoyn to use the originally
     * expected method name in any "wire" operations.
     *
     * @param busName the remote endpoint name (well-known or unique)
     * @param objPath the absolute (non-relative) object path for the object
     * @param sessionId the session corresponding to the connection to the the object
     * @param busInterfaces an array of BusInterfaces that this proxy should respond to
     * @return a ProxyBusObject for an object that implements all interfaces listed in busInterfaces
     * @see org.alljoyn.bus.annotation.BusMethod
     */
    CARAPI GetProxyBusObject(
        /* [in] */ const String& busName,
        /* [in] */ const String& objPath,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
        /* [out] */ IProxyBusObject** proxy);

    /**
     * Creates a proxy bus object for a remote bus object.
     * Methods on the remote object can be invoked through the proxy object.
     * <p>
     * There is no guarantee that the remote object referred to by the proxy
     * acutally exists.  If the remote object does not exist, proxy method
     * calls will fail.
     * <p>
     * Java proxy classes do not allow methods from two different interfaces to
     * have the same name and calling parameters. If two AllJoyn methods from two
     * different interfaces are implemented by the same remote object, one (or
     * both) of the method names must be modified. You may then use an
     * annotation for the renamed method to cause AllJoyn to use the originally
     * expected method name in any "wire" operations.
     *
     * @param busName        the remote endpoint name (well-known or unique)
     * @param objPath        the absolute (non-relative) object path for the object
     * @param sessionId      the session corresponding to the connection to the the object
     * @param busInterfaces  an array of BusInterfaces that this proxy should respond to
     * @param secure         the security mode for the remote object
     *
     * @return a ProxyBusObject for an object that implements all interfaces listed in busInterfaces
     *
     * @see org.alljoyn.bus.annotation.BusMethod
     */
    // public ProxyBusObject getProxyBusObject(String busName,
    //         String objPath,
    //         int sessionId,
    //         Class<?>[] busInterfaces,
    //         boolean secure);

    /**
     * Gets the DBusProxyObj interface of the org.freedesktop.DBus proxy object.
     * The DBusProxyObj interface is provided for backwards compatibility with
     * the DBus protocol.
     *
     * @return the DBusProxyObj interface
     */
    // public DBusProxyObj getDBusProxyObj();

    /**
     * Get the unique name of this BusAttachment.
     *
     * @return the unique name of this BusAttachment
     */
    CARAPI GetUniqueName(
        /* [out] */ String* uniqueName);

    /**
     * Get the GUID of this BusAttachment.
     *
     * The returned value may be appended to an advertised well-known name in order to guarantee
     * that the resulting name is globally unique.
     *
     * @return GUID of this BusAttachment as a string.
     */
    CARAPI GetGlobalGUIDString(
        /* [out] */ String* guid);

    /**
     * Registers a public method to receive a signal from all objects emitting
     * it.
     * Once registered, the method of the object will receive the signal
     * specified from all objects implementing the interface.
     *
     * @param ifaceName the interface name of the signal
     * @param signalName the member name of the signal
     * @param obj the object receiving the signal
     * @param handlerMethod the signal handler method
     * @return OK if the register is succesful
     */
    CARAPI RegisterSignalHandler(
        /* [in] */ const String& ifaceName,
        /* [in] */ const String& signalName,
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* handlerMethod);

    /**
     * Registers a public method to receive a signal from specific objects emitting it. Once registered, the method of
     * the object will receive the signal specified from objects implementing the interface.
     *
     * @param ifaceName the interface name of the signal
     * @param signalName the member name of the signal
     * @param obj the object receiving the signal
     * @param handlerMethod the signal handler method
     * @param source the object path of the emitter of the signal
     * @return OK if the register is successful
     */
    CARAPI RegisterSignalHandler(
        /* [in] */ const String& ifaceName,
        /* [in] */ const String& signalName,
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* handlerMethod,
        /* [in] */ const String& source);

    /**
     * Register a signal handler.
     *
     * Signals are forwarded to the signalHandler if sender, interface, member and rule qualifiers are ALL met.
     *
     * @param ifaceName the interface name of the signal
     * @param signalName the member name of the signal
     * @param obj the object receiving the signal
     * @param handlerMethod the signal handler method
     * @param matchRule a filter rule.
     * @return OK if the register is successful
     */
    CARAPI RegisterSignalHandlerWithRule(
        /* [in] */ const String& ifaceName,
        /* [in] */ const String& signalName,
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* handlerMethod,
        /* [in] */ const String& matchRule);

    /**
     * Registers all public methods that are annotated as signal handlers.
     *
     * @param obj object with methods annotated with as signal handlers
     * @return <ul>
     *         <li>OK if the register is succesful
     *         <li>BUS_NO_SUCH_INTERFACE if the interface and signal
     *         specified in any {@code @BusSignalHandler} annotations
     *         of {@code obj} are unknown to this BusAttachment.  See
     *         {@link org.alljoyn.bus.annotation.BusSignalHandler} for
     *         a discussion of how to annotate signal handlers.
     *         </ul>
     */
    CARAPI RegisterSignalHandlers(
        /* [in] */ IInterface* obj);

    /**
     * Unregisters a signal handler.
     *
     * @param obj the object receiving the signal
     * @param handlerMethod the signal handler method
     */
    CARAPI UnregisterSignalHandler(
        /* [in] */ IInterface* obj,
        /* [in] */ IMethodInfo* handlerMethod);

    /**
     * Unregisters all public methods annotated as signal handlers.
     *
     * @param obj object with previously annotated signal handlers that have
     *            been registered
     * @see org.alljoyn.bus.annotation.BusSignalHandler
     */
    CARAPI UnregisterSignalHandlers(
        /* [in] */ IInterface* obj);

    /**
     * Registers a user-defined key store listener to override the default key store.  This must be
     * called prior to {@link #connect()}.
     *
     * @param listener the key store listener
     */
    CARAPI RegisterKeyStoreListener(
        /* [in] */ IKeyStoreListener* listener);

    /**
     * Clears all stored keys from the key store. All store keys and authentication information is
     * deleted and cannot be recovered. Any passwords or other credentials will need to be reentered
     * when establishing secure peer connections.
     */
    CARAPI ClearKeyStore();

    /**
     * Clear the keys associated with a specific peer identified by its GUID.
     *
     * @param guid  The guid of a remote authenticated peer.
     *
     * @return
     * <ul>
     * <li>OK if the key was cleared</li>
     * <li>UNKNOWN GUID if theere is no peer with the specified GUID</li>
     * <li>other error status codes indicating a failure</li>
     * </ul>
     */
    CARAPI ClearKeys(
        /* [in] */ const String& guid);

    /**
     * Sets the expiration time on keys associated with a specific remote peer as identified by its
     * peer GUID. The peer GUID associated with a bus name can be obtained by calling
     * getPeerGUID(String, Mutable.StringValue).  If the timeout is 0 this is equivalent to calling
     * clearKeys(String).
     *
     * @param guid the GUID of a remote authenticated peer
     * @param timeout the time in seconds relative to the current time to expire the keys
     *
     * @return
     * <ul>
     * <li>OK if the expiration time was succesfully set</li>
     * <li>UNKNOWN_GUID if there is no authenticated peer with the specified GUID</li>
     * </ul>
     */
    CARAPI SetKeyExpiration(
        /* [in] */ const String& guid,
        /* [in] */ Int32 timeout);

    /**
     * Gets the expiration time on keys associated with a specific authenticated remote peer as
     * identified by its peer GUID. The peer GUID associated with a bus name can be obtained by
     * calling getPeerGUID(String, Mutable.StringValue).
     *
     * @param guid the GUID of a remote authenticated peer
     * @param timeout the time in seconds relative to the current time when the keys will expire
     *
     * @return
     * <ul>
     * <li>OK if the expiration time was succesfully set</li>
     * <li>UNKNOWN_GUID if there is no authenticated peer with the specified GUID</li>
     * </ul>
     */
    CARAPI GetKeyExpiration(
        /* [in] */ const String& guid,
        /* [in] */ IMutableInteger32Value* timeout);

    /**
     * Reloads the key store for this bus attachment. This function would normally only be called in
     * the case where a single key store is shared between multiple bus attachments, possibly by different
     * applications. It is up to the applications to coordinate how and when the shared key store is
     * modified.
     *
     * @return
     * <ul>
     * <li>OK if the key store was succesfully reloaded</li>
     * <li>An error status indicating that the key store reload failed</li>
     * </ul>
     */
    CARAPI ReloadKeyStore();

    /**
     * Registers a user-defined authentication listener class with a specific
     * default key store.
     *
     * @param authMechanisms the authentication mechanism(s) to use
     *                         for peer-to-peer authentication. This
     *                         is a space separated list of any of the
     *                         following values: ALLJOYN_SRP_LOGON,
     *                         ALLJOYN_SRP_KEYX, ALLJOYN_ECDHE_NULL,
     *                         ALLJOYN_ECDHE_PSK, ALLJOYN_ECDHE_ECDSA,
     *                         GSSAPI.
     * @param listener the authentication listener
     * @param keyStoreFileName the name of the default key store.
     *                         Under Android, the recommended value of
     *                         this parameter is {@code
     *                         Context.getFileStreamPath("alljoyn_keystore").getAbsolutePath()}.
     *                         Note that the default key store
     *                         implementation may be overrided with
     *                         {@link
     *                         #registerKeyStoreListener(KeyStoreListener)}.
     * @param isShared Set to true if the default keystore will be
     *                         shared between multiple programs. All
     *                         programs must have read/write
     *                         permissions to the keyStoreFileName
     *                         file.
     * @return OK if successful
     */
    CARAPI RegisterAuthListener(
        /* [in] */ const String& authMechanisms,
        /* [in] */ IAuthListener* listener,
        /* [in] */ const String& keyStoreFileName,
        /* [in] */ Boolean isShared);

    /**
     * Registers a user-defined authentication listener class with a specific default key store.
     *
     * @param authMechanisms the authentication mechanism(s) to use
     *                         for peer-to-peer authentication.  This
     *                         is a space separated list of any of the
     *                         following values: ALLJOYN_SRP_LOGON,
     *                         ALLJOYN_SRP_KEYX, ALLJOYN_ECDHE_NULL,
     *                         ALLJOYN_ECDHE_PSK, ALLJOYN_ECDHE_ECDSA,
     *                         GSSAPI.
     * @param listener the authentication listener
     * @param keyStoreFileName the name of the default key store.
     *                         Under Android, the recommended value of
     *                         this parameter is {@code
     *                         Context.getFileStreamPath("alljoyn_keystore").getAbsolutePath()}.
     *                         Note that the default key store
     *                         implementation may be overrided with
     *                         {@link
     *                         #registerKeyStoreListener(KeyStoreListener)}.
     * @return OK if successful
     */
    CARAPI RegisterAuthListener(
        /* [in] */ const String& authMechanisms,
        /* [in] */ IAuthListener* listener,
        /* [in] */ const String& keyStoreFileName);

    /**
     * Registers a user-defined authentication listener class.  Under Android, it is recommended to
     * use {@link #registerAuthListener(String, AuthListener, String)} instead to specify the path
     * of the default key store.
     *
     * @param authMechanisms the authentication mechanism(s) to use
     *                       for peer-to-peer authentication.  This is
     *                       a space separated list of any of the
     *                       following values: ALLJOYN_SRP_LOGON,
     *                       ALLJOYN_SRP_KEYX, ALLJOYN_ECDHE_NULL,
     *                       ALLJOYN_ECDHE_PSK, ALLJOYN_ECDHE_ECDSA,
     *                       GSSAPI.
     * @param listener the authentication listener
     * @return OK if successful
     */
    CARAPI RegisterAuthListener(
        /* [in] */ const String& authMechanisms,
        /* [in] */ IAuthListener* listener);

    /**
     * Registers a user-defined security violation listener class.
     *
     * @param listener the security violation listener
     */
    CARAPI RegisterSecurityViolationListener(
        /* [in] */ ISecurityViolationListener* listener);

    /**
     * Gets the message context of the currently executing method, signal
     * handler, or security violation.
     * The context contains information about the method invoker or signal
     * sender including any authentication that exists with this remote entity.
     * <p>
     * This method can only be called from within the method, signal, or
     * security violation handler itself since the caller's thread information
     * is used to find the appropriate context.
     *
     * @return message context for the currently executing method, signal
     *         handler, security violation, or null if no message can be found
     *         for the calling thread
     */
    CARAPI GetMessageContext(
        /* [out] */ IMessageContext** context);

    /**
     * Enable callbacks within the context of the currently executing method
     * handler, signal handler or other AllJoyn callback.
     * <p>
     * This method can ONLY be called from within the body of a signal handler,
     * method handler or other AllJoyn callback. It allows AllJoyn to dispatch
     * a single (additional) callback while the current one is still executing.
     * This method is typically used when a method, signal handler or other
     * AllJoyn callback needs to execute for a long period of time or when the
     * callback needs to make any kind of blocking call.
     * <p>
     * This method MUST be called prior to making any non-asynchronous AllJoyn
     * remote procedure calls from within an AllJoyn callback. This includes
     * calls such as joinSession(), advertiseName(), cancelAdvertisedName(),
     * findAdvertisedName(), cancelFindAdvertisedName(), setLinkTimeout(), etc.
     */
    CARAPI EnableConcurrentCallbacks();

private:
    /** Allocate native resources. */
    CARAPI_(void) Create(
        /* [in] */ const String& applicationName,
        /* [in] */ Boolean allowRemoteMessages,
        /* [in] */ Int32 concurrency);

    /** Start and connect to the bus. */
    CARAPI Connect(
        /* [in] */ const String& connectArgs,
        /* [in] */ IKeyStoreListener* keyStoreListener,
        /* [in] */ const String& authMechanisms,
        /* [in] */ AuthListenerInternal* busAuthListener,
        /* [in] */ const String& keyStoreFileName,
        /* [in] */ Boolean isShared);

    CARAPI RegisterBusObject(
        /* [in] */ const String& objPath,
        /* [in] */ IBusObject* busObj,
        /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces,
        /* [in] */ Boolean secure,
        /* [in] */ const String& languageTag,
        /* [in] */ const String& description,
        /* [in] */ ITranslator* dt);

    void ReleaseWithoutRemove();

    void Destroy();

    ECode LeaveGenericSession(
        /* [in] */ Int32 sessionId,
        /* [in] */ BusAttachmentSessionListenerIndex index);

    ECode SetGenericSessionListener(
        /* [in] */ Int32 sessionId,
        /* [in] */ ISessionListener* _listener,
        /* [in] */ BusAttachmentSessionListenerIndex index);

private:
    friend class InterfaceDescription;
    friend class NativeBusListener;

    AutoPtr<ISet> mRegisteredAboutListeners;

    /** The native connection handle. */
    Int64 mHandle;

    /** The connect spec. */
    String mAddress;

    /**
     * {@code true} if this attachment is allowed to receive messages from
     * remote devices.
     */
    Boolean mAllowRemoteMessages;

    AutoPtr<IKeyStoreListener> mKeyStoreListener;

    AutoPtr<AuthListenerInternal> mBusAuthListener;

    /** End-to-end authentication mechanisms. */
    String mAuthMechanisms;

    /** Default key store file name. */
    String mKeyStoreFileName;

    /** Specify if the default key store is shared */
    Boolean mIsShared;

    /** Specify if the attachment is connected */
    Boolean mIsConnected;

    AutoPtr<IExecutorService> mExecutor;

    AutoPtr<IDBusProxyObj> mDbus;
    AutoPtr<ProxyBusObject> mDbusbo;

    /**
     * The maximum number of concurrent method and signal handlers locally
     * executing by default.
     */
    static const Int32 DEFAULT_CONCURRENCY;

    /* Set of all the connected BusAttachments. Maintain a weakreference so we dont delay garbage collection */
    static AutoPtr<IHashSet> sBusAttachmentSet;

    static Boolean sShutdownHookRegistered;

    static const String TAG;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CBUSATTACHMENT_H__
