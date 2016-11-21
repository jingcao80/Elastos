
#ifndef __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/AboutObj.h>
#include <alljoyn/AboutDataListener.h>
#include <alljoyn/MsgArg.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeAboutObject : public ajn::AboutObj, public ajn::AboutDataListener
{
public:
    NativeAboutObject(
        /* [in] */ NativeBusAttachment* bus,
        /* [in] */ AnnounceFlag isAboutIntfAnnounced);

    ~NativeAboutObject();

    QStatus Announce(
        /* [in] */ IAboutObj* thiz,
        /* [in] */ Int16 sessionPort,
        /* [in] */ IAboutDataListener* aboutDataListener);

    QStatus GetAboutData(
        /* [in] */ ajn::MsgArg* msgArg,
        /* [in] */ const char* language);

    QStatus GetAnnouncedAboutData(
        /* [in] */ ajn::MsgArg* msgArg);

    /**
     * This will check if the last method call threw an exception Since we are
     * looking for ErrorReplyBusExceptions we know that the exception thrown
     * correlates to a QStatus that we are trying to get.  If ER_FAIL is returned
     * then we had an issue resolving the java method calls.
     *
     * @return QStatus indicating the status that was thrown from the ErrReplyBusException
     */
    QStatus CheckForThrownException(
        /* [in] */ ECode ec);

public:
    NativeBusAttachment* mBusPtr;
    AutoPtr<IAboutDataListener> mAboutDataListenerRef;
    AutoPtr<IAboutObj> mAboutObjGlobalRef;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEABOUTOBJECT_H__
