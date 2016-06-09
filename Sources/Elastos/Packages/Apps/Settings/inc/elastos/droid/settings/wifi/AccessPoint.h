
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_ACCESSPOINT_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_ACCESSPOINT_H__

#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/utility/LruCache.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::LruCache;
using Elastos::Droid::View::IView;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiInfo;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

class AccessPoint
    : public Elastos::Droid::Preference::Preference
    , public IAccessPoint
{
public:
    CAR_INTERFACE_DECL();

    AccessPoint();

    ~AccessPoint();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWifiConfiguration* config);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IScanResult* result);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IBundle* savedState);

    static CARAPI_(Int32) GetSecurity(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(String) GetSecurityString(
        /* [in] */ Boolean concise);

    CARAPI SaveWifiState(
        /* [in] */ IBundle* savedState);

    //@Override
    CARAPI CompareTo(
        /* [in] */ IInterface* preference,
        /* [out] */ Int32* result);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI_(Boolean) Update(
        /* [in] */ IScanResult* result);

    CARAPI_(void) Update(
        /* [in] */ IWifiInfo* info,
        /* [in] */ NetworkInfoDetailedState state);

    CARAPI_(Int32) GetLevel();

    CARAPI_(AutoPtr<IWifiConfiguration>) GetConfig();

    CARAPI_(AutoPtr<IWifiInfo>) GetInfo();

    CARAPI_(NetworkInfoDetailedState) GetState();

    static CARAPI_(String) RemoveDoubleQuotes(
        /* [in] */ const String& string);

    static CARAPI_(String) ConvertToQuotedString(
        /* [in] */ const String& string);

    /**
     * Shows or Hides the Summary of an AccessPoint.
     *
     * @param showSummary TRUE will show the summary, FALSE will hide the summary
     */
    CARAPI SetShowSummary(
        /* [in] */ Boolean showSummary);

    /**
     * Generate and save a default wifiConfiguration with common values.
     * Can only be called for unsecured networks.
     * @hide
     */
    CARAPI GenerateOpenNetworkConfig();

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

    CARAPI_(void) UpdateIcon(
        /* [in] */ Int32 level,
        /* [in] */ IContext* context);

private:
    static CARAPI_(Int32) GetSecurity(
        /* [in] */ IScanResult* result);

    static CARAPI_(AccessPointPskType) GetPskType(
        /* [in] */ IScanResult* result);

    CARAPI_(void) LoadConfig(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(void) LoadResult(
        /* [in] */ IScanResult* result);

    /**
     * Returns the visibility status of the WifiConfiguration.
     *
     * @return autojoin debugging information
     * TODO: use a string formatter
     * ["rssi 5Ghz", "num results on 5GHz" / "rssi 5Ghz", "num results on 5GHz"]
     * For instance [-40,5/-30,2]
     */
    CARAPI_(String) GetVisibilityStatus();

    /**
     * Updates the title and summary; may indirectly call NotifyChanged().
     */
    CARAPI_(void) Refresh();

public:
    static const String TAG;

    /**
     * Lower bound on the 2.4 GHz (802.11b/g/n) WLAN channels
     */
    static const Int32 LOWER_FREQ_24GHZ;

    /**
     * Upper bound on the 2.4 GHz (802.11b/g/n) WLAN channels
     */
    static const Int32 HIGHER_FREQ_24GHZ;

    /**
     * Lower bound on the 5.0 GHz (802.11a/h/j/n/ac) WLAN channels
     */
    static const Int32 LOWER_FREQ_5GHZ;

    /**
     * Upper bound on the 5.0 GHz (802.11a/h/j/n/ac) WLAN channels
     */
    static const Int32 HIGHER_FREQ_5GHZ;

    /**
     * Experimental: we should be able to show the user the list of BSSIDs and bands
     *  for that SSID.
     *  For now this data is used only with Verbose Logging so as to show the band and number
     *  of BSSIDs on which that network is seen.
     */
    AutoPtr< LruCache< String, AutoPtr<IScanResult> > > mScanResultCache;

    /**
     * These values are matched in string arrays -- changes must be kept in sync
     */
    static const Int32 SECURITY_NONE = 0;
    static const Int32 SECURITY_WEP = 1;
    static const Int32 SECURITY_PSK = 2;
    static const Int32 SECURITY_EAP = 3;

    String mSsid;
    String mBssid;
    Int32 mSecurity;
    Int32 mNetworkId;
    Boolean mWpsAvailable;
    Boolean mShowSummary;

    AccessPointPskType mPskType;
    /* package */AutoPtr<IScanResult> mScanResult;

private:
    static const String KEY_DETAILEDSTATE;
    static const String KEY_WIFIINFO;
    static const String KEY_SCANRESULT;
    static const String KEY_CONFIG;

    static const AutoPtr< ArrayOf<Int32> > STATE_SECURED;
    static const AutoPtr< ArrayOf<Int32> > STATE_NONE;

    static AutoPtr< ArrayOf<Int32> > wifi_signal_attributes;

    AutoPtr<IWifiConfiguration> mConfig;

    Int32 mRssi;
    Int64 mSeen;

    AutoPtr<IWifiInfo> mInfo;
    NetworkInfoDetailedState mState;

    static const Int32 VISIBILITY_MAX_AGE_IN_MILLI;
    static const Int32 VISIBILITY_OUTDATED_AGE_IN_MILLI;
    static const Int32 SECOND_TO_MILLI;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_ACCESSPOINT_H__
