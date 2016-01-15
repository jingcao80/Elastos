
#ifndef __ELASTOS_DROID_INTERNAL_NET_CVPNCONFIG_H__
#define __ELASTOS_DROID_INTERNAL_NET_CVPNCONFIG_H__

#include "Elastos.Droid.App.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Net_CVpnConfig.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CVpnConfig)
    , public Object
    , public IVpnConfig
    , public IParcelable
{
public:
    CVpnConfig();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI UpdateAllowedFamilies(
        /* [in] */ IInetAddress* address);

    CARAPI AddLegacyRoutes(
        /* [in] */ const String& routesStr);

    CARAPI AddLegacyAddresses(
        /* [in] */ const String& addressesStr);

    CARAPI GetUser(
        /* [out] */ String* user);

    CARAPI SetUser(
        /* [in] */ const String& user);

    CARAPI GetInterfaze(
        /* [out] */ String* interfaze);

    CARAPI SetInterfaze(
        /* [in] */ const String& interfaze);

    CARAPI GetSession(
        /* [out] */ String* session);

    CARAPI SetSession(
        /* [in] */ const String& session);

    CARAPI GetMtu(
        /* [out] */ Int32* mtu);

    CARAPI SetMtu(
        /* [in] */ Int32 mtu);

    CARAPI GetAddresses(
        /* [out] */ IList** addresses);

    CARAPI SetAddresses(
        /* [in] */ IList* addresses);

    CARAPI GetRoutes(
        /* [out] */ IList** routes);

    CARAPI SetRoutes(
        /* [in] */ IList* routes);

    CARAPI GetDnsServers(
        /* [out] */ IList** dnsServers);

    CARAPI SetDnsServers(
        /* [in] */ IList* dnsServers);

    CARAPI GetSearchDomains(
        /* [out] */ IList** searchDomains);

    CARAPI SetSearchDomains(
        /* [in] */ IList* searchDomains);

    CARAPI GetAllowedApplications(
        /* [out] */ IList** allowedApplications);

    CARAPI SetAllowedApplications(
        /* [in] */ IList* allowedApplications);

    CARAPI GetDisallowedApplications(
        /* [out] */ IList** disallowedApplications);

    CARAPI SetDisallowedApplications(
        /* [in] */ IList* disallowedApplications);

    CARAPI GetConfigureIntent(
        /* [out] */ IPendingIntent** configureIntent);

    CARAPI SetConfigureIntent(
        /* [in] */ IPendingIntent* configureIntent);

    CARAPI GetStartTime(
        /* [out] */ Int64* startTime);

    CARAPI SetStartTime(
        /* [in] */ Int64 startTime);

    CARAPI GetLegacy(
        /* [out] */ Boolean* legacy);

    CARAPI SetLegacy(
        /* [in] */ Boolean legacy);

    CARAPI GetBlocking(
        /* [out] */ Boolean* blocking);

    CARAPI SetBlocking(
        /* [in] */ Boolean blocking);

    CARAPI GetAllowBypass(
        /* [out] */ Boolean* allowBypass);

    CARAPI SetAllowBypass(
        /* [in] */ Boolean allowBypass);

    CARAPI GetAllowIPv4(
        /* [out] */ Boolean* allowIPv4);

    CARAPI SetAllowIPv4(
        /* [in] */ Boolean allowIPv4);

    CARAPI GetAllowIPv6(
        /* [out] */ Boolean* allowIPv6);

    CARAPI SetAllowIPv6(
        /* [in] */ Boolean allowIPv6);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI GetIntentForConfirmation(
        /* [out] */ IIntent** intent);

    static CARAPI GetIntentForStatusPanel(
        /* [in] */ IContext* context,
        /* [out] */ IPendingIntent** intent);

    static CARAPI GetVpnLabel(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ ICharSequence** label);

public:
    String mUser;
    String mInterfaze;
    String mSession;
    Int32 mMtu;
    AutoPtr<IList> mAddresses;
    AutoPtr<IList> mRoutes;
    AutoPtr<IList> mDnsServers;
    AutoPtr<IList> mSearchDomains;
    AutoPtr<IList> mAllowedApplications;
    AutoPtr<IList> mDisallowedApplications;
    AutoPtr<IPendingIntent> mConfigureIntent;
    Int64 mStartTime;
    Boolean mLegacy;
    Boolean mBlocking;
    Boolean mAllowBypass;
    Boolean mAllowIPv4;
    Boolean mAllowIPv6;
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CVPNCONFIG_H__
