
#include "org/alljoyn/bus/NativeSessionListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeSessionListener::NativeSessionListener(
    /* [in] */ ISessionListener* sessionListener)
{
    IWeakReferenceSource::Probe(sessionListener)->GetWeakReference((IWeakReference**)&mSessionListener);
    assert(mSessionListener != NULL);
}

NativeSessionListener::~NativeSessionListener()
{}

void NativeSessionListener::SessionLost(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ ajn::SessionListener::SessionLostReason reason)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionLost(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionLost((Int32)sessionId, (Int32)reason);
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionLost(): Exception");
    }
}

void NativeSessionListener::SessionMemberAdded(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionMemberAdded(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionMemberAdded((Int32)sessionId, String(uniqueName));
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionMemberAdded(): Exception");
    }
}

void NativeSessionListener::SessionMemberRemoved(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName)
{
    AutoPtr<ISessionListener> sl;
    mSessionListener->Resolve(EIID_ISessionListener, (IInterface**)&sl);
    if (sl == NULL) {
        Logger::E("NativeSessionListener", "SessionMemberRemoved(): Can't get new local reference to ISessionListener");
        return;
    }

    ECode ec = sl->SessionMemberRemoved((Int32)sessionId, String(uniqueName));
    if (FAILED(ec)) {
        Logger::E("NativeSessionListener", "SessionMemberRemoved(): Exception");
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
