
#ifndef __ELASTOS_DROID_NET_NETWORKPOLICYMANAGER_H__
#define __ELASTOS_DROID_NET_NETWORKPOLICYMANAGER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Format::ITime;

using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Manager for creating and modifying network policy rules.
 *
 * {@hide}
 */
class NetworkPolicyManager
    : public Object
    , public INetworkPolicyManager
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IINetworkPolicyManager* service);

    static CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ INetworkPolicyManager** result);

    /**
     * Set policy flags for specific UID.
     *
     * @param policy {@link #POLICY_NONE} or combination of flags like
     * {@link #POLICY_REJECT_METERED_BACKGROUND}, {@link #POLICY_ALLOW_BACKGROUND_BATTERY_SAVE}.
     */
    CARAPI SetUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    /**
     * Add policy flags for specific UID.  The given policy bits will be set for
     * the uid.  Policy flags may be either
     * {@link #POLICY_REJECT_METERED_BACKGROUND} or {@link #POLICY_ALLOW_BACKGROUND_BATTERY_SAVE}.
     */
    CARAPI AddUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    /**
     * Clear/remove policy flags for specific UID.  The given policy bits will be set for
     * the uid.  Policy flags may be either
     * {@link #POLICY_REJECT_METERED_BACKGROUND} or {@link #POLICY_ALLOW_BACKGROUND_BATTERY_SAVE}.
     */
    CARAPI RemoveUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    CARAPI GetUidPolicy(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI GetUidsWithPolicy(
        /* [in] */ Int32 policy,
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetPowerSaveAppIdWhitelist(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI RegisterListener(
        /* [in] */ IINetworkPolicyListener* listener);

    CARAPI UnregisterListener(
        /* [in] */ IINetworkPolicyListener* listener);

    CARAPI SetNetworkPolicies(
        /* [in] */ ArrayOf<INetworkPolicy*>* policies);

    CARAPI GetNetworkPolicies(
        /* [out, callee] */ ArrayOf<INetworkPolicy*>** result);

    CARAPI SetRestrictBackground(
        /* [in] */ Boolean restrictBackground);

    CARAPI GetRestrictBackground(
        /* [out] */ Boolean* result);

    /**
     * Compute the last cycle boundary for the given {@link NetworkPolicy}. For
     * example, if cycle day is 20th, and today is June 15th, it will return May
     * 20th. When cycle day doesn't exist in current month, it snaps to the 1st
     * of following month.
     *
     * @hide
     */
    static CARAPI ComputeLastCycleBoundary(
        /* [in] */ Int64 currentTime,
        /* [in] */ INetworkPolicy* policy,
        /* [out] */ Int64* result);

    /** {@hide} */
    static CARAPI ComputeNextCycleBoundary(
        /* [in] */ Int64 currentTime,
        /* [in] */ INetworkPolicy* policy,
        /* [out] */ Int64* result);

    /**
     * Snap to the cycle day for the current month given; when cycle day doesn't
     * exist, it snaps to last second of current month.
     *
     * @hide
     */
    static CARAPI SnapToCycleDay(
        /* [in] */ ITime* time,
        /* [in] */ Int32 cycleDay);

    /**
     * Check if given UID can have a {@link #setUidPolicy(int, int)} defined,
     * usually to protect critical system services.
     */
    // @Deprecated
    static CARAPI IsUidValidForPolicy(
        /* [in] */ IContext* context,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    /** {@hide} */
    static CARAPI DumpPolicy(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ Int32 policy);

    /** {@hide} */
    static CARAPI DumpRules(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ Int32 rules);

private:
    static const Boolean ALLOW_PLATFORM_APP_POLICY;

    AutoPtr<IINetworkPolicyManager> mService;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKPOLICYMANAGER_H__
