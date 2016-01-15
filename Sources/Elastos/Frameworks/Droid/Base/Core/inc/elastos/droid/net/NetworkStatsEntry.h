
#ifndef __ELASTOS_DROID_NET_NETWORKSTATSENTRY_H__
#define __ELASTOS_DROID_NET_NETWORKSTATSENTRY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

class NetworkStatsEntry
    : public Object
    , public INetworkStatsEntry
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations);

    CARAPI constructor(
        /* [in] */ const String& iface,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 set,
        /* [in] */ Int32 tag,
        /* [in] */ Int64 rxBytes,
        /* [in] */ Int64 rxPackets,
        /* [in] */ Int64 txBytes,
        /* [in] */ Int64 txPackets,
        /* [in] */ Int64 operations);

    CARAPI IsNegative(
        /* [out] */ Boolean* result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI Add(
        /* [in] */ INetworkStatsEntry* another);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetIface(
    /* [out] */ String* result);

    CARAPI SetIface(
    /* [in] */ const String& iface);

    CARAPI GetUid(
    /* [out] */ Int32* result);

    CARAPI SetUid(
    /* [in] */ Int32 uid);

    CARAPI GetSet(
    /* [out] */ Int32* result);

    CARAPI SetSet(
    /* [in] */ Int32 set);

    CARAPI GetTag(
    /* [out] */ Int32* result);

    CARAPI SetTag(
    /* [in] */ Int32 tag);

    CARAPI GetRxBytes(
    /* [out] */ Int64* result);

    CARAPI SetRxBytes(
    /* [in] */ Int64 rxBytes);

    CARAPI GetRxPackets(
    /* [out] */ Int64* result);

    CARAPI SetRxPackets(
    /* [in] */ Int64 rxPackets);

    CARAPI GetTxBytes(
    /* [out] */ Int64* result);

    CARAPI SetTxBytes(
    /* [in] */ Int64 txBytes);

    CARAPI GetTxPackets(
    /* [out] */ Int64* result);

    CARAPI SetTxPackets(
    /* [in] */ Int64 txPackets);

    CARAPI GetOperations(
    /* [out] */ Int64* result);

    CARAPI SetOperations(
    /* [in] */ Int64 operations);

private:
    String mIface;

    Int32 mUid;

    Int32 mSet;

    Int32 mTag;

    Int64 mRxBytes;

    Int64 mRxPackets;

    Int64 mTxBytes;

    Int64 mTxPackets;

    Int64 mOperations;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKSTATSENTRY_H__
