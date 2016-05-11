
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/wifi/CWifiConfigurationKeyMgmt.h"
#include "elastos/droid/wifi/CWifiConfigurationProtocol.h"
#include "elastos/droid/wifi/CWifiConfigurationPairwiseCipher.h"
#include "elastos/droid/wifi/CWifiConfigurationAuthAlgorithm.h"
#include "elastos/droid/wifi/CWifiConfigurationGroupCipher.h"
#include "elastos/droid/wifi/CWifiConfiguration.h"
#include "elastos/droid/wifi/CWifiConfigurationVisibility.h"
#include "elastos/droid/wifi/CWifiEnterpriseConfig.h"
#include "elastos/droid/wifi/CWifiSsid.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CIpConfiguration.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloneable;
using Elastos::Core::IInteger32;
using Elastos::Core::IString;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CBitSet;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::IMap;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Wifi {

//================================================================
//               CWifiConfiguration::InnerComparator
//================================================================

CAR_INTERFACE_IMPL(CWifiConfiguration::InnerComparator, Object, IComparator)

CWifiConfiguration::InnerComparator::InnerComparator(
    /* [in] */ CWifiConfiguration* owner)
    : mOnwer(owner)
{
}

ECode CWifiConfiguration::InnerComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IScanResult> a = IScanResult::Probe(lhs);
    AutoPtr<IScanResult> b = IScanResult::Probe(rhs);
    Int32 aNumIpConfigFailures, bNumIpConfigFailures;
    a->GetNumIpConfigFailures(&aNumIpConfigFailures);
    b->GetNumIpConfigFailures(&bNumIpConfigFailures);
    if (aNumIpConfigFailures > bNumIpConfigFailures) {
        *result = 1;
        return NOERROR;
    }

    if (aNumIpConfigFailures < bNumIpConfigFailures) {
        *result = -1;
        return NOERROR;
    }

    Int64 aSeen, bSeen;
    a->GetSeen(&aSeen);
    b->GetSeen(&bSeen);
    if (aSeen > bSeen) {
        *result = -1;
        return NOERROR;
    }

    if (aSeen < bSeen) {
        *result = 1;
        return NOERROR;
    }

    Int32 aLevel, bLevel;
    a->GetLevel(&aLevel);
    b->GetLevel(&bLevel);
    if (aLevel > bLevel) {
        *result = -1;
        return NOERROR;
    }

    if (aLevel < bLevel) {
        *result = 1;
        return NOERROR;
    }

    String aBSSID, bBSSID;
    a->GetBSSID(&aBSSID);
    b->GetBSSID(&bBSSID);
    *result = aBSSID.Compare(bBSSID);

    return NOERROR;
}

//================================================================
//                      CWifiConfiguration
//================================================================

CAR_INTERFACE_IMPL_2(CWifiConfiguration, Object, IWifiConfiguration, IParcelable)

CAR_OBJECT_IMPL(CWifiConfiguration)

ECode CWifiConfiguration::constructor()
{
    mNetworkId = IWifiConfiguration::INVALID_NETWORK_ID;
    mPriority = 0;
    mHiddenSSID = FALSE;
    mIsIBSS = FALSE;
    mFrequency = 0;
    mDisableReason = IWifiConfiguration::DISABLED_UNKNOWN_REASON;
    CBitSet::New((IBitSet**)&mAllowedKeyManagement);
    CBitSet::New((IBitSet**)&mAllowedProtocols);
    CBitSet::New((IBitSet**)&mAllowedAuthAlgorithms);
    CBitSet::New((IBitSet**)&mAllowedPairwiseCiphers);
    CBitSet::New((IBitSet**)&mAllowedGroupCiphers);
    mWepKeys = ArrayOf<String>::Alloc(4);
    for (Int32 i = 0; i < mWepKeys->GetLength(); i++) {
        (*mWepKeys)[i] = NULL;
    }

    CWifiEnterpriseConfig::New((IWifiEnterpriseConfig**)&mEnterpriseConfig);
    mAutoJoinStatus = AUTO_JOIN_ENABLED;
    mSelfAdded = FALSE;
    mDidSelfAdd = FALSE;
    mEphemeral = FALSE;
    mNoInternetAccess = FALSE;

    mStatus = 0;
    mDirty = FALSE;
    mWepTxKeyIndex = 0;
    mRequirePMF = FALSE;
    mCreatorUid = 0;
    mLastConnectUid = 0;
    mLastUpdateUid = 0;

    mNumConnectionFailures = 0;
    mNumIpConfigFailures = 0;
    mNumAuthFailures = 0;
    mBlackListTimestamp = 0;
    mLastConnected = 0;
    mLastConnectionFailure = 0;
    mLastDisconnected = 0;
    mAutoJoinBailedDueToLowRssi = FALSE;
    mAutoJoinUseAggressiveJoinAttemptThreshold = 0;
    mNumScorerOverride = 0;
    mNumScorerOverrideAndSwitchedNetwork = 0;
    mNumAssociation = 0;
    mNumUserTriggeredWifiDisableLowRSSI = 0;
    mNumUserTriggeredWifiDisableBadRSSI = 0;
    mNumUserTriggeredWifiDisableNotHighRSSI = 0;
    mNumTicksAtLowRSSI = 0;
    mNumTicksAtBadRSSI = 0;
    mNumTicksAtNotHighRSSI = 0;
    mNumUserTriggeredJoinAttempts = 0;
    mDuplicateNetwork = FALSE;

    return CIpConfiguration::New((IIpConfiguration**)&mIpConfiguration);
}

ECode CWifiConfiguration::constructor(
    /* [in] */ IWifiConfiguration* source)
{
    if (source != NULL) {
        source->GetNetworkId(&mNetworkId);
        source->GetStatus(&mStatus);
        source->GetDisableReason(&mDisableReason);
        source->GetSSID(&mSSID);
        source->GetBSSID(&mBSSID);
        source->GetFQDN(&mFQDN);
        source->GetNaiRealm(&mNaiRealm);
        source->GetPreSharedKey(&mPreSharedKey);

        mWepKeys = ArrayOf<String>::Alloc(4);
        AutoPtr< ArrayOf<String> > wepKeys;
        source->GetWepKeys((ArrayOf<String>**)&wepKeys);
        for (Int32 i = 0; i < wepKeys->GetLength(); i++) {
            (*mWepKeys)[i] = (*wepKeys)[i];
        }

        source->GetWepTxKeyIndex(&mWepTxKeyIndex);
        source->GetPriority(&mPriority);
        source->GetHiddenSSID(&mHiddenSSID);
        source->GetIsIBSS(&mIsIBSS);
        source->GetFrequency(&mFrequency);
        AutoPtr<IBitSet> allowedKeyManagement;
        AutoPtr<IBitSet> allowedProtocols;
        AutoPtr<IBitSet> allowedAuthAlgorithms;
        AutoPtr<IBitSet> allowedPairwiseCiphers;
        AutoPtr<IBitSet> allowedGroupCiphers;

        source->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
        AutoPtr<IInterface> obj;
        ICloneable::Probe(allowedKeyManagement)->Clone((IInterface**)&obj);
        mAllowedKeyManagement = IBitSet::Probe(obj);

        obj = NULL;
        source->GetAllowedKeyManagement((IBitSet**)&allowedProtocols);
        ICloneable::Probe(allowedProtocols)->Clone((IInterface**)&obj);
        mAllowedProtocols = IBitSet::Probe(obj);

        obj = NULL;
        source->GetAllowedKeyManagement((IBitSet**)&allowedAuthAlgorithms);
        ICloneable::Probe(allowedAuthAlgorithms)->Clone((IInterface**)&obj);
        mAllowedAuthAlgorithms = IBitSet::Probe(obj);

        obj = NULL;
        source->GetAllowedKeyManagement((IBitSet**)&allowedPairwiseCiphers);
        ICloneable::Probe(allowedPairwiseCiphers)->Clone((IInterface**)&obj);
        mAllowedPairwiseCiphers = IBitSet::Probe(obj);

        obj = NULL;
        source->GetAllowedKeyManagement((IBitSet**)&allowedGroupCiphers);
        ICloneable::Probe(allowedGroupCiphers)->Clone((IInterface**)&obj);
        mAllowedGroupCiphers = IBitSet::Probe(obj);

        AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
        source->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
        CWifiEnterpriseConfig::New(enterpriseConfig, (IWifiEnterpriseConfig**)&mEnterpriseConfig);

        source->GetDefaultGwMacAddress(&mDefaultGwMacAddress);

        AutoPtr<IIpConfiguration> ipConfiguration;
        source->GetIpConfiguration((IIpConfiguration**)&ipConfiguration);
        CIpConfiguration::New(ipConfiguration, (IIpConfiguration**)&mIpConfiguration);

        AutoPtr<IHashMap> scanResultCache;
        source->GetScanResultCache((IHashMap**)&scanResultCache);
        Int32 size;
        if ((scanResultCache != NULL) && (scanResultCache->GetSize(&size), size > 0)) {
            CHashMap::New((IHashMap**)&mScanResultCache);
            mScanResultCache->PutAll(IMap::Probe(scanResultCache));
        }

        AutoPtr<IHashMap> connectChoices;
        source->GetConnectChoices((IHashMap**)&connectChoices);
        if (connectChoices != NULL && (connectChoices->GetSize(&size), size > 0)) {
            CHashMap::New((IHashMap**)&mConnectChoices);
            mConnectChoices->PutAll(IMap::Probe(connectChoices));
        }

        AutoPtr<IHashMap>  linkedConfigurations;
        source->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
        if ((linkedConfigurations != NULL)
                && (linkedConfigurations->GetSize(&size), size > 0)) {
            CHashMap::New((IHashMap**)&mLinkedConfigurations);
            mLinkedConfigurations->PutAll(IMap::Probe(linkedConfigurations));
        }
        mCachedConfigKey = NULL; //force null configKey
        source->GetAutoJoinStatus(&mAutoJoinStatus);
        source->GetSelfAdded(&mSelfAdded);
        source->GetNoInternetAccess(&mNoInternetAccess);
        AutoPtr<IWifiConfigurationVisibility> visibility;
        source->GetVisibility((IWifiConfigurationVisibility**)&visibility);
        if (visibility != NULL) {
            CWifiConfigurationVisibility::New(visibility, (IWifiConfigurationVisibility**)&mVisibility);
        }

        source->GetLastFailure(&mLastFailure);
        source->GetDidSelfAdd(&mDidSelfAdd);
        source->GetLastConnectUid(&mLastConnectUid);
        source->GetLastUpdateUid(&mLastUpdateUid);
        source->GetCreatorUid(&mCreatorUid);
        source->GetPeerWifiConfiguration(&mPeerWifiConfiguration);
        source->GetBlackListTimestamp(&mBlackListTimestamp);
        source->GetLastConnected(&mLastConnected);
        source->GetLastDisconnected(&mLastDisconnected);
        source->GetLastConnectionFailure(&mLastConnectionFailure);
        source->GetNumConnectionFailures(&mNumConnectionFailures);
        source->GetNumIpConfigFailures(&mNumIpConfigFailures);
        source->GetNumAuthFailures(&mNumAuthFailures);
        source->GetNumScorerOverride(&mNumScorerOverride);
        source->GetNumScorerOverrideAndSwitchedNetwork(&mNumScorerOverrideAndSwitchedNetwork);
        source->GetNumAssociation(&mNumAssociation);
        source->GetNumUserTriggeredWifiDisableLowRSSI(&mNumUserTriggeredWifiDisableLowRSSI);
        source->GetNumUserTriggeredWifiDisableBadRSSI(&mNumUserTriggeredWifiDisableBadRSSI);
        source->GetNumUserTriggeredWifiDisableNotHighRSSI(&mNumUserTriggeredWifiDisableNotHighRSSI);
        source->GetNumTicksAtLowRSSI(&mNumTicksAtLowRSSI);
        source->GetNumTicksAtBadRSSI(&mNumTicksAtBadRSSI);
        source->GetNumTicksAtNotHighRSSI(&mNumTicksAtNotHighRSSI);
        source->GetNumUserTriggeredJoinAttempts(&mNumUserTriggeredJoinAttempts);
        source->GetAutoJoinBSSID(&mAutoJoinBSSID);
        source->GetAutoJoinUseAggressiveJoinAttemptThreshold(&mAutoJoinUseAggressiveJoinAttemptThreshold);
        source->GetAutoJoinBailedDueToLowRssi(&mAutoJoinBailedDueToLowRssi);
        source->GetDirty(&mDirty);
        source->GetDuplicateNetwork(&mDuplicateNetwork);
    }

    return NOERROR;
}

ECode CWifiConfiguration::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    StringBuilder sbuf;
    if (mStatus == IWifiConfigurationStatus::CURRENT) {
        sbuf.Append("* ");
    }
    else if (mStatus == IWifiConfigurationStatus::DISABLED) {
        sbuf.Append("- DSBLE ");
    }

    sbuf.Append("ID: ");
    sbuf.Append(mNetworkId);
    sbuf.Append(" SSID: ");
    sbuf.Append(mSSID);
    sbuf.Append(" BSSID: ");
    sbuf.Append(mBSSID);
    sbuf.Append(" FQDN: ");
    sbuf.Append(mFQDN);
    sbuf.Append(" REALM: ");
    sbuf.Append(mNaiRealm);
    sbuf.Append(" PRIO: ");
    sbuf.Append(mPriority);
    sbuf.AppendChar('\n');

    if (mNumConnectionFailures > 0) {
        sbuf.Append(" numConnectFailures ");
        sbuf.Append(mNumConnectionFailures);
        sbuf.Append("\n");
    }

    if (mNumIpConfigFailures > 0) {
        sbuf.Append(" numIpConfigFailures ");
        sbuf.Append(mNumIpConfigFailures);
        sbuf.Append("\n");
    }

    if (mNumAuthFailures > 0) {
        sbuf.Append(" numAuthFailures ");
        sbuf.Append(mNumAuthFailures);
        sbuf.Append("\n");
    }

    if (mAutoJoinStatus > 0) {
        sbuf.Append(" autoJoinStatus ");
        sbuf.Append(mAutoJoinStatus);
        sbuf.Append("\n");
    }

    if (mDisableReason > 0) {
        sbuf.Append(" disableReason ");
        sbuf.Append(mDisableReason);
        sbuf.Append("\n");
    }

    if (mNumAssociation > 0) {
        sbuf.Append(" numAssociation ");
        sbuf.Append(mNumAssociation);
        sbuf.Append("\n");
    }

    if (mDidSelfAdd) sbuf.Append(" didSelfAdd");
    if (mSelfAdded) sbuf.Append(" selfAdded");
    if (mNoInternetAccess) sbuf.Append(" noInternetAccess");
    if (mDidSelfAdd || mSelfAdded || mNoInternetAccess) {
        sbuf.Append("\n");
    }
    sbuf.Append(" KeyMgmt:");
    Int32 size;
    mAllowedKeyManagement->GetSize(&size);
    for (Int32 k = 0; size; k++) {
        Boolean bFlag;
        mAllowedKeyManagement->Get(k, &bFlag);
        if (bFlag) {
            sbuf.Append(" ");
            AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
            CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
            AutoPtr< ArrayOf<String> > strings;
            keyMgmt->GetStrings((ArrayOf<String>**)&strings);
            if (k < strings->GetLength()) {
                sbuf.Append((*strings)[k]);
            }
            else {
                sbuf.Append("??");
            }
        }
    }

    sbuf.Append(" Protocols:");
    mAllowedProtocols->GetSize(&size);
    for (Int32 p = 0; p < size; p++) {
        Boolean bFlag;
        mAllowedProtocols->Get(p, &bFlag);
        if (bFlag) {
            sbuf.Append(" ");
            AutoPtr<IWifiConfigurationProtocol> protocol;
            CWifiConfigurationProtocol::AcquireSingleton((IWifiConfigurationProtocol**)&protocol);
            AutoPtr< ArrayOf<String> > strings;
            protocol->GetStrings((ArrayOf<String>**)&strings);
            if (p < strings->GetLength()) {
                sbuf.Append((*strings)[p]);
            }
            else {
                sbuf.Append("??");
            }
        }
    }

    sbuf.AppendChar('\n');
    sbuf.Append(" AuthAlgorithms:");

    mAllowedAuthAlgorithms->GetSize(&size);
    for (Int32 a = 0; a < size; a++) {
        Boolean bFlag;
        mAllowedAuthAlgorithms->Get(a, &bFlag);
        if (bFlag) {
            sbuf.Append(" ");
            AutoPtr<IWifiConfigurationAuthAlgorithm> algorithm;
            CWifiConfigurationAuthAlgorithm::AcquireSingleton((IWifiConfigurationAuthAlgorithm**)&algorithm);
            AutoPtr< ArrayOf<String> > strings;
            algorithm->GetStrings((ArrayOf<String>**)&strings);
            if (a < strings->GetLength()) {
                sbuf.Append((*strings)[a]);
            }
            else {
                sbuf.Append("??");
            }
        }
    }

    sbuf.AppendChar('\n');
    sbuf.Append(" PairwiseCiphers:");

    mAllowedPairwiseCiphers->GetSize(&size);
    for (Int32 pc = 0; pc < size; pc++) {
        Boolean bFlag;
        mAllowedPairwiseCiphers->Get(pc, &bFlag);
        if (bFlag) {
            sbuf.Append(" ");
            AutoPtr<IWifiConfigurationPairwiseCipher> clipher;
            CWifiConfigurationPairwiseCipher::AcquireSingleton((IWifiConfigurationPairwiseCipher**)&clipher);
            AutoPtr< ArrayOf<String> > strings;
            clipher->GetStrings((ArrayOf<String>**)&strings);
            if (pc < strings->GetLength()) {
                sbuf.Append((*strings)[pc]);
            }
            else {
                sbuf.Append("??");
            }
        }
    }

    sbuf.AppendChar('\n');
    sbuf.Append(" GroupCiphers:");

    mAllowedGroupCiphers->GetSize(&size);
    for (Int32 gc = 0; gc < size; gc++) {
        Boolean bFlag;
        mAllowedGroupCiphers->Get(gc, &bFlag);
        if (bFlag) {
            sbuf.Append(" ");
            AutoPtr<IWifiConfigurationGroupCipher> cipher;
            CWifiConfigurationGroupCipher::AcquireSingleton((IWifiConfigurationGroupCipher**)&cipher);
            AutoPtr< ArrayOf<String> > strings;
            cipher->GetStrings((ArrayOf<String>**)&strings);
            if (gc < strings->GetLength()) {
                sbuf.Append((*strings)[gc]);
            }
            else {
                sbuf.Append("??");
            }
        }
    }

    sbuf.AppendChar('\n');
    sbuf.Append(" PSK: ");
    if (mPreSharedKey != NULL) {
        sbuf.AppendChar('*');
    }

    sbuf.Append("\nEnterprise config:\n");
    sbuf.Append(mEnterpriseConfig);

    sbuf.Append("IP config:\n");
    String ipConfigurationStr;
    IObject::Probe(mIpConfiguration)->ToString(&ipConfigurationStr);
    sbuf.Append(ipConfigurationStr);

    if (mCreatorUid != 0)  {
        sbuf.Append(" uid=");
        sbuf.Append(mCreatorUid);
    }

    if (mAutoJoinBSSID != NULL) {
        sbuf.Append(" autoJoinBSSID=");
        sbuf.Append(mAutoJoinBSSID);
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now_ms;
    system->GetCurrentTimeMillis(&now_ms);
    if (mBlackListTimestamp != 0) {
        sbuf.AppendChar('\n');
        Int64 diff = now_ms - mBlackListTimestamp;
        if (diff <= 0) {
            sbuf.Append(" blackListed since <incorrect>");
        }
        else {
            sbuf.Append(" blackListed: ");
            sbuf.Append(diff/1000);
            sbuf.Append( "sec");
        }
    }

    if (mLastConnected != 0) {
        sbuf.AppendChar('\n');
        Int64 diff = now_ms - mLastConnected;
        if (diff <= 0) {
            sbuf.Append("lastConnected since <incorrect>");
        }
        else {
            sbuf.Append("lastConnected: ");
            sbuf.Append(diff/1000);
            sbuf.Append( "sec");
        }
    }

    if (mLastConnectionFailure != 0) {
        sbuf.AppendChar('\n');
        Int64 diff = now_ms - mLastConnectionFailure;
        if (diff <= 0) {
            sbuf.Append("lastConnectionFailure since <incorrect>");
        }
        else {
            sbuf.Append("lastConnectionFailure: ");
            sbuf.Append(diff/1000);
            sbuf.Append( "sec");
        }
    }

    sbuf.AppendChar('\n');
    if (mLinkedConfigurations != NULL) {
        AutoPtr<ISet> keySet;
        mLinkedConfigurations->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> iter;
        keySet->GetIterator((IIterator**)&iter);
        Boolean bNext;
        iter->HasNext(&bNext);
        for (; bNext; iter->HasNext(&bNext)) {
            AutoPtr<IInterface> iKey;
            iter->GetNext((IInterface**)&iKey);
            String key;
            ICharSequence::Probe(iKey)->ToString(&key);
            sbuf.Append(" linked: ");
            sbuf.Append(key);
            sbuf.AppendChar('\n');
        }
    }

    if (mConnectChoices != NULL) {
        AutoPtr<ISet> keySet;
        mConnectChoices->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> iter;
        keySet->GetIterator((IIterator**)&iter);
        Boolean bNext;
        iter->HasNext(&bNext);
        for (; bNext; iter->HasNext(&bNext)) {
            AutoPtr<IInterface> iKey;
            iter->GetNext((IInterface**)&iKey);
            AutoPtr<IInterface> choice;
            mConnectChoices->Get(iKey, (IInterface**)&choice);
            if (choice != NULL) {
                sbuf.Append(" choice: ");
                String key;
                ICharSequence::Probe(iKey)->ToString(&key);
                sbuf.Append(key);
                sbuf.Append(" = ");
                Int32 value;
                IInteger32::Probe(choice)->GetValue(&value);
                sbuf.Append(value);
                sbuf.AppendChar('\n');
            }
        }
    }

    if (mScanResultCache != NULL) {
        sbuf.Append("Scan Cache:  ");
        sbuf.AppendChar('\n');
        AutoPtr<IArrayList> list = SortScanResults();
        Int32 listSize;
        list->GetSize(&listSize);
        if (listSize > 0) {
            AutoPtr<IIterable> iterable = IIterable::Probe(list);
            AutoPtr<IIterator> iter;
            iterable->GetIterator((IIterator**)&iter);
            Boolean bNext;
            iter->HasNext(&bNext);
            for (; bNext; iter->HasNext(&bNext)) {
                AutoPtr<IInterface> obj;
                iter->GetNext((IInterface**)&obj);
                IScanResult* result = IScanResult::Probe(obj);
                Int64 seen;
                result->GetSeen(&seen);
                Int64 milli = now_ms - seen;
                Int64 ageSec = 0;
                Int64 ageMin = 0;
                Int64 ageHour = 0;
                Int64 ageMilli = 0;
                Int64 ageDay = 0;
                if (now_ms > seen && seen > 0) {
                    ageMilli = milli % 1000;
                    ageSec   = (milli / 1000) % 60;
                    ageMin   = (milli / (60*1000)) % 60;
                    ageHour  = (milli / (60*60*1000)) % 24;
                    ageDay   = (milli / (24*60*60*1000));
                }

                sbuf.Append("{");
                String BSSID;
                result->GetBSSID(&BSSID);
                sbuf.Append(BSSID);
                sbuf.Append(",");
                Int32 frequency;
                result->GetFrequency(&frequency);
                sbuf.Append(frequency);
                sbuf.Append(",");
                Int32 level;
                result->GetLevel(&level);
                sbuf.Append(String().AppendFormat("%3d", level));
                Int32 autoJoinStatus;
                result->GetAutoJoinStatus(&autoJoinStatus);
                if (autoJoinStatus > 0) {
                    sbuf.Append(",st=");
                    sbuf.Append(autoJoinStatus);
                }

                if (ageSec > 0 || ageMilli > 0) {
                    sbuf.Append(String().AppendFormat(",%4d.%02d.%02d.%02d.%03dms", ageDay,
                            ageHour, ageMin, ageSec, ageMilli));
                }

                Int32 numIpConfigFailures;
                result->GetNumIpConfigFailures(&numIpConfigFailures);
                if (numIpConfigFailures > 0) {
                    sbuf.Append(",ipfail=");
                    sbuf.Append(numIpConfigFailures);
                }

                sbuf.Append("} ");
            }

            sbuf.AppendChar('\n');
        }
    }
    sbuf.Append("triggeredLow: ");
    sbuf.Append(mNumUserTriggeredWifiDisableLowRSSI);
    sbuf.Append(" triggeredBad: ");
    sbuf.Append(mNumUserTriggeredWifiDisableBadRSSI);
    sbuf.Append(" triggeredNotHigh: ");
    sbuf.Append(mNumUserTriggeredWifiDisableNotHighRSSI);
    sbuf.AppendChar('\n');
    sbuf.Append("ticksLow: ");
    sbuf.Append(mNumTicksAtLowRSSI);
    sbuf.Append(" ticksBad: ");
    sbuf.Append(mNumTicksAtBadRSSI);
    sbuf.Append(" ticksNotHigh: ");
    sbuf.Append(mNumTicksAtNotHighRSSI);
    sbuf.AppendChar('\n');
    sbuf.Append("triggeredJoin: ");
    sbuf.Append(mNumUserTriggeredJoinAttempts);
    sbuf.AppendChar('\n');
    sbuf.Append("autoJoinBailedDueToLowRssi: ");
    sbuf.Append(mAutoJoinBailedDueToLowRssi);
    sbuf.AppendChar('\n');
    sbuf.Append("autoJoinUseAggressiveJoinAttemptThreshold: ");
    sbuf.Append(mAutoJoinUseAggressiveJoinAttemptThreshold);
    sbuf.AppendChar('\n');

    return sbuf.ToString(value);
}

ECode CWifiConfiguration::GetPrintableSsid(
    /* [out] */ String* ssid)
{
    VALIDATE_NOT_NULL(ssid);

    if (mSSID.IsNull()) {
        *ssid = "";
        return NOERROR;
    }

    AutoPtr<ArrayOf<Char32> > charArray = mSSID.GetChars();
    Int32 length = charArray->GetLength();
    if (length > 2 && ((*charArray)[0] == '"') && (*charArray)[length - 1] == '"') {
        *ssid = mSSID.Substring(1, length - 1);
        return NOERROR;
    }

    /** The ascii-encoded string format is P"<ascii-encoded-string>"
     * The decoding is implemented in the supplicant for a newly configured
     * network.
     */
    if (length > 3 && ((*charArray)[0] == 'P') && ((*charArray)[1] == '"') &&
            ((*charArray)[length - 1] == '"')) {
        AutoPtr<IWifiSsid> wifiSsid;
        CWifiSsid::CreateFromAsciiEncoded(mSSID.Substring(2, length - 1),
            (IWifiSsid**)&wifiSsid);
        return IObject::Probe(wifiSsid)->ToString(ssid);
    }
    *ssid = mSSID;
    return NOERROR;
}

AutoPtr<IBitSet> CWifiConfiguration::ReadBitSet(
    /* [in] */ IParcel* src)
{
    Int32 cardinality;
    src->ReadInt32(&cardinality);

    AutoPtr<IBitSet> set;
    CBitSet::New((IBitSet**)&set);
    for (Int32 i = 0; i < cardinality; i++) {
        Int32 index;
        src->ReadInt32(&index);
        set->Set(index);
    }

    return set;
}

void CWifiConfiguration::WriteBitSet(
    /* [in] */ IParcel* dest,
    /* [in] */ IBitSet* set)
{
    Int32 nextSetBit = -1;

    Int32 number = 0;
    set->Cardinality(&number);
    dest->WriteInt32(number);

    while (set->NextSetBit(nextSetBit + 1, &nextSetBit), nextSetBit != -1) {
        dest->WriteInt32(nextSetBit);
    }
}

AutoPtr<IArrayList> CWifiConfiguration::SortScanResults()
{
    AutoPtr<ICollection> values;
    mScanResultCache->GetValues((ICollection**)&values);
    AutoPtr<IArrayList> list;
    CArrayList::New(values, (IArrayList**)&list);
    Int32 size;
    list->GetSize(&size);
    if (size != 0) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        AutoPtr<IComparator> compare = new InnerComparator(this);
        collections->Sort(IList::Probe(list), compare);
    }
    return list;
}

String CWifiConfiguration::TrimStringForKeyId(
    /* [in] */ const String& string)
{
    // Remove quotes and spaces
    String temp;
    StringUtils::Replace(string, "\"", "", &temp);
    String temp2;
    StringUtils::Replace(temp, " ", "", &temp2);
    //return string.Replace("\"", "").Replace(" ", "");
    return temp2;
}

ECode CWifiConfiguration::GetAuthType(
    /* [out] */ Int32* authType)
{
    VALIDATE_NOT_NULL(authType);
    *authType = IWifiConfigurationKeyMgmt::NONE;

    Boolean bIsValid;
    IsValid(&bIsValid);
    if (bIsValid == FALSE) {
        //throw new IllegalStateException("Invalid configuration");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean temp;
    if (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &temp), temp) {
        *authType = IWifiConfigurationKeyMgmt::WPA_PSK;
    }
    else if (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA2_PSK, &temp), temp) {
        *authType = IWifiConfigurationKeyMgmt::WPA2_PSK;
    }
    else if (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &temp), temp) {
        *authType = IWifiConfigurationKeyMgmt::WPA_EAP;
    }
    else if (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &temp), temp) {
        *authType = IWifiConfigurationKeyMgmt::IEEE8021X;
    }

    return NOERROR;
}

ECode CWifiConfiguration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNetworkId);
    source->ReadInt32(&mStatus);
    source->ReadInt32(&mDisableReason);
    source->ReadString(&mSSID);
    source->ReadString(&mBSSID);
    source->ReadString(&mPreSharedKey);
    for (Int32 i = 0; i < mWepKeys->GetLength(); ++i) {
        String key;
        source->ReadString(&key);
        (*mWepKeys)[i] = key;
    }
    source->ReadInt32(&mWepTxKeyIndex);
    source->ReadInt32(&mPriority);
    source->ReadBoolean(&mHiddenSSID);
    source->ReadBoolean(&mIsIBSS);
    source->ReadInt32(&mFrequency);

    mAllowedKeyManagement = ReadBitSet(source);
    mAllowedProtocols = ReadBitSet(source);
    mAllowedAuthAlgorithms = ReadBitSet(source);
    mAllowedPairwiseCiphers = ReadBitSet(source);
    mAllowedGroupCiphers = ReadBitSet(source);

    assert(0);
    // TODO
    // for (Int32 i = 0; i < mEnterpriseFields->GetLength(); ++i) {
    //     String value;
    //     source->ReadString(&value);
    //     ((*mEnterpriseFields)[i])->SetValue(value);
    // }

    source->ReadInt32(&mIpAssignment);
    source->ReadInt32(&mProxySettings);

    return source->ReadInterfacePtr((Handle32*)(ILinkProperties**)&mLinkProperties);
}

ECode CWifiConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mNetworkId);
    dest->WriteInt32(mStatus);
    dest->WriteInt32(mDisableReason);
    dest->WriteString(mSSID);
    dest->WriteString(mBSSID);
    dest->WriteString(mAutoJoinBSSID);
    dest->WriteString(mFQDN);
    dest->WriteString(mNaiRealm);
    dest->WriteString(mPreSharedKey);
    for (Int32 i = 0; i < mWepKeys->GetLength(); ++i) {
        dest->WriteString((*mWepKeys)[i]);
    }
    dest->WriteInt32(mWepTxKeyIndex);
    dest->WriteInt32(mPriority);
    dest->WriteInt32(mHiddenSSID ? 1 : 0);
    dest->WriteInt32(mIsIBSS ? 1 : 0);
    dest->WriteInt32(mFrequency);
    dest->WriteInt32(mRequirePMF ? 1 : 0);
    dest->WriteString(mUpdateIdentifier);

    WriteBitSet(dest, mAllowedKeyManagement);
    WriteBitSet(dest, mAllowedProtocols);
    WriteBitSet(dest, mAllowedAuthAlgorithms);
    WriteBitSet(dest, mAllowedPairwiseCiphers);
    WriteBitSet(dest, mAllowedGroupCiphers);

    assert(0);
    // TODO
    // dest->WriteParcelable(enterpriseConfig, flags);

    // dest->WriteParcelable(mIpConfiguration, flags);
    dest->WriteString(mDhcpServer);
    dest->WriteString(mDefaultGwMacAddress);
    dest->WriteInt32(mAutoJoinStatus);
    dest->WriteInt32(mSelfAdded ? 1 : 0);
    dest->WriteInt32(mDidSelfAdd ? 1 : 0);
    dest->WriteInt32(mNoInternetAccess ? 1 : 0);
    dest->WriteInt32(mCreatorUid);
    dest->WriteInt32(mLastConnectUid);
    dest->WriteInt32(mLastUpdateUid);
    dest->WriteInt64(mBlackListTimestamp);
    dest->WriteInt64(mLastConnectionFailure);
    dest->WriteInt32(mNumConnectionFailures);
    dest->WriteInt32(mNumIpConfigFailures);
    dest->WriteInt32(mNumAuthFailures);
    dest->WriteInt32(mNumScorerOverride);
    dest->WriteInt32(mNumScorerOverrideAndSwitchedNetwork);
    dest->WriteInt32(mNumAssociation);
    dest->WriteInt32(mNumUserTriggeredWifiDisableLowRSSI);
    dest->WriteInt32(mNumUserTriggeredWifiDisableBadRSSI);
    dest->WriteInt32(mNumUserTriggeredWifiDisableNotHighRSSI);
    dest->WriteInt32(mNumTicksAtLowRSSI);
    dest->WriteInt32(mNumTicksAtBadRSSI);
    dest->WriteInt32(mNumTicksAtNotHighRSSI);
    dest->WriteInt32(mNumUserTriggeredJoinAttempts);
    dest->WriteInt32(mAutoJoinUseAggressiveJoinAttemptThreshold);
    return dest->WriteInt32(mAutoJoinBailedDueToLowRssi ? 1 : 0);
}

String CWifiConfiguration::ConfigKey(
    /* [in] */ IScanResult* result)
{
    String key("\"");
    String SSID;
    result->GetSSID(&SSID);
    key += SSID;
    key += "\"";

    String capabilities;
    result->GetCapabilities(&capabilities);
    if (capabilities.Contains("WEP")) {
        key += "-WEP";
    }

    if (capabilities.Contains("PSK")) {
        key += "-";
        AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
        AutoPtr< ArrayOf<String> > strings;
        keyMgmt->GetStrings((ArrayOf<String>**)&strings);
        key += (*strings)[IWifiConfigurationKeyMgmt::WPA_PSK];
    }

    if (capabilities.Contains("EAP")) {
        key += "-";
        AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
        AutoPtr< ArrayOf<String> > strings;
        keyMgmt->GetStrings((ArrayOf<String>**)&strings);
        key += (*strings)[IWifiConfigurationKeyMgmt::WPA_EAP];
    }

    return key;
}

ECode CWifiConfiguration::GetWepKeyVarName(
    /* [in] */ Int32 index,
    /* [out] */ String* wepKeyVarName)
{
    VALIDATE_NOT_NULL(wepKeyVarName);
    if (index < 0 || index > 3) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    if (index == 0) *wepKeyVarName = "wep_key0";
    else if (index == 1) *wepKeyVarName = "wep_key1";
    else if (index == 2) *wepKeyVarName = "wep_key2";
    else if (index == 3) *wepKeyVarName = "wep_key3";
    return NOERROR;
}

ECode CWifiConfiguration::GetNetworkId(
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);
    *networkId = mNetworkId;
    return NOERROR;
}

ECode CWifiConfiguration::SetNetworkId(
    /* [in] */ Int32 networkId)
{
    mNetworkId = networkId;
    return NOERROR;
}

ECode CWifiConfiguration::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = mStatus;
    return NOERROR;
}

ECode CWifiConfiguration::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

ECode CWifiConfiguration::GetDirty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDirty;
    return NOERROR;
}

ECode CWifiConfiguration::SetDirty(
    /* [in] */ Boolean dirty)
{
    mDirty = dirty;
    return NOERROR;
}

ECode CWifiConfiguration::GetDisableReason(
    /* [out] */ Int32* disableReason)
{
    VALIDATE_NOT_NULL(disableReason);
    *disableReason = mDisableReason;
    return NOERROR;
}

ECode CWifiConfiguration::SetDisableReason(
    /* [in] */ Int32 disableReason)
{
    mDisableReason = disableReason;
    return NOERROR;
}

ECode CWifiConfiguration::GetSSID(
    /* [out] */ String* SSID)
{
    VALIDATE_NOT_NULL(SSID);
    *SSID = mSSID;
    return NOERROR;
}

ECode CWifiConfiguration::SetSSID(
    /* [in] */ const String& SSID)
{
    mSSID = SSID;
    return NOERROR;
}

ECode CWifiConfiguration::GetBSSID(
    /* [out] */ String* BSSID)
{
    VALIDATE_NOT_NULL(BSSID);
    *BSSID = mBSSID;
    return NOERROR;
}

ECode CWifiConfiguration::SetBSSID(
    /* [in] */ const String& BSSID)
{
    mBSSID = BSSID;
    return NOERROR;
}

ECode CWifiConfiguration::GetFQDN(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFQDN;
    return NOERROR;
}

ECode CWifiConfiguration::SetFQDN(
    /* [in] */ const String& FQDN)
{
    mFQDN = FQDN;
    return NOERROR;
}

ECode CWifiConfiguration::GetNaiRealm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNaiRealm;
    return NOERROR;
}

ECode CWifiConfiguration::SetNaiRealm(
    /* [in] */ const String& naiRealm)
{
    mNaiRealm = naiRealm;
    return NOERROR;
}

ECode CWifiConfiguration::GetPreSharedKey(
    /* [out] */ String* preSharedKey)
{
    VALIDATE_NOT_NULL(preSharedKey);
    *preSharedKey = mPreSharedKey;
    return NOERROR;
}

ECode CWifiConfiguration::SetPreSharedKey(
    /* [in] */ const String& preSharedKey)
{
    mPreSharedKey = preSharedKey;
    return NOERROR;
}

ECode CWifiConfiguration::GetWepKeys(
    /* [out, callee] */ ArrayOf<String>** wepKeys)
{
    VALIDATE_NOT_NULL(wepKeys);
    *wepKeys = mWepKeys;
    REFCOUNT_ADD(*wepKeys);
    return NOERROR;
}

ECode CWifiConfiguration::SetWepKeys(
    /* [in] */ ArrayOf<String>* wepKeys)
{
    mWepKeys = wepKeys;
    return NOERROR;
}

ECode CWifiConfiguration::GetWepTxKeyIndex(
    /* [out] */ Int32* wepTxKeyIndex)
{
    VALIDATE_NOT_NULL(wepTxKeyIndex);
    *wepTxKeyIndex = mWepTxKeyIndex;
    return NOERROR;
}

ECode CWifiConfiguration::SetWepTxKeyIndex(
    /* [in] */ Int32 wepTxKeyIndex)
{
    mWepTxKeyIndex = wepTxKeyIndex;
    return NOERROR;
}

ECode CWifiConfiguration::GetPriority(
    /* [out] */ Int32* priority)
{
    VALIDATE_NOT_NULL(priority);
    *priority = mPriority;
    return NOERROR;
}

ECode CWifiConfiguration::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}

ECode CWifiConfiguration::GetHiddenSSID(
    /* [out] */ Boolean* hiddenSSID)
{
    VALIDATE_NOT_NULL(hiddenSSID);
    *hiddenSSID = mHiddenSSID;
    return NOERROR;
}

ECode CWifiConfiguration::SetHiddenSSID(
    /* [in] */ Boolean hiddenSSID)
{
    mHiddenSSID = hiddenSSID;
    return NOERROR;
}

ECode CWifiConfiguration::GetRequirePMF(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRequirePMF;
    return NOERROR;
}

ECode CWifiConfiguration::SetRequirePMF(
    /* [in] */ Boolean requirePMF)
{
    mRequirePMF = requirePMF;
    return NOERROR;
}

ECode CWifiConfiguration::GetUpdateIdentifier(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUpdateIdentifier;
    return NOERROR;
}

ECode CWifiConfiguration::SetUpdateIdentifier(
    /* [in] */ const String& updateIdentifier)
{
    mUpdateIdentifier = updateIdentifier;
    return NOERROR;
}

ECode CWifiConfiguration::GetIsIBSS(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsIBSS;
    return NOERROR;
}

ECode CWifiConfiguration::SetIsIBSS(
    /* [in] */ Boolean isIBSS)
{
    mIsIBSS = isIBSS;
    return NOERROR;
}

ECode CWifiConfiguration::GetFrequency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFrequency;
    return NOERROR;
}

ECode CWifiConfiguration::SetFrequency(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    return NOERROR;
}

ECode CWifiConfiguration::GetAllowedKeyManagement(
    /* [out] */ IBitSet** allowedKeyManagement)
{
    VALIDATE_NOT_NULL(allowedKeyManagement);
    *allowedKeyManagement = mAllowedKeyManagement;
    REFCOUNT_ADD(*allowedKeyManagement);
    return NOERROR;
}

ECode CWifiConfiguration::SetAllowedKeyManagement(
    /* [in] */ IBitSet* allowedKeyManagement)
{
    mAllowedKeyManagement = allowedKeyManagement;
    return NOERROR;
}

ECode CWifiConfiguration::GetAllowedProtocols(
    /* [out] */ IBitSet** allowedProtocols)
{
    VALIDATE_NOT_NULL(allowedProtocols);
    *allowedProtocols = mAllowedProtocols;
    REFCOUNT_ADD(*allowedProtocols);
    return NOERROR;
}

ECode CWifiConfiguration::SetAllowedProtocols(
    /* [in] */ IBitSet* allowedProtocols)
{
    mAllowedProtocols = allowedProtocols;
    return NOERROR;
}

ECode CWifiConfiguration::GetAllowedAuthAlgorithms(
    /* [out] */ IBitSet** allowedAuthAlgorithms)
{
    VALIDATE_NOT_NULL(allowedAuthAlgorithms);
    *allowedAuthAlgorithms = mAllowedAuthAlgorithms;
    REFCOUNT_ADD(*allowedAuthAlgorithms);
    return NOERROR;
}

ECode CWifiConfiguration::SetAllowedAuthAlgorithms(
    /* [in] */ IBitSet* allowedAuthAlgorithms)
{
    mAllowedAuthAlgorithms = allowedAuthAlgorithms;
    return NOERROR;
}

ECode CWifiConfiguration::GetAllowedPairwiseCiphers(
    /* [out] */ IBitSet** allowedPairwiseCiphers)
{
    VALIDATE_NOT_NULL(allowedPairwiseCiphers);
    *allowedPairwiseCiphers = mAllowedPairwiseCiphers;
    REFCOUNT_ADD(*allowedPairwiseCiphers);
    return NOERROR;
}

ECode CWifiConfiguration::SetAllowedPairwiseCiphers(
    /* [in] */ IBitSet* allowedPairwiseCiphers)
{
    mAllowedPairwiseCiphers = allowedPairwiseCiphers;
    return NOERROR;
}

ECode CWifiConfiguration::GetAllowedGroupCiphers(
    /* [out] */ IBitSet** allowedGroupCiphers)
{
    VALIDATE_NOT_NULL(allowedGroupCiphers);
    *allowedGroupCiphers = mAllowedGroupCiphers;
    REFCOUNT_ADD(*allowedGroupCiphers);
    return NOERROR;
}

ECode CWifiConfiguration::SetAllowedGroupCiphers(
    /* [in] */ IBitSet* allowedGroupCiphers)
{
    mAllowedGroupCiphers = allowedGroupCiphers;
    return NOERROR;
}

ECode CWifiConfiguration::GetEnterpriseConfig(
    /* [out] */ IWifiEnterpriseConfig** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEnterpriseConfig;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetEnterpriseConfig(
    /* [in] */ IWifiEnterpriseConfig* enterpriseConfig)
{
    mEnterpriseConfig = enterpriseConfig;
    return NOERROR;
}

ECode CWifiConfiguration::GetDhcpServer(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDhcpServer;
    return NOERROR;
}

ECode CWifiConfiguration::SetDhcpServer(
    /* [in] */ const String& dhcpServer)
{
    mDhcpServer = dhcpServer;
    return NOERROR;
}

ECode CWifiConfiguration::GetDefaultGwMacAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDefaultGwMacAddress;
    return NOERROR;
}

ECode CWifiConfiguration::SetDefaultGwMacAddress(
    /* [in] */ const String& defaultGwMacAddress)
{
    mDefaultGwMacAddress = defaultGwMacAddress;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastFailure(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastFailure;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastFailure(
    /* [in] */ const String& lastFailure)
{
    mLastFailure = lastFailure;
    return NOERROR;
}

ECode CWifiConfiguration::GetNoInternetAccess(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNoInternetAccess;
    return NOERROR;
}

ECode CWifiConfiguration::SetNoInternetAccess(
    /* [in] */ Boolean noInternetAccess)
{
    mNoInternetAccess = noInternetAccess;
    return NOERROR;
}

ECode CWifiConfiguration::GetCreatorUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCreatorUid;
    return NOERROR;
}

ECode CWifiConfiguration::SetCreatorUid(
    /* [in] */ Int32 creatorUid)
{
    mCreatorUid = creatorUid;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastConnectUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastConnectUid;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastConnectUid(
    /* [in] */ Int32 lastConnectUid)
{
    mLastConnectUid = lastConnectUid;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastUpdateUid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastUpdateUid;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastUpdateUid(
    /* [in] */ Int32 lastUpdateUid)
{
    mLastUpdateUid = lastUpdateUid;
    return NOERROR;
}

ECode CWifiConfiguration::GetAutoJoinBSSID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAutoJoinBSSID;
    return NOERROR;
}

ECode CWifiConfiguration::SetAutoJoinBSSID(
    /* [in] */ const String& autoJoinBSSID)
{
    mAutoJoinBSSID = autoJoinBSSID;
    return NOERROR;
}

ECode CWifiConfiguration::GetScanResultCache(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScanResultCache;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetScanResultCache(
    /* [in] */ IHashMap* scanResultCache)
{
    mScanResultCache = scanResultCache;
    return NOERROR;
}

ECode CWifiConfiguration::GetVisibility(
    /* [out] */ IWifiConfigurationVisibility** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVisibility;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetVisibility(
    /* [in] */ IWifiConfigurationVisibility* visibility)
{
    mVisibility = visibility;
    return NOERROR;
}

ECode CWifiConfiguration::GetAutoJoinStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAutoJoinStatus;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumConnectionFailures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumConnectionFailures;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumConnectionFailures(
    /* [in] */ Int32 numConnectionFailures)
{
    mNumConnectionFailures = numConnectionFailures;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumIpConfigFailures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumIpConfigFailures;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumIpConfigFailures(
    /* [in] */ Int32 numIpConfigFailures)
{
    mNumIpConfigFailures = numIpConfigFailures;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumAuthFailures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumAuthFailures;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumAuthFailures(
    /* [in] */ Int32 numAuthFailures)
{
    mNumAuthFailures = numAuthFailures;
    return NOERROR;
}

ECode CWifiConfiguration::GetBlackListTimestamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBlackListTimestamp;
    return NOERROR;
}

ECode CWifiConfiguration::SetBlackListTimestamp(
    /* [in] */ Int64 blackListTimestamp)
{
    mBlackListTimestamp = blackListTimestamp;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastConnected(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastConnected;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastConnected(
    /* [in] */ Int64 lastConnected)
{
    mLastConnected = lastConnected;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastConnectionFailure(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastConnectionFailure;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastConnectionFailure(
    /* [in] */ Int64 lastConnectionFailure)
{
    mLastConnectionFailure = lastConnectionFailure;
    return NOERROR;
}

ECode CWifiConfiguration::GetLastDisconnected(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLastDisconnected;
    return NOERROR;
}

ECode CWifiConfiguration::SetLastDisconnected(
    /* [in] */ Int64 lastDisconnected)
{
    mLastDisconnected = lastDisconnected;
    return NOERROR;
}

ECode CWifiConfiguration::GetSelfAdded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSelfAdded;
    return NOERROR;
}

ECode CWifiConfiguration::SetSelfAdded(
    /* [in] */ Boolean selfAdded)
{
    mSelfAdded = selfAdded;
    return NOERROR;
}

ECode CWifiConfiguration::GetDidSelfAdd(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDidSelfAdd;
    return NOERROR;
}

ECode CWifiConfiguration::SetDidSelfAdd(
    /* [in] */ Boolean didSelfAdd)
{
    mDidSelfAdd = didSelfAdd;
    return NOERROR;
}

ECode CWifiConfiguration::GetPeerWifiConfiguration(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPeerWifiConfiguration;
    return NOERROR;
}

ECode CWifiConfiguration::SetPeerWifiConfiguration(
    /* [in] */ const String& peerWifiConfiguration)
{
    mPeerWifiConfiguration = peerWifiConfiguration;
    return NOERROR;
}

ECode CWifiConfiguration::GetEphemeral(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEphemeral;
    return NOERROR;
}

ECode CWifiConfiguration::SetEphemeral(
    /* [in] */ Boolean ephemeral)
{
    mEphemeral = ephemeral;
    return NOERROR;
}

ECode CWifiConfiguration::GetAutoJoinBailedDueToLowRssi(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAutoJoinBailedDueToLowRssi;
    return NOERROR;
}

ECode CWifiConfiguration::SetAutoJoinBailedDueToLowRssi(
    /* [in] */ Boolean autoJoinBailedDueToLowRssi)
{
    mAutoJoinBailedDueToLowRssi = autoJoinBailedDueToLowRssi;
    return NOERROR;
}

ECode CWifiConfiguration::GetAutoJoinUseAggressiveJoinAttemptThreshold(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAutoJoinUseAggressiveJoinAttemptThreshold;
    return NOERROR;
}

ECode CWifiConfiguration::SetAutoJoinUseAggressiveJoinAttemptThreshold(
    /* [in] */ Int32 autoJoinUseAggressiveJoinAttemptThreshold)
{
    mAutoJoinUseAggressiveJoinAttemptThreshold = autoJoinUseAggressiveJoinAttemptThreshold;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumScorerOverride(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumScorerOverride;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumScorerOverride(
    /* [in] */ Int32 numScorerOverride)
{
    mNumScorerOverride = numScorerOverride;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumScorerOverrideAndSwitchedNetwork(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumScorerOverrideAndSwitchedNetwork;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumScorerOverrideAndSwitchedNetwork(
    /* [in] */ Int32 numScorerOverrideAndSwitchedNetwork)
{
    mNumScorerOverrideAndSwitchedNetwork = numScorerOverrideAndSwitchedNetwork;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumAssociation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumAssociation;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumAssociation(
    /* [in] */ Int32 numAssociation)
{
    mNumAssociation = numAssociation;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumUserTriggeredWifiDisableLowRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUserTriggeredWifiDisableLowRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumUserTriggeredWifiDisableLowRSSI(
    /* [in] */ Int32 numUserTriggeredWifiDisableLowRSSI)
{
    mNumUserTriggeredWifiDisableLowRSSI = numUserTriggeredWifiDisableLowRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumUserTriggeredWifiDisableBadRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUserTriggeredWifiDisableBadRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumUserTriggeredWifiDisableBadRSSI(
    /* [in] */ Int32 numUserTriggeredWifiDisableBadRSSI)
{
    mNumUserTriggeredWifiDisableBadRSSI = numUserTriggeredWifiDisableBadRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumUserTriggeredWifiDisableNotHighRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUserTriggeredWifiDisableNotHighRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumUserTriggeredWifiDisableNotHighRSSI(
    /* [in] */ Int32 numUserTriggeredWifiDisableNotHighRSSI)
{
    mNumUserTriggeredWifiDisableNotHighRSSI = numUserTriggeredWifiDisableNotHighRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumTicksAtLowRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumTicksAtLowRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumTicksAtLowRSSI(
    /* [in] */ Int32 numTicksAtLowRSSI)
{
    mNumTicksAtLowRSSI = numTicksAtLowRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumTicksAtBadRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumTicksAtBadRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumTicksAtBadRSSI(
    /* [in] */ Int32 numTicksAtBadRSSI)
{
    mNumTicksAtBadRSSI = numTicksAtBadRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumTicksAtNotHighRSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumTicksAtNotHighRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumTicksAtNotHighRSSI(
    /* [in] */ Int32 numTicksAtNotHighRSSI)
{
    mNumTicksAtNotHighRSSI = numTicksAtNotHighRSSI;
    return NOERROR;
}

ECode CWifiConfiguration::GetNumUserTriggeredJoinAttempts(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUserTriggeredJoinAttempts;
    return NOERROR;
}

ECode CWifiConfiguration::SetNumUserTriggeredJoinAttempts(
    /* [in] */ Int32 numUserTriggeredJoinAttempts)
{
    mNumUserTriggeredJoinAttempts = numUserTriggeredJoinAttempts;
    return NOERROR;
}

ECode CWifiConfiguration::GetConnectChoices(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConnectChoices;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetConnectChoices(
    /* [in] */ IHashMap* connectChoices)
{
    mConnectChoices = connectChoices;
    return NOERROR;
}

ECode CWifiConfiguration::GetLinkedConfigurations(
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLinkedConfigurations;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetLinkedConfigurations(
    /* [in] */ IHashMap* linkedConfigurations)
{
    mLinkedConfigurations = linkedConfigurations;
    return NOERROR;
}

ECode CWifiConfiguration::GetDuplicateNetwork(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDuplicateNetwork;
    return NOERROR;
}

ECode CWifiConfiguration::SetDuplicateNetwork(
    /* [in] */ Boolean duplicateNetwork)
{
    mDuplicateNetwork = duplicateNetwork;
    return NOERROR;
}

ECode CWifiConfiguration::GetWepKeyVarNames(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    //"wep_key0", "wep_key1", "wep_key2", "wep_key3"
    array->Set(0, String("wep_key0"));
    array->Set(1, String("wep_key1"));
    array->Set(2, String("wep_key2"));
    array->Set(3, String("wep_key3"));

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetVisibility(
    /* [in] */ Int64 age,
    /* [out] */ IWifiConfigurationVisibility** resultOut)
{
    VALIDATE_NOT_NULL(resultOut);

    if (mScanResultCache == NULL) {
        mVisibility = NULL;
        *resultOut = NULL;
        return NOERROR;
    }

    AutoPtr<IWifiConfigurationVisibility> status;
    CWifiConfigurationVisibility::New((IWifiConfigurationVisibility**)&status);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now_ms;
    system->GetCurrentTimeMillis(&now_ms);
    AutoPtr<ICollection> values;
    mScanResultCache->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IScanResult* result = IScanResult::Probe(obj);
        Int64 seen;
        result->GetSeen(&seen);
        if (seen == 0)
            continue;

        Boolean bIs5GHz, bIs24GHz;
        result->Is5GHz(&bIs5GHz);
        if (bIs5GHz) {
            //strictly speaking: [4915, 5825]
            //number of known BSSID on 5GHz band
            Int32 num5;
            status->GetNum5(&num5);
            status->SetNum5(num5 + 1);
        }
        else if (result->Is24GHz(&bIs24GHz), bIs24GHz) {
            //strictly speaking: [2412, 2482]
            //number of known BSSID on 2.4Ghz band
            Int32 num24;
            status->GetNum24(&num24);
            status->SetNum24(num24 + 1);
        }

        if ((now_ms - seen) > age) continue;

        if (bIs5GHz) {
            Int32 level, rssi5;
            result->GetLevel(&level);
            status->GetRssi5(&rssi5);
            if (level > rssi5) {
                status->SetRssi5(level);
                status->SetAge5(seen);
                String BSSID;
                result->GetBSSID(&BSSID);
                status->SetBSSID5(BSSID);
            }
        } else if (result->Is24GHz(&bIs24GHz), bIs24GHz) {
            Int32 level, rssi24;
            result->GetLevel(&level);
            status->GetRssi24(&rssi24);
            if (level > rssi24) {
                status->SetRssi24(level);
                status->SetAge24(seen);
                String BSSID;
                result->GetBSSID(&BSSID);
                status->SetBSSID24(BSSID);
            }
        }
    }

    mVisibility = status;
    *resultOut = status;
    REFCOUNT_ADD(*resultOut)

    return NOERROR;
}

ECode CWifiConfiguration::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mAllowedKeyManagement == NULL) {
        *result = FALSE;
    }

    Int32 cardinality;
    mAllowedKeyManagement->Cardinality(&cardinality);
    if (cardinality > 1) {
        if (cardinality != 2) {
            *result = FALSE;
            return NOERROR;
        }

        Boolean bFlag1;
        mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &bFlag1);
        if (bFlag1 == FALSE) {
            return false;
        }

        Boolean bFlag2, bFlag3;
        mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &bFlag2);
        if (bFlag1 == FALSE
                && (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &bFlag3), bFlag3 == FALSE)) {
            *result = FALSE;
        }
    }

    // TODO: Add more checks
    *result = TRUE;

    return NOERROR;
}

ECode CWifiConfiguration::IsLinked(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IHashMap> linkedConfigurations;
    config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
    if (linkedConfigurations != NULL && mLinkedConfigurations != NULL) {
        String key;
        ConfigKey(&key);
        AutoPtr<ICharSequence> keyObj;
        CString::New(key, (ICharSequence**)&keyObj);

        AutoPtr<IInterface> value;
        linkedConfigurations->Get(keyObj, (IInterface**)&value);
        if (value != NULL) {
            config->ConfigKey(&key);
            keyObj = NULL;
            value = NULL;
            CString::New(key, (ICharSequence**)&keyObj);
            mLinkedConfigurations->Get(keyObj, (IInterface**)&value);
            if (value != NULL) {
                *result = TRUE;
            }
        }
    }

    return NOERROR;
}

ECode CWifiConfiguration::LastSeen(
    /* [out] */ IScanResult** resultOut)
{
    VALIDATE_NOT_NULL(resultOut);

    AutoPtr<IScanResult> mostRecent;

    if (mScanResultCache == NULL) {
        *resultOut = NULL;
        return NOERROR;
    }

    AutoPtr<ICollection> values;
    mScanResultCache->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IScanResult* result = IScanResult::Probe(obj);
        if (mostRecent == NULL) {
            Int64 seen;
            result->GetSeen(&seen);
            if (seen != 0)
               mostRecent = result;
        }
        else {
            Int64 resultSeen, mostRecentSeen;
            result->GetSeen(&resultSeen);
            mostRecent->GetSeen(&mostRecentSeen);
            if (resultSeen > mostRecentSeen) {
               mostRecent = result;
            }
        }
    }

    *resultOut = mostRecent;
    REFCOUNT_ADD(*resultOut)

    return NOERROR;
}

ECode CWifiConfiguration::SetAutoJoinStatus(
    /* [in] */ Int32 status)
{
    if (status < 0) status = 0;

    if (status == 0) {
        mBlackListTimestamp = 0;
    }
    else if (status > mAutoJoinStatus) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&mBlackListTimestamp);
    }

    if (status != mAutoJoinStatus) {
        mAutoJoinStatus = status;
        mDirty = TRUE;
    }

    return NOERROR;
}

ECode CWifiConfiguration::GetKeyIdForCredentials(
    /* [in] */ IWifiConfiguration* current,
    /* [out] */ String* result)
{
    String keyMgmt;

    // try {
        // Get current config details for fields that are not initialized
        if (TextUtils::IsEmpty(mSSID)) {
            current->GetSSID(&mSSID);
        }

        Int32 cardinality;
        mAllowedKeyManagement->Cardinality(&cardinality);
        if (cardinality == 0) {
            current->GetAllowedKeyManagement((IBitSet**)&mAllowedKeyManagement);
        }

        Boolean bFlag;
        mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &bFlag);
        if (bFlag) {
            AutoPtr<IWifiConfigurationKeyMgmt> iKeyMgmt;
            CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
            AutoPtr< ArrayOf<String> > strings;
            iKeyMgmt->GetStrings((ArrayOf<String>**)&strings);
            keyMgmt = (*strings)[IWifiConfigurationKeyMgmt::WPA_EAP];
        }

        mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &bFlag);
        if (bFlag) {
            AutoPtr<IWifiConfigurationKeyMgmt> iKeyMgmt;
            CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
            AutoPtr< ArrayOf<String> > strings;
            iKeyMgmt->GetStrings((ArrayOf<String>**)&strings);
            keyMgmt += (*strings)[IWifiConfigurationKeyMgmt::IEEE8021X];
        }

        if (TextUtils::IsEmpty(keyMgmt)) {
            //throw new IllegalStateException("Not an EAP network");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
        if (current != NULL) {
            current->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
        }
        String keyId;
        ((CWifiEnterpriseConfig*)mEnterpriseConfig.Get())->GetKeyId(enterpriseConfig, &keyId);
        *result = TrimStringForKeyId(mSSID);
        *result += "_";
        *result += keyMgmt;
        *result += "_";
        *result += TrimStringForKeyId(keyId);

        return NOERROR;
    // } catch (NullPointerException e) {
    //     throw new IllegalStateException("Invalid config details");
    // }
}

ECode CWifiConfiguration::ConfigKey(
    /* [in] */ Boolean allowCached,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String key;
    if (allowCached && mCachedConfigKey != NULL) {
        key = mCachedConfigKey;
    }
    else {
        AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
        AutoPtr< ArrayOf<String> > strings;
        keyMgmt->GetStrings((ArrayOf<String>**)&strings);
        Boolean bFlag1, bFlag2, bFlag3;
        mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &bFlag1);
        if (bFlag1) {
            key = mSSID;
            key += (*strings)[IWifiConfigurationKeyMgmt::WPA_PSK];
        }
        else if ((mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &bFlag2), bFlag2) ||
                (mAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &bFlag3), bFlag3)) {
            key = mSSID;
            key += (*strings)[IWifiConfigurationKeyMgmt::WPA_EAP];
        }
        else if ((*mWepKeys)[0] != NULL) {
            key = mSSID;
            key += "WEP";
        }
        else {
            key = mSSID;
            key += (*strings)[IWifiConfigurationKeyMgmt::NONE];
        }
        mCachedConfigKey = key;
    }

    *result = key;

    return NOERROR;
}

ECode CWifiConfiguration::ConfigKey(
    /* [out] */ String* result)
{
    return ConfigKey(FALSE, result);
}

ECode CWifiConfiguration::GetIpConfiguration(
    /* [out] */ IIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIpConfiguration;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiConfiguration::SetIpConfiguration(
    /* [in] */ IIpConfiguration* ipConfiguration)
{
    mIpConfiguration = ipConfiguration;
    return NOERROR;
}

ECode CWifiConfiguration::GetStaticIpConfiguration(
    /* [out] */ IStaticIpConfiguration** result)
{
    return mIpConfiguration->GetStaticIpConfiguration(result);
}

ECode CWifiConfiguration::SetStaticIpConfiguration(
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration)
{
    return mIpConfiguration->SetStaticIpConfiguration(staticIpConfiguration);
}

ECode CWifiConfiguration::GetHttpProxy(
    /* [out] */ IProxyInfo** result)
{
    return mIpConfiguration->GetHttpProxy(result);
}

ECode CWifiConfiguration::SetHttpProxy(
    /* [in] */ IProxyInfo* httpProxy)
{
    mIpConfiguration->SetHttpProxy(httpProxy);
    return NOERROR;
}

ECode CWifiConfiguration::SetProxy(
    /* [in] */ IpConfigurationProxySettings settings,
    /* [in] */ IProxyInfo* proxy)
{
    mIpConfiguration->SetProxySettings(settings);
    mIpConfiguration->SetHttpProxy(proxy);
    return NOERROR;
}

ECode CWifiConfiguration::GetIpAssignment(
    /* [out] */ IpConfigurationIpAssignment* ipAssignment)
{
    VALIDATE_NOT_NULL(ipAssignment);
    *ipAssignment = mIpAssignment;
    return NOERROR;
}

ECode CWifiConfiguration::SetIpAssignment(
    /* [in] */ IpConfigurationIpAssignment ipAssignment)
{
    mIpAssignment = ipAssignment;
    return NOERROR;
}

ECode CWifiConfiguration::GetProxySettings(
    /* [out] */ IpConfigurationProxySettings* proxySettings)
{
    VALIDATE_NOT_NULL(proxySettings);
    *proxySettings = mProxySettings;
    return NOERROR;
}

ECode CWifiConfiguration::SetProxySettings(
    /* [in] */ IpConfigurationProxySettings proxySettings)
{
    mProxySettings = proxySettings;
    return NOERROR;
}

ECode CWifiConfiguration::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);
    *linkProperties = mLinkProperties;
    REFCOUNT_ADD(*linkProperties);
    return NOERROR;
}

ECode CWifiConfiguration::SetLinkProperties(
    /* [in] */ ILinkProperties* linkProperties)
{
    mLinkProperties = linkProperties;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
