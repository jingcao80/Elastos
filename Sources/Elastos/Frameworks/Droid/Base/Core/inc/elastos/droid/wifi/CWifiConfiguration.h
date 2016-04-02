
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATION_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATION_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Wifi_CWifiConfiguration.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IpConfigurationIpAssignment;
using Elastos::Droid::Net::IpConfigurationProxySettings;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Core::IComparator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiConfiguration)
    , public Object
    , public IWifiConfiguration
    , public IParcelable
{
private:
    class InnerComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        InnerComparator(
            /* [in] */ CWifiConfiguration* owner);

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        CWifiConfiguration* mOnwer;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiConfiguration* source);

    static CARAPI GetWepKeyVarName(
        /* [in] */ Int32 index,
        /* [out] */ String* wepKeyVarName);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkId);

    CARAPI SetNetworkId(
        /* [in] */ Int32 networkId);

    CARAPI GetStatus(
        /* [out] */ Int32* status);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    CARAPI GetDirty(
        /* [out] */ Boolean* result);

    CARAPI SetDirty(
        /* [in] */ Boolean dirty);

    CARAPI GetDisableReason(
        /* [out] */ Int32* disableReason);

    CARAPI SetDisableReason(
        /* [in] */ Int32 disableReason);

    CARAPI GetSSID(
        /* [out] */ String* SSID);

    CARAPI SetSSID(
        /* [in] */ const String& SSID);

    CARAPI GetBSSID(
        /* [out] */ String* BSSID);

    CARAPI SetBSSID(
        /* [in] */ const String& BSSID);

    CARAPI GetFQDN(
        /* [out] */ String* result);

    CARAPI SetFQDN(
        /* [in] */ const String& FQDN);

    CARAPI GetNaiRealm(
        /* [out] */ String* result);

    CARAPI SetNaiRealm(
        /* [in] */ const String& naiRealm);

    CARAPI GetPreSharedKey(
        /* [out] */ String* preSharedKey);

    CARAPI SetPreSharedKey(
        /* [in] */ const String& preSharedKey);

    CARAPI GetWepKeys(
        /* [out, callee] */ ArrayOf<String>** wepKeys);

    CARAPI SetWepKeys(
        /* [in] */ ArrayOf<String>* wepKeys);

    CARAPI GetWepTxKeyIndex(
        /* [out] */ Int32* wepTxKeyIndex);

    CARAPI SetWepTxKeyIndex(
        /* [in] */ Int32 wepTxKeyIndex);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    CARAPI GetHiddenSSID(
        /* [out] */ Boolean* hiddenSSID);

    CARAPI SetHiddenSSID(
        /* [in] */ Boolean hiddenSSID);

    CARAPI GetRequirePMF(
        /* [out] */ Boolean* result);

    CARAPI SetRequirePMF(
        /* [in] */ Boolean requirePMF);

    CARAPI GetUpdateIdentifier(
        /* [out] */ String* result);

    CARAPI SetUpdateIdentifier(
        /* [in] */ const String& updateIdentifier);

    CARAPI GetAllowedKeyManagement(
        /* [out] */ IBitSet** allowedKeyManagement);

    CARAPI SetAllowedKeyManagement(
        /* [in] */ IBitSet* allowedKeyManagement);

    CARAPI GetAllowedProtocols(
        /* [out] */ IBitSet** allowedProtocols);

    CARAPI SetAllowedProtocols(
        /* [in] */ IBitSet* allowedProtocols);

    CARAPI GetAllowedAuthAlgorithms(
        /* [out] */ IBitSet** allowedAuthAlgorithms);

    CARAPI SetAllowedAuthAlgorithms(
        /* [in] */ IBitSet* allowedAuthAlgorithms);

    CARAPI GetAllowedPairwiseCiphers(
        /* [out] */ IBitSet** allowedPairwiseCiphers);

    CARAPI SetAllowedPairwiseCiphers(
        /* [in] */ IBitSet* allowedPairwiseCiphers);

    CARAPI GetAllowedGroupCiphers(
        /* [out] */ IBitSet** allowedGroupCiphers);

    CARAPI SetAllowedGroupCiphers(
        /* [in] */ IBitSet* allowedGroupCiphers);

    CARAPI GetEnterpriseConfig(
        /* [out] */ IWifiEnterpriseConfig** result);

    CARAPI SetEnterpriseConfig(
        /* [in] */ IWifiEnterpriseConfig* enterpriseConfig);

    CARAPI GetDhcpServer(
        /* [out] */ String* result);

    CARAPI SetDhcpServer(
        /* [in] */ const String& dhcpServer);

    CARAPI GetDefaultGwMacAddress(
        /* [out] */ String* result);

    CARAPI SetDefaultGwMacAddress(
        /* [in] */ const String& defaultGwMacAddress);

    CARAPI GetLastFailure(
        /* [out] */ String* result);

    CARAPI SetLastFailure(
        /* [in] */ const String& lastFailure);

    CARAPI GetNoInternetAccess(
        /* [out] */ Boolean* result);

    CARAPI SetNoInternetAccess(
        /* [in] */ Boolean noInternetAccess);

    CARAPI GetCreatorUid(
        /* [out] */ Int32* result);

    CARAPI SetCreatorUid(
        /* [in] */ Int32 creatorUid);

    CARAPI GetLastConnectUid(
        /* [out] */ Int32* result);

    CARAPI SetLastConnectUid(
        /* [in] */ Int32 lastConnectUid);

    CARAPI GetLastUpdateUid(
        /* [out] */ Int32* result);

    CARAPI SetLastUpdateUid(
        /* [in] */ Int32 lastUpdateUid);

    CARAPI GetAutoJoinBSSID(
        /* [out] */ String* result);

    CARAPI SetAutoJoinBSSID(
        /* [in] */ const String& autoJoinBSSID);

    CARAPI GetScanResultCache(
        /* [out] */ IHashMap** result);

    CARAPI SetScanResultCache(
        /* [in] */ IHashMap* scanResultCache);

    CARAPI GetVisibility(
        /* [out] */ IWifiConfigurationVisibility** result);

    CARAPI SetVisibility(
        /* [in] */ IWifiConfigurationVisibility* visibility);

    CARAPI GetAutoJoinStatus(
        /* [out] */ Int32* result);

    CARAPI GetNumConnectionFailures(
        /* [out] */ Int32* result);

    CARAPI SetNumConnectionFailures(
        /* [in] */ Int32 numConnectionFailures);

    CARAPI GetNumIpConfigFailures(
        /* [out] */ Int32* result);

    CARAPI SetNumIpConfigFailures(
        /* [in] */ Int32 numIpConfigFailures);

    CARAPI GetNumAuthFailures(
        /* [out] */ Int32* result);

    CARAPI SetNumAuthFailures(
        /* [in] */ Int32 numAuthFailures);

    CARAPI GetBlackListTimestamp(
        /* [out] */ Int64* result);

    CARAPI SetBlackListTimestamp(
        /* [in] */ Int64 blackListTimestamp);

    CARAPI GetLastConnected(
        /* [out] */ Int64* result);

    CARAPI SetLastConnected(
        /* [in] */ Int64 lastConnected);

    CARAPI GetLastConnectionFailure(
        /* [out] */ Int64* result);

    CARAPI SetLastConnectionFailure(
        /* [in] */ Int64 lastConnectionFailure);

    CARAPI GetLastDisconnected(
        /* [out] */ Int64* result);

    CARAPI SetLastDisconnected(
        /* [in] */ Int64 lastDisconnected);

    CARAPI GetSelfAdded(
        /* [out] */ Boolean* result);

    CARAPI SetSelfAdded(
        /* [in] */ Boolean selfAdded);

    CARAPI GetDidSelfAdd(
        /* [out] */ Boolean* result);

    CARAPI SetDidSelfAdd(
        /* [in] */ Boolean didSelfAdd);

    CARAPI GetPeerWifiConfiguration(
        /* [out] */ String* result);

    CARAPI SetPeerWifiConfiguration(
        /* [in] */ const String& peerWifiConfiguration);

    CARAPI GetEphemeral(
        /* [out] */ Boolean* result);

    CARAPI SetEphemeral(
        /* [in] */ Boolean ephemeral);

    CARAPI GetAutoJoinBailedDueToLowRssi(
        /* [out] */ Boolean* result);

    CARAPI SetAutoJoinBailedDueToLowRssi(
        /* [in] */ Boolean autoJoinBailedDueToLowRssi);

    CARAPI GetAutoJoinUseAggressiveJoinAttemptThreshold(
        /* [out] */ Int32* result);

    CARAPI SetAutoJoinUseAggressiveJoinAttemptThreshold(
        /* [in] */ Int32 autoJoinUseAggressiveJoinAttemptThreshold);

    CARAPI GetNumScorerOverride(
        /* [out] */ Int32* result);

    CARAPI SetNumScorerOverride(
        /* [in] */ Int32 numScorerOverride);

    CARAPI GetNumScorerOverrideAndSwitchedNetwork(
        /* [out] */ Int32* result);

    CARAPI SetNumScorerOverrideAndSwitchedNetwork(
        /* [in] */ Int32 numScorerOverrideAndSwitchedNetwork);

    CARAPI GetNumAssociation(
        /* [out] */ Int32* result);

    CARAPI SetNumAssociation(
        /* [in] */ Int32 numAssociation);

    CARAPI GetNumUserTriggeredWifiDisableLowRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumUserTriggeredWifiDisableLowRSSI(
        /* [in] */ Int32 numUserTriggeredWifiDisableLowRSSI);

    CARAPI GetNumUserTriggeredWifiDisableBadRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumUserTriggeredWifiDisableBadRSSI(
        /* [in] */ Int32 numUserTriggeredWifiDisableBadRSSI);

    CARAPI GetNumUserTriggeredWifiDisableNotHighRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumUserTriggeredWifiDisableNotHighRSSI(
        /* [in] */ Int32 numUserTriggeredWifiDisableNotHighRSSI);

    CARAPI GetNumTicksAtLowRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumTicksAtLowRSSI(
        /* [in] */ Int32 numTicksAtLowRSSI);

    CARAPI GetNumTicksAtBadRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumTicksAtBadRSSI(
        /* [in] */ Int32 numTicksAtBadRSSI);

    CARAPI GetNumTicksAtNotHighRSSI(
        /* [out] */ Int32* result);

    CARAPI SetNumTicksAtNotHighRSSI(
        /* [in] */ Int32 numTicksAtNotHighRSSI);

    CARAPI GetNumUserTriggeredJoinAttempts(
        /* [out] */ Int32* result);

    CARAPI SetNumUserTriggeredJoinAttempts(
        /* [in] */ Int32 numUserTriggeredJoinAttempts);

    CARAPI GetConnectChoices(
        /* [out] */ IHashMap** result);

    CARAPI SetConnectChoices(
        /* [in] */ IHashMap* connectChoices);

    CARAPI GetLinkedConfigurations(
        /* [out] */ IHashMap** result);

    CARAPI SetLinkedConfigurations(
        /* [in] */ IHashMap* linkedConfigurations);

    CARAPI GetWepKeyVarNames(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetVisibility(
        /* [in] */ Int64 age,
        /* [out] */ IWifiConfigurationVisibility** result);

    CARAPI IsValid(
        /* [out] */ Boolean* result);

    CARAPI IsLinked(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Boolean* result);

    CARAPI LastSeen(
        /* [out] */ IScanResult** result);

    CARAPI SetAutoJoinStatus(
        /* [in] */ Int32 status);

    CARAPI GetKeyIdForCredentials(
        /* [in] */ IWifiConfiguration* current,
        /* [out] */ String* result);

    CARAPI ConfigKey(
        /* [in] */ Boolean allowCached,
        /* [out] */ String* result);

    CARAPI ConfigKey(
        /* [out] */ String* result);

    CARAPI GetIpConfiguration(
        /* [out] */ IIpConfiguration** result);

    CARAPI SetIpConfiguration(
        /* [in] */ IIpConfiguration* ipConfiguration);

    CARAPI GetStaticIpConfiguration(
        /* [out] */ IStaticIpConfiguration** result);

    CARAPI SetStaticIpConfiguration(
        /* [in] */ IStaticIpConfiguration* staticIpConfiguration);

    CARAPI GetHttpProxy(
        /* [out] */ IProxyInfo** result);

    CARAPI SetHttpProxy(
        /* [in] */ IProxyInfo* httpProxy);

    CARAPI SetProxy(
        /* [in] */ IpConfigurationProxySettings settings,
        /* [in] */ IProxyInfo* proxy);

    CARAPI GetIpAssignment(
        /* [out] */ IpConfigurationIpAssignment* ipAssignment);

    CARAPI SetIpAssignment(
        /* [in] */ IpConfigurationIpAssignment ipAssignment);

    CARAPI GetProxySettings(
        /* [out] */ IpConfigurationProxySettings* proxySettings);

    CARAPI SetProxySettings(
        /* [in] */ IpConfigurationProxySettings proxySettings);

    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** linkProperties);

    CARAPI SetLinkProperties(
        /* [in] */ ILinkProperties* linkProperties);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI GetPrintableSsid(
        /* [out] */ String* printableSsid);

    CARAPI GetAuthType(
        /* [out] */ Int32* authType);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI_(String) ConfigKey(
        /* [in] */ IScanResult* result);

private:
    static CARAPI_(AutoPtr<IBitSet>) ReadBitSet(
        /* [in] */ IParcel* src);

    static CARAPI_(void) WriteBitSet(
        /* [in] */ IParcel* dest,
        /* [in] */ IBitSet* set);

    CARAPI_(AutoPtr<IArrayList>) SortScanResults();

    CARAPI_(String) TrimStringForKeyId(
        /* [in] */ const String& string);

private:
    Int32 mNetworkId;
    Int32 mStatus;
    Boolean mDirty;
    Int32 mDisableReason;
    String mSSID;
    String mBSSID;
    String mFQDN;
    String mPreSharedKey;
    AutoPtr< ArrayOf<String> > mWepKeys;
    Int32 mWepTxKeyIndex;
    Int32 mPriority;
    Boolean mHiddenSSID;
    Boolean mRequirePMF;
    String mUpdateIdentifier;

    AutoPtr<IBitSet> mAllowedKeyManagement;
    AutoPtr<IBitSet> mAllowedProtocols;
    AutoPtr<IBitSet> mAllowedAuthAlgorithms;
    AutoPtr<IBitSet> mAllowedPairwiseCiphers;
    AutoPtr<IBitSet> mAllowedGroupCiphers;
    AutoPtr<IWifiEnterpriseConfig> mEnterpriseConfig;
    AutoPtr<IIpConfiguration> mIpConfiguration;

    String mDhcpServer;
    String mDefaultGwMacAddress;

    String mLastFailure;
    Boolean mNoInternetAccess;
    Int32 mCreatorUid;
    Int32 mLastConnectUid;
    Int32 mLastUpdateUid;
    String mAutoJoinBSSID;
    AutoPtr<IHashMap> mScanResultCache;

    AutoPtr<IWifiConfigurationVisibility> mVisibility;

    IpConfigurationIpAssignment mIpAssignment;
    IpConfigurationProxySettings mProxySettings;
    AutoPtr<ILinkProperties> mLinkProperties;

    Int32 mAutoJoinStatus;
    Int32 mNumConnectionFailures;
    Int32 mNumIpConfigFailures;
    Int32 mNumAuthFailures;
    Int64 mBlackListTimestamp;
    Int64 mLastConnected;
    Int64 mLastConnectionFailure;
    Int64 mLastDisconnected;
    Boolean mSelfAdded;
    Boolean mDidSelfAdd;
    String mPeerWifiConfiguration;
    String mNaiRealm;
    Boolean mEphemeral;
    Boolean mAutoJoinBailedDueToLowRssi;
    Int32 mAutoJoinUseAggressiveJoinAttemptThreshold;
    Int32 mNumScorerOverride;
    Int32 mNumScorerOverrideAndSwitchedNetwork;
    Int32 mNumAssociation;
    Int32 mNumUserTriggeredWifiDisableLowRSSI;
    Int32 mNumUserTriggeredWifiDisableBadRSSI;
    Int32 mNumUserTriggeredWifiDisableNotHighRSSI;
    Int32 mNumTicksAtLowRSSI;
    Int32 mNumTicksAtBadRSSI;
    Int32 mNumTicksAtNotHighRSSI;
    Int32 mNumUserTriggeredJoinAttempts;
    AutoPtr<IHashMap> mConnectChoices;
    AutoPtr<IHashMap>  mLinkedConfigurations;
    String mCachedConfigKey;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATION_H__
