
#ifndef __ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__
#define __ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__

#include "_Elastos_Droid_Server_Am_CContentProviderConnection.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ContentProviderRecord;
class ProcessRecord;

CarClass(CContentProviderConnection)
    , public Object
    , public IBinder
{
public:
    CContentProviderConnection();

    ~CContentProviderConnection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    constructor(
        /* [in] */ Handle32 provider,
        /* [in] */ Handle32 client);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

    CARAPI_(String) ToShortString();

    CARAPI_(String) ToClientString();

    CARAPI_(void) ToShortString(
        /* [in] */ StringBuilder* sb);

    CARAPI_(void) ToClientString(
        /* [in] */ StringBuilder* sb);

public:
    AutoPtr<ContentProviderRecord> mProvider;
    AutoPtr<ProcessRecord> mClient;
    Int64 mCreateTime;
    Int32 mStableCount;
    Int32 mUnstableCount;
    // The client of this connection is currently waiting for the provider to appear.
    // Protected by the provider lock.
    Boolean mWaiting;
    // The provider of this connection is now dead.
    Boolean mDead;

    // For debugging.
    Int32 mNumStableIncs;
    Int32 mNumUnstableIncs;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_SERVER_AM_CCONTENTPROVIDERCONNECTION_H__
