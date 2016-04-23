
#ifndef __ELASTOS_DROID_SERVER_AM_RECEIVERLIST_H__
#define __ELASTOS_DROID_SERVER_AM_RECEIVERLIST_H__

#include "elastos/droid/server/am/BroadcastFilter.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IIntentReceiver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessRecord;
class BroadcastRecord;
class CActivityManagerService;

class ReceiverList
    : public Object
    , public List< AutoPtr<BroadcastFilter> >
    , public IProxyDeathRecipient
{
public:
    CAR_INTERFACE_DECL()

    ReceiverList(
        /* [in] */ CActivityManagerService* owner,
        /* [in] */ ProcessRecord* callerApp,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 userId,
        /* [in] */ IIntentReceiver* receiver);

    virtual ~ReceiverList();

    // Want object identity, not the array identity we are inheriting.
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ProxyDied();

    CARAPI_(void) DumpLocal(
        /* [in] */ IPrintWriter* pw,
        /* [out] */ const String& prefix);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [out] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    CActivityManagerService* mOwner;
    AutoPtr<IIntentReceiver> mReceiver;
    AutoPtr<ProcessRecord> mApp;
    Int32 mPid;
    Int32 mUid;
    Int32 mUserId;
    AutoPtr<BroadcastRecord> mCurBroadcast;
    Boolean mLinkedToDeath;

    String mStringName;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::ReceiverList)

#endif //__ELASTOS_DROID_SERVER_AM_RECEIVERLIST_H__
