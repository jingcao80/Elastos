
#ifndef __ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__
#define __ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/content/IntentFilter.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IntentFilter;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ReceiverList;

class BroadcastFilter
    : public IntentFilter
    , public IBroadcastFilter
{
public:
    BroadcastFilter(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ ReceiverList* receiverList,
        /* [in] */ const String& packageName,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 owningUid,
        /* [in] */ Int32 userId);

    ~BroadcastFilter();

    CAR_INTERFACE_DECL()

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpBrief(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpInReceiverList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IPrinter* pr,
        /* [in] */ const String& prefix);

    CARAPI_(void) DumpBroadcastFilterState(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    // Back-pointer to the list this filter is in.
    ReceiverList* mReceiverList; // BroadcastFilter will be PushBack to ReceiverList
    String mPackageName;
    String mRequiredPermission;
    Int32 mOwningUid;
    Int32 mOwningUserId;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Server::Am::BroadcastFilter*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SERVER_AM_BROADCASTFILTER_H__
