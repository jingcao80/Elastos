
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_MOBILEDATACONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_MOBILEDATACONTROLLER_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IINetworkStatsSession;
using Elastos::Droid::Net::IINetworkStatsService;
using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::INetworkStatsHistoryEntry;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Core::IStringBuilder;
using Elastos::Core::Object;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class MobileDataController
    : public Object
    , public IMobileDataController
{
public:
    CAR_INTERFACE_DECL();

    MobileDataController(
        /* [in] */ IContext* context);

    CARAPI SetCallback(
        /* [in] */ IMobileDataControllerCallback* callback);

    CARAPI GetDataUsageInfo(
        /* [out] */ INetworkControllerDataUsageInfo** info);

    CARAPI SetMobileDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsMobileDataSupported(
        /* [out] */ Boolean* result);

    CARAPI IsMobileDataEnabled(
        /* [out] */ Boolean* result);

private:
    CARAPI_(AutoPtr<IINetworkStatsSession>) GetSession();

    CARAPI_(AutoPtr<INetworkControllerDataUsageInfo>) Warn(
        /* [in] */ const String& msg);

    static CARAPI_(AutoPtr<ITime>) AddMonth(
        /* [in] */ ITime* t,
        /* [in] */ Int32 months);

    CARAPI_(AutoPtr<INetworkPolicy>) FindNetworkPolicy(
        /* [in] */ INetworkTemplate* t);

    static CARAPI_(String) HistoryEntryToString(
        /* [in] */ INetworkStatsHistoryEntry* entry);

    static CARAPI_(String) GetActiveSubscriberId(
        /* [in] */ IContext* context);

    CARAPI_(String) FormatDateRange(
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    static CARAPI_(Boolean) InitStatic();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int64 DEFAULT_WARNING_LEVEL;
    static const Int32 FIELDS;
    static Boolean sInit;
    static AutoPtr<IStringBuilder> PERIOD_BUILDER;
    static AutoPtr<IFormatter> PERIOD_FORMATTER;

    AutoPtr<IContext> mContext;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<IConnectivityManager> mConnectivityManager;
    AutoPtr<IINetworkStatsService> mStatsService;
    AutoPtr<INetworkPolicyManager> mPolicyManager;

    AutoPtr<IINetworkStatsSession> mSession;
    AutoPtr<IMobileDataControllerCallback> mCallback;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_MOBILEDATACONTROLLER_H__
