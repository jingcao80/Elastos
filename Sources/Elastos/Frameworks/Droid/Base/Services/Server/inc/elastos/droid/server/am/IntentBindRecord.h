
#ifndef __ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__

#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Os::IBinder;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessRecord;
class CServiceRecord;

/**
 * A particular Intent that has been bound to a Service.
 */
class IntentBindRecord : public Object
{
public:
    IntentBindRecord(
        /* [in] */ CServiceRecord* service,
        /* [in] */ IIntentFilterComparison* intent);

    virtual ~IntentBindRecord();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpInService(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(Int32) CollectFlags();

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
     /** The running service. */
    CServiceRecord* mService;
    /** The intent that is bound.*/
    AutoPtr<IIntentFilterComparison> mIntent; //
     /** All apps that have bound to this Intent. */
    HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> > mApps;
    /** Binder published from service. */
    AutoPtr<IBinder> mBinder;
    /** Set when we have initiated a request for this binder. */
    Boolean mRequested;
    /** Set when we have received the requested binder. */
    Boolean mReceived;
    /** Set when we still need to tell the service all clients are unbound. */
    Boolean mHasBound;
    /** Set when the service's onUnbind() has asked to be told about new clients. */
    Boolean mDoRebind;

    String mStringName;      // caching of toString
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_INTENTBINDRECORD_H__
