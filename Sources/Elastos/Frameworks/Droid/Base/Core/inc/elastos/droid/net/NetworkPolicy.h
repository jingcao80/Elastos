
#ifndef __ELASTOS_DROID_NET_NETWORKPOLICY_H__
#define __ELASTOS_DROID_NET_NETWORKPOLICY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Policy for networks matching a {@link NetworkTemplate}, including usage cycle
 * and limits to be enforced.
 *
 * @hide
 */
class NetworkPolicy
    : public Object
    , public IParcelable
    , public IComparable
    , public INetworkPolicy
{
public:
    CAR_INTERFACE_DECL()

    NetworkPolicy();

    CARAPI constructor();
    // @Deprecated
    CARAPI constructor(
        /* [in] */ INetworkTemplate* networkTemplate,
        /* [in] */ Int32 cycleDay,
        /* [in] */ const String& cycleTimezone,
        /* [in] */ Int64 warningBytes,
        /* [in] */ Int64 limitBytes,
        /* [in] */ Boolean metered);

    CARAPI constructor(
        /* [in] */ INetworkTemplate* networkTemplate,
        /* [in] */ Int32 cycleDay,
        /* [in] */ const String& cycleTimezone,
        /* [in] */ Int64 warningBytes,
        /* [in] */ Int64 limitBytes,
        /* [in] */ Int64 lastWarningSnooze,
        /* [in] */ Int64 lastLimitSnooze,
        /* [in] */ Boolean metered,
        /* [in] */ Boolean inferred);

    /**
     * Test if given measurement is over {@link #warningBytes}.
     */
    CARAPI IsOverWarning(
        /* [in] */ Int64 totalBytes,
        /* [out] */ Boolean* result);

    /**
     * Test if given measurement is near enough to {@link #limitBytes} to be
     * considered over-limit.
     */
    CARAPI IsOverLimit(
        /* [in] */ Int64 totalBytes,
        /* [out] */ Boolean* result);

    /**
     * Clear any existing snooze values, setting to {@link #SNOOZE_NEVER}.
     */
    CARAPI ClearSnooze();

    /**
     * Test if this policy has a cycle defined, after which usage should reset.
     */
    CARAPI HasCycle(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetTemplate(
        /* [out] */ INetworkTemplate** result);

    CARAPI GetCycleDay(
        /* [out] */ Int32* result);

    CARAPI SetCycleDay(
        /* [in] */ Int32 cycleDay);

    CARAPI GetCycleTimezone(
        /* [out] */ String* result);

    CARAPI SetCycleTimezone(
        /* [in] */ const String& cycleTimezone);

    CARAPI GetWarningBytes(
        /* [out] */ Int64* result);

    CARAPI SetWarningBytes(
        /* [in] */ Int64 warningBytes);

    CARAPI GetLimitBytes(
        /* [out] */ Int64* result);

    CARAPI SetLimitBytes(
        /* [in] */ Int64 limitBytes);

    CARAPI GetLastWarningSnooze(
        /* [out] */ Int64* result);

    CARAPI SetLastWarningSnooze(
        /* [in] */ Int64 lastWarningSnooze);

    CARAPI GetLastLimitSnooze(
        /* [out] */ Int64* result);

    CARAPI SetLastLimitSnooze(
        /* [in] */ Int64 lastLimitSnooze);

    CARAPI GetMetered(
        /* [out] */ Boolean* result);

    CARAPI SetMetered(
        /* [in] */ Boolean metered);

    CARAPI GetInferred(
        /* [out] */ Boolean* result);

    CARAPI SetInferred(
        /* [in] */ Boolean inferred);

private:
    /* const */ AutoPtr<INetworkTemplate> mTemplate;

    Int32 mCycleDay;

    String mCycleTimezone;

    Int64 mWarningBytes;

    Int64 mLimitBytes;

    Int64 mLastWarningSnooze;

    Int64 mLastLimitSnooze;

    Boolean mMetered;

    Boolean mInferred;

    static const Int64 DEFAULT_MTU;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKPOLICY_H__
