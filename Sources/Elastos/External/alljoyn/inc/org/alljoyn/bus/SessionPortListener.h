
#ifndef __ORG_ALLJOYN_BUS_SESSIONPORTLISTENER_H__
#define __ORG_ALLJOYN_BUS_SESSIONPORTLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * A SessionPortListener is responsible for handling session port related callbacks
 * from the AllJoyn system. It is expected that an AllJoyn session creator will
 * specialize this class in order to handle callbacks required for accepting session
 * joiners.
 *
 * Listener objects are the Java objects that handle notification events and are
 * called from AllJoyn in the context of one of its threads.  All listener
 * objects are expected to be multithread safe (MT-Safe) between construction
 * and destruction.  That is, every thread executing in a listener object's
 * methods 1) gets a unique copy of all temporary data (it is re-entrant); and
 * 2) all shared data -- the object instance's member variables or any globals
 * must contain no read-modify-write access patterns (okay to write or read,
 * just never to read-modify-write).  If such access patterns are required, it
 * is the responsibility of the client to, for example, add the synchronized
 * keyword when overriding one of the listener methods or provide some other
 * serialization mechanism in order to preserve MT-Safe operation.
 *
 * This rule extends to other objects accessed during processing of
 * notifications.  For example, it is a programming error to allow a notifiation
 * method to update a collection in another object without serializing access
 * to the collection.
 *
 * The important consideration in this case is that as soon as one sets up a
 * listener object to receive notifications from AllJoyn, one is implicitly
 * dealing with multithreaded code.
 *
 * Since listener objects generally run in the context of the AllJoyn thread
 * which manages reception of events, If a blocking AllJoyn call is made in
 * the context of a notification, the necessary and sufficient conditions for
 * deadlock are established.
 *
 * The important consideration in this case is that when one receives a
 * notification from AllJoyn, that notification is executing in the context of
 * an AllJoyn thread.  If one makes a blocking call back into AllJoyn on that
 * thread, a deadlock cycle is likely, and if this happens your bus attachment
 * receive thread will deadlock (with itself).  The deadlock is typically broken
 * after a bus timeout eventually happens.
 */
class ECO_PUBLIC SessionPortListener
    : public Object
    , public ISessionPortListener
{
public:
    CAR_INTERFACE_DECL()

    SessionPortListener();

    virtual ~SessionPortListener();

    /**
     * Accept or reject an incoming JoinSession request. The session does not
     * exist until this after this function returns.
     *
     * Any implementation of this function must be multithread safe.  See the
     * class documentation for details.
     *
     * @param sessionPort    Session port that was joined.
     * @param joiner         Unique name of potential joiner.
     * @param opts           Session options requested by the joiner.
     *
     * @return Return true if JoinSession request is accepted. false if rejected.
     */
    CARAPI AcceptSessionJoiner(
        /* [in] */ Int16 sessionPort,
        /* [in] */ const String& joiner,
        /* [in] */ ISessionOpts* opts,
        /* [out] */ Boolean* accepted);

    /**
     * Called by the bus when a session has been successfully joined. The
     * session is fully up when this method is called.
     *
     * Any implementation of this function must be multithread safe.  See the
     * class documentation for details.
     *
     * @param sessionPort    Session port that was joined.
     * @param id             Id of session.
     * @param joiner         Unique name of the joiner.
     */
    CARAPI SessionJoined(
        /* [in] */ Int16 sessionPort,
        /* [in] */ Int32 id,
        /* [in] */ const String& joiner);

private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this listener object.
     */
    CARAPI_(void) Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this listener object.
     */
    CARAPI_(void) Destroy();

private:
    friend class CBusAttachment;

    /**
     * The opaque pointer to the underlying C++ object which is actually tied
     * to the AllJoyn code.
     */
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_SESSIONPORTLISTENER_H__
