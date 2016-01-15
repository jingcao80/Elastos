
#ifndef __ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashSet.h>
#include "elastos/droid/server/am/ConnectionRecord.h"

using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CServiceRecord;
class IntentBindRecord;
class ProcessRecord;

/**
 * An association between a service and one of its client applications.
 */
class AppBindRecord : public Object
{
public:
    AppBindRecord(
        /* [in] */ CServiceRecord* service,
        /* [in] */ IntentBindRecord* intent,
        /* [in] */ ProcessRecord* client);

    void Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    void DumpInIntentBind(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<CServiceRecord> mService;   // The running service.
    AutoPtr<IntentBindRecord> mIntent;  // The intent we are bound to.
    ProcessRecord* mClient;     // Who has started/bound the service.

    HashSet< AutoPtr<ConnectionRecord> > mConnections;// All ConnectionRecord for this client.
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPBINDRECORD_H__
