
#ifndef __ORG_ALLJOYN_BUS_ONJOINSESSIONLISTENER_H__
#define __ORG_ALLJOYN_BUS_ONJOINSESSIONLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * Signature provides static methods for converting between Java and DBus type signatures.
 * This class is used internally.
 */
class OnJoinSessionListener
    : public Object
    , public IOnJoinSessionListener
{
public:
    CAR_INTERFACE_DECL()

    OnJoinSessionListener();

    virtual ~OnJoinSessionListener();

    CARAPI constructor();

    /**
     * Notification callback that happens when a response to an asynchronous
     * joinSession request is ready.
     *
     * Any implementation of this function must be multithread safe.  See the
     * class documentation for details.
     *
     * @param status <ul><li>OK if the session was joined.</li>
     *                   <li>BUS_NOT_CONNECTED if a connection has not been made with a local
     *                       bus</li>
     *                   <li>other error status codes indicating a failure.</li></ul>
     * @param sessionId Set to the unique identifier for session.
     * @param opts      Set to the actual session options of the joined session.
     * @param context   The user-defined context object supplied in the call to {@link
     *                  BusAttachment#joinSession(String, short, SessionOpts, SessionListener,
     *                  OnJoinSessionListener, Object)}.
     */
    CARAPI OnJoinSession(
        /* [in] */ Int32 status,
        /* [in] */ Int32 sessionId,
        /* [in] */ ISessionOpts* opts,
        /* [in] */ IInterface* context);


private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this OnJoinSessionListener object.
     */
    void Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this OnJoinSessionListener object.
     */
    void Destroy();

private:
    friend class CBusAttachment;

    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ONJOINSESSIONLISTENER_H__
