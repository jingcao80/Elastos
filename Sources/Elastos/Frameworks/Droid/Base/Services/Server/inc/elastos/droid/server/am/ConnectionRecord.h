
#ifndef __ELASTOS_DROID_SERVER_AM_CONNECTIONRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_CONNECTIONRECORD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IIServiceConnection;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppBindRecord;
class ActivityRecord;

/**
 * Description of a single binding to a service.
 */
class ConnectionRecord : public Object
{
public:
    ConnectionRecord(
        /* [in] */ AppBindRecord* binding,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ IIServiceConnection* conn,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 clientLabel,
        /* [in] */ IPendingIntent* clientIntent);

    void Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

public:
    AutoPtr<AppBindRecord> mBinding;    // weak-ref The application/service binding.
    ActivityRecord* mActivity;  // weak-ref If non-null, the owning activity.
    AutoPtr<IIServiceConnection> mConn;  // The client connection.
    Int32 mFlags;   // Binding options.
    Int32 mClientLabel;          // String resource labeling this client.
    AutoPtr<IPendingIntent> mClientIntent; // How to launch the client.
    String mStringName;     // Caching of toString.
    Boolean mServiceDead;            // Well is it?
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::ConnectionRecord)

#endif //__ELASTOS_DROID_SERVER_AM_CONNECTIONRECORD_H__
