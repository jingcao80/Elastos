
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/server/wifi/WifiConfigStore.h"
#include "elastos/droid/server/wifi/WifiAutoJoinController.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/StringBuffer.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/StringToIntegral.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::IpConfigurationIpAssignment;
using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IFileObserver;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::R;
using Elastos::Droid::Server::Net::EIID_IDelayedDiskWriteWriter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Wifi::CScanResult;
using Elastos::Droid::Wifi::CWifiEnterpriseConfig;
using Elastos::Droid::Wifi::IWifiConfigurationVisibility;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConfigurationStatus;
using Elastos::Droid::Wifi::IScanResultHelper;
using Elastos::Droid::Wifi::CScanResultHelper;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWpsResult;
using Elastos::Droid::Wifi::CWpsResult;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::CWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::IWifiConfigurationHelper;
using Elastos::Droid::Wifi::CWifiConfigurationHelper;
using Elastos::Droid::Wifi::IWifiConfigurationAuthAlgorithm;
using Elastos::Droid::Wifi::CWifiConfigurationAuthAlgorithm;
using Elastos::Droid::Wifi::IWifiConfigurationPairwiseCipher;
using Elastos::Droid::Wifi::IWifiConfigurationGroupCipher;
using Elastos::Droid::Wifi::CWifiConfigurationGroupCipher;
using Elastos::Droid::Wifi::CWifiConfigurationPairwiseCipher;
using Elastos::Droid::Wifi::IWifiSsidHelper;
using Elastos::Droid::Wifi::CWifiSsidHelper;
using Elastos::Droid::Wifi::IWifiSsid;
using Elastos::Droid::Wifi::IWifiConfigurationProtocol;
using Elastos::Droid::Wifi::CWifiConfigurationProtocol;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::StringToIntegral;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IDataInput;
using Elastos::Security::IKey;
//TODO using Elastos::Security::ICredentials;
//TODO using Elastos::Security::IKeyChain;
using Elastos::Security::IPublicKey;
using Elastos::Security::IKeyStoreHelper;
//TODO using Elastos::Security::KeyStoreState;
using Elastos::Security::Cert::IX509Certificate;
//TODO using Elastos::Security::CKeyStoreHelper;
using Elastos::Utility::IBitSet;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                WifiConfigStore::WpaConfigFileObserver
//=====================================================================
WifiConfigStore::WpaConfigFileObserver::WpaConfigFileObserver()
    : FileObserver(SUPPLICANT_CONFIG_FILE, IFileObserver::CLOSE_WRITE)
{
}

ECode WifiConfigStore::WpaConfigFileObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
    if (event == IFileObserver::CLOSE_WRITE) {
        AutoPtr<IFile> file;
        CFile::New(SUPPLICANT_CONFIG_FILE, (IFile**)&file);
        Int64 length;
        file->GetLength(&length);
        //if (VDBG) LocalLog(String("wpa_supplicant.conf changed; new size = ") + StringUtils::ToString(length));
        if (VDBG) Logger::D("WifiConfigStore::WpaConfigFileObserver",
                "wpa_supplicant.conf changed; new size = %lld", length);
    }
    return NOERROR;
}

//====================================================================
//                    WifiConfigStore::InnerDelayedDiskWriteWriter
//===================================================================
CAR_INTERFACE_IMPL(WifiConfigStore::InnerDelayedDiskWriteWriter, Object, IDelayedDiskWriteWriter);

WifiConfigStore::InnerDelayedDiskWriteWriter::InnerDelayedDiskWriteWriter(
    /* [in] */ WifiConfigStore* owner,
    /* [in] */ IList* networks)
    : mOwner(owner)
    , mNetworks(networks)
{
}

ECode WifiConfigStore::InnerDelayedDiskWriteWriter::OnWriteCalled(
    /* [in] */ IDataOutputStream* out)
{
    Int32 size;
    mNetworks->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mNetworks->Get(i, (IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        //loge("onWriteCalled write SSID: " + config.SSID);
        /* if (config.getLinkProperties() != null)
           loge(" lp " + config.getLinkProperties().toString());
           else
           loge("attempt config w/o lp");
         */

        if (VDBG) {
            Int32 num = 0;
            Int32 numlink = 0;
            AutoPtr<IHashMap> connectChoices;
            config->GetConnectChoices((IHashMap**)&connectChoices);
            if (connectChoices != NULL) {
                connectChoices->GetSize(&num);
            }
            AutoPtr<IHashMap> linkedConfigurations;
            config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
            if (linkedConfigurations != NULL) {
                linkedConfigurations->GetSize(&numlink);
            }
            mOwner->Loge(String("saving network history: "));
                    //+ config.configKey()  + " gw: " +
                    //config.defaultGwMacAddress + " autojoin-status: " +
                    //config.autoJoinStatus + " ephemeral=" + config.ephemeral
                    //+ " choices:" + Integer.toString(num)
                    //+ " link:" + Integer.toString(numlink)
                    //+ " status:" + Integer.toString(config.status)
                    //+ " nid:" + Integer.toString(config.networkId));
        }

        Boolean isValid;
        if ((config->IsValid(&isValid), isValid) == FALSE)
            continue;

        String ssid;
        config->GetSSID(&ssid);
        if (ssid.IsNull()) {
            if (VDBG) {
                mOwner->Loge(String("writeKnownNetworkHistory trying to write config with null SSID"));
            }
            continue;
        }
        if (VDBG) {
            String ck;
            config->ConfigKey(&ck);
            mOwner->Loge(String("writeKnownNetworkHistory write config ") + ck);
        }
        String configKey;
        config->ConfigKey(&configKey);
        out->WriteUTF(CONFIG_KEY + configKey + SEPARATOR_KEY);

        String SSID, FQDN;
        config->GetSSID(&SSID);
        config->GetFQDN(&FQDN);
        out->WriteUTF(SSID_KEY + SSID + SEPARATOR_KEY);
        out->WriteUTF(FQDN_KEY + FQDN + SEPARATOR_KEY);

        Int32 priority, autoJoinStatus, status, disableReason, networkId;
        config->GetPriority(&priority);
        config->GetAutoJoinStatus(&autoJoinStatus);
        config->GetStatus(&status);
        config->GetDisableReason(&disableReason);
        config->GetNetworkId(&networkId);
        out->WriteUTF(PRIORITY_KEY + StringUtils::ToString(priority) + SEPARATOR_KEY);
        out->WriteUTF(STATUS_KEY + StringUtils::ToString(autoJoinStatus) + SEPARATOR_KEY);
        out->WriteUTF(SUPPLICANT_STATUS_KEY + StringUtils::ToString(status) + SEPARATOR_KEY);
        out->WriteUTF(SUPPLICANT_DISABLE_REASON_KEY + StringUtils::ToString(disableReason) + SEPARATOR_KEY);
        out->WriteUTF(NETWORK_ID_KEY + StringUtils::ToString(networkId) + SEPARATOR_KEY);

        Boolean selfAdded, didSelfAdd, noInternetAccess, ephemeral;
        config->GetSelfAdded(&selfAdded);
        config->GetDidSelfAdd(&didSelfAdd);
        config->GetNoInternetAccess(&noInternetAccess);
        config->GetEphemeral(&ephemeral);
        out->WriteUTF(SELF_ADDED_KEY + StringUtils::BooleanToString(selfAdded) + SEPARATOR_KEY);
        out->WriteUTF(DID_SELF_ADD_KEY + StringUtils::BooleanToString(didSelfAdd) + SEPARATOR_KEY);
        out->WriteUTF(NO_INTERNET_ACCESS_KEY + StringUtils::BooleanToString(noInternetAccess) + SEPARATOR_KEY);
        out->WriteUTF(EPHEMERAL_KEY + StringUtils::BooleanToString(ephemeral) + SEPARATOR_KEY);

        String peerWifiConfiguration;
        config->GetPeerWifiConfiguration(&peerWifiConfiguration);
        if (!peerWifiConfiguration.IsNull()) {
            out->WriteUTF(PEER_CONFIGURATION_KEY + peerWifiConfiguration + SEPARATOR_KEY);
        }

        Int32 numConnectionFailures, numAuthFailures, numIpConfigFailures;
        Int32 numScorerOverride, numScorerOverrideAndSwitchedNetwork, numAssociation;
        Int32 autoJoinUseAggressiveJoinAttemptThreshold;
        config->GetNumConnectionFailures(&numConnectionFailures);
        config->GetNumAuthFailures(&numAuthFailures);
        config->GetNumIpConfigFailures(&numIpConfigFailures);
        config->GetNumScorerOverride(&numScorerOverride);
        config->GetNumScorerOverrideAndSwitchedNetwork(&numScorerOverrideAndSwitchedNetwork);
        config->GetNumAssociation(&numAssociation);
        config->GetAutoJoinUseAggressiveJoinAttemptThreshold(&autoJoinUseAggressiveJoinAttemptThreshold);

        out->WriteUTF(NUM_CONNECTION_FAILURES_KEY
                + StringUtils::ToString(numConnectionFailures)
                + SEPARATOR_KEY);
        out->WriteUTF(NUM_AUTH_FAILURES_KEY
                + StringUtils::ToString(numAuthFailures)
                + SEPARATOR_KEY);
        out->WriteUTF(NUM_IP_CONFIG_FAILURES_KEY
                + StringUtils::ToString(numIpConfigFailures)
                + SEPARATOR_KEY);
        out->WriteUTF(SCORER_OVERRIDE_KEY + StringUtils::ToString(numScorerOverride)
                + SEPARATOR_KEY);
        out->WriteUTF(SCORER_OVERRIDE_AND_SWITCH_KEY
                + StringUtils::ToString(numScorerOverrideAndSwitchedNetwork)
                + SEPARATOR_KEY);
        out->WriteUTF(NUM_ASSOCIATION_KEY
                + StringUtils::ToString(numAssociation)
                + SEPARATOR_KEY);
        out->WriteUTF(JOIN_ATTEMPT_BOOST_KEY
                + StringUtils::ToString(autoJoinUseAggressiveJoinAttemptThreshold)
                + SEPARATOR_KEY);
        //out->WriteUTF(BLACKLIST_MILLI_KEY + Long.toString(config.blackListTimestamp)
        //        + SEPARATOR_KEY);
        Int32 creatorUid, lastConnectUid, lastUpdateUid;
        config->GetCreatorUid(&creatorUid);
        config->GetLastConnectUid(&lastConnectUid);
        config->GetLastUpdateUid(&lastUpdateUid);
        out->WriteUTF(CREATOR_UID_KEY + StringUtils::ToString(creatorUid)
                + SEPARATOR_KEY);
        out->WriteUTF(CONNECT_UID_KEY + StringUtils::ToString(lastConnectUid)
                + SEPARATOR_KEY);
        out->WriteUTF(UPDATE_UID_KEY + StringUtils::ToString(lastUpdateUid)
                + SEPARATOR_KEY);

        AutoPtr<IBitSet> allowedKeyManagement;
        config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
        AutoPtr<IWifiConfigurationKeyMgmt> wfcKeyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&wfcKeyMgmt);
        AutoPtr<ArrayOf<String> > strings;
        wfcKeyMgmt->GetStrings((ArrayOf<String>**)&strings);
        String allowedKeyManagementString =
            MakeString(allowedKeyManagement, strings);
        out->WriteUTF(AUTH_KEY + allowedKeyManagementString + SEPARATOR_KEY);

        AutoPtr<IHashMap> connectChoices;
        config->GetConnectChoices((IHashMap**)&connectChoices);
        if (connectChoices != NULL) {
            AutoPtr<ISet> keySet;
            connectChoices->GetKeySet((ISet**)&keySet);
            AutoPtr<IIterator> keyIter;
            keySet->GetIterator((IIterator**)&keyIter);
            Boolean isflag = FALSE;
            while ((keyIter->HasNext(&isflag), isflag)) {
                AutoPtr<IInterface> keyObj;
                keyIter->GetNext((IInterface**)&keyObj);
                AutoPtr<IInterface> valueObj;
                connectChoices->Get(keyObj, (IInterface**)&valueObj);
                IInteger32* i32 = IInteger32::Probe(valueObj);
                Int32 choice;
                i32->GetValue(&choice);
                String key;
                ICharSequence::Probe(keyObj)->ToString(&key);
                out->WriteUTF(CHOICE_KEY + key + "="
                        + StringUtils::ToString(choice) + SEPARATOR_KEY);
            }
        }

        AutoPtr<IHashMap> linkedConfigurations;
        config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
        if (linkedConfigurations != NULL) {
            Int32 size;
            linkedConfigurations->GetSize(&size);
            mOwner->Loge(String("writeKnownNetworkHistory write linked ")
                        + StringUtils::ToString(size));

            AutoPtr<ISet> keySet;
            linkedConfigurations->GetKeySet((ISet**)&keySet);
            AutoPtr<IIterator> keyIter;
            keySet->GetIterator((IIterator**)&keyIter);
            Boolean isflag = FALSE;
            while ((keyIter->HasNext(&isflag), isflag)) {
                AutoPtr<IInterface> keyObj;
                keyIter->GetNext((IInterface**)&keyObj);
                String key;
                ICharSequence::Probe(keyObj)->ToString(&key);
                out->WriteUTF(LINK_KEY + key + SEPARATOR_KEY);
            }
        }

        String macAddress;
        config->GetDefaultGwMacAddress(&macAddress);
        if (!macAddress.IsNull()) {
            out->WriteUTF(DEFAULT_GW_KEY + macAddress + SEPARATOR_KEY);
        }

        AutoPtr<IHashMap> scanResultCache;
        config->GetScanResultCache((IHashMap**)&scanResultCache);
        if (scanResultCache != NULL) {
            AutoPtr<ICollection> values;
            scanResultCache->GetValues((ICollection**)&values);
            AutoPtr<IIterator> iter;
            values->GetIterator((IIterator**)&iter);
            Boolean bNext;
            iter->HasNext(&bNext);
            for (; bNext; iter->HasNext(&bNext)) {
                AutoPtr<IInterface> obj;
                iter->GetNext((IInterface**)&obj);
                IScanResult* result = IScanResult::Probe(obj);

                String bssid;
                result->GetBSSID(&bssid);
                out->WriteUTF(BSSID_KEY + bssid + SEPARATOR_KEY);
                Int32 frequency, level, autoJoinStatus;
                result->GetFrequency(&frequency);
                out->WriteUTF(FREQ_KEY + StringUtils::ToString(frequency) + SEPARATOR_KEY);
                result->GetLevel(&level);
                out->WriteUTF(RSSI_KEY + StringUtils::ToString(level) + SEPARATOR_KEY);
                result->GetAutoJoinStatus(&autoJoinStatus);
                out->WriteUTF(BSSID_STATUS_KEY + StringUtils::ToString(autoJoinStatus) + SEPARATOR_KEY);
                //if (result.seen != 0) {
                //    out->WriteUTF(MILLI_KEY + Long.toString(result.seen)
                //            + SEPARATOR_KEY);
                //}
                out->WriteUTF(BSSID_KEY_END + SEPARATOR_KEY);
            }
        }

        String lastFailure;
        config->GetLastFailure(&lastFailure);
        if (!lastFailure.IsNull()) {
            out->WriteUTF(FAILURE_KEY + lastFailure + SEPARATOR_KEY);
        }
        out->WriteUTF(SEPARATOR_KEY);
        // Add extra blank lines for clarity
        out->WriteUTF(SEPARATOR_KEY);
        out->WriteUTF(SEPARATOR_KEY);
    }
    return NOERROR;
}

//=====================================================================
//                           WifiConfigStore
//=====================================================================
const String WifiConfigStore::OLD_PRIVATE_KEY_NAME("private_key");
const String WifiConfigStore::EMPTY_VALUE("NULL");
const String WifiConfigStore::TAG("WifiConfigStore");
const Boolean WifiConfigStore::DBG = TRUE;
Boolean WifiConfigStore::VDBG = FALSE;
Boolean WifiConfigStore::VVDBG = FALSE;
const String WifiConfigStore::SUPPLICANT_CONFIG_FILE("/data/misc/wifi/wpa_supplicant.conf");
static String GetDataDirectory()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dFile;
    env->GetDataDirectory((IFile**)&dFile);
    String dir;
    dFile->GetAbsolutePath(&dir);
    return dir;
}
const String WifiConfigStore::ipConfigFile(GetDataDirectory() +
             "/misc/wifi/ipconfig.txt");
const String WifiConfigStore::networkHistoryConfigFile(GetDataDirectory() +
             "/misc/wifi/networkHistory.txt");
const String WifiConfigStore::autoJoinConfigFile(GetDataDirectory() +
             "/misc/wifi/autojoinconfig.txt");
const String WifiConfigStore::SSID_KEY("SSID:  ");
const String WifiConfigStore::CONFIG_KEY("CONFIG:  ");
const String WifiConfigStore::CHOICE_KEY("CHOICE:  ");
const String WifiConfigStore::LINK_KEY("LINK:  ");
const String WifiConfigStore::BSSID_KEY("BSSID:  ");
const String WifiConfigStore::BSSID_KEY_END("/BSSID:  ");
const String WifiConfigStore::RSSI_KEY("RSSI:  ");
const String WifiConfigStore::FREQ_KEY("FREQ:  ");
const String WifiConfigStore::DATE_KEY("DATE:  ");
const String WifiConfigStore::MILLI_KEY("MILLI:  ");
const String WifiConfigStore::BLACKLIST_MILLI_KEY("BLACKLIST_MILLI:  ");
const String WifiConfigStore::NETWORK_ID_KEY("ID:  ");
const String WifiConfigStore::PRIORITY_KEY("PRIORITY:  ");
const String WifiConfigStore::DEFAULT_GW_KEY("DEFAULT_GW:  ");
const String WifiConfigStore::AUTH_KEY("AUTH:  ");
const String WifiConfigStore::SEPARATOR_KEY("\n");
const String WifiConfigStore::STATUS_KEY("AUTO_JOIN_STATUS:  ");
const String WifiConfigStore::BSSID_STATUS_KEY("BSSID_STATUS:  ");
const String WifiConfigStore::SELF_ADDED_KEY("SELF_ADDED:  ");
const String WifiConfigStore::FAILURE_KEY("FAILURE:  ");
const String WifiConfigStore::DID_SELF_ADD_KEY("DID_SELF_ADD:  ");
const String WifiConfigStore::PEER_CONFIGURATION_KEY("PEER_CONFIGURATION:  ");
const String WifiConfigStore::CREATOR_UID_KEY("CREATOR_UID_KEY:  ");
const String WifiConfigStore::CONNECT_UID_KEY("CONNECT_UID_KEY:  ");
const String WifiConfigStore::UPDATE_UID_KEY("UPDATE_UID:  ");
const String WifiConfigStore::SUPPLICANT_STATUS_KEY("SUP_STATUS:  ");
const String WifiConfigStore::SUPPLICANT_DISABLE_REASON_KEY("SUP_DIS_REASON:  ");
const String WifiConfigStore::FQDN_KEY("FQDN:  ");
const String WifiConfigStore::NUM_CONNECTION_FAILURES_KEY("CONNECT_FAILURES:  ");
const String WifiConfigStore::NUM_IP_CONFIG_FAILURES_KEY("IP_CONFIG_FAILURES:  ");
const String WifiConfigStore::NUM_AUTH_FAILURES_KEY("AUTH_FAILURES:  ");
const String WifiConfigStore::SCORER_OVERRIDE_KEY("SCORER_OVERRIDE:  ");
const String WifiConfigStore::SCORER_OVERRIDE_AND_SWITCH_KEY("SCORER_OVERRIDE_AND_SWITCH:  ");
const String WifiConfigStore::NO_INTERNET_ACCESS_KEY("NO_INTERNET_ACCESS:  ");
const String WifiConfigStore::EPHEMERAL_KEY("EPHEMERAL:   ");
const String WifiConfigStore::NUM_ASSOCIATION_KEY("NUM_ASSOCIATION:  ");
const String WifiConfigStore::JOIN_ATTEMPT_BOOST_KEY("JOIN_ATTEMPT_BOOST:  ");
const String WifiConfigStore::THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY("THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G:  ");
const String WifiConfigStore::THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY("THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_HARD_5G_KEY("THRESHOLD_UNBLACKLIST_HARD_5G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_SOFT_5G_KEY("THRESHOLD_UNBLACKLIST_SOFT_5G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_HARD_24G_KEY("THRESHOLD_UNBLACKLIST_HARD_24G:  ");
const String WifiConfigStore::THRESHOLD_UNBLACKLIST_SOFT_24G_KEY("THRESHOLD_UNBLACKLIST_SOFT_24G:  ");
const String WifiConfigStore::THRESHOLD_GOOD_RSSI_5_KEY("THRESHOLD_GOOD_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_LOW_RSSI_5_KEY("THRESHOLD_LOW_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_BAD_RSSI_5_KEY("THRESHOLD_BAD_RSSI_5:  ");
const String WifiConfigStore::THRESHOLD_GOOD_RSSI_24_KEY("THRESHOLD_GOOD_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_LOW_RSSI_24_KEY("THRESHOLD_LOW_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_BAD_RSSI_24_KEY("THRESHOLD_BAD_RSSI_24:  ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING:   ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY("THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY("THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY("MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY("MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS:   ");
const String WifiConfigStore::A_BAND_PREFERENCE_RSSI_THRESHOLD_LOW_KEY("A_BAND_PREFERENCE_RSSI_THRESHOLD_LOW:   ");
const String WifiConfigStore::A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY("A_BAND_PREFERENCE_RSSI_THRESHOLD:   ");
const String WifiConfigStore::G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY("G_BAND_PREFERENCE_RSSI_THRESHOLD:   ");
const String WifiConfigStore::ENABLE_AUTOJOIN_WHILE_ASSOCIATED_KEY("ENABLE_AUTOJOIN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY("ASSOCIATED_PARTIAL_SCAN_PERIOD:   ");
const String WifiConfigStore::ASSOCIATED_FULL_SCAN_BACKOFF_KEY("ASSOCIATED_FULL_SCAN_BACKOFF_PERIOD:   ");
const String WifiConfigStore::ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY("ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY("AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED:   ");
const String WifiConfigStore::ONLY_LINK_SAME_CREDENTIAL_CONFIGURATIONS_KEY("ONLY_LINK_SAME_CREDENTIAL_CONFIGURATIONS:   ");
const String WifiConfigStore::ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY("ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY("ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY("ENABLE_AUTO_JOIN_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY("ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED:   ");
const String WifiConfigStore::ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY("ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY:   ");
const String WifiConfigStore::WIFI_VERBOSE_LOGS_KEY("WIFI_VERBOSE_LOGS:   ");
const String WifiConfigStore::DELETED_CONFIG_PSK("Mjkd86jEMGn79KhKll298Uu7-deleted");
static AutoPtr<IPattern> InitConnectChoice()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("(.*)=([0-9]+)"), (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> WifiConfigStore::mConnectChoice = InitConnectChoice();
AutoPtr< ArrayOf<String> > WifiConfigStore::ENTERPRISE_CONFIG_SUPPLICANT_KEYS = WifiConfigStore::MiddleInitEnterpriseConfigSupplicantKeys();
const Int32 WifiConfigStore::DEFAULT_MAX_DHCP_RETRIES;

WifiConfigStore::WifiConfigStore(
    /* [in] */ IContext* c,
    /* [in] */ WifiNative* wn)
    : enableAutoJoinScanWhenAssociated(TRUE)
    , enableAutoJoinWhenAssociated(TRUE)
    , enableChipWakeUpWhenAssociated(TRUE)
    , enableRssiPollWhenAssociated(TRUE)
    , maxTxPacketForNetworkSwitching(40)
    , maxRxPacketForNetworkSwitching(80)
    , maxTxPacketForFullScans(8)
    , maxRxPacketForFullScans(16)
    , maxTxPacketForPartialScans(40)
    , maxRxPacketForPartialScans(80)
    , enableFullBandScanWhenAssociated(TRUE)
    , thresholdInitialAutoJoinAttemptMin5RSSI(IWifiConfiguration::INITIAL_AUTO_JOIN_ATTEMPT_MIN_5)
    , thresholdInitialAutoJoinAttemptMin24RSSI(IWifiConfiguration::INITIAL_AUTO_JOIN_ATTEMPT_MIN_24)
    , thresholdBadRssi5(IWifiConfiguration::BAD_RSSI_5)
    , thresholdLowRssi5(IWifiConfiguration::LOW_RSSI_5)
    , thresholdGoodRssi5(IWifiConfiguration::GOOD_RSSI_5)
    , thresholdBadRssi24(IWifiConfiguration::BAD_RSSI_24)
    , thresholdLowRssi24(IWifiConfiguration::LOW_RSSI_24)
    , thresholdGoodRssi24(IWifiConfiguration::GOOD_RSSI_24)
    , associatedFullScanBackoff(12)
    , associatedFullScanMaxIntervalMilli(300000)
    , associatedPartialScanPeriodMilli(0)
    , bandPreferenceBoostFactor5(5)
    , bandPreferencePenaltyFactor5(2)
    , bandPreferencePenaltyThreshold5(IWifiConfiguration::G_BAND_PREFERENCE_RSSI_THRESHOLD)
    , bandPreferenceBoostThreshold5(IWifiConfiguration::A_BAND_PREFERENCE_RSSI_THRESHOLD)
    , badLinkSpeed24(6)
    , badLinkSpeed5(12)
    , goodLinkSpeed24(24)
    , goodLinkSpeed5(36)
    , maxAuthErrorsToBlacklist(4)
    , maxConnectionErrorsToBlacklist(4)
    , wifiConfigBlacklistMinTimeMilli(1000 * 60 * 5)
    , associatedHysteresisHigh(+14)
    , associatedHysteresisLow(+8)
    , thresholdUnblacklistThreshold5Hard(IWifiConfiguration::UNBLACKLIST_THRESHOLD_5_HARD)
    , thresholdUnblacklistThreshold5Soft(IWifiConfiguration::UNBLACKLIST_THRESHOLD_5_SOFT)
    , thresholdUnblacklistThreshold24Hard(IWifiConfiguration::UNBLACKLIST_THRESHOLD_24_HARD)
    , thresholdUnblacklistThreshold24Soft(IWifiConfiguration::UNBLACKLIST_THRESHOLD_24_SOFT)
    , enableVerboseLogging(0)
    , showNetworks(TRUE)
    , alwaysEnableScansWhileAssociated(0)
    , autoJoinScanIntervalWhenP2pConnected(300000)
    , maxNumActiveChannelsForPartialScans(6)
    , maxNumPassiveChannelsForPartialScans(2)
    , roamOnAny(FALSE)
    , onlyLinkSameCredentialConfigurations(TRUE)
    , enableLinkDebouncing(TRUE)
    , enable5GHzPreference(TRUE)
    , enableWifiCellularHandoverUserTriggeredAdjustment(TRUE)
    , lastUnwantedNetworkDisconnectTimestamp(0)
    , mLastPriority(-1)
{
    IpConfigStore::constructor();
    CHashMap::New((IHashMap**)&mConfiguredNetworks);
    CHashMap::New((IHashMap**)&mNetworkIds);
    AutoPtr<IKeyStoreHelper> keyStoreHelper;
    //TODO CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&keyStoreHelper);
    //TODO String type;
    //TODO keyStoreHelper->GetDefaultType(&type);
    //TODO keyStoreHelper->GetInstance(type, (IKeyStore**)&mKeyStore);

    mContext = c;
    mWifiNative = wn;

    if (showNetworks) {
        mLocalLog = mWifiNative->GetLocalLog();
        mFileObserver = new WpaConfigFileObserver();
        mFileObserver->StartWatching();
    } else {
        mLocalLog = NULL;
        mFileObserver = NULL;
    }

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_wifi_framework_associated_scan_interval, &associatedPartialScanPeriodMilli);
    Loge(String("associatedPartialScanPeriodMilli set to ") + StringUtils::ToString(associatedPartialScanPeriodMilli));

    resources->GetBoolean(R::bool_::config_wifi_only_link_same_credential_configurations,
            &onlyLinkSameCredentialConfigurations);
    resources->GetInteger(R::integer::config_wifi_framework_associated_partial_scan_max_num_active_channels,
            &maxNumActiveChannelsForPartialScans);
    resources->GetInteger(R::integer::config_wifi_framework_associated_partial_scan_max_num_passive_channels,
            &maxNumPassiveChannelsForPartialScans);
    resources->GetInteger(R::integer::config_wifi_framework_associated_full_scan_max_interval,
            &associatedFullScanMaxIntervalMilli);
    resources->GetInteger(R::integer::config_wifi_framework_associated_full_scan_backoff,
            &associatedFullScanBackoff);
    resources->GetBoolean(R::bool_::config_wifi_enable_disconnection_debounce,
            &enableLinkDebouncing);

    resources->GetBoolean(R::bool_::config_wifi_enable_5GHz_preference,
            &enable5GHzPreference);

    resources->GetInteger(R::integer::config_wifi_framework_5GHz_preference_boost_factor,
            &bandPreferenceBoostFactor5);
    resources->GetInteger(R::integer::config_wifi_framework_5GHz_preference_penalty_factor,
            &bandPreferencePenaltyFactor5);

    resources->GetInteger(R::integer::config_wifi_framework_5GHz_preference_penalty_threshold,
            &bandPreferencePenaltyThreshold5);
    resources->GetInteger(R::integer::config_wifi_framework_5GHz_preference_boost_threshold,
            &bandPreferenceBoostThreshold5);

    resources->GetInteger(R::integer::config_wifi_framework_current_association_hysteresis_high,
            &associatedHysteresisHigh);
    resources->GetInteger(R::integer::config_wifi_framework_current_association_hysteresis_low,
            &associatedHysteresisLow);

    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_bad_rssi_threshold_5GHz, &thresholdBadRssi5);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_low_rssi_threshold_5GHz, &thresholdLowRssi5);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_good_rssi_threshold_5GHz, &thresholdGoodRssi5);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_bad_rssi_threshold_24GHz, &thresholdBadRssi24);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_low_rssi_threshold_24GHz, &thresholdLowRssi24);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_good_rssi_threshold_24GHz, &thresholdGoodRssi24);

    resources->GetBoolean(R::bool_::config_wifi_framework_cellular_handover_enable_user_triggered_adjustment,
            &enableWifiCellularHandoverUserTriggeredAdjustment);

    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_bad_link_speed_24,
            &badLinkSpeed24);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_bad_link_speed_5, &badLinkSpeed5);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_good_link_speed_24, &goodLinkSpeed24);
    resources->GetInteger(R::integer::config_wifi_framework_wifi_score_good_link_speed_5, &goodLinkSpeed5);

    resources->GetInteger(R::integer::config_wifi_framework_max_auth_errors_to_blacklist, &maxAuthErrorsToBlacklist);
    resources->GetInteger(R::integer::config_wifi_framework_max_connection_errors_to_blacklist, &maxConnectionErrorsToBlacklist);
    resources->GetInteger(R::integer::config_wifi_framework_network_black_list_min_time_milli, &wifiConfigBlacklistMinTimeMilli);


    resources->GetBoolean(R::bool_::config_wifi_framework_enable_associated_autojoin_scan, &enableAutoJoinScanWhenAssociated);

    resources->GetBoolean(R::bool_::config_wifi_framework_enable_associated_network_selection, &enableAutoJoinWhenAssociated);
}

ECode WifiConfigStore::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    enableVerboseLogging = verbose;
    if (verbose > 0) {
        VDBG = TRUE;
        showNetworks = TRUE;
    } else {
        VDBG = FALSE;
    }
    if (verbose > 1) {
        VVDBG = TRUE;
    } else {
        VVDBG = FALSE;
    }
    return NOERROR;
}

ECode WifiConfigStore::LoadAndEnableAllNetworks()
{
    if (DBG) Log(String("Loading config and enabling all networks "));
    LoadConfiguredNetworks();
    EnableAllNetworks();
    return NOERROR;
}

ECode WifiConfigStore::GetConfiguredNetworksSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mConfiguredNetworks->GetSize(result);
}

ECode WifiConfigStore::GetConfiguredNetworks(
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> networks = GetConfiguredNetworks((IMap*)NULL);
    *result = networks;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::GetPrivilegedConfiguredNetworks(
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMap> pskMap = GetCredentialsBySsidMap();//String, String
    AutoPtr<IList> networks = GetConfiguredNetworks(pskMap);
    *result = networks;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::GetconfiguredNetworkSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mConfiguredNetworks == NULL) {
        *result = 0;
        return NOERROR;
    }
    return mConfiguredNetworks->GetSize(result);
}

ECode WifiConfigStore::GetRecentConfiguredNetworks(
    /* [in] */ Int32 milli,
    /* [in] */ Boolean copy,
    /* [out] */ IList** result)//WifiConfiguration
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> networks;//WifiConfiguration

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IInterface> obj = (*array)[i];
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        Int32 autoJoinStatus;
        config->GetAutoJoinStatus(&autoJoinStatus);
        if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DELETED) {
            // Do not enumerate and return this configuration to any one,
            // instead treat it as unknown. the configuration can still be retrieved
            // directly by the key or networkId
            continue;
        }

        // Calculate the RSSI for scan results that are more recent than milli
        AutoPtr<IWifiConfigurationVisibility> wfcVisibility;
        config->SetVisibility(milli, (IWifiConfigurationVisibility**)&wfcVisibility);
        config->GetVisibility((IWifiConfigurationVisibility**)&wfcVisibility);
        if (wfcVisibility == NULL) {
            continue;
        }
        Int32 rssi5;
        wfcVisibility->GetRssi5(&rssi5);
        Int32 rssi24;
        wfcVisibility->GetRssi24(&rssi24);
        if (rssi5 == IWifiConfiguration::INVALID_RSSI &&
                rssi24 == IWifiConfiguration::INVALID_RSSI) {
            continue;
        }
        if (networks == NULL)
            CArrayList::New((IList**)&networks);
        if (copy) {
            AutoPtr<IWifiConfiguration> config2;
            CWifiConfiguration::New(config, (IWifiConfiguration**)&config2);
            networks->Add(config2);
        } else {
            networks->Add(config);
        }
    }
    *result = networks;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::UpdateConfiguration(
    /* [in] */ IWifiInfo* info)
{
    AutoPtr<IWifiConfiguration> config;
    Int32 networkId;
    info->GetNetworkId(&networkId);
    GetWifiConfiguration(networkId, (IWifiConfiguration**)&config);

    AutoPtr<IHashMap> scanResultCache;
    if (config != NULL && (config->GetScanResultCache((IHashMap**)&scanResultCache), scanResultCache) != NULL) {
        String bssid;
        info->GetBSSID(&bssid);
        AutoPtr<IInterface> obj;
        scanResultCache->Get(CoreUtils::Convert(bssid), (IInterface**)&obj);
        IScanResult* result = IScanResult::Probe(obj);
        if (result != NULL) {
            Int64 previousSeen;
            result->GetSeen(&previousSeen);;
            Int32 previousRssi;
            result->GetLevel(&previousRssi);

            // Update the scan result
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 currentTimeMillis;
            system->GetCurrentTimeMillis(&currentTimeMillis);
            result->SetSeen(currentTimeMillis);
            Int32 rssi;
            info->GetRssi(&rssi);
            result->SetLevel(rssi);;

            // Average the RSSI value
            result->AverageRssi(previousRssi, previousSeen,
                    WifiAutoJoinController::mScanResultMaximumAge);
            if (VDBG) {
                Int32 frequency;
                result->GetFrequency(&frequency);
                String configkey;
                config->ConfigKey(&configkey);
                Loge(String("updateConfiguration freq=") +
                    StringUtils::ToString(frequency)
                    + " BSSID=" + bssid
                    + " RSSI=" + StringUtils::ToString(rssi)
                    + " " + configkey);
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::GetWifiConfiguration(
    /* [in] */ Int32 netId,
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    if (mConfiguredNetworks == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(obj);
    *result = config;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::GetWifiConfiguration(
    /* [in] */ const String& key,
    /* [out] */ IWifiConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    if (key.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    Int32 hash = key.GetHashCode();
    if (mNetworkIds == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mNetworkIds->Get(CoreUtils::Convert(hash), (IInterface**)&obj);
    AutoPtr<IInteger32> n = IInteger32::Probe(obj);
    if (n == NULL) {
        *result = NULL;
        return NOERROR;
    }
    Int32 netId;
    n->GetValue(&netId);
    return GetWifiConfiguration(netId, result);
}

ECode WifiConfigStore::EnableAllNetworks()
{
    Int64 now;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&now);
    Boolean networkEnabledStateChanged = FALSE;

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        Int32 status, autoJoinStatus;
        if(config != NULL && (config->GetStatus(&status), status) == IWifiConfigurationStatus::DISABLED
                && ((config->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus)
                <= IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE)) {

            // Wait for 20 minutes before reenabling config that have known, repeated connection
            // or DHCP failures
            Int32 disableReason;
            config->GetDisableReason(&disableReason);
            if (disableReason == IWifiConfiguration::DISABLED_DHCP_FAILURE
                    || disableReason == IWifiConfiguration::DISABLED_ASSOCIATION_REJECT
                    || disableReason == IWifiConfiguration::DISABLED_AUTH_FAILURE) {
                Int64 blackListTimestamp;
                config->GetBlackListTimestamp(&blackListTimestamp);
                if (blackListTimestamp != 0
                       && now > blackListTimestamp
                       && (now - blackListTimestamp) < wifiConfigBlacklistMinTimeMilli) {
                    continue;
                }
            }

            Int32 networkId;
            config->GetNetworkId(&networkId);
            if(mWifiNative->EnableNetwork(networkId, FALSE)) {
                networkEnabledStateChanged = TRUE;
                config->SetStatus(IWifiConfigurationStatus::ENABLED);

                // Reset the blacklist condition
                config->SetNumConnectionFailures(0);
                config->SetNumIpConfigFailures(0);
                config->SetNumAuthFailures(0);

                // Reenable the wifi configuration
                config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
            } else {
                Loge(String("Enable network failed on ") + StringUtils::ToString(networkId));
            }
        }
    }

    if (networkEnabledStateChanged) {
        mWifiNative->SaveConfig();
        SendConfiguredNetworksChangedBroadcast();
    }
    return NOERROR;
}

ECode WifiConfigStore::SelectNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) LocalLog(String("selectNetwork"), netId);
    if (netId == IWifiConfiguration::INVALID_NETWORK_ID) {
        *result = FALSE;
        return NOERROR;
    }

    // Reset the priority of each network at start or if it goes too high.
    if (mLastPriority == -1 || mLastPriority > 1000000) {
        AutoPtr<ICollection> values;
        mConfiguredNetworks->GetValues((ICollection**)&values);
        AutoPtr<IIterator> iter;
        values->GetIterator((IIterator**)&iter);
        Boolean bNext;
        iter->HasNext(&bNext);
        for (; bNext; iter->HasNext(&bNext)) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

            Int32 networkId;
            config->GetNetworkId(&networkId);
            if (networkId != IWifiConfiguration::INVALID_NETWORK_ID) {
                config->SetPriority(0);
                AddOrUpdateNetworkNative(config, -1);
            }
        }
        mLastPriority = 0;
    }

    // Set to the highest priority and save the configuration.
    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);
    config->SetNetworkId(netId);
    config->SetPriority(++mLastPriority);

    AddOrUpdateNetworkNative(config, -1);
    mWifiNative->SaveConfig();

    /* Enable the given network while disabling all other networks */
    Boolean bTemp;
    EnableNetworkWithoutBroadcast(netId, TRUE, &bTemp);

    /* Avoid saving the config & sending a broadcast to prevent settings
    * from displaying a disabled list of networks */
    *result = TRUE;
    return NOERROR;
}

ECode WifiConfigStore::SaveNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid,
    /* [out] */ NetworkUpdateResult** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWifiConfiguration> conf;

    // A new network cannot have null SSID
    Int32 networkId;
    String ssid;
    if (config != NULL)
        config->GetSSID(&ssid);
    if (config == NULL || ((config->GetNetworkId(&networkId), networkId) == IWifiConfiguration::INVALID_NETWORK_ID &&
            ssid.IsNull())) {
        AutoPtr<NetworkUpdateResult> nur = new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
        *result = nur;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    if (VDBG) LocalLog(String("WifiConfigStore: saveNetwork netId"), networkId);
    if (VDBG) {
        Int32 size;
        mConfiguredNetworks->GetSize(&size);
        Int32 creatorUid;
        config->GetCreatorUid(&creatorUid);
        Int32 lastUpdateUid;
        config->GetLastUpdateUid(&lastUpdateUid);
        Loge(String("WifiConfigStore saveNetwork, size=") + StringUtils::ToString(size)
                + String(" SSID=") + ssid
                + String(" Uid=") + StringUtils::ToString(creatorUid)
                + String("/") + StringUtils::ToString(lastUpdateUid));
    }
    Boolean newNetwork = (networkId == IWifiConfiguration::INVALID_NETWORK_ID);
    AutoPtr<NetworkUpdateResult> nwuresult = AddOrUpdateNetworkNative(config, uid);
    Int32 netId = nwuresult->GetNetworkId();

    if (VDBG) LocalLog(String("WifiConfigStore: saveNetwork got it back netId="), netId);

    /* enable a new network */
    if (newNetwork && netId != IWifiConfiguration::INVALID_NETWORK_ID) {
        if (VDBG) LocalLog(String("WifiConfigStore: will enable netId="), netId);

        mWifiNative->EnableNetwork(netId, FALSE);
        AutoPtr<IInterface> configObj;
        mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&configObj);
        conf = IWifiConfiguration::Probe(configObj);
        if (conf != NULL)
            conf->SetStatus(IWifiConfigurationStatus::ENABLED);
    }

    AutoPtr<IInterface> configObj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&configObj);
    conf = IWifiConfiguration::Probe(configObj);
    if (conf != NULL) {
        Int32 autoJoinStatus;
        String ssid;
        config->GetSSID(&ssid);
        if ((conf->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus) != IWifiConfiguration::AUTO_JOIN_ENABLED) {
            if (VDBG) LocalLog(String("WifiConfigStore: re-enabling: ") + ssid);

            // reenable autojoin, since new information has been provided
            conf->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
            Boolean bTemp;
            EnableNetworkWithoutBroadcast(networkId, FALSE, &bTemp);
        }
        if (VDBG) {
            Int32 creatorUid;
            config->GetCreatorUid(&creatorUid);
            Loge(String("WifiConfigStore: saveNetwork got config back netId=")
                    + StringUtils::ToString(netId)
                    + String(" uid=")
                    + StringUtils::ToString(creatorUid));
        }
    }

    mWifiNative->SaveConfig();
    SendConfiguredNetworksChangedBroadcast(conf, nwuresult->IsNewNetwork() ?
            IWifiManager::CHANGE_REASON_ADDED : IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    *result = nwuresult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::DriverRoamedFrom(
    /* [in] */ IWifiInfo* info)
{
    VALIDATE_NOT_NULL(info);
    if (info != NULL) {
        String bssid;
        Int32 frequency;
        info->GetFrequency(&frequency);
        Int32 rssi;
        info->GetRssi(&rssi);
        AutoPtr<IScanResultHelper> scanResultHelper;
        CScanResultHelper::AcquireSingleton((IScanResultHelper**)&scanResultHelper);
        Boolean bTemp;
        if (!((info->GetBSSID(&bssid), bssid).IsNull())
                && (scanResultHelper->Is5GHz(frequency, &bTemp), bTemp)
                && rssi > (bandPreferenceBoostThreshold5 + 3)) {
            Int32 networkId;
            info->GetNetworkId(&networkId);
            AutoPtr<IWifiConfiguration> config;
            GetWifiConfiguration(networkId, (IWifiConfiguration**)&config);
            if (config != NULL) {
                AutoPtr<IHashMap> scanResultCache;
                config->GetScanResultCache((IHashMap**)&scanResultCache);
                if (scanResultCache != NULL) {
                    AutoPtr<IInterface> obj;
                    scanResultCache->Get(CoreUtils::Convert(bssid), (IInterface**)&obj);
                    IScanResult* result = IScanResult::Probe(obj);
                    if (result != NULL) {
                        result->SetAutoJoinStatus(IScanResult::AUTO_ROAM_DISABLED + 1);
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::SaveWifiConfigBSSID(
    /* [in] */ IWifiConfiguration* config)
{
    // Sanity check the config is valid
    Int32 networkId;
    String ssid;
    if (config == NULL || ((config->GetNetworkId(&networkId), networkId) == IWifiConfiguration::INVALID_NETWORK_ID &&
            (config->GetSSID(&ssid), ssid).IsNull())) {
        return NOERROR;
    }

    // If an app specified a BSSID then dont over-write it
    String bssid;
    if (!((config->GetBSSID(&bssid), bssid).IsNull()) && !bssid.Equals(String("any"))) {
        return NOERROR;
    }

    // If autojoin specified a BSSID then write it in the network block
    String autoJoinBSSID;
    if (!((config->GetAutoJoinBSSID(&autoJoinBSSID), autoJoinBSSID).IsNull())) {
        String configKey;
        config->ConfigKey(&configKey);
        Loge(String("saveWifiConfig BSSID Setting BSSID for ") + configKey
                + String(" to ") + autoJoinBSSID);
        if (!mWifiNative->SetNetworkVariable(
                networkId,
                IWifiConfiguration::bssidVarName,
                autoJoinBSSID)) {
            Loge(String("failed to set BSSID: ") + autoJoinBSSID);
        } else if (autoJoinBSSID.Equals(String("any"))) {
            // Paranoia, we just want to make sure that we restore the config to normal
            mWifiNative->SaveConfig();
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::UpdateStatus(
    /* [in] */ Int32 netId,
    /* [in] */ NetworkInfoDetailedState state)
{
    if (netId != IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<IInterface> obj;
        mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        if (config == NULL) return NOERROR;
        switch (state) {
            case Elastos::Droid::Net::NetworkInfoState_CONNECTED:
                config->SetStatus(IWifiConfigurationStatus::CURRENT);
                //we successfully connected, hence remove the blacklist
                config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
                break;
            case Elastos::Droid::Net::NetworkInfoState_DISCONNECTED: {
                //If network is already disabled, keep the status
                Int32 status;
                config->GetStatus(&status);
                if (status == IWifiConfigurationStatus::CURRENT) {
                    config->SetStatus(IWifiConfigurationStatus::ENABLED);
                }
                break;
            }
            default:
                //do nothing, retain the existing state
                break;
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::ForgetNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (showNetworks) LocalLog(String("forgetNetwork"), netId);

    Boolean remove = RemoveConfigAndSendBroadcastIfNeeded(netId);
    if (!remove) {
        //success but we dont want to remove the network from supplicant conf file
        *result = TRUE;
        return NOERROR;
    }
    if (mWifiNative->RemoveNetwork(netId)) {
        mWifiNative->SaveConfig();
        *result = TRUE;
        return NOERROR;
    } else {
        Loge(String("Failed to remove network ") + StringUtils::ToString(netId));
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode WifiConfigStore::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 networkId;
    config->GetNetworkId(&networkId);
    if (showNetworks) LocalLog(String("addOrUpdateNetwork id="), networkId);
    //adding unconditional message to chase b/15111865
    //Log.e(TAG, " key=" + config.configKey() + " netId=" + Integer.toString(config.networkId)
    //        + " uid=" + Integer.toString(config.creatorUid)
    //        + "/" + Integer.toString(config.lastUpdateUid));
    AutoPtr<NetworkUpdateResult> res= AddOrUpdateNetworkNative(config, uid);
    Int32 resultNetworkId = res->GetNetworkId();

    if (resultNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<IInterface> obj;
        mConfiguredNetworks->Get(CoreUtils::Convert(resultNetworkId), (IInterface**)&obj);
        IWifiConfiguration* conf = IWifiConfiguration::Probe(obj);
        if (conf != NULL) {
            SendConfiguredNetworksChangedBroadcast(conf,
                res->IsNewNetwork() ? IWifiManager::CHANGE_REASON_ADDED :
                        IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
        }
    }
    *result = resultNetworkId;
    return NOERROR;
}

ECode WifiConfigStore::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (showNetworks) LocalLog(String("removeNetwork"), netId);
    Boolean ret = mWifiNative->RemoveNetwork(netId);
    if (ret) {
        RemoveConfigAndSendBroadcastIfNeeded(netId);
    }
    *result = ret;
    return NOERROR;
}

ECode WifiConfigStore::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean ret;
    EnableNetworkWithoutBroadcast(netId, disableOthers, &ret);
    if (disableOthers) {
        if (VDBG) LocalLog(String("enableNetwork(disableOthers=true) "), netId);
        SendConfiguredNetworksChangedBroadcast();
    } else {
        if (VDBG) LocalLog(String("enableNetwork(disableOthers=false) "), netId);
        AutoPtr<IWifiConfiguration> enabledNetwork;
        {    AutoLock syncLock(mConfiguredNetworks);
            AutoPtr<IInterface> obj;
            mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
            enabledNetwork = IWifiConfiguration::Probe(obj);
        }
        // check just in case the network was removed by someone else.
        if (enabledNetwork != NULL) {
            SendConfiguredNetworksChangedBroadcast(enabledNetwork,
                    IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
        }
    }
    *result = ret;
    return NOERROR;
}

ECode WifiConfigStore::EnableNetworkWithoutBroadcast(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean ret = mWifiNative->EnableNetwork(netId, disableOthers);

    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) config->SetStatus(IWifiConfigurationStatus::ENABLED);

    if (disableOthers) {
        MarkAllNetworksDisabledExcept(netId);
    }
    *result = ret;
    return NOERROR;
}

ECode WifiConfigStore::DisableAllNetworks()
{
    if (VDBG) LocalLog(String("disableAllNetworks"));
    Boolean networkDisabled = FALSE;
    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        Int32 status;
        if(config != NULL && (config->GetStatus(&status), status) != IWifiConfigurationStatus::DISABLED) {
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if(mWifiNative->DisableNetwork(networkId)) {
                networkDisabled = TRUE;
                config->SetStatus(IWifiConfigurationStatus::DISABLED);
            } else {
                Loge(String("Disable network failed on ") + StringUtils::ToString(networkId));
            }
        }
    }

    if (networkDisabled) {
        SendConfiguredNetworksChangedBroadcast();
    }
    return NOERROR;
}

ECode WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    return DisableNetwork(netId, IWifiConfiguration::DISABLED_UNKNOWN_REASON, result);
}

ECode WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Int32 reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) LocalLog(String("disableNetwork"), netId);
    Boolean ret = mWifiNative->DisableNetwork(netId);
    AutoPtr<IWifiConfiguration> network;

    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

    if (VDBG) {
        if (config != NULL) {
            Loge(String("disableNetwork netId=") + StringUtils::ToString(netId));
                    //+ " SSID=" + config.SSID
                    //+ " disabled=" + (config.status == Status.DISABLED)
                    //+ " reason=" + Integer.toString(config.disableReason));
        }
    }
    /* Only change the reason if the network was not previously disabled
     * and the reason is not DISABLED_BY_WIFI_MANAGER, that is, if a 3rd party
     * set its configuration as disabled, then leave it disabled */
    Int32 status, disableReason;
    if (config != NULL && (config->GetStatus(&status), status) != IWifiConfigurationStatus::DISABLED
            && (config->GetDisableReason(&disableReason), disableReason) != IWifiConfiguration::DISABLED_BY_WIFI_MANAGER) {
        config->SetStatus(IWifiConfigurationStatus::DISABLED);
        config->SetDisableReason(reason);
        network = config;
    }
    if (reason == IWifiConfiguration::DISABLED_BY_WIFI_MANAGER) {
        // Make sure autojoin wont reenable this configuration without further user
        // intervention
        config->SetStatus(IWifiConfigurationStatus::DISABLED);
        config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_DISABLED_USER_ACTION);
    }
    if (network != NULL) {
        SendConfiguredNetworksChangedBroadcast(network,
                IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    }
    *result = ret;
    return NOERROR;
}

ECode WifiConfigStore::SaveConfig(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWifiNative->SaveConfig();
    return NOERROR;
}

ECode WifiConfigStore::StartWpsWithPinFromAccessPoint(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWpsResult> res;
    CWpsResult::New((IWpsResult**)&res);
    String bssid;
    config->GetBSSID(&bssid);
    String pin;
    config->GetPin(&pin);
    if (mWifiNative->StartWpsRegistrar(bssid, pin)) {
        /* WPS leaves all networks disabled */
        MarkAllNetworksDisabled();
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_SUCCESS);
    } else {
        Loge(String("Failed to start WPS pin method configuration"));
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_FAILURE);
    }
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::StartWpsWithPinFromDevice(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWpsResult> res;
    CWpsResult::New((IWpsResult**)&res);
    String bssid;
    config->GetBSSID(&bssid);
    String pin = mWifiNative->StartWpsPinDisplay(bssid);
    res->SetPin(pin);
    /* WPS leaves all networks disabled */
    if (!TextUtils::IsEmpty(pin)) {
        MarkAllNetworksDisabled();
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_SUCCESS);
    } else {
        Loge(String("Failed to start WPS pin method configuration"));
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_FAILURE);
    }
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::StartWpsPbc(
    /* [in] */ IWpsInfo* config,
    /* [out] */ IWpsResult** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IWpsResult> res;
    CWpsResult::New((IWpsResult**)&res);
    String bssid;
    config->GetBSSID(&bssid);
    if (mWifiNative->StartWpsPbc(bssid)) {
        /* WPS leaves all networks disabled */
        MarkAllNetworksDisabled();
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_SUCCESS);
    } else {
        Loge(String("Failed to start WPS push button configuration"));
        res->SetStatus(Elastos::Droid::Wifi::WpsResultStatus_FAILURE);
    }

    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::GetStaticIpConfiguration(
    /* [in] */ Int32 netId,
    /* [out] */ IStaticIpConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        return config->GetStaticIpConfiguration(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode WifiConfigStore::SetStaticIpConfiguration(
    /* [in] */ Int32 netId,
    /* [in] */ IStaticIpConfiguration* staticIpConfiguration)
{
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        return config->SetStaticIpConfiguration(staticIpConfiguration);
    }
    return NOERROR;
}

ECode WifiConfigStore::SetDefaultGwMacAddress(
    /* [in] */ Int32 netId,
    /* [in] */ const String& macAddress)
{
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        //update defaultGwMacAddress
        return config->SetDefaultGwMacAddress(macAddress);
    }
    return NOERROR;
}

ECode WifiConfigStore::GetProxyProperties(
    /* [in] */ Int32 netId,
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        return config->GetHttpProxy(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode WifiConfigStore::IsUsingStaticIp(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        IpConfigurationIpAssignment ipcia;
        config->GetIpAssignment(&ipcia);
        if (ipcia == Elastos::Droid::Net::STATIC_IpAssignment) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode WifiConfigStore::LoadConfiguredNetworks()
{
    String listStr = mWifiNative->ListNetworks();
    mLastPriority = 0;

    mConfiguredNetworks->Clear();
    mNetworkIds->Clear();

    if (listStr.IsNull())
        return NOERROR;

    AutoPtr<ArrayOf<String> > lines;
    StringUtils::Split(listStr, String("\n"), (ArrayOf<String>**)&lines);

    if (showNetworks) {
        LocalLog(String("WifiConfigStore: loadConfiguredNetworks:  "));
        for (Int32 i = 0; i < lines->GetLength(); ++i) {
            String net = (*lines)[i];
            LocalLog(net);
        }
    }

    // Skip the first line, which is a header
    for (Int32 i = 1; i < lines->GetLength(); i++) {
        AutoPtr<ArrayOf<String> > result;
        StringUtils::Split((*lines)[i], String("\t"), (ArrayOf<String>**)&result);
        // network-id | ssid | bssid | flags
        AutoPtr<IWifiConfiguration> config;
        CWifiConfiguration::New((IWifiConfiguration**)&config);
        //try {
        Int32 networkId;
        ECode ec = StringToIntegral::Parse((*result)[0], 10, &networkId);
        if (FAILED(ec)) {
            Loge(String("Failed to read network-id '") + (*result)[0] + String("'"));
            continue;
        }
        config->SetNetworkId(networkId);
        //} catch(NumberFormatException e) {
        //    loge("Failed to read network-id '" + result[0] + "'");
        //    continue;
        //}
        if (result->GetLength() > 3) {
            if ((*result)[3].IndexOf("[CURRENT]") != -1)
                config->SetStatus(IWifiConfigurationStatus::CURRENT);
            else if ((*result)[3].IndexOf("[DISABLED]") != -1)
                config->SetStatus(IWifiConfigurationStatus::DISABLED);
            else
                config->SetStatus(IWifiConfigurationStatus::ENABLED);
        } else {
            config->SetStatus(IWifiConfigurationStatus::ENABLED);
        }

        ReadNetworkVariables(config);

        String ssid;
        config->GetSSID(&ssid);
        String psk = ReadNetworkVariableFromSupplicantFile(ssid, String("psk"));
        if (!psk.IsNull() && psk.Equals(DELETED_CONFIG_PSK)) {
            // This is a config we previously deleted, ignore it
            if (showNetworks) {
                LocalLog(String("found deleted network ") + ssid);// + " ", config.networkId);
            }
            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_DELETED);
            config->SetPriority(0);
        }

        Int32 priority;
        config->GetPriority(&priority);
        if (priority > mLastPriority) {
            mLastPriority = priority;
        }

        config->SetIpAssignment(Elastos::Droid::Net::DHCP_IpAssignment);
        config->SetProxySettings(Elastos::Droid::Net::NONE_ProxySettings);

        Boolean containsKey;
        Int32 key = ConfigKey(config);
        Boolean isValid;
        if (mNetworkIds->ContainsKey(CoreUtils::Convert(key), &containsKey), containsKey) {
            // That SSID is already known, just ignore this duplicate entry
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if (showNetworks)
                LocalLog(String("Duplicate network found "), networkId);

            AutoPtr<IInterface> valueObj;
            mNetworkIds->Get(CoreUtils::Convert(key), (IInterface**)&valueObj);
            IInteger32* n = IInteger32::Probe(valueObj);
            AutoPtr<IInterface> obj;
            mConfiguredNetworks->Get(n, (IInterface**)&obj);
            IWifiConfiguration* tempCfg = IWifiConfiguration::Probe(obj);

            Int32 tempStatus, status;
            if ((tempCfg != NULL &&
                  (tempCfg->GetStatus(&tempStatus), tempStatus) != IWifiConfigurationStatus::CURRENT) &&
                  (config->GetStatus(&status), status) == IWifiConfigurationStatus::CURRENT) {

                // Clear the existing entry, we don't need it
                Int32 tempNetworkId;
                tempCfg->GetNetworkId(&tempNetworkId);
                mConfiguredNetworks->Remove(CoreUtils::Convert(tempNetworkId));
                Int32 tempkey = ConfigKey(tempCfg);
                mNetworkIds->Remove(CoreUtils::Convert(tempkey));

                // Add current entry to the list
                mConfiguredNetworks->Put(CoreUtils::Convert(networkId), config);
                mNetworkIds->Put(CoreUtils::Convert(ConfigKey(config)), CoreUtils::Convert(networkId));

                // Enable AutoJoin status and indicate the network as
                // duplicate The duplicateNetwork flag will be used
                // to decide whether to restore network configurations
                // in readNetworkHistory() along with IP and proxy settings
                config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
                config->SetDuplicateNetwork(TRUE);
            }
        } else if (config->IsValid(&isValid), isValid) {
            Int32 networkId;
            config->GetNetworkId(&networkId);
            mConfiguredNetworks->Put(CoreUtils::Convert(networkId), config);
            mNetworkIds->Put(CoreUtils::Convert(ConfigKey(config)), CoreUtils::Convert(networkId));
            if (showNetworks) LocalLog(String("loaded configured network"), networkId);
        } else {
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if (showNetworks) Log(String("Ignoring loaded configured for network ") + StringUtils::ToString(networkId)
                + String(" because config are not valid"));
        }
    }

    ReadIpAndProxyConfigurations();
    ReadNetworkHistory();
    ReadAutoJoinConfig();

    SendConfiguredNetworksChangedBroadcast();

    Int32 networksIdsSize;
    mNetworkIds->GetSize(&networksIdsSize);
    if (showNetworks) LocalLog(String("loadConfiguredNetworks loaded ") +
            StringUtils::ToString(networksIdsSize) + String(" networks"));

    if (networksIdsSize == 0) {
        // no networks? Lets log if the wpa_supplicant.conf file contents
        AutoPtr<IBufferedReader> reader;
        //try {
        AutoPtr<IFileReader> fileReader;
        CFileReader::New(SUPPLICANT_CONFIG_FILE, (IFileReader**)&fileReader);
        CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&reader);
            if (DBG) {
                LocalLog(String("--- Begin wpa_supplicant.conf Contents ---"), TRUE);
                String line;
                for (reader->ReadLine(&line); !line.IsNull(); reader->ReadLine(&line)) {
                    LocalLog(line, TRUE);
                }
                LocalLog(String("--- End wpa_supplicant.conf Contents ---"), TRUE);
            }
        //} catch (FileNotFoundException e) {
        //    localLog("Could not open " + SUPPLICANT_CONFIG_FILE + ", " + e, true);
        //} catch (IOException e) {
        //    localLog("Could not read " + SUPPLICANT_CONFIG_FILE + ", " + e, true);
        //} finally {
        //    try {
                if (reader != NULL) {
                    ICloseable::Probe(reader)->Close();
                }
        //    } catch (IOException e) {
                // Just ignore the fact that we couldn't close
        //    }
        //}
    }
    return NOERROR;
}

ECode WifiConfigStore::GetNetworkIdFromSsid(
    /* [in] */ const String& ssid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 networkId = 0;
    String listStr = mWifiNative->ListNetworks();
    if (VDBG) Loge(String("getNetworkIdFromSsid ") + ssid);
    if (listStr.IsNull()) {
        *result = -1;
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > lines;
    StringUtils::Split(listStr, String("\n"), (ArrayOf<String>**)&lines);

    /* Skip the first line, which is a header */
    for (Int32 i = 1; i < lines->GetLength(); i++) {
        AutoPtr<ArrayOf<String> > line;
        StringUtils::Split((*lines)[i], String("\t"), (ArrayOf<String>**)&line);
        if (VDBG) Loge(String("getNetworkIdFromSsid ") + (*line)[1]);
        /* network-id | ssid | bssid | flags */

        if ((*line)[1].Equals(ssid)) {
            //try {
            ECode ec = StringToIntegral::Parse((*line)[0], 10, &networkId);
            if (FAILED(ec)) {
                Loge(String("Failed to read network-id '") + (*line)[0] + String("'"));
                continue;
            }
            if (VDBG) Loge(String("getNetworkIdFromSsid ") + StringUtils::ToString(networkId));
            *result = networkId;
            return NOERROR;
            //} catch(NumberFormatException e) {
            //    loge("Failed to read network-id '" + line[0] + "'");
            //    continue;
            //}
        }
    }
    *result = -1;
    return NOERROR;
}

ECode WifiConfigStore::NeedsUnlockedKeyStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Any network using certificates to authenticate access requires
    // unlocked key store; unless the certificates can be stored with
    // hardware encryption

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        AutoPtr<IBitSet> allowedKeyManagement;
        config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
        Boolean wpa_eap, ieee80211x;
        if ((allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &wpa_eap), wpa_eap)
                && (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &ieee80211x), ieee80211x)) {

            AutoPtr<IWifiEnterpriseConfig> econfig;
            config->GetEnterpriseConfig((IWifiEnterpriseConfig**)&econfig);
            if (NeedsSoftwareBackedKeyStore(econfig)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode WifiConfigStore::WriteKnownNetworkHistory()
{
    Boolean needUpdate = FALSE;

    /* Make a copy */
    //final List<WifiConfiguration> networks = new ArrayList<WifiConfiguration>();
    AutoPtr<IList> networks;
    CArrayList::New((IList**)&networks);

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        AutoPtr<IWifiConfiguration> newConfig;
        CWifiConfiguration::New(config, (IWifiConfiguration**)&newConfig);
        networks->Add(newConfig);
        Boolean dirty;
        config->GetDirty(&dirty);
        if (dirty == TRUE) {
            config->SetDirty(FALSE);
            needUpdate = TRUE;
        }
    }
    if (VDBG) {
        Int32 size;
        mConfiguredNetworks->GetSize(&size);
        Loge(String(" writeKnownNetworkHistory() num networks:")
                + StringUtils::ToString(size) + " needWrite=" + StringUtils::ToString(needUpdate));
    }
    if (needUpdate == FALSE) {
        return NOERROR;
    }

    AutoPtr<IDelayedDiskWriteWriter> ddww = new InnerDelayedDiskWriteWriter(this, networks);
    mWriter->Write(networkHistoryConfigFile, ddww);
    return NOERROR;
}

ECode WifiConfigStore::SetLastSelectedConfiguration(
    /* [in] */ Int32 netId)
{
    if (DBG) {
        Loge(String("setLastSelectedConfiguration ") + StringUtils::ToString(netId));
    }
    if (netId == IWifiConfiguration::INVALID_NETWORK_ID) {
        lastSelectedConfiguration = String(NULL);
    } else {
        AutoPtr<IWifiConfiguration> selected;
        GetWifiConfiguration(netId, (IWifiConfiguration**)&selected);
        if (selected == NULL) {
            lastSelectedConfiguration = String(NULL);
        } else {
            selected->ConfigKey(&lastSelectedConfiguration);
            selected->SetNumConnectionFailures(0);
            selected->SetNumIpConfigFailures(0);
            selected->SetNumAuthFailures(0);
            if (VDBG) {
                Loge(String("setLastSelectedConfiguration now: ") + lastSelectedConfiguration);
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::GetLastSelectedConfiguration(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = lastSelectedConfiguration;
    return NOERROR;
}

ECode WifiConfigStore::IsLastSelectedConfiguration(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String configKey;
    *result = (!lastSelectedConfiguration.IsNull()
            && config != NULL
            && lastSelectedConfiguration.Equals((config->ConfigKey(&configKey), configKey)));
    return NOERROR;
}

ECode WifiConfigStore::LinkConfiguration(
    /* [in] */ IWifiConfiguration* config)
{
    AutoPtr<IHashMap> scanResultCache;
    config->GetScanResultCache((IHashMap**)&scanResultCache);
    Int32 size;
    if (scanResultCache != NULL && (scanResultCache->GetSize(&size), size) > 6) {
        // Ignore configurations with large number of BSSIDs
        return NOERROR;
    }
    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    Boolean wpa_psk;
    if (!(allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &wpa_psk), wpa_psk)) {
        // Only link WPA_PSK config
        return NOERROR;
    }

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* link = IWifiConfiguration::Probe(obj);

        Boolean doLink = FALSE;

        String linkConfigKey, configConfigKey;
        link->ConfigKey(&linkConfigKey);
        config->ConfigKey(&configConfigKey);
        if (linkConfigKey.Equals(configConfigKey)) {
            continue;
        }

        Int32 autoJoinStatus;
        link->GetAutoJoinStatus(&autoJoinStatus);
        if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DELETED) {
            continue;
        }

        // Autojoin will be allowed to dynamically jump from a linked configuration
        // to another, hence only link configurations that have equivalent level of security
        AutoPtr<IBitSet> linkAllowedKeyManagement;
        link->GetAllowedKeyManagement((IBitSet**)&linkAllowedKeyManagement);
        Boolean eq;
        if (!(linkAllowedKeyManagement->Equals(allowedKeyManagement, &eq), eq)) {
            continue;
        }

        AutoPtr<IHashMap> linkScanResultCache;
        link->GetScanResultCache((IHashMap**)&linkScanResultCache);
        Int32 linkSize;
        if (linkScanResultCache != NULL && (linkScanResultCache->GetSize(&linkSize), linkSize) > 6) {
            // Ignore configurations with large number of BSSIDs
            continue;
        }

        String configDefaultGwMacAddress, linkDefaultGwMacAddress;
        config->GetDefaultGwMacAddress(&configDefaultGwMacAddress);
        link->GetDefaultGwMacAddress(&linkDefaultGwMacAddress);
        if (!configDefaultGwMacAddress.IsNull() && !linkDefaultGwMacAddress.IsNull()) {
            // If both default GW are known, link only if they are equal
            if (configDefaultGwMacAddress.Equals(linkDefaultGwMacAddress)) {
                if (VDBG) {
                    String linkSSID;
                    link->GetSSID(&linkSSID);
                    String configSSID;
                    config->GetSSID(&configSSID);
                    Loge(String("linkConfiguration link due to same gw ") + linkSSID +
                            String(" and ") + configSSID + String(" GW ") + configDefaultGwMacAddress);
                }
                doLink = TRUE;
            }
        } else {
            // We do not know BOTH default gateways hence we will try to link
            // hoping that WifiConfigurations are indeed behind the same gateway.
            // once both WifiConfiguration have been tried and thus once both efault gateways
            // are known we will revisit the choice of linking them
            if ((scanResultCache != NULL) && ((scanResultCache->GetSize(&size), size) <= 6)
                    && (linkScanResultCache != NULL) && ((linkScanResultCache->GetSize(&linkSize), linkSize) <= 6)) {
                AutoPtr<ISet> keySet;
                scanResultCache->GetKeySet((ISet**)&keySet);
                AutoPtr<IIterator> keyIter;
                keySet->GetIterator((IIterator**)&keyIter);
                Boolean isflag = FALSE;
                while ((keyIter->HasNext(&isflag), isflag)) {
                    AutoPtr<IInterface> keyObj;
                    keyIter->GetNext((IInterface**)&keyObj);
                    String abssid;
                    ICharSequence::Probe(keyObj)->ToString(&abssid);

                    AutoPtr<ISet> keySet2;
                    linkScanResultCache->GetKeySet((ISet**)&keySet2);
                    AutoPtr<IIterator> keyIter2;
                    keySet2->GetIterator((IIterator**)&keyIter2);
                    Boolean isflag2 = FALSE;
                    while ((keyIter2->HasNext(&isflag2), isflag2)) {
                        AutoPtr<IInterface> keyObj2;
                        keyIter2->GetNext((IInterface**)&keyObj2);
                        String bbssid;
                        ICharSequence::Probe(keyObj2)->ToString(&bbssid);
                        if (VVDBG) {
                            Loge(String("linkConfiguration try to link due to DBDC BSSID match "));
                                    //+ link.SSID +
                                    //" and " + config.SSID + " bssida " + abssid
                                    //+ " bssidb " + bbssid);
                        }
                        if (abssid.RegionMatches(/*TODO TRUE,*/ 0, bbssid, 0, 16)) {
                            // If first 16 ascii characters of BSSID matches,
                            // we assume this is a DBDC
                            doLink = TRUE;
                        }
                    }
                }
            }
        }

        if (doLink == TRUE && onlyLinkSameCredentialConfigurations) {
            String linkSSID, configSSID;
            link->GetSSID(&linkSSID);
            config->GetSSID(&configSSID);
            String apsk = ReadNetworkVariableFromSupplicantFile(linkSSID, String("psk"));
            String bpsk = ReadNetworkVariableFromSupplicantFile(configSSID, String("psk"));
            if (apsk.IsNull() || bpsk.IsNull()
                    || TextUtils::IsEmpty(apsk) || TextUtils::IsEmpty(apsk)
                    || apsk.Equals(String("*")) || apsk.Equals(DELETED_CONFIG_PSK)
                    || !apsk.Equals(bpsk)) {
                doLink = FALSE;
            }
        }

        if (doLink) {
            if (VDBG) {
               Loge(String("linkConfiguration: will link "));
                       //+ link.configKey()
                       //+ " and " + config.configKey());
            }
            AutoPtr<IHashMap> linkLinkedConfigurations;
            link->GetLinkedConfigurations((IHashMap**)&linkLinkedConfigurations);
            if (linkLinkedConfigurations == NULL) {
                CHashMap::New((IHashMap**)&linkLinkedConfigurations);
                link->SetLinkedConfigurations(linkLinkedConfigurations);
            }
            AutoPtr<IHashMap> configLinkedConfigurations;
            config->GetLinkedConfigurations((IHashMap**)&configLinkedConfigurations);
            if (configLinkedConfigurations == NULL) {
                CHashMap::New((IHashMap**)&configLinkedConfigurations);
                config->SetLinkedConfigurations(configLinkedConfigurations);
            }
            String configConfigKey;
            config->ConfigKey(&configConfigKey);
            AutoPtr<IInterface> lcObj;
            linkLinkedConfigurations->Get(CoreUtils::Convert(configConfigKey), (IInterface**)&lcObj);
            if (lcObj == NULL) {
                linkLinkedConfigurations->Put(CoreUtils::Convert(configConfigKey), CoreUtils::Convert(1));
                link->SetDirty(TRUE);
            }

            String linkConfigKey;
            link->ConfigKey(&linkConfigKey);
            AutoPtr<IInterface> lcObj2;
            configLinkedConfigurations->Get(CoreUtils::Convert(linkConfigKey), (IInterface**)&lcObj2);
            if (lcObj2 == NULL) {
                configLinkedConfigurations->Put(CoreUtils::Convert(linkConfigKey), CoreUtils::Convert(1));
                config->SetDirty(TRUE);
            }
        } else {
            String configConfigKey;
            config->ConfigKey(&configConfigKey);
            String linkConfigKey;
            link->ConfigKey(&linkConfigKey);
            AutoPtr<IHashMap> linkLinkedConfigurations;
            link->GetLinkedConfigurations((IHashMap**)&linkLinkedConfigurations);
            AutoPtr<IInterface> lcObj1;
            if (linkLinkedConfigurations != NULL
                    && ((linkLinkedConfigurations->Get(CoreUtils::Convert(configConfigKey), (IInterface**)&lcObj1), lcObj1) != NULL)) {
                if (VDBG) {
                    Loge(String("linkConfiguration: un-link ") + configConfigKey
                            + String(" from ") + linkConfigKey);
                }
                link->SetDirty(TRUE);
                linkLinkedConfigurations->Remove(CoreUtils::Convert(configConfigKey));
            }

            AutoPtr<IHashMap> configLinkedConfigurations;
            config->GetLinkedConfigurations((IHashMap**)&configLinkedConfigurations);
            AutoPtr<IInterface> lcObj2;
            if (configLinkedConfigurations != NULL
                    && ((configLinkedConfigurations->Get(CoreUtils::Convert(linkConfigKey), (IInterface**)&lcObj2), lcObj2) != NULL)) {
                if (VDBG) {
                    Loge(String("linkConfiguration: un-link ") + linkConfigKey
                            + String(" from ") + configConfigKey);
                }
                config->SetDirty(TRUE);
                configLinkedConfigurations->Remove(CoreUtils::Convert(linkConfigKey));
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::AssociateWithConfiguration(
    /* [in] */ IScanResult* result,
    /* [out] */ IWifiConfiguration** wifiConfig)
{
    VALIDATE_NOT_NULL(wifiConfig);
    AutoPtr<IWifiConfigurationHelper> wcHelper;
    CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&wcHelper);
    String configKey;
    wcHelper->ConfigKey(result, &configKey);
    if (configKey.IsNull()) {
        if (DBG) Loge(String("associateWithConfiguration(): no config key " ));
        *wifiConfig = NULL;
        return NOERROR;
    }

    // Need to compare with quoted string
    String ssid;
    result->GetSSID(&ssid);
    String SSID = String("\"") + ssid + String("\"");

    if (VVDBG) {
        Loge(String("associateWithConfiguration(): try ") + configKey);
    }

    AutoPtr<IWifiConfiguration> config;

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* link = IWifiConfiguration::Probe(obj);

        Boolean doLink = FALSE;

        Int32 autoJoinStatus;
        link->GetAutoJoinStatus(&autoJoinStatus);
        Boolean selfAdded;
        link->GetSelfAdded(&selfAdded);

        String linkConfigKey;
        link->ConfigKey(&linkConfigKey);
        if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DELETED || selfAdded) {
            if (VVDBG) Loge(String("associateWithConfiguration(): skip selfadd ") + linkConfigKey);
            // Make sure we dont associate the scan result to a deleted config
            continue;
        }

        AutoPtr<IBitSet> linkAllowedKeyManagement;
        link->GetAllowedKeyManagement((IBitSet**)&linkAllowedKeyManagement);
        Boolean eq;
        if (!(linkAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &eq), eq)) {
            if (VVDBG) Loge(String("associateWithConfiguration(): skip non-PSK ") + linkConfigKey);
            // Make sure we dont associate the scan result to a non-PSK config
            continue;
        }

        if (configKey.Equals(linkConfigKey)) {
            if (VVDBG) Loge(String("associateWithConfiguration(): found it!!! ") + configKey );
            *wifiConfig = link; // Found it exactly
            REFCOUNT_ADD(*wifiConfig);
            return NOERROR;
        }

        AutoPtr<IHashMap> scanResultCache;
        link->GetScanResultCache((IHashMap**)&scanResultCache);
        Int32 size;
        if ((scanResultCache != NULL) && ((scanResultCache->GetSize(&size), size) <= 6)) {
            AutoPtr<ISet> keySet;
            scanResultCache->GetKeySet((ISet**)&keySet);
            AutoPtr<IIterator> keyIter;
            keySet->GetIterator((IIterator**)&keyIter);
            Boolean isflag = FALSE;
            while ((keyIter->HasNext(&isflag), isflag)) {
                AutoPtr<IInterface> keyObj;
                keyIter->GetNext((IInterface**)&keyObj);
                String bssid;
                ICharSequence::Probe(keyObj)->ToString(&bssid);

                String resultBssid;
                result->GetBSSID(&resultBssid);
                String linkSSID;
                link->GetSSID(&linkSSID);
                if (resultBssid.RegionMatches(/*TODO TRUE, */0, bssid, 0, 16)
                        && SSID.RegionMatches(/*TODO false, */0, linkSSID, 0, 4)) {
                    // If first 16 ascii characters of BSSID matches, and first 3
                    // characters of SSID match, we assume this is a home setup
                    // and thus we will try to transfer the password from the known
                    // BSSID/SSID to the recently found BSSID/SSID

                    // If (VDBG)
                    //    loge("associateWithConfiguration OK " );
                    doLink = TRUE;
                    break;
                }
            }
        }

        if (doLink) {
            // Try to make a non verified WifiConfiguration, but only if the original
            // configuration was not self already added
            if (VDBG) {
                Loge(String("associateWithConfiguration: will create "));
                        //+ result.SSID + " and associate it with: " + link.SSID);
            }
            WifiConfigurationFromScanResult(result, (IWifiConfiguration**)&config);
            if (config != NULL) {
                config->SetSelfAdded(TRUE);
                config->SetDidSelfAdd(TRUE);
                config->SetDirty(TRUE);
                String linkConfigKey;
                link->ConfigKey(&linkConfigKey);
                config->SetPeerWifiConfiguration(linkConfigKey);

                AutoPtr<IBitSet> configAllowedKeyManagement;
                config->GetAllowedKeyManagement((IBitSet**)&configAllowedKeyManagement);
                Boolean eq1, eq2;
                if ((configAllowedKeyManagement->Equals(linkAllowedKeyManagement, &eq1), eq1) &&
                        (configAllowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &eq2), eq2)) {
                    // Transfer the credentials from the configuration we are linking from
                    String linkSSID;
                    link->GetSSID(&linkSSID);
                    String psk = ReadNetworkVariableFromSupplicantFile(linkSSID, String("psk"));
                    if (!psk.IsNull()) {
                        config->SetPreSharedKey(psk);
                        if (VDBG) {
                            //if (config.preSharedKey != null)
                            Loge(String(" transfer PSK : ") + psk);//config.preSharedKey);
                        }

                        // Link configurations
                        AutoPtr<IHashMap> linkLinkedConfigurations;
                        link->GetLinkedConfigurations((IHashMap**)&linkLinkedConfigurations);
                        if (linkLinkedConfigurations == NULL) {
                            CHashMap::New((IHashMap**)&linkLinkedConfigurations);
                            link->SetLinkedConfigurations(linkLinkedConfigurations);
                        }
                        AutoPtr<IHashMap> configLinkedConfigurations;
                        config->GetLinkedConfigurations((IHashMap**)&configLinkedConfigurations);
                        if (configLinkedConfigurations == NULL) {
                            CHashMap::New((IHashMap**)&configLinkedConfigurations);
                            config->SetLinkedConfigurations(configLinkedConfigurations);
                        }

                        String configConfigKey;
                        config->ConfigKey(&configConfigKey);
                        linkLinkedConfigurations->Put(CoreUtils::Convert(configConfigKey), CoreUtils::Convert(1));
                        configLinkedConfigurations->Put(CoreUtils::Convert(linkConfigKey), CoreUtils::Convert(1));
                    } else {
                        config = NULL;
                    }
                } else {
                    config = NULL;
                }
                if (config != NULL) break;
            }
        }
    }
    *wifiConfig = config;
    REFCOUNT_ADD(*wifiConfig);
    return NOERROR;
}

ECode WifiConfigStore::MakeChannelList(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 age,
    /* [in] */ Boolean _restrict,
    /* [out] */ IHashSet** result)
{
    VALIDATE_NOT_NULL(result);
    if (config == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now_ms;
    system->GetCurrentTimeMillis(&now_ms);

    //HashSet<Integer> channels = new HashSet<Integer>();
    AutoPtr<IHashSet> channels;
    CHashSet::New((IHashSet**)&channels);

    //get channels for this configuration, if there are at least 2 BSSIDs
    AutoPtr<IHashMap> scanResultCache;
    config->GetScanResultCache((IHashMap**)&scanResultCache);
    AutoPtr<IHashMap> linkedConfigurations;
    config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
    if (scanResultCache == NULL && linkedConfigurations == NULL) {
        *result = NULL;
        return NOERROR;
    }

    String configKey;
    config->ConfigKey(&configKey);
    Int32 cacheSize;
    scanResultCache->GetSize(&cacheSize);
    Int32 configurationSize;
    linkedConfigurations->GetSize(&configurationSize);
    if (VDBG) {
        AutoPtr<StringBuilder> dbg = new StringBuilder();
        dbg->Append("makeChannelList age=");
        dbg->Append(age);
        dbg->Append(" for ");
        dbg->Append(configKey);
        dbg->Append(" max=");
        dbg->Append(maxNumActiveChannelsForPartialScans);
        if (scanResultCache != NULL) {
            dbg->Append(" bssids=");
            dbg->Append(cacheSize);
        }
        if (linkedConfigurations != NULL) {
            dbg->Append(" linked=");
            dbg->Append(configurationSize);
        }
        Loge(dbg->ToString());
    }

    Int32 numChannels = 0;
    if (scanResultCache != NULL && cacheSize > 0) {
        AutoPtr<ICollection> values;
        scanResultCache->GetValues((ICollection**)&values);
        AutoPtr<IIterator> iter;
        values->GetIterator((IIterator**)&iter);
        Boolean bNext;
        iter->HasNext(&bNext);
        for (; bNext; iter->HasNext(&bNext)) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            IScanResult* result = IScanResult::Probe(obj);
            //TODO : cout active and passive channels separately
            if (numChannels > maxNumActiveChannelsForPartialScans) {
                break;
            }
            Int64 seen;
            result->GetSeen(&seen);
            if (VDBG) {
                //Boolean test = (now_ms - seen) < age;
                Loge(String("has freq= age= ?= TODO"));
                        //+ result.BSSID + " freq=" + Integer.toString(result.frequency)
                        //+ " age=" + Long.toString(now_ms - result.seen) + " ?=" + test);
            } if (((now_ms - seen) < age)/*||(!_restrict || result.is24GHz())*/) {
                Int32 frequency;
                result->GetFrequency(&frequency);
                channels->Add(CoreUtils::Convert(frequency));
                numChannels++;
            }
        }
    }

    //get channels for linked configurations
    if (linkedConfigurations != NULL) {
        AutoPtr<ISet> keySet;
        linkedConfigurations->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> keyIter;
        keySet->GetIterator((IIterator**)&keyIter);
        Boolean isflag = FALSE;
        while ((keyIter->HasNext(&isflag), isflag)) {
            AutoPtr<IInterface> keyObj;
            keyIter->GetNext((IInterface**)&keyObj);
            String key;
            ICharSequence::Probe(keyObj)->ToString(&key);
            AutoPtr<IWifiConfiguration> linked;
            GetWifiConfiguration(key, (IWifiConfiguration**)&linked);
            if (linked == NULL)
                continue;

            AutoPtr<IHashMap> linkedScanResultCache;
            linked->GetScanResultCache((IHashMap**)&linkedScanResultCache);
            if (linkedScanResultCache == NULL) {
                continue;
            }
            AutoPtr<ICollection> values;
            linkedScanResultCache->GetValues((ICollection**)&values);
            AutoPtr<IIterator> iter;
            values->GetIterator((IIterator**)&iter);
            Boolean bNext;
            iter->HasNext(&bNext);
            for (; bNext; iter->HasNext(&bNext)) {
                AutoPtr<IInterface> obj;
                iter->GetNext((IInterface**)&obj);
                IScanResult* result = IScanResult::Probe(obj);
                if (VDBG) {
                    Loge(String("has link: "));
                            //+ result.BSSID
                            //+ " freq=" + Integer.toString(result.frequency)
                            //+ " age=" + Long.toString(now_ms - result.seen));
                }
                if (numChannels > maxNumActiveChannelsForPartialScans) {
                    break;
                }
                Int64 seen;
                result->GetSeen(&seen);
                Int32 frequency;
                result->GetFrequency(&frequency);
                if (((now_ms - seen) < age)/*||(!_restrict || result.is24GHz())*/) {
                    channels->Add(CoreUtils::Convert(frequency));
                    numChannels++;
                }
            }
        }
    }
    *result = channels;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WifiConfigStore::UpdateSavedNetworkHistory(
    /* [in] */ IScanResult* scanResult,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 numConfigFound = 0;
    if (scanResult == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    String ssid;
    scanResult->GetSSID(&ssid);
    String SSID = String("\"") + ssid + String("\"");

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        Boolean found = FALSE;

        String configSSID;
        config->GetSSID(&configSSID);
        String configConfigKey;
        config->ConfigKey(&configConfigKey);
        String capabilities;
        scanResult->GetCapabilities(&capabilities);
        if (configSSID.IsNull() || !configSSID.Equals(SSID)) {
            // SSID mismatch
            if (VVDBG) {
                Loge(String("updateSavedNetworkHistory(): SSID mismatch ")
                        + configConfigKey
                        + " SSID=" + configSSID + " " + SSID);
            }
            continue;
        }
        if (VDBG) {
            Int32 autoJoinStatus;
            config->GetAutoJoinStatus(&autoJoinStatus);
            Loge(String("updateSavedNetworkHistory(): try ")
                    + configConfigKey
                    + " SSID=" + configSSID + " " + ssid
                    + " cap=" + capabilities
                    + " ajst=" + StringUtils::ToString(autoJoinStatus));
        }
        if (capabilities.Contains("WEP")
                && configConfigKey.Contains("WEP")) {
            found = TRUE;
        } else if (capabilities.Contains("PSK")
                && configConfigKey.Contains("PSK")) {
            found = TRUE;
        } else if (capabilities.Contains("EAP")
                && configConfigKey.Contains("EAP")) {
            found = TRUE;
        } else if (!capabilities.Contains("WEP")
            && !capabilities.Contains("PSK")
            && !capabilities.Contains("EAP")
            && !configConfigKey.Contains("WEP")
                && !configConfigKey.Contains("PSK")
                && !configConfigKey.Contains("EAP")) {
            found = TRUE;
        }

        if (found) {
            numConfigFound ++;

            Int32 autoJoinStatus;
            config->GetAutoJoinStatus(&autoJoinStatus);
            if (autoJoinStatus >= IWifiConfiguration::AUTO_JOIN_DELETED) {
                if (VVDBG) {
                    Loge(String("updateSavedNetworkHistory(): found a deleted, skip it...  configkey")
                            + configConfigKey);
                }
                // The scan result belongs to a deleted config:
                //   - increment numConfigFound to remember that we found a config
                //            matching for this scan result
                //   - dont do anything since the config was deleted, just skip...
                continue;
            }

            AutoPtr<IHashMap> scanResultCache;
            config->GetScanResultCache((IHashMap**)&scanResultCache);
            if (scanResultCache == NULL) {
                CHashMap::New((IHashMap**)&scanResultCache);
                config->SetScanResultCache(scanResultCache);
            }

            // Adding a new BSSID
            String bssid;
            scanResult->GetBSSID(&bssid);
            AutoPtr<IInterface> obj;
            scanResultCache->Get(CoreUtils::Convert(bssid), (IInterface**)&obj);
            IScanResult* sresult = IScanResult::Probe(obj);
            if (sresult == NULL) {
                config->SetDirty(TRUE);
            } else {
                // transfer the black list status
                Int32 autoJoinStatus;
                sresult->GetAutoJoinStatus(&autoJoinStatus);
                scanResult->SetAutoJoinStatus(autoJoinStatus);
                Int64 blackListTimestamp;
                sresult->GetBlackListTimestamp(&blackListTimestamp);
                scanResult->SetBlackListTimestamp(blackListTimestamp);
                Int32 numIpConfigFailures;
                sresult->GetNumIpConfigFailures(&numIpConfigFailures);
                scanResult->SetNumIpConfigFailures(numIpConfigFailures);
                Int32 numConnection;
                sresult->GetNumConnection(&numConnection);
                scanResult->SetNumConnection(numConnection);
                Int32 isAutoJoinCandidate;
                sresult->GetIsAutoJoinCandidate(&isAutoJoinCandidate);
                scanResult->SetIsAutoJoinCandidate(isAutoJoinCandidate);
            }

            // Add the scan result to this WifiConfiguration
            scanResultCache->Put(CoreUtils::Convert(bssid), scanResult);
            // Since we added a scan result to this configuration, re-attempt linking
            LinkConfiguration(config);
        }

        if (VDBG && found) {
            Int32 autoJoinStatus;
            scanResult->GetAutoJoinStatus(&autoJoinStatus);
            String status("");
            if (autoJoinStatus > 0) {
                status = String(" status=") + StringUtils::ToString(autoJoinStatus);
            }
            String bssid;
            scanResult->GetBSSID(&bssid);
            Loge(String("        got known scan result ")
                    + bssid  + " key : "
                    + configConfigKey
                    //+ " num: " +
                    //Integer.toString(config.scanResultCache.size())
                    //+ " rssi=" + Integer.toString(scanResult.level)
                    //+ " freq=" + Integer.toString(scanResult.frequency)
                    + status);
        }
    }
    *result = numConfigFound != 0;
    return NOERROR;
}

ECode WifiConfigStore::WifiConfigurationFromScanResult(
    /* [in] */ IScanResult* result,
    /* [out] */ IWifiConfiguration** wifiConfig)
{
    VALIDATE_NOT_NULL(wifiConfig);
    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);

    String ssid;
    result->GetSSID(&ssid);
    config->SetSSID(String("\"") + ssid + String("\""));

    if (VDBG) {
        Loge(String("WifiConfiguration from scan results "));
                //+ config.SSID + " cap " + result.capabilities);
    }
    String capabilities;
    result->GetCapabilities(&capabilities);
    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    AutoPtr<IBitSet> allowedAuthAlgorithms;
    config->GetAllowedAuthAlgorithms((IBitSet**)&allowedAuthAlgorithms);
    if (capabilities.Contains("WEP")) {
        allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::NONE);
        allowedAuthAlgorithms->Set(IWifiConfigurationAuthAlgorithm::OPEN); //?
        allowedAuthAlgorithms->Set(IWifiConfigurationAuthAlgorithm::SHARED);
    }

    if (capabilities.Contains("PSK")) {
        allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::WPA_PSK);
    }

    if (capabilities.Contains("EAP")) {
        //this is probably wrong, as we don't have a way to enter the enterprise config
        allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::WPA_EAP);
        allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::IEEE8021X);
    }

    AutoPtr<IHashMap> src;
    CHashMap::New((IHashMap**)&src);
    config->SetScanResultCache(src);
    if (src == NULL) {
        *wifiConfig = NULL;
        return NOERROR;
    }

    String bssid;
    result->GetBSSID(&bssid);
    src->Put(CoreUtils::Convert(bssid), result);

    *wifiConfig = config;
    REFCOUNT_ADD(*wifiConfig);
    return NOERROR;
}

ECode WifiConfigStore::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // pw.println("Dump of WifiConfigStore");
    // pw.println("mLastPriority " + mLastPriority);
    // pw.println("Configured networks");
    // for (WifiConfiguration conf : getConfiguredNetworks()) {
    //     pw.println(conf);
    // }
    // pw.println();
    //
    // if (mLocalLog != null) {
    //     pw.println("WifiConfigStore - Log Begin ----");
    //     mLocalLog.dump(fd, pw, args);
    //     pw.println("WifiConfigStore - Log End ----");
    // }
    assert(0);
    return NOERROR;
}

ECode WifiConfigStore::GetConfigFile(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ipConfigFile;
    return NOERROR;
}

Boolean WifiConfigStore::NeedsKeyStore(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    // Has no keys to be installed
    AutoPtr<IX509Certificate> client;
    config->GetClientCertificate((IX509Certificate**)&client);
    AutoPtr<IX509Certificate> ca;
    config->GetCaCertificate((IX509Certificate**)&ca);
    if (client == NULL && ca == NULL)
        return FALSE;
    return TRUE;
}

Boolean WifiConfigStore::IsHardwareBackedKey(
    /* [in] */ IPrivateKey* key)
{
    //TODO AutoPtr<IKeyChain> keyChain;
    assert(0);
    //TODO CKeyChain::AcquireSingleton((IKeyChain**)&keyChain);
    String algorithm;
    IKey::Probe(key)->GetAlgorithm(&algorithm);
    Boolean bTemp = FALSE;
    //TODO KeyChain->IsBoundKeyAlgorithm(algorithm, &bTemp);
    return bTemp;
}

Boolean WifiConfigStore::HasHardwareBackedKey(
    /* [in] */ ICertificate* certificate)
{
    //TODO AutoPtr<IKeyChain> keyChain;
    assert(0);
    //TODO CKeyChain::AcquireSingleton((IKeyChain**)&keyChain);
    AutoPtr<IPublicKey> publickKey;
    certificate->GetPublicKey((IPublicKey**)&publickKey);
    String algorithm;
    IKey::Probe(publickKey)->GetAlgorithm(&algorithm);
    Boolean bTemp = FALSE;
    //TODO KeyChain->IsBoundKeyAlgorithm(algorithm, &bTemp);
    return bTemp;
}

Boolean WifiConfigStore::NeedsSoftwareBackedKeyStore(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    String client;
    config->GetClientCertificateAlias(&client);
    if (!TextUtils::IsEmpty(client)) {
        // a valid client certificate is configured

        // BUGBUG: keyStore.get() never returns certBytes; because it is not
        // taking WIFI_UID as a parameter. It always looks for certificate
        // with SYSTEM_UID, and never finds any Wifi certificates. Assuming that
        // all certificates need software keystore until we get the get() API
        // fixed.

        return TRUE;
    }

    /*
    try {

        if (DBG) Slog.d(TAG, "Loading client certificate " + Credentials
                .USER_CERTIFICATE + client);

        CertificateFactory factory = CertificateFactory.getInstance("X.509");
        if (factory == null) {
            Slog.e(TAG, "Error getting certificate factory");
            return;
        }

        byte[] certBytes = keyStore.get(Credentials.USER_CERTIFICATE + client);
        if (certBytes != null) {
            Certificate cert = (X509Certificate) factory.generateCertificate(
                    new ByteArrayInputStream(certBytes));

            if (cert != null) {
                mNeedsSoftwareKeystore = hasHardwareBackedKey(cert);

                if (DBG) Slog.d(TAG, "Loaded client certificate " + Credentials
                        .USER_CERTIFICATE + client);
                if (DBG) Slog.d(TAG, "It " + (mNeedsSoftwareKeystore ? "needs" :
                        "does not need" ) + " software key store");
            } else {
                Slog.d(TAG, "could not generate certificate");
            }
        } else {
            Slog.e(TAG, "Could not load client certificate " + Credentials
                    .USER_CERTIFICATE + client);
            mNeedsSoftwareKeystore = true;
        }

    } catch(CertificateException e) {
        Slog.e(TAG, "Could not read certificates");
        mCaCert = null;
        mClientCertificate = null;
    }
    */
    return FALSE;
}

ECode WifiConfigStore::HandleBadNetworkDisconnectReport(
    /* [in] */ Int32 netId,
    /* [in] */ IWifiInfo* info)
{
    /* TODO verify the bad network is current */
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        Int32 rssi;
        info->GetRssi(&rssi);
        Boolean is24GHz;
        info->Is24GHz(&is24GHz);
        Boolean is5GHz;
        info->Is5GHz(&is5GHz);
        if ((rssi < IWifiConfiguration::UNWANTED_BLACKLIST_SOFT_RSSI_24
                && is24GHz) || (rssi <
                        IWifiConfiguration::UNWANTED_BLACKLIST_SOFT_RSSI_5 && is5GHz)) {
            // We got disconnected and RSSI was bad, so disable light
            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_TEMPORARY_DISABLED
                    + IWifiConfiguration::UNWANTED_BLACKLIST_SOFT_BUMP);
            Loge(String("handleBadNetworkDisconnectReport (+4) "));
                    //+ Integer.toString(netId) + " " + info);
        } else {
            // We got disabled but RSSI is good, so disable hard
            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_TEMPORARY_DISABLED
                    + IWifiConfiguration::UNWANTED_BLACKLIST_HARD_BUMP);
            Loge(String("handleBadNetworkDisconnectReport (+8) "));
                    //+ Integer.toString(netId) + " " + info);
        }
    }
    // Record last time Connectivity Service switched us away from WiFi and onto Cell
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&lastUnwantedNetworkDisconnectTimestamp);
    return NOERROR;
}

ECode WifiConfigStore::HandleBSSIDBlackList(
    /* [in] */ Int32 netId,
    /* [in] */ const String& BSSID,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean found = FALSE;
    if (BSSID.IsNull()) {
        *result = found;
        return NOERROR;
    }

    // Look for the BSSID in our config store
    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        AutoPtr<IHashMap> scanResultCache;
        config->GetScanResultCache((IHashMap**)&scanResultCache);
        if (scanResultCache != NULL) {
            AutoPtr<ICollection> cacheValues;
            scanResultCache->GetValues((ICollection**)&cacheValues);
            AutoPtr<IIterator> cacheIter;
            cacheValues->GetIterator((IIterator**)&cacheIter);
            Boolean bCacheNext;
            cacheIter->HasNext(&bCacheNext);
            for (; bCacheNext; cacheIter->HasNext(&bCacheNext)) {
                AutoPtr<IInterface> cacheObj;
                cacheIter->GetNext((IInterface**)&cacheObj);
                IScanResult* sresult = IScanResult::Probe(cacheObj);
                String bssid;
                sresult->GetBSSID(&bssid);
                if (bssid.Equals(BSSID)) {
                    if (enable) {
                        sresult->SetAutoJoinStatus(IScanResult::ENABLED);
                    } else {
                        // Black list the BSSID we were trying to join
                        // so as the Roam state machine
                        // doesn't pick it up over and over
                        sresult->SetAutoJoinStatus(IScanResult::AUTO_ROAM_DISABLED);
                        found = TRUE;
                    }
                }
            }
        }
    }
    *result = found;
    return NOERROR;
}

ECode WifiConfigStore::HandleDisabledAPs(
    /* [in] */ Boolean enable,
    /* [in] */ const String& BSSID,
    /* [in] */ Int32 reason)
{
    if (BSSID.IsNull())
        return NOERROR;
    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
        AutoPtr<IHashMap> scanResultCache;
        config->GetScanResultCache((IHashMap**)&scanResultCache);

        if (scanResultCache != NULL) {
            AutoPtr<ICollection> cacheValues;
            scanResultCache->GetValues((ICollection**)&cacheValues);
            AutoPtr<IIterator> cacheIter;
            cacheValues->GetIterator((IIterator**)&cacheIter);
            Boolean bCacheNext;
            cacheIter->HasNext(&bCacheNext);
            for (; bCacheNext; cacheIter->HasNext(&bCacheNext)) {
                AutoPtr<IInterface> cacheObj;
                cacheIter->GetNext((IInterface**)&cacheObj);
                IScanResult* sresult = IScanResult::Probe(cacheObj);

                String bssid;
                sresult->GetBSSID(&bssid);
                if (bssid.Equals(BSSID)) {
                    if (enable) {
                        config->SetBSSID(String("any"));
                        sresult->SetAutoJoinStatus(IScanResult::ENABLED);
                        // enable auto join for the blacklisted BSSID
                        config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
                    } else {
                        sresult->SetAutoJoinStatus(IScanResult::AUTO_ROAM_DISABLED);
                        config->SetBSSID(BSSID);
                        config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_TEMPORARY_DISABLED);
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::GetMaxDhcpRetries(
    /* [out] */ Int32* result)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    return Settings::Global::GetInt32(cr,
             ISettingsGlobal::WIFI_MAX_DHCP_RETRY_COUNT,
             DEFAULT_MAX_DHCP_RETRIES, result);
}

ECode WifiConfigStore::HandleSSIDStateChange(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean enabled,
    /* [in] */ const String& message,
    /* [in] */ const String& BSSID)
{
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        if (enabled) {
            Loge(String("SSID re-enabled for  "));
                    //+ config.configKey() +
                    //" had autoJoinStatus=" + Integer.toString(config.autoJoinStatus)
                    //+ " self added " + config.selfAdded + " ephemeral " + config.ephemeral);
            //TODO: http://b/16381983 Fix Wifi Network Blacklisting
            //TODO: really I don't know if re-enabling is right but we
            //TODO: should err on the side of trying to connect
            //TODO: even if the attempt will fail
            Int32 autoJoinStatus;
            config->GetAutoJoinStatus(&autoJoinStatus);
            if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE) {
                config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
            }
        } else {
            Loge(String("SSID temp disabled for  "));
                    //+ config.configKey() +
                    //" had autoJoinStatus=" + Integer.toString(config.autoJoinStatus)
                    //+ " self added " + config.selfAdded + " ephemeral " + config.ephemeral);
            if (!message.IsNull()) {
                Loge(String(" message=") + message);
            }
            Boolean selfAdded;
            config->GetSelfAdded(&selfAdded);
            Int64 lastConnected;
            config->GetLastConnected(&lastConnected);
            if (selfAdded && lastConnected == 0) {
                // This is a network we self added, and we never succeeded,
                // the user did not create this network and never entered its credentials,
                // so we want to be very aggressive in disabling it completely.
                Int32 networkId;
                config->GetNetworkId(&networkId);
                RemoveConfigAndSendBroadcastIfNeeded(networkId);
            } else {
                if (!message.IsNull()) {
                    if (message.Contains("no identity")) {
                        config->SetAutoJoinStatus(
                                IWifiConfiguration::AUTO_JOIN_DISABLED_NO_CREDENTIALS);
                        if (DBG) {
                            Loge(String("no identity blacklisted "));
                                    //+ config.configKey() + " to "
                                    //+ Integer.toString(config.autoJoinStatus));
                        }
                    } else if (message.Contains("WRONG_KEY")
                            || message.Contains("AUTH_FAILED")) {
                        // This configuration has received an auth failure, so disable it
                        // temporarily because we don't want auto-join to try it out.
                        // this network may be re-enabled by the "usual"
                        // enableAllNetwork function
                        Int32 numAuthFailures;
                        config->GetNumAuthFailures(&numAuthFailures);
                        config->SetNumAuthFailures(++numAuthFailures);
                        if (numAuthFailures > maxAuthErrorsToBlacklist) {
                            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
                            Boolean bTemp;
                            DisableNetwork(netId, IWifiConfiguration::DISABLED_AUTH_FAILURE, &bTemp);
                            Loge(String("Authentication failure, blacklist "));
                                        //+ config.configKey() + " "
                                        //+ Integer.toString(config.networkId)
                                        //+ " num failures " + config.numAuthFailures);
                        }
                    } else if (message.Contains("DHCP FAILURE")) {
                        Int32 numIpConfigFailures;
                        config->GetNumIpConfigFailures(&numIpConfigFailures);
                        config->SetNumIpConfigFailures(++numIpConfigFailures);;
                        AutoPtr<ISystem> system;
                        CSystem::AcquireSingleton((ISystem**)&system);
                        Int64 currentTimeMillis;
                        system->GetCurrentTimeMillis(&currentTimeMillis);
                        config->SetLastConnectionFailure(currentTimeMillis);
                        Int32 maxRetries;
                        GetMaxDhcpRetries(&maxRetries);
                        // maxRetries == 0 means keep trying forever
                        if (maxRetries > 0 && numIpConfigFailures > maxRetries) {
                            /**
                             * If we've exceeded the maximum number of retries for DHCP
                             * to a given network, disable the network
                             */
                            config->SetAutoJoinStatus
                                    (IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
                            Boolean bTemp;
                            DisableNetwork(netId, IWifiConfiguration::DISABLED_DHCP_FAILURE, &bTemp);
                            Loge(String("DHCP failure, blacklist "));
                                    // + config.configKey() + " "
                                    //+ Integer.toString(config.networkId)
                                    //+ " num failures " + config.numIpConfigFailures);
                        }

                        // Also blacklist the BSSId if we find it
                        AutoPtr<IScanResult> result;
                        String bssidDbg("");
                        AutoPtr<IHashMap> scanResultCache;
                        config->GetScanResultCache((IHashMap**)&scanResultCache);
                        if (scanResultCache != NULL && !BSSID.IsNull()) {
                            AutoPtr<IInterface> obj;
                            scanResultCache->Get(CoreUtils::Convert(BSSID), (IInterface**)&obj);
                            result = IScanResult::Probe(obj);
                        }
                        if (result != NULL) {
                            Int32 numIpConfigFailures;
                            result->GetNumIpConfigFailures(&numIpConfigFailures);
                            result->SetNumIpConfigFailures(++numIpConfigFailures);;
                            bssidDbg = BSSID + String(" ipfail=") + StringUtils::ToString(numIpConfigFailures);
                            if (numIpConfigFailures > 3) {
                                // Tell supplicant to stop trying this BSSID
                                mWifiNative->AddToBlacklist(BSSID);
                                result->SetAutoJoinStatus(IScanResult::AUTO_JOIN_DISABLED);
                            }
                        }

                        if (DBG) {
                            Loge(String("blacklisted ")
                                    //+ config.configKey() + " to "
                                    //+ config.autoJoinStatus
                                    //+ " due to IP config failures, count="
                                    //+ config.numIpConfigFailures
                                    //+ " disableReason=" + config.disableReason
                                    + String(" ") + bssidDbg);
                        }
                    } else if (message.Contains("CONN_FAILED")) {
                        Int32 numConnectionFailures;
                        config->GetNumConnectionFailures(&numConnectionFailures);
                        config->SetNumConnectionFailures(++numConnectionFailures);
                        if (numConnectionFailures > maxConnectionErrorsToBlacklist) {
                            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE);
                            Boolean bTemp;
                            DisableNetwork(netId, IWifiConfiguration::DISABLED_ASSOCIATION_REJECT, &bTemp);
                            Loge(String("Connection failure, blacklist "));
                                    //+ config.configKey() + " "
                                    //+ config.networkId
                                    //+ " num failures " + config.numConnectionFailures);
                        }
                    }
                    String newMessage1;
                    StringUtils::Replace(message,"\n", "", &newMessage1);
                    String newMessage2;
                    StringUtils::Replace(newMessage1,"\r", "", &newMessage2);
                    config->SetLastFailure(newMessage2);
                }
            }
        }
    }
    return NOERROR;
}

ECode WifiConfigStore::InstallKeys(
    /* [in] */ IWifiEnterpriseConfig* config,
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean ret = TRUE;

    String privKeyName = /*TODO ICredentials::USER_PRIVATE_KEY*/ String("USRPKEY_") + name;
    String userCertName = /*TODO ICredentials::USER_CERTIFICATE*/ String("USRCERT_") + name;
    String caCertName = /*TODO ICredentials::CA_CERTIFICATE*/ String("CACERT_") + name;

    AutoPtr<IX509Certificate> clientCert;
    config->GetClientCertificate((IX509Certificate**)&clientCert);
    if (clientCert != NULL) {
        assert(0);//TODO
        AutoPtr<IPrivateKey> clientPriavateKey;
        config->GetClientPrivateKey((IPrivateKey**)&clientPriavateKey);
        AutoPtr<ArrayOf<Byte> > privKeyData;
        IKey::Probe(clientPriavateKey)->GetEncoded((ArrayOf<Byte>**)&privKeyData);
        if (IsHardwareBackedKey(clientPriavateKey)) {
            // Hardware backed key store is secure enough to store keys un-encrypted, this
            // removes the need for user to punch a PIN to get access to these keys
            if (DBG) Logger::D(TAG, "importing keys %s in hardware backed store", name.string());
            //TODO ret = mKeyStore->ImportKey(privKeyName, privKeyData, IProcess::WIFI_UID, IKeyStore::FLAG_NONE);
        } else {
            // Software backed key store is NOT secure enough to store keys un-encrypted.
            // Save keys encrypted so they are protected with user's PIN. User will
            // have to unlock phone before being able to use these keys and connect to
            // networks.
            if (DBG) Logger::D(TAG, "importing keys %s in software backed store", name.string());
            //TODO ret = mKeyStore->ImportKey(privKeyName, privKeyData, IProcess::WIFI_UID, IKeyStore::FLAG_ENCRYPTED);
        }
        if (ret == FALSE) {
            *result = ret;
            return NOERROR;
        }

        ret = PutCertInKeyStore(userCertName, ICertificate::Probe(clientCert));
        if (ret == FALSE) {
            // Remove private key installed
            //TODO mKeyStore->DelKey(privKeyName, IProcess::WIFI_UID);
            *result = ret;
            return NOERROR;
        }
    }

    AutoPtr<IX509Certificate> caCert;
    config->GetCaCertificate((IX509Certificate**)&caCert);
    if (caCert != NULL) {
        ret = PutCertInKeyStore(caCertName, ICertificate::Probe(caCert));
        if (ret == FALSE) {
            if (clientCert != NULL) {
                // Remove client key+cert
                //TODO mKeyStore->DelKey(privKeyName, IProcess::WIFI_UID);
                //TODO mKeyStore->Delete(userCertName, IProcess::WIFI_UID);
            }
            *result = ret;
            return NOERROR;
        }
    }

    // Set alias names
    if (clientCert != NULL) {
        config->SetClientCertificateAlias(name);
        config->ResetClientKeyEntry();
    }

    if (caCert != NULL) {
        config->SetCaCertificateAlias(name);
        config->ResetCaCertificate();
    }

    *result = ret;
    return NOERROR;
}

ECode WifiConfigStore::RemoveKeys(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    VALIDATE_NOT_NULL(config);
    String client;
    config->GetClientCertificateAlias(&client);
    // a valid client certificate is configured
    if (!TextUtils::IsEmpty(client)) {
        if (DBG) Logger::D(TAG, "removing client private key and user cert");
        assert(0);
        //TODO mKeyStore->DelKey(ICredentials::USER_PRIVATE_KEY + client, IProcess::WIFI_UID);
        //TODO mKeyStore->Delete(ICredentials::USER_CERTIFICATE + client, IProcess::WIFI_UID);
    }

    String ca;
    config->GetCaCertificateAlias(&ca);
    // a valid ca certificate is configured
    if (!TextUtils::IsEmpty(ca)) {
        if (DBG) Logger::D(TAG, "removing CA cert");
        assert(0);
        //TODO mKeyStore->Delete(ICredentials::CA_CERTIFICATE + ca, IProcess::WIFI_UID);
    }
    return NOERROR;
}

ECode WifiConfigStore::MigrateOldEapTlsNative(
    /* [in] */ IWifiEnterpriseConfig* config,
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String oldPrivateKey = mWifiNative->GetNetworkVariable(netId, OLD_PRIVATE_KEY_NAME);
    /*
     * If the old configuration value is not present, then there is nothing
     * to do.
     */
    if (TextUtils::IsEmpty(oldPrivateKey)) {
        *result = FALSE;
        return NOERROR;
    } else {
        // Also ignore it if it's empty quotes.
        oldPrivateKey = RemoveDoubleQuotes(oldPrivateKey);
        if (TextUtils::IsEmpty(oldPrivateKey)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    config->SetFieldValue(IWifiEnterpriseConfig::ENGINE_KEY, IWifiEnterpriseConfig::ENGINE_ENABLE);
    config->SetFieldValue(IWifiEnterpriseConfig::ENGINE_ID_KEY, IWifiEnterpriseConfig::ENGINE_ID_KEYSTORE);

    /*
    * The old key started with the keystore:// URI prefix, but we don't
    * need that anymore. Trim it off if it exists.
    */
    String keyName;
    if (oldPrivateKey.StartWith(IWifiEnterpriseConfig::KEYSTORE_URI)) {
        keyName = oldPrivateKey.Substring(IWifiEnterpriseConfig::KEYSTORE_URI.GetLength());
    } else {
        keyName = oldPrivateKey;
    }
    config->SetFieldValue(IWifiEnterpriseConfig::PRIVATE_KEY_ID_KEY, keyName);

    String engineKey;
    config->GetFieldValue(IWifiEnterpriseConfig::ENGINE_KEY, String(""), &engineKey);
    mWifiNative->SetNetworkVariable(netId, IWifiEnterpriseConfig::ENGINE_KEY, engineKey);

    String engineIDKey;
    config->GetFieldValue(IWifiEnterpriseConfig::ENGINE_ID_KEY, String(""), &engineIDKey);
    mWifiNative->SetNetworkVariable(netId, IWifiEnterpriseConfig::ENGINE_ID_KEY, engineIDKey);

    String privatekeyIDKey;
    config->GetFieldValue(IWifiEnterpriseConfig::PRIVATE_KEY_ID_KEY, String(""), &privatekeyIDKey);
    mWifiNative->SetNetworkVariable(netId, IWifiEnterpriseConfig::PRIVATE_KEY_ID_KEY, privatekeyIDKey);

    // Remove old private_key string so we don't run this again.
    mWifiNative->SetNetworkVariable(netId, OLD_PRIVATE_KEY_NAME, EMPTY_VALUE);

    *result = TRUE;
    return NOERROR;
}

ECode WifiConfigStore::MigrateCerts(
    /* [in] */ IWifiEnterpriseConfig* config)
{
    VALIDATE_NOT_NULL(config);
    String client;
    config->GetClientCertificateAlias(&client);
    // a valid client certificate is configured
    if (!TextUtils::IsEmpty(client)) {
        Boolean contains = TRUE;
        assert(0);//TODO
        //TODO mKeyStore->Contains(ICredentials::USER_PRIVATE_KEY + client, IProcess::WIFI_UID, &contains);
        if (!contains) {
            //TODO mKeyStore->Duplicate(ICredentials::USER_PRIVATE_KEY + client, -1, ICredentials::USER_PRIVATE_KEY + client, IProcess::WIFI_UID);
            //TODO mKeyStore->Duplicate(ICredentials::USER_CERTIFICATE + client, -1, ICredentials::USER_CERTIFICATE + client, IProcess::WIFI_UID);
        }
    }

    String ca;
    config->GetCaCertificateAlias(&ca);
    // a valid ca certificate is configured
    if (!TextUtils::IsEmpty(ca)) {
        Boolean contains = TRUE;
        //TODO mKeyStore->Contains(ICredentials::CA_CERTIFICATE + ca, IProcess::WIFI_UID))
        if (!contains) {
            //TODO mKeyStore->Duplicate(ICredentials::CA_CERTIFICATE + ca, -1, ICredentials::CA_CERTIFICATE + ca, IProcess::WIFI_UID);
        }
    }
    return NOERROR;
}

void WifiConfigStore::Loge(
    /* [in] */ const String& s)
{
    Loge(s, FALSE);
}

void WifiConfigStore::Loge(
    /* [in] */ const String& s,
    /* [in] */ Boolean stack)
{
    if (stack) {
        //Log.e(TAG, s + " stack:" + Thread.currentThread().getStackTrace()[2].getMethodName()
        //        + " - " + Thread.currentThread().getStackTrace()[3].getMethodName()
        //        + " - " + Thread.currentThread().getStackTrace()[4].getMethodName()
        //        + " - " + Thread.currentThread().getStackTrace()[5].getMethodName());
        Logger::E(TAG, s);
    } else {
        Logger::E(TAG, s);
    }
}

void WifiConfigStore::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, s);
}

AutoPtr<ArrayOf<String> > WifiConfigStore::MiddleInitEnterpriseConfigSupplicantKeys()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(11);
    result->Set(0, IWifiEnterpriseConfig::EAP_KEY);
    result->Set(1, IWifiEnterpriseConfig::PHASE2_KEY);
    result->Set(2, IWifiEnterpriseConfig::IDENTITY_KEY);
    result->Set(3, IWifiEnterpriseConfig::ANON_IDENTITY_KEY);
    result->Set(4, IWifiEnterpriseConfig::PASSWORD_KEY);
    result->Set(5, IWifiEnterpriseConfig::CLIENT_CERT_KEY);
    result->Set(6, IWifiEnterpriseConfig::CA_CERT_KEY);
    result->Set(7, IWifiEnterpriseConfig::SUBJECT_MATCH_KEY);
    result->Set(8, IWifiEnterpriseConfig::ENGINE_KEY);
    result->Set(9, IWifiEnterpriseConfig::ENGINE_ID_KEY);
    result->Set(10, IWifiEnterpriseConfig::PRIVATE_KEY_ID_KEY);

    return result;
}

//IWifiConfiguration
AutoPtr<IList> WifiConfigStore::GetConfiguredNetworks(
    /* [in] */ IMap* pskMap)//String, String
{
    AutoPtr<IList> networks;
    CParcelableList::New((IList**)&networks);//WifiConfiguration

    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        AutoPtr<IWifiConfiguration> newConfig;
        CWifiConfiguration::New(config, (IWifiConfiguration**)&newConfig);
        Int32 autoJoinStatus;
        config->GetAutoJoinStatus(&autoJoinStatus);
        if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DELETED) {
            // Do not enumerate and return this configuration to any one,
            // for instance WiFi Picker.
            // instead treat it as unknown. the configuration can still be retrieved
            // directly by the key or networkId
            continue;
        }

        if (pskMap != NULL) {
            AutoPtr<IBitSet> allowedKeyManagement;
            config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
            Boolean wpa_psk;
            String ssid;
            config->GetSSID(&ssid);
            Boolean containsKey;
            if(allowedKeyManagement != NULL
                    && allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &wpa_psk)
                    && (pskMap->ContainsKey(CoreUtils::Convert(ssid), &containsKey), containsKey)) {
                AutoPtr<IInterface> value;
                pskMap->Get(CoreUtils::Convert(ssid), (IInterface**)&value);
                String pskey;
                ICharSequence::Probe(value)->ToString(&pskey);
                newConfig->SetPreSharedKey(pskey);
            }
        }
        networks->Add(newConfig);
    }
    return networks;
}

//String, String
AutoPtr<IMap> WifiConfigStore::GetCredentialsBySsidMap()
{
    return ReadNetworkVariablesFromSupplicantFile(String("psk"));
}

Boolean WifiConfigStore::RemoveConfigAndSendBroadcastIfNeeded(
    /* [in] */ Int32 netId)
{
    Boolean remove = TRUE;
    AutoPtr<IInterface> obj;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&obj);
    IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
    if (config != NULL) {
        String configKey;
        config->ConfigKey(&configKey);
        Int32 id;
        config->GetNetworkId(&id);
        if (VDBG) {
            Loge(String("removeNetwork ")
                    + StringUtils::ToString(netId) + " key=" +
                    configKey + " config.id=" + StringUtils::ToString(id));
        }

        // cancel the last user choice
        if (configKey.Equals(lastSelectedConfiguration)) {
            lastSelectedConfiguration = String(NULL);
        }

        // Remove any associated keys
        AutoPtr<IWifiEnterpriseConfig> eConfig;
        config->GetEnterpriseConfig((IWifiEnterpriseConfig**)&eConfig);
        if (eConfig != NULL) {
            RemoveKeys(eConfig);
        }

        Boolean selfAdded;
        config->GetSelfAdded(&selfAdded);
        AutoPtr<IHashMap> linkedConfigurations;
        config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
        AutoPtr<IBitSet> allowedKeyManagement;
        config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
        Boolean wpa_psk;
        allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &wpa_psk);
        if (selfAdded || linkedConfigurations != NULL || wpa_psk) {
            remove = FALSE;
            Loge(String("removeNetwork ")
                    + StringUtils::ToString(netId)
                    + " key=" + configKey
                    + " config.id=" + StringUtils::ToString(id)
                    + " -> mark as deleted");
        }

        if (remove) {
            mConfiguredNetworks->Remove(CoreUtils::Convert(netId));
            mNetworkIds->Remove(CoreUtils::Convert(ConfigKey(config)));
        } else {
            /**
             * We can't directly remove the configuration since we could re-add it ourselves,
             * and that would look weird to the user.
             * Instead mark it as deleted and completely hide it from the rest of the system.
             */
            config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_DELETED);
            // Disable
            Int32 networkId;
            config->GetNetworkId(&networkId);
            mWifiNative->DisableNetwork(networkId);
            config->SetStatus(IWifiConfigurationStatus::DISABLED);
            // Since we don't delete the configuration, clean it up and loose the history
            config->SetLinkedConfigurations(NULL);
            config->SetScanResultCache(NULL);
            config->SetConnectChoices(NULL);
            config->SetDefaultGwMacAddress(String(NULL));
            AutoPtr<IIpConfiguration> ipconfig;
            CIpConfiguration::New((IIpConfiguration**)&ipconfig);
            config->SetIpConfiguration(ipconfig);
            // Loose the PSK
            if (!mWifiNative->SetNetworkVariable(
                    networkId,
                    IWifiConfiguration::pskVarName,
                    String("\"") + DELETED_CONFIG_PSK + String("\""))) {
                Loge(String("removeNetwork, failed to clear PSK, nid=") + StringUtils::ToString(networkId));
            }
            // Loose the BSSID
            config->SetBSSID(String(NULL));
            config->SetAutoJoinBSSID(String(NULL));
            if (!mWifiNative->SetNetworkVariable(
                    networkId,
                    IWifiConfiguration::bssidVarName,
                    String("any"))) {
                Loge(String("removeNetwork, failed to remove BSSID"));
            }
            // Loose the hiddenSSID flag
            config->SetHiddenSSID(FALSE);
            if (!mWifiNative->SetNetworkVariable(
                    networkId,
                    IWifiConfiguration::hiddenSSIDVarName,
                    StringUtils::ToString(0))) {
                Loge(String("removeNetwork, failed to remove hiddenSSID"));
            }

            mWifiNative->SaveConfig();
        }

        WriteIpAndProxyConfigurations();
        SendConfiguredNetworksChangedBroadcast(config, IWifiManager::CHANGE_REASON_REMOVED);
        WriteKnownNetworkHistory();
    }
    return remove;
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast(
    /* [in] */ IWifiConfiguration* network,
    /* [in] */ Int32 reason)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::CONFIGURED_NETWORKS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutBooleanExtra(IWifiManager::EXTRA_MULTIPLE_NETWORKS_CHANGED, FALSE);
    intent->PutExtra(IWifiManager::EXTRA_WIFI_CONFIGURATION, IParcelable::Probe(network));
    intent->PutExtra(IWifiManager::EXTRA_CHANGE_REASON, reason);
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> uh;
    uhHelper->GetALL((IUserHandle**)&uh);

    mContext->SendBroadcastAsUser(intent, uh);
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::CONFIGURED_NETWORKS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutBooleanExtra(IWifiManager::EXTRA_MULTIPLE_NETWORKS_CHANGED, TRUE);

    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> uh;
    uhHelper->GetALL((IUserHandle**)&uh);
    mContext->SendBroadcastAsUser(intent, uh);
}

AutoPtr<IMap> WifiConfigStore::ReadNetworkVariablesFromSupplicantFile(
    /* [in] */ const String& key)
{
    AutoPtr<IMap> result;
    CHashMap::New((IMap**)&result);
    AutoPtr<IBufferedReader> reader;
    if (VDBG) Loge(String("readNetworkVariablesFromSupplicantFile key=") + key);

    //try {
        AutoPtr<IFileReader> fileReader;
        ECode ec = CFileReader::New(String(SUPPLICANT_CONFIG_FILE), (IFileReader**)&fileReader);
        if (FAILED(ec)) {
            if (VDBG) Loge(String("Could not open ") + SUPPLICANT_CONFIG_FILE);
            return result;
        }

        ec = CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&reader);
        if (FAILED(ec)) {
            if (VDBG) Loge(String("Could not open ") + SUPPLICANT_CONFIG_FILE);
            if (fileReader != NULL)
                ICloseable::Probe(fileReader)->Close();
            return result;
        }
        Boolean found = FALSE;
        String networkSsid;
        String value;

        String line;
        for (reader->ReadLine(&line); !line.IsNull(); reader->ReadLine(&line)) {

            Boolean match;
            if (StringUtils::Matches(line, String("[ \\t]*network=\\{"), &match), match) {
                found = TRUE;
                networkSsid = String(NULL);
                value = String(NULL);
            } else if (StringUtils::Matches(line, String("[ \\t]*\\}"), &match), match) {
                found = FALSE;
                networkSsid = String(NULL);
                value = String(NULL);
            }

            if (found) {
                String trimmedLine = line.Trim();
                if (trimmedLine.StartWith(String("ssid="))) {
                    networkSsid = trimmedLine.Substring(5);
                } else if (trimmedLine.StartWith(key + String("="))) {
                    value = trimmedLine.Substring(key.GetLength() + 1);
                }

                if (!networkSsid.IsNull() && !value.IsNull()) {
                    result->Put(CoreUtils::Convert(networkSsid), CoreUtils::Convert(value));
                }
            }
        }
    //} catch (FileNotFoundException e) {
    //    if (VDBG) loge("Could not open " + SUPPLICANT_CONFIG_FILE + ", " + e);
    //} catch (IOException e) {
    //    if (VDBG) loge("Could not read " + SUPPLICANT_CONFIG_FILE + ", " + e);
    //} finally {
    //    try {
            if (reader != NULL) {
                ICloseable::Probe(reader)->Close();
            }
    //    } catch (IOException e) {
    //        // Just ignore the fact that we couldn't close
    //    }
    //}

    return result;
}

String WifiConfigStore::ReadNetworkVariableFromSupplicantFile(
    /* [in] */ const String& ssid,
    /* [in] */ const String& key)
{
    AutoPtr<IMap> data = ReadNetworkVariablesFromSupplicantFile(key);

    if (VDBG) {
        StringBuilder builder;
        builder += "readNetworkVariableFromSupplicantFile ssid=[";
        builder += ssid;
        builder += "] key=";
        builder += key;
        Loge(builder.ToString());
    }
    AutoPtr<IInterface> obj;
    data->Get(CoreUtils::Convert(ssid), (IInterface**)&obj);
    String result;
    if (obj != NULL) {
        ICharSequence::Probe(obj)->ToString(&result);
    }
    return result;
}

void WifiConfigStore::MarkAllNetworksDisabledExcept(
    /* [in] */ Int32 netId)
{
    AutoPtr<ICollection> values;
    mConfiguredNetworks->GetValues((ICollection**)&values);
    AutoPtr<IIterator> iter;
    values->GetIterator((IIterator**)&iter);
    Boolean bNext;
    while (iter->HasNext(&bNext), bNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        IWifiConfiguration* config = IWifiConfiguration::Probe(obj);

        Int32 networkId;
        if(config != NULL && (config->GetNetworkId(&networkId), networkId) != netId) {
            Int32 status;
            config->GetStatus(&status);
            if (status != IWifiConfigurationStatus::DISABLED) {
                config->SetStatus(IWifiConfigurationStatus::DISABLED);
                config->SetDisableReason(IWifiConfiguration::DISABLED_UNKNOWN_REASON);
            }
        }
    }
}

void WifiConfigStore::MarkAllNetworksDisabled()
{
    MarkAllNetworksDisabledExcept(IWifiConfiguration::INVALID_NETWORK_ID);
}

void WifiConfigStore::ReadNetworkHistory()
{
    if (showNetworks) {
        LocalLog(String("readNetworkHistory() path:") + networkHistoryConfigFile);
    }
    AutoPtr<IDataInputStream> in;
    //try {
    AutoPtr<IFileInputStream> ifs;
    CFileInputStream::New(networkHistoryConfigFile, (IFileInputStream**)&ifs);
    AutoPtr<IBufferedInputStream> bis;
    CBufferedInputStream::New(IInputStream::Probe(ifs), (IBufferedInputStream**)&bis);
    CDataInputStream::New(IInputStream::Probe(bis), (IDataInputStream**)&in);

    AutoPtr<IWifiConfiguration> config;
    while (TRUE) {
        IDataInput* di = IDataInput::Probe(in);
        //Int32 id = -1;
        String key;
        ECode ec = di->ReadUTF(&key);
        if (FAILED(ec)) {
            if(in!=NULL) {
                ICloseable::Probe(in)->Close();
            }
            if ((unsigned)ec == E_EOF_EXCEPTION)
                Loge(String("ReadNetworkHistory, read to end"));
            else
                Loge(String("readNetworkHistory: No config file, revert to default"));
            return;
        }
        String bssid;
        String ssid;

        Int32 freq = 0;
        Int32 status = 0;
        Int64 seen = 0;
        Int32 rssi = IWifiConfiguration::INVALID_RSSI;
        String caps;
        Boolean duplicateNetwork = FALSE;
        if (key.StartWith(CONFIG_KEY)) {

            if (config != NULL) {
                config = NULL;
            }
            String configKey1;
            StringUtils::Replace(key, CONFIG_KEY, String(""), &configKey1);
            String configKey;
            StringUtils::Replace(configKey1, SEPARATOR_KEY, String(""), &configKey);
            // get the networkId for that config Key
            AutoPtr<IInterface> obj;
            mNetworkIds->Get(CoreUtils::Convert(configKey.GetHashCode()), (IInterface**)&obj);
            IInteger32* n = IInteger32::Probe(obj);
            // skip reading that configuration data
            // since we don't have a corresponding network ID
            if (n == NULL) {
                LocalLog(String("readNetworkHistory didnt find netid for hash="));
                        //+ Integer.toString(configKey.hashCode())
                        //+ " key: " + configKey);
                continue;
            }
            AutoPtr<IInterface> configObj;
            mConfiguredNetworks->Get(n, (IInterface**)&configObj);
            config = IWifiConfiguration::Probe(configObj);;
            if (config == NULL) {
                LocalLog(String("readNetworkHistory didnt find config for netid="));
                        //+ n.toString()
                        //+ " key: " + configKey);
            }
            status = 0;
            ssid = String(NULL);
            bssid = String(NULL);
            freq = 0;
            seen = 0;
            rssi = IWifiConfiguration::INVALID_RSSI;
            caps = String(NULL);

        } else if (config != NULL && (config->GetDuplicateNetwork(&duplicateNetwork), duplicateNetwork) ==  FALSE) {
            if (key.StartWith(SSID_KEY)) {
                String key1;
                StringUtils::Replace(key, SSID_KEY, String(""), &key1);
                StringUtils::Replace(key1, SEPARATOR_KEY, String(""), &ssid);
                String SSID;
                config->GetSSID(&SSID);
                if (!SSID.IsNull() && !SSID.Equals(ssid)) {
                    Loge(String("Error parsing network history file, mismatched SSIDs"));
                    config = NULL; //error
                    ssid = String(NULL);
                } else {
                    config->SetSSID(ssid);
                }
            }

            if (key.StartWith(FQDN_KEY)) {
                String fqdn1;
                StringUtils::Replace(key, FQDN_KEY, String(""), &fqdn1);
                String fqdn;
                StringUtils::Replace(fqdn1, SEPARATOR_KEY, String(""), &fqdn);
                config->SetFQDN(fqdn);
            }

            if (key.StartWith(DEFAULT_GW_KEY)) {
                String gateway1;
                StringUtils::Replace(key, DEFAULT_GW_KEY, String(""), &gateway1);
                String gateway;
                StringUtils::Replace(gateway1, SEPARATOR_KEY, String(""), &gateway);
                config->SetDefaultGwMacAddress(gateway);
            }

            if (key.StartWith(STATUS_KEY)) {
                String st1;
                StringUtils::Replace(key, STATUS_KEY, String(""), &st1);
                String st;
                StringUtils::Replace(st1, SEPARATOR_KEY, String(""), &st);
                config->SetAutoJoinStatus(StringUtils::ParseInt32(st));
            }

            if (key.StartWith(SUPPLICANT_DISABLE_REASON_KEY)) {
                String reason1;
                StringUtils::Replace(key, SUPPLICANT_DISABLE_REASON_KEY, String(""), &reason1);
                String reason;
                StringUtils::Replace(reason1, SEPARATOR_KEY, String(""), &reason);
                config->SetDisableReason(StringUtils::ParseInt32(reason));
            }

            if (key.StartWith(SELF_ADDED_KEY)) {
                String selfAdded1;
                StringUtils::Replace(key, SELF_ADDED_KEY, String(""), &selfAdded1);
                String selfAdded;
                StringUtils::Replace(selfAdded1, SEPARATOR_KEY, String(""), &selfAdded);
                config->SetSelfAdded(StringUtils::ParseBoolean(selfAdded));
            }

            if (key.StartWith(DID_SELF_ADD_KEY)) {
                String didSelfAdd1;
                StringUtils::Replace(key, DID_SELF_ADD_KEY, String(""), &didSelfAdd1);
                String didSelfAdd;
                StringUtils::Replace(didSelfAdd1, SEPARATOR_KEY, String(""), &didSelfAdd);
                config->SetDidSelfAdd(StringUtils::ParseBoolean(didSelfAdd));
            }

            if (key.StartWith(NO_INTERNET_ACCESS_KEY)) {
                String access1;
                StringUtils::Replace(key, NO_INTERNET_ACCESS_KEY, String(""), &access1);
                String access;
                StringUtils::Replace(access1, SEPARATOR_KEY, String(""), &access);
                config->SetNoInternetAccess(StringUtils::ParseBoolean(access));
            }

            if (key.StartWith(EPHEMERAL_KEY)) {
                String temp;
                StringUtils::Replace(key, EPHEMERAL_KEY, String(""), &temp);
                String access;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &access);
                config->SetEphemeral(StringUtils::ParseBoolean(access));
            }

            if (key.StartWith(CREATOR_UID_KEY)) {
                String temp;
                StringUtils::Replace(key, CREATOR_UID_KEY, String(""), &temp);
                String uid;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &uid);
                config->SetCreatorUid(StringUtils::ParseInt32(uid));
            }

            if (key.StartWith(BLACKLIST_MILLI_KEY)) {
                String temp;
                StringUtils::Replace(key, BLACKLIST_MILLI_KEY, String(""), &temp);
                String milli;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &milli);
                config->SetBlackListTimestamp(StringUtils::ParseInt64(milli));
            }

            if (key.StartWith(NUM_CONNECTION_FAILURES_KEY)) {
                String temp;
                StringUtils::Replace(key, NUM_CONNECTION_FAILURES_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumConnectionFailures(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(NUM_IP_CONFIG_FAILURES_KEY)) {
                String temp;
                StringUtils::Replace(key, NUM_IP_CONFIG_FAILURES_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumIpConfigFailures(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(NUM_AUTH_FAILURES_KEY)) {
                String temp;
                StringUtils::Replace(key, NUM_AUTH_FAILURES_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumIpConfigFailures(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(SCORER_OVERRIDE_KEY)) {
                String temp;
                StringUtils::Replace(key, SCORER_OVERRIDE_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumScorerOverride(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(SCORER_OVERRIDE_AND_SWITCH_KEY)) {
                String temp;
                StringUtils::Replace(key, SCORER_OVERRIDE_AND_SWITCH_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumScorerOverrideAndSwitchedNetwork(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(NUM_ASSOCIATION_KEY)) {
                String temp;
                StringUtils::Replace(key, NUM_ASSOCIATION_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetNumAssociation(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(JOIN_ATTEMPT_BOOST_KEY)) {
                String temp;
                StringUtils::Replace(key, JOIN_ATTEMPT_BOOST_KEY, String(""), &temp);
                String num;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &num);
                config->SetAutoJoinUseAggressiveJoinAttemptThreshold(StringUtils::ParseInt32(num));
            }

            if (key.StartWith(CONNECT_UID_KEY)) {
                String temp;
                StringUtils::Replace(key, CONNECT_UID_KEY, String(""), &temp);
                String uid;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &uid);
                config->SetLastConnectUid(StringUtils::ParseInt32(uid));
            }

            if (key.StartWith(UPDATE_UID_KEY)) {
                String temp;
                StringUtils::Replace(key, UPDATE_UID_KEY, String(""), &temp);
                String uid;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &uid);
                config->SetLastUpdateUid(StringUtils::ParseInt32(uid));
            }

            if (key.StartWith(FAILURE_KEY)) {
                String temp;
                StringUtils::Replace(key, FAILURE_KEY, String(""), &temp);
                String temp2;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &temp2);
                config->SetLastFailure(temp2);
            }

            if (key.StartWith(PEER_CONFIGURATION_KEY)) {
                String temp;
                StringUtils::Replace(key, PEER_CONFIGURATION_KEY, String(""), &temp);
                String temp2;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &temp2);
                config->SetPeerWifiConfiguration(temp2);
            }

            if (key.StartWith(CHOICE_KEY)) {
                String temp;
                StringUtils::Replace(key, CHOICE_KEY, String(""), &temp);
                String choiceStr;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &choiceStr);
                String configKey("");
                Int32 choice = 0;
                AutoPtr<IMatcher> match;
                mConnectChoice->Matcher(choiceStr, (IMatcher**)&match);
                Boolean matchFound;
                if (!(match->Find(&matchFound), matchFound)) {
                    if (DBG) Logger::D(TAG, "WifiConfigStore: connectChoice:  Couldnt match pattern : %s", choiceStr.string());
                } else {
                    IMatchResult* matchResult = IMatchResult::Probe(match);
                    matchResult->Group(1, &configKey);
                    //try {
                    String g2;
                    matchResult->Group(2, &g2);
                    ECode ec = StringToIntegral::Parse(g2, 10, &choice);
                    if (FAILED(ec)) {
                        choice = 0;
                    }
                    //} catch (NumberFormatException e) {
                    //    choice = 0;
                    //}
                    if (choice > 0) {
                        AutoPtr<IHashMap> connectChoices;
                        config->GetConnectChoices((IHashMap**)&connectChoices);
                        if (connectChoices == NULL) {
                            CHashMap::New((IHashMap**)&connectChoices);
                            config->SetConnectChoices(connectChoices);
                        }
                        connectChoices->Put(CoreUtils::Convert(configKey), CoreUtils::Convert(choice));
                    }
                }
            }

            if (key.StartWith(LINK_KEY)) {
                String temp;
                StringUtils::Replace(key, LINK_KEY, String(""), &temp);
                String configKey;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &configKey);
                AutoPtr<IHashMap> linkedConfigurations;
                config->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
                if (linkedConfigurations == NULL) {
                    CHashMap::New((IHashMap**)&linkedConfigurations);
                    config->SetLinkedConfigurations(linkedConfigurations);
                }
                if (linkedConfigurations != NULL) {
                    linkedConfigurations->Put(CoreUtils::Convert(configKey), CoreUtils::Convert(-1));
                }
            }

            if (key.StartWith(BSSID_KEY)) {
                if (key.StartWith(BSSID_KEY)) {
                    String temp;
                    StringUtils::Replace(key, BSSID_KEY, String(""), &temp);
                    StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &bssid);
                    freq = 0;
                    seen = 0;
                    rssi = IWifiConfiguration::INVALID_RSSI;
                    caps = String("");
                    status = 0;
                }

                if (key.StartWith(RSSI_KEY)) {
                    String temp;
                    StringUtils::Replace(key, RSSI_KEY, String(""), &temp);
                    String lvl;
                    StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &lvl);
                    rssi = StringUtils::ParseInt32(lvl);
                }

                if (key.StartWith(BSSID_STATUS_KEY)) {
                    String temp;
                    StringUtils::Replace(key, BSSID_STATUS_KEY, String(""), &temp);
                    String st;
                    StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                    status = StringUtils::ParseInt32(st);
                }

                if (key.StartWith(FREQ_KEY)) {
                    String temp;
                    StringUtils::Replace(key, FREQ_KEY, String(""), &temp);
                    String channel;
                    StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &channel);
                    freq = StringUtils::ParseInt32(channel);
                }

                if (key.StartWith(DATE_KEY)) {
                    /*
                     * when reading the configuration from file we don't update the date
                     * so as to avoid reading back stale or non-sensical data that would
                     * depend on network time.
                     * The date of a WifiConfiguration should only come from actual scan result.
                     *
                     String s = key.replace(FREQ_KEY, "");
                     seen = Integer.getInteger(s);
                     */
                }

                if (key.StartWith(BSSID_KEY_END)) {

                    if ((!bssid.IsNull()) && (!ssid.IsNull())) {

                        AutoPtr<IHashMap> scanResultCache;
                        config->GetScanResultCache((IHashMap**)&scanResultCache);
                        if (scanResultCache == NULL) {
                            CHashMap::New((IHashMap**)&scanResultCache);
                            config->SetScanResultCache(scanResultCache);
                        }
                        AutoPtr<IWifiSsidHelper> wssidHelper;
                        CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&wssidHelper);
                        AutoPtr<IWifiSsid> wssid;
                        wssidHelper->CreateFromAsciiEncoded(ssid, (IWifiSsid**)&wssid);
                        AutoPtr<IScanResult> result;
                        CScanResult::New(wssid, bssid,
                                caps, rssi, freq, (Int64) 0, (IScanResult**)&result);
                        result->SetSeen(seen);
                        scanResultCache->Put(CoreUtils::Convert(bssid), result);
                        result->SetAutoJoinStatus(status);
                    }
                }
            }
        }
    }
    //} catch (EOFException ignore) {
    //    if (in != null) {
    //        try {
    //            in.close();
    //        } catch (Exception e) {
    //            loge("readNetworkHistory: Error reading file" + e);
    //        }
    //    }
    //} catch (IOException e) {
    //    loge("readNetworkHistory: No config file, revert to default" + e);
    //}

    if(in!=NULL) {
        //try {
        ICloseable::Probe(in)->Close();
        //} catch (Exception e) {
        //    loge("readNetworkHistory: Error closing file" + e);
        //}
    }
}

void WifiConfigStore::ReadAutoJoinConfig()
{
    AutoPtr<IBufferedReader> reader;
    // try {

        AutoPtr<IFileReader> fr;
        CFileReader::New(autoJoinConfigFile, (IFileReader**)&fr);
        if (fr == NULL) {
            Logger::D(TAG, "open %s failed", autoJoinConfigFile.string());
            return;
        }
        CBufferedReader::New(IReader::Probe(fr), (IBufferedReader**)&reader);
        if (reader == NULL) {
            Logger::D(TAG, "open %s failed", autoJoinConfigFile.string());
            return;
        }

        String key;
        for (reader->ReadLine(&key); !(key.IsNull()); reader->ReadLine(&key)) {
            if (!(key.IsNull())) {
                Logger::D(TAG, "readAutoJoinConfig line: %s", key.string());
            }
            if (key.StartWith(ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ENABLE_AUTO_JOIN_WHILE_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                enableAutoJoinWhenAssociated = StringUtils::ParseInt32(st) != 0;
                Logger::D(TAG, "readAutoJoinConfig: enabled = %b", enableAutoJoinWhenAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG,"readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ENABLE_FULL_BAND_SCAN_WHEN_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                enableFullBandScanWhenAssociated = StringUtils::ParseInt32(st) != 0;
                Logger::D(TAG, "readAutoJoinConfig: enableFullBandScanWhenAssociated = %b",
                        enableFullBandScanWhenAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ENABLE_AUTO_JOIN_SCAN_WHILE_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                enableAutoJoinScanWhenAssociated = StringUtils::ParseInt32(st) != 0;
                Logger::D(TAG, "readAutoJoinConfig: enabled = %b",
                        enableAutoJoinScanWhenAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ENABLE_CHIP_WAKE_UP_WHILE_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                enableChipWakeUpWhenAssociated = StringUtils::ParseInt32(st) != 0;
                Logger::D(TAG, "readAutoJoinConfig: enabled = %b", enableChipWakeUpWhenAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ENABLE_RSSI_POLL_WHILE_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    enableRssiPollWhenAssociated = StringUtils::ParseInt32(st) != 0;
                    Logger::D(TAG, "readAutoJoinConfig: enabled = %b",
                            enableRssiPollWhenAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_5G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdInitialAutoJoinAttemptMin5RSSI = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdInitialAutoJoinAttemptMin5RSSI = %d",
                            thresholdInitialAutoJoinAttemptMin5RSSI);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_INITIAL_AUTO_JOIN_ATTEMPT_RSSI_MIN_24G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdInitialAutoJoinAttemptMin24RSSI = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdInitialAutoJoinAttemptMin24RSSI = %d",
                            thresholdInitialAutoJoinAttemptMin24RSSI);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_UNBLACKLIST_HARD_5G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_UNBLACKLIST_HARD_5G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdUnblacklistThreshold5Hard = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdUnblacklistThreshold5Hard = %d",
                        thresholdUnblacklistThreshold5Hard);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_UNBLACKLIST_SOFT_5G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_UNBLACKLIST_SOFT_5G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdUnblacklistThreshold5Soft = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdUnblacklistThreshold5Soft = %d",
                        thresholdUnblacklistThreshold5Soft);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_UNBLACKLIST_HARD_24G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_UNBLACKLIST_HARD_24G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdUnblacklistThreshold24Hard = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdUnblacklistThreshold24Hard = %d",
                        thresholdUnblacklistThreshold24Hard);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_UNBLACKLIST_SOFT_24G_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_UNBLACKLIST_SOFT_24G_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdUnblacklistThreshold24Soft = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdUnblacklistThreshold24Soft = %d",
                        thresholdUnblacklistThreshold24Soft);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_GOOD_RSSI_5_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_GOOD_RSSI_5_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdGoodRssi5 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdGoodRssi5 = %d",
                        thresholdGoodRssi5);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_LOW_RSSI_5_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_LOW_RSSI_5_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdLowRssi5 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdLowRssi5 = %d",
                        thresholdLowRssi5);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_BAD_RSSI_5_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_BAD_RSSI_5_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdBadRssi5 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdBadRssi5 = %d",
                        thresholdBadRssi5);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_GOOD_RSSI_24_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_GOOD_RSSI_24_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdGoodRssi24 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdGoodRssi24 = %d",
                        thresholdGoodRssi24);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_LOW_RSSI_24_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_LOW_RSSI_24_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdLowRssi24 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdLowRssi24 = %d",
                        thresholdLowRssi24);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_BAD_RSSI_24_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_BAD_RSSI_24_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    thresholdBadRssi24 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: thresholdBadRssi24 = %d",
                        thresholdBadRssi24);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_TX_PACKETS_FOR_NETWORK_SWITCHING_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxTxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxTxPacketForNetworkSwitching = %d",
                        maxTxPacketForNetworkSwitching);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_RX_PACKETS_FOR_NETWORK_SWITCHING_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxRxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxRxPacketForNetworkSwitching = %d",
                        maxRxPacketForNetworkSwitching);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_TX_PACKETS_FOR_FULL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxTxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxTxPacketForFullScans = %d",
                            maxTxPacketForFullScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_RX_PACKETS_FOR_FULL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxRxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxRxPacketForFullScans = %d",
                            maxRxPacketForFullScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_TX_PACKETS_FOR_PARTIAL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxTxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxTxPacketForPartialScans = %d",
                            maxTxPacketForPartialScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, THRESHOLD_MAX_RX_PACKETS_FOR_PARTIAL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxRxPacketForNetworkSwitching = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxRxPacketForPartialScans = %d",
                            maxRxPacketForPartialScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }

            if (key.StartWith(WIFI_VERBOSE_LOGS_KEY)) {
                String temp;
                StringUtils::Replace(key, WIFI_VERBOSE_LOGS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    enableVerboseLogging = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: enable verbose logs = %d",
                            enableVerboseLogging);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY)) {
                String temp;
                StringUtils::Replace(key, A_BAND_PREFERENCE_RSSI_THRESHOLD_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    bandPreferenceBoostThreshold5 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: bandPreferenceBoostThreshold5 = %d",
                        bandPreferenceBoostThreshold5);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY)) {
                String temp;
                StringUtils::Replace(key, ASSOCIATED_PARTIAL_SCAN_PERIOD_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    associatedPartialScanPeriodMilli = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: associatedScanPeriod = %d",
                            associatedPartialScanPeriodMilli);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(ASSOCIATED_FULL_SCAN_BACKOFF_KEY)) {
                String temp;
                StringUtils::Replace(key, ASSOCIATED_FULL_SCAN_BACKOFF_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    associatedFullScanBackoff = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: associatedFullScanBackoff = %d",
                            associatedFullScanBackoff);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY)) {
                String temp;
                StringUtils::Replace(key, G_BAND_PREFERENCE_RSSI_THRESHOLD_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    bandPreferencePenaltyThreshold5 = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: bandPreferencePenaltyThreshold5 = %d",
                        bandPreferencePenaltyThreshold5);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY)) {
                String temp;
                StringUtils::Replace(key, ALWAYS_ENABLE_SCAN_WHILE_ASSOCIATED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    alwaysEnableScansWhileAssociated = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: alwaysEnableScansWhileAssociated = %d",
                            alwaysEnableScansWhileAssociated);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, MAX_NUM_PASSIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxNumPassiveChannelsForPartialScans = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxNumPassiveChannelsForPartialScans = %d",
                            maxNumPassiveChannelsForPartialScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY)) {
                String temp;
                StringUtils::Replace(key, MAX_NUM_ACTIVE_CHANNELS_FOR_PARTIAL_SCANS_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    maxNumActiveChannelsForPartialScans = StringUtils::ParseInt32(st);
                    Logger::D(TAG, "readAutoJoinConfig: maxNumActiveChannelsForPartialScans = %d",
                            maxNumActiveChannelsForPartialScans);
                // } catch (NumberFormatException e) {
                //     Log.d(TAG, "readAutoJoinConfig: incorrect format :" + key);
                // }
            }
            if (key.StartWith(
                AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY)) {
                Int32 scanInterval = 0;
                String temp;
                StringUtils::Replace(key, AUTO_JOIN_SCAN_INTERVAL_WHEN_P2P_CONNECTED_KEY, String(""), &temp);
                String st;
                StringUtils::Replace(temp, SEPARATOR_KEY, String(""), &st);
                // try {
                    scanInterval = StringUtils::ParseInt32(st);
                    if (scanInterval >= 10000) {
                        autoJoinScanIntervalWhenP2pConnected = scanInterval;
                    }
                    else {
                        Logger::D(TAG,
                                "Cfg value is less then 10sec, Using default=%d",
                                autoJoinScanIntervalWhenP2pConnected);
                    }
                    Logger::D(TAG, "readAutoJoinConfig: autoJoinScanIntervalWhenP2pConnected = %d",
                            autoJoinScanIntervalWhenP2pConnected);
                // } catch (NumberFormatException e) {
                //     Logger::D(TAG, "readAutoJoinConfig: incorrect format :" +
                //           key);
                // }
            }
        }
    // } catch (EOFException ignore) {
    //     if (reader != NULL) {
    //         // try {
    //             reader->Close();
    //             reader = NULL;
    //         // } catch (Exception e) {
    //         //     loge("readAutoJoinStatus: Error closing file" + e);
    //         // }
    //     }
    // } catch (FileNotFoundException ignore) {
    //     if (reader != NULL) {
    //         // try {
    //             reader->Close();
    //             reader = NULL;
    //         // } catch (Exception e) {
    //         //     loge("readAutoJoinStatus: Error closing file" + e);
    //         // }
    //     }
    // } catch (IOException e) {
    //     loge("readAutoJoinStatus: Error parsing configuration" + e);
    // }

    if (reader != NULL) {
       // try {
        ICloseable::Probe(reader)->Close();
       // } catch (Exception e) {
       //     loge("readAutoJoinStatus: Error closing file" + e);
       // }
    }
}

void WifiConfigStore::WriteIpAndProxyConfigurations()
{
    AutoPtr<ISparseArray> networks;
    CSparseArray::New((ISparseArray**)&networks);
    AutoPtr<ICollection> clt;
    mConfiguredNetworks->GetValues((ICollection**)&clt);
    AutoPtr<IIterator> it;
    clt->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(p);
        Boolean ephemeral = FALSE;
        config->GetEphemeral(&ephemeral);
        Int32 autoJoinStatus = 0;
        config->GetAutoJoinStatus(&autoJoinStatus);
        if (!ephemeral && autoJoinStatus != IWifiConfiguration::AUTO_JOIN_DELETED) {
            Int32 ck = ConfigKey(config);
            AutoPtr<IIpConfiguration> ipc;
            config->GetIpConfiguration((IIpConfiguration**)&ipc);
            networks->Put(ck, ipc);
        }
    }

    IpConfigStore::WriteIpAndProxyConfigurations(ipConfigFile, networks);
}

void WifiConfigStore::ReadIpAndProxyConfigurations()
{
    AutoPtr<ISparseArray> networks;
    IpConfigStore::ReadIpAndProxyConfigurations(ipConfigFile, (ISparseArray**)&networks);

    if (networks == NULL) {
        // IpConfigStore.readIpAndProxyConfigurations has already logged an error.
        return;
    }

    Int32 size = 0;
    networks->GetSize(&size);
    if (size == 0) {
        // IpConfigStore.readIpAndProxyConfigurations has already logged an error.
        return;
    }

    for (Int32 i = 0; i < size; i++) {
        Int32 id = 0;
        networks->KeyAt(i, &id);
        AutoPtr<IInterface> obj;
        mNetworkIds->Get(CoreUtils::Convert(id), (IInterface**)&obj);
        AutoPtr<IInterface> p;
        mConfiguredNetworks->Get(obj, (IInterface**)&p);
        AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(p);

        Int32 autoJoinStatus = 0;
        Boolean duplicateNetwork;
        if (config == NULL || (config->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus) == IWifiConfiguration::AUTO_JOIN_DELETED) {
            // String str("configuration found for missing network, nid=");
            // str += id; str += ", ignored, networks.size=";
            // str += size;
            // Loge(str);
        }
        else if (config != NULL && (config->GetDuplicateNetwork(&duplicateNetwork), duplicateNetwork) == TRUE) {
            if (VDBG) {
                // Int32 networkId = 0;
                // config->GetNetworkId(&networkId);
                // String ssid;
                // config->GetSSID(&ssid);
                // String str("Network configuration is not updated for duplicate network id=");
                // str += networkId; str += " SSID="; str += ssid;
                // Loge(str);
            }
        }
        else {
            AutoPtr<IInterface> p;
            networks->ValueAt(i, (IInterface**)&p);
            config->SetIpConfiguration(IIpConfiguration::Probe(p));
        }
    }
}

String WifiConfigStore::EncodeSSID(
    /* [in] */ const String& str)
{
    String tmp = RemoveDoubleQuotes(str);
    // return String.format("%x", new BigInteger(1, tmp.getBytes(Charset.forName("UTF-8"))));
    AutoPtr<ArrayOf<Byte> > bytes = tmp.GetBytes(/*Charset.forName("UTF-8")*/);
    AutoPtr<IBigInteger> bigInteger;
    CBigInteger::New(1, (*bytes), (IBigInteger**)&bigInteger);
    String value;
    bigInteger->ToString(16, &value);
    return value;
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::AddOrUpdateNetworkNative(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 uid)
{
    /*
     * If the supplied networkId is INVALID_NETWORK_ID, we create a new empty
     * network configuration. Otherwise, the networkId should
     * refer to an existing configuration.
     */

    if (VDBG) {
        String str;
        config->GetPrintableSsid(&str);
        LocalLog(String("addOrUpdateNetworkNative ") +  str);
    }

    Int32 netId = 0;
    config->GetNetworkId(&netId);
    Boolean newNetwork = FALSE;
    // networkId of INVALID_NETWORK_ID means we want to create a new network
    if (netId == IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<IInterface> _savedNetId;
        mNetworkIds->Get(CoreUtils::Convert(ConfigKey(config)), (IInterface**)&_savedNetId);
        AutoPtr<IInteger32> savedNetId = IInteger32::Probe(_savedNetId);
        // Check if either we have a network Id or a WifiConfiguration
        // matching the one we are trying to add.
        if (savedNetId == NULL) {
            AutoPtr<ICollection> clt;
            mConfiguredNetworks->GetValues((ICollection**)&clt);
            AutoPtr<IIterator> it;
            clt->GetIterator((IIterator**)&it);
            Boolean bHasNxt = FALSE;
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IWifiConfiguration> test = IWifiConfiguration::Probe(p);
                String ck, cfck;
                test->ConfigKey(&ck);
                config->ConfigKey(&cfck);
                if (ck.Equals(cfck)) {
                    Int32 nid;
                    test->GetNetworkId(&nid);
                    CInteger32::New(nid, (IInteger32**)&savedNetId);
                    String str("addOrUpdateNetworkNative ");
                    str += cfck; str += " was found, but no network Id";
                    Loge(str);
                    break;
                }
            }
        }
        if (savedNetId != NULL) {
            savedNetId->GetValue(&netId);
        }
        else {
            newNetwork = TRUE;
            netId = mWifiNative->AddNetwork();
            if (netId < 0) {
                Loge(String("Failed to add a network!"));
                return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
            }
            else {
                String str("addOrUpdateNetworkNative created netId=");
                str += StringUtils::ToString(netId);
                Loge(str);
            }
        }
    }

    Boolean updateFailed = TRUE;

    {

        String ssid;
        config->GetSSID(&ssid);
        if (!ssid.IsNull() &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::ssidVarName,
                    EncodeSSID(ssid))) {
            String str("failed to set SSID: ");
            str += ssid;
            Loge(str);
            goto setVariables;
        }

        String bssid;
        config->GetBSSID(&bssid);
        if (!bssid.IsNull()) {
            String ck;
            config->ConfigKey(&ck);
            String str("Setting BSSID for ");
            str += ck; str += " to ";
            str += bssid;
            Loge(str);
            if (!mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::bssidVarName,
                    bssid)) {
                String str("failed to set BSSID: ");
                str += bssid;
                Loge(str);
                goto setVariables;
            }
        }

        Boolean isIBSS = FALSE;
        config->GetIsIBSS(&isIBSS);
        if (isIBSS) {
            if(!mWifiNative->SetNetworkVariable(
                    netId,
                    String("mode"),//TODO IWifiConfiguration::modeVarName,
                    String("1"))) {
                Loge(String("failed to set adhoc mode"));
                goto setVariables;
            }
            Int32 freq;
            config->GetFrequency(&freq);
            if(!mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::frequencyVarName,
                    StringUtils::ToString(freq))) {
                Loge(String("failed to set frequency"));
                goto setVariables;
            }
        }

        AutoPtr<IWifiConfigurationKeyMgmt> mgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&mgmt);
        AutoPtr<ArrayOf<String> > strs;
        mgmt->GetStrings((ArrayOf<String>**)&strs);
        AutoPtr<IBitSet> bs;
        config->GetAllowedKeyManagement((IBitSet**)&bs);
        String allowedKeyManagementString = MakeString(bs, strs);
        Int32 card = 0;
        bs->Cardinality(&card);
        if (card != 0 &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationKeyMgmt::VAR_NAME,
                    allowedKeyManagementString)) {
            String str("failed to set key_mgmt: ");
            str += allowedKeyManagementString;
            Loge(str);
            goto setVariables;
        }

        AutoPtr<IBitSet> bsp;
        config->GetAllowedProtocols((IBitSet**)&bsp);
        AutoPtr<IWifiConfigurationProtocol> prot;
        CWifiConfigurationProtocol::AcquireSingleton((IWifiConfigurationProtocol**)&prot);
        AutoPtr<ArrayOf<String> > strprot;
        prot->GetStrings((ArrayOf<String>**)&strprot);
        String allowedProtocolsString = MakeString(bsp, strprot);
        bsp->Cardinality(&card);
        if (card != 0 &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationProtocol::VAR_NAME,
                    allowedProtocolsString)) {
            String str("failed to set proto: ");
            str += allowedProtocolsString;
            Loge(str);
            goto setVariables;
        }

        AutoPtr<IBitSet> bsa;
        config->GetAllowedAuthAlgorithms((IBitSet**)&bsa);
        AutoPtr<IWifiConfigurationAuthAlgorithm> algo;
        CWifiConfigurationAuthAlgorithm::AcquireSingleton((IWifiConfigurationAuthAlgorithm**)&algo);
        AutoPtr<ArrayOf<String> > stralgo;
        algo->GetStrings((ArrayOf<String>**)&stralgo);
        String allowedAuthAlgorithmsString = MakeString(bsa, stralgo);
        bsa->Cardinality(&card);
        if (card != 0 &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationAuthAlgorithm::VAR_NAME,
                    allowedAuthAlgorithmsString)) {
            String str("failed to set auth_alg: ");
            str += allowedAuthAlgorithmsString;
            Loge(str);
            goto setVariables;
        }

        AutoPtr<IBitSet> bsc;
        config->GetAllowedPairwiseCiphers((IBitSet**)&bsc);
        AutoPtr<IWifiConfigurationPairwiseCipher> paci;
        CWifiConfigurationPairwiseCipher::AcquireSingleton((IWifiConfigurationPairwiseCipher**)&paci);
        AutoPtr<ArrayOf<String> > strpaci;
        paci->GetStrings((ArrayOf<String>**)&strpaci);
        String allowedPairwiseCiphersString = MakeString(bsc, strpaci);
        bsc->Cardinality(&card);
        if (card != 0 &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationPairwiseCipher::VAR_NAME,
                    allowedPairwiseCiphersString)) {
            String str("failed to set pairwise: ");
            str += allowedPairwiseCiphersString;
            Loge(str);
            goto setVariables;
        }

        AutoPtr<IBitSet> bsgc;
        config->GetAllowedGroupCiphers((IBitSet**)&bsgc);
        AutoPtr<IWifiConfigurationGroupCipher> guci;
        CWifiConfigurationGroupCipher::AcquireSingleton((IWifiConfigurationGroupCipher**)&guci);
        AutoPtr<ArrayOf<String> > strguci;
        guci->GetStrings((ArrayOf<String>**)&strguci);
        String allowedGroupCiphersString = MakeString(bsgc, strguci);
        bsgc->Cardinality(&card);
        if (card != 0 &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationGroupCipher::VAR_NAME,
                    allowedGroupCiphersString)) {
            String str("failed to set group: ");
            str += allowedGroupCiphersString;
            Loge(str);
            goto setVariables;
        }

        // Prevent client screw-up by passing in a WifiConfiguration we gave it
        // by preventing "*" as a key.
        String preSharedKey;
        config->GetPreSharedKey(&preSharedKey);
        if (!preSharedKey.IsNull() && !preSharedKey.Equals("*") &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::pskVarName,
                    preSharedKey)) {
            Loge(String("failed to set psk"));
            goto setVariables;
        }

        Boolean hasSetKey = FALSE;
        AutoPtr<ArrayOf<String> > wepKeys;
        config->GetWepKeys((ArrayOf<String>**)&wepKeys);
        if (wepKeys != NULL) {
            AutoPtr<ArrayOf<String> > wepKeyVarNames;
            config->GetWepKeyVarNames((ArrayOf<String>**)&wepKeyVarNames);
            for (Int32 i = 0; i < wepKeys->GetLength(); i++) {
                // Prevent client screw-up by passing in a WifiConfiguration we gave it
                // by preventing "*" as a key.
                if (!((*wepKeys)[i]).IsNull() && !(*wepKeys)[i].Equals("*")) {
                    if (!mWifiNative->SetNetworkVariable(
                                netId,
                                (*wepKeyVarNames)[i],
                                (*wepKeys)[i])) {
                        // String str("failed to set wep_key");
                        // str += i; str += ": ";
                        // str += (*wepKeys)[i];
                        // Loge(str);
                        goto setVariables;
                    }
                    hasSetKey = TRUE;
                }
            }
        }

        if (hasSetKey) {
            Int32 wepTxKeyIndex = 0;
            config->GetWepTxKeyIndex(&wepTxKeyIndex);
            if (!mWifiNative->SetNetworkVariable(
                        netId,
                        IWifiConfiguration::wepTxKeyIdxVarName,
                        StringUtils::ToString(wepTxKeyIndex))) {
                // String str("failed to set wep_tx_keyidx: ");
                // str += wepTxKeyIndex;
                // Loge(str);
                goto setVariables;
            }
        }

        Int32 priority = 0;
        config->GetPriority(&priority);
        if (!mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::priorityVarName,
                    StringUtils::ToString(priority))) {
            // String str(ssid);
            // str += ": failed to set priority: ";
            // str += priority;
            // Loge(str);
            goto setVariables;
        }

        Boolean hiddenSSID = FALSE;
        config->GetHiddenSSID(&hiddenSSID);
        if (hiddenSSID && !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::hiddenSSIDVarName,
                    StringUtils::ToString(hiddenSSID ? 1 : 0))) {
            // String str(ssid);
            // str += ": failed to set hiddenSSID: ";
            // str += hiddenSSID;
            // Loge(str);
            goto setVariables;
        }

        Boolean requirePMF = FALSE;
        config->GetRequirePMF(&requirePMF);
        if (requirePMF && !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::pmfVarName,
                    String("2"))) {
            // String str(ssid);
            // str += ": failed to set requirePMF: ";
            // str += requirePMF;
            // Loge(str);
            goto setVariables;
        }

        String updateIdentifier;
        config->GetUpdateIdentifier(&updateIdentifier);
        if (!updateIdentifier.IsNull() && !mWifiNative->SetNetworkVariable(
                netId,
                IWifiConfiguration::updateIdentiferVarName,
                updateIdentifier)) {
            String str(ssid);
            str += ": failed to set updateIdentifier: ";
            str += updateIdentifier;
            Loge(str);
            goto setVariables;
        }

        AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
        config->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
        Int32 eapmd = 0;
        if (enterpriseConfig != NULL &&
                (enterpriseConfig->GetEapMethod(&eapmd), eapmd) != Elastos::Droid::Wifi::IWifiEnterpriseConfigEap::NONE) {

            if (NeedsKeyStore(enterpriseConfig)) {
                /**
                 * Keyguard settings may eventually be controlled by device policy.
                 * We check here if keystore is unlocked before installing
                 * credentials.
                 * TODO: Do we need a dialog here ?
                 */
                assert(0);//TODO
                //TODO KeyStoreState st;
                //TODO mKeyStore->State(&st);
                //TODO if (st != Elastos::Security::KeyStoreState_UNLOCKED) {
                //TODO     String str(ssid);
                //TODO    str += ": key store is locked";
                //TODO    Loge(str);
                //TODO    goto setVariables;
                //TODO }

                // try {
                    /* config passed may include only fields being updated.
                     * In order to generate the key id, fetch uninitialized
                     * fields from the currently tracked configuration
                     */
                    AutoPtr<IInterface> _currentConfig;
                    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&_currentConfig);
                    AutoPtr<IWifiConfiguration> currentConfig = IWifiConfiguration::Probe(_currentConfig);
                    String keyId;
                    config->GetKeyIdForCredentials(currentConfig, &keyId);

                    Boolean installKeys;
                    if (!(InstallKeys(enterpriseConfig, keyId, &installKeys), installKeys)) {
                        String str(ssid);
                        str += ": failed to install keys";
                        Loge(str);
                        goto setVariables;
                    }
                // } catch (IllegalStateException e) {
                //     Loge("%s invalid config for key installation", ssid.string());
                //     break setVariables;
                // }
            }

            AutoPtr<IHashMap> enterpriseFields;
            enterpriseConfig->GetFields((IHashMap**)&enterpriseFields);
            AutoPtr<ISet> st;
            enterpriseFields->GetKeySet((ISet**)&st);
            AutoPtr<IIterator> itSt;
            st->GetIterator((IIterator**)&itSt);
            Boolean bHasNxt = FALSE;
            while ((itSt->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                itSt->GetNext((IInterface**)&p);
                String key;
                ICharSequence::Probe(p)->ToString(&key);
                AutoPtr<IInterface> val;
                enterpriseFields->Get(p, (IInterface**)&val);
                String value;
                ICharSequence::Probe(val)->ToString(&value);
                if (key.Equals("password") && !value.IsNull() && value.Equals("*")) {
                    // No need to try to set an obfuscated password, which will fail
                    continue;
                }
                if (!mWifiNative->SetNetworkVariable(
                            netId,
                            key,
                            value)) {
                    RemoveKeys(enterpriseConfig);
                    // String str(ssid);
                    // str += ": failed to set ";
                    // str += key; str += ": "; str += value;
                    // Loge(str);
                    goto setVariables;
                }
            }
        }
        updateFailed = FALSE;
    } // End of setVariables
    setVariables:

    if (updateFailed) {
        if (newNetwork) {
            mWifiNative->RemoveNetwork(netId);
            // String str("Failed to set a network variable, removed network: ");
            // str += netId;
            // Loge(str);
        }
        return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    /* An update of the network variables requires reading them
     * back from the supplicant to update mConfiguredNetworks.
     * This is because some of the variables (SSID, wep keys &
     * passphrases) reflect different values when read back than
     * when written. For example, wep key is stored as * irrespective
     * of the value sent to the supplicant
     */
    AutoPtr<IInterface> _currentConfig;
    mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&_currentConfig);
    AutoPtr<IWifiConfiguration> currentConfig = IWifiConfiguration::Probe(_currentConfig);
    if (currentConfig == NULL) {
        CWifiConfiguration::New((IWifiConfiguration**)&currentConfig);
        currentConfig->SetIpAssignment(Elastos::Droid::Net::DHCP_IpAssignment);
        currentConfig->SetProxySettings(Elastos::Droid::Net::NONE_ProxySettings);
        currentConfig->SetNetworkId(netId);
        if (config != NULL) {
            // Carry over the creation parameters
            Boolean selfAdded = FALSE;
            config->GetSelfAdded(&selfAdded);
            currentConfig->SetSelfAdded(selfAdded);
            Boolean didSelfAdd = FALSE;
            config->GetDidSelfAdd(&didSelfAdd);
            currentConfig->SetDidSelfAdd(didSelfAdd);
            Boolean ephemeral = FALSE;
            config->GetEphemeral(&ephemeral);
            currentConfig->SetEphemeral(ephemeral);
            Int32 autoJoinUseAggressiveJoinAttemptThreshold = 0;
            config->GetAutoJoinUseAggressiveJoinAttemptThreshold(&autoJoinUseAggressiveJoinAttemptThreshold);
            currentConfig->SetAutoJoinUseAggressiveJoinAttemptThreshold(autoJoinUseAggressiveJoinAttemptThreshold);
            Int32 lastConnectUid = 0;
            config->GetLastConnectUid(&lastConnectUid);
            currentConfig->SetLastConnectUid(lastConnectUid);
            Int32 lastUpdateUid = 0;
            config->GetLastUpdateUid(&lastUpdateUid);
            currentConfig->SetLastUpdateUid(lastUpdateUid);
            Int32 creatorUid = 0;
            config->GetCreatorUid(&creatorUid);
            currentConfig->SetCreatorUid(creatorUid);
            String peerWifiConfiguration;
            config->GetPeerWifiConfiguration(&peerWifiConfiguration);
            currentConfig->SetPeerWifiConfiguration(peerWifiConfiguration);
        }
        if (DBG) {
            Int32 creatorUid = 0;
            config->GetCreatorUid(&creatorUid);
            String str("created new config netId=");
            str += StringUtils::ToString(netId); str += " uid=";
            str += StringUtils::ToString(creatorUid);
            Loge(str);
        }
    }

    if (uid >= 0) {
        if (newNetwork) {
            currentConfig->SetCreatorUid(uid);
        }
        else {
            currentConfig->SetLastUpdateUid(uid);
        }
    }

    if (newNetwork) {
        currentConfig->SetDirty(TRUE);
    }

    Int32 autoJoinStatus = 0;
    currentConfig->GetAutoJoinStatus(&autoJoinStatus);
    if (autoJoinStatus == IWifiConfiguration::AUTO_JOIN_DELETED) {
        // Make sure the configuration is not deleted anymore since we just
        // added or modified it.
        currentConfig->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
        currentConfig->SetSelfAdded(FALSE);
        currentConfig->SetDidSelfAdd(FALSE);
        if (DBG) {
            // String ck;
            // currentConfig->ConfigKey(&ck);
            // String str("remove deleted status netId=");
            // str += netId; str += " ";
            // str += ck;
            // Loge(str);
        }
    }

    Int32 status = 0;
    currentConfig->GetStatus(&status);
    if (status == IWifiConfigurationStatus::ENABLED) {
        // Make sure autojoin remain in sync with user modifying the configuration
        currentConfig->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
    }

    if (DBG) {
        String str("will read network variables netId=");
        str += StringUtils::ToString(netId);
        Loge(str);
    }

    ReadNetworkVariables(currentConfig);

    mConfiguredNetworks->Put(CoreUtils::Convert(netId), currentConfig);
    mNetworkIds->Put(CoreUtils::Convert(ConfigKey(currentConfig)), CoreUtils::Convert(netId));

    AutoPtr<NetworkUpdateResult> result = WriteIpAndProxyConfigurationsOnChange(currentConfig, config);
    result->SetIsNewNetwork(newNetwork);
    result->SetNetworkId(netId);

    WriteKnownNetworkHistory();

    return result;
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::WriteIpAndProxyConfigurationsOnChange(
    /* [in] */ IWifiConfiguration* currentConfig,
    /* [in] */ IWifiConfiguration* newConfig)
{
    Boolean ipChanged = FALSE;
    Boolean proxyChanged = FALSE;

    String ssid;
    currentConfig->GetSSID(&ssid);
    String nssid;
    newConfig->GetSSID(&nssid);
    if (VDBG) {
        String str("writeIpAndProxyConfigurationsOnChange: ");
        str += ssid;
        str += " -> ";
        str += nssid;
        str += " path: ";
        str += ipConfigFile;
        Loge(str);
    }

    IpConfigurationIpAssignment newAssi;
    newConfig->GetIpAssignment(&newAssi);
    IpConfigurationIpAssignment assi;
    currentConfig->GetIpAssignment(&assi);
    AutoPtr<IStaticIpConfiguration> newIpcon, ipcon;
    currentConfig->GetStaticIpConfiguration((IStaticIpConfiguration**)&newIpcon);
    newConfig->GetStaticIpConfiguration((IStaticIpConfiguration**)&ipcon);
    switch (newAssi) {
        case Elastos::Droid::Net::STATIC_IpAssignment:
            if (assi != newAssi) {
                ipChanged = TRUE;
            }
            else {
                ipChanged = !Object::Equals(newIpcon, ipcon);
            }
            break;
        case Elastos::Droid::Net::DHCP_IpAssignment:
            if (assi != newAssi) {
                ipChanged = TRUE;
            }
            break;
        case Elastos::Droid::Net::UNASSIGNED_IpAssignment:
            /* Ignore */
            break;
        default:
            Loge(String("Ignore invalid ip assignment during write"));
            break;
    }

    Elastos::Droid::Net::IpConfigurationProxySettings newsettings, settings;
    newConfig->GetProxySettings(&newsettings);
    currentConfig->GetProxySettings(&settings);
    AutoPtr<IProxyInfo> newHttpProxy;
    newConfig->GetHttpProxy((IProxyInfo**)&newHttpProxy);
    AutoPtr<IProxyInfo> currentHttpProxy;
    currentConfig->GetHttpProxy((IProxyInfo**)&currentHttpProxy);
    switch (newsettings) {
        case Elastos::Droid::Net::STATIC_ProxySettings:
        case Elastos::Droid::Net::PAC_ProxySettings:
            if (newHttpProxy != NULL) {
                proxyChanged = !Object::Equals(newHttpProxy, currentHttpProxy);
            }
            else {
                proxyChanged = (currentHttpProxy != NULL);
            }
            break;
        case Elastos::Droid::Net::NONE_ProxySettings:
            if (settings != newsettings) {
                proxyChanged = TRUE;
            }
            break;
        case Elastos::Droid::Net::UNASSIGNED_ProxySettings:
            /* Ignore */
            break;
        default:
            Loge(String("Ignore invalid proxy configuration during write"));
            break;
    }

    if (ipChanged) {
        currentConfig->SetIpAssignment(assi);
        currentConfig->SetStaticIpConfiguration(newIpcon);
        String str("IP config changed SSID = ");
        str += ssid;
        Loge(str);
        if (ipcon != NULL) {
            String str;
            IObject::Probe(ipcon)->ToString(&str);
            String er(" static configuration: ");
            er += str;
            Loge(er);
        }
    }

    if (proxyChanged) {
        currentConfig->SetProxySettings(newsettings);
        currentConfig->SetHttpProxy(newHttpProxy);
        String er("proxy changed SSID = ");
        er += ssid;
        Loge(er);
        if (currentHttpProxy != NULL) {
            String str;
            IObject::Probe(currentHttpProxy)->ToString(&str);
            er = " proxyProperties: ";
            er += str;
            Loge(er);
        }
    }

    if (ipChanged || proxyChanged) {
        WriteIpAndProxyConfigurations();
        SendConfiguredNetworksChangedBroadcast(currentConfig,
                IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    }
    return new NetworkUpdateResult(ipChanged, proxyChanged);
}

Boolean WifiConfigStore::EnterpriseConfigKeyShouldBeQuoted(
    /* [in] */ const String& key)
{
    //switch (key) {
    //    case IWifiEnterpriseConfig::EAP_KEY:
    //    case IWifiEnterpriseConfig::ENGINE_KEY:
    //        return FALSE;
    //    default:
    //        return TRUE;
    //}
    if ( key.Equals(IWifiEnterpriseConfig::EAP_KEY)
        || key.Equals(IWifiEnterpriseConfig::ENGINE_KEY)) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

void WifiConfigStore::ReadNetworkVariables(
    /* [in] */ IWifiConfiguration* config)
{

    Int32 netId = 0;
    config->GetNetworkId(&netId);
    if (netId < 0)
        return;

    /*
     * TODO: maybe should have a native method that takes an array of
     * variable names and returns an array of values. But we'd still
     * be doing a round trip to the supplicant daemon for each variable.
     */
    String value;

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::ssidVarName);
    if (!TextUtils::IsEmpty(value)) {
        if (value.GetChar(0) != '"') {
            AutoPtr<IWifiSsidHelper> hlp;
            CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&hlp);
            AutoPtr<IWifiSsid> ssid;
            hlp->CreateFromHex(value, (IWifiSsid**)&ssid);
            String str;
            IObject::Probe(ssid)->ToString(&str);
            String res("\"");
            res += str;
            res += "\"";
            config->SetSSID(res);
            //TODO: convert a hex string that is not UTF-8 decodable to a P-formatted
            //supplicant string
        }
        else {
            config->SetSSID(value);
        }
    }
    else {
        config->SetSSID(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::bssidVarName);
    if (!TextUtils::IsEmpty(value)) {
        config->SetBSSID(value);
    }
    else {
        config->SetBSSID(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::priorityVarName);
    config->SetPriority(-1);
    if (!TextUtils::IsEmpty(value)) {
        config->SetPriority(StringUtils::ParseInt32(value));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::hiddenSSIDVarName);
    config->SetHiddenSSID(FALSE);
    if (!TextUtils::IsEmpty(value)) {
        config->SetHiddenSSID(StringUtils::ParseInt32(value) != 0);
    }

    value = mWifiNative->GetNetworkVariable(netId, String("mode")/*TODO IWifiConfiguration::modeVarName*/);
    config->SetIsIBSS(FALSE);
    if (!TextUtils::IsEmpty(value)) {
        config->SetIsIBSS(StringUtils::ParseInt32(value) != 0);
    }

    value = mWifiNative->GetNetworkVariable(netId, String("frequency")/*TODO IWifiConfiguration::frequencyVarName*/);
    config->SetFrequency(0);
    if (!TextUtils::IsEmpty(value)) {
        config->SetFrequency(StringUtils::ParseInt32(value));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::wepTxKeyIdxVarName);
    config->SetWepTxKeyIndex(-1);
    if (!TextUtils::IsEmpty(value)) {
        config->SetWepTxKeyIndex(StringUtils::ParseInt32(value));
    }

    for (Int32 i = 0; i < 4; i++) {
        AutoPtr<ArrayOf<String> > wepKeyVarNames;
        config->GetWepKeyVarNames((ArrayOf<String>**)&wepKeyVarNames);
        value = mWifiNative->GetNetworkVariable(netId, (*wepKeyVarNames)[i]);
        AutoPtr<ArrayOf<String> > wepKeys;
        config->GetWepKeys((ArrayOf<String>**)&wepKeys);
        if (!TextUtils::IsEmpty(value)) {
            wepKeys->Set(i, value);
        }
        else {
            wepKeys->Set(i, String(NULL));
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::pskVarName);
    if (!TextUtils::IsEmpty(value)) {
        config->SetPreSharedKey(value);
    }
    else {
        config->SetPreSharedKey(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfigurationProtocol::VAR_NAME);
    if (!TextUtils::IsEmpty(value)) {
        AutoPtr<ArrayOf<String> > vals;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        for (Int32 i = 0; i < vals->GetLength(); i++) {
            String val = (*vals)[i];
            AutoPtr<IWifiConfigurationProtocol> prot;
            CWifiConfigurationProtocol::AcquireSingleton((IWifiConfigurationProtocol**)&prot);
            AutoPtr<ArrayOf<String> > arr;
            prot->GetStrings((ArrayOf<String>**)&arr);
            Int32 index = LookupString(val, arr);
            if (0 <= index) {
                AutoPtr<IBitSet> bs;
                config->GetAllowedProtocols((IBitSet**)&bs);
                bs->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfigurationKeyMgmt::VAR_NAME);
    if (!TextUtils::IsEmpty(value)) {
        AutoPtr<ArrayOf<String> > vals;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        for (Int32 i = 0; i < vals->GetLength(); i++) {
            String val = (*vals)[i];
            AutoPtr<IWifiConfigurationKeyMgmt> mgmt;
            CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&mgmt);
            AutoPtr<ArrayOf<String> > arr;
            mgmt->GetStrings((ArrayOf<String>**)&arr);
            Int32 index = LookupString(val, arr);
            if (0 <= index) {
                AutoPtr<IBitSet> bs;
                config->GetAllowedKeyManagement((IBitSet**)&bs);
                bs->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfigurationAuthAlgorithm::VAR_NAME);
    if (!TextUtils::IsEmpty(value)) {
        AutoPtr<ArrayOf<String> > vals;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        for (Int32 i = 0; i < vals->GetLength(); i++) {
            String val = (*vals)[i];
            AutoPtr<IWifiConfigurationAuthAlgorithm> algo;
            CWifiConfigurationAuthAlgorithm::AcquireSingleton((IWifiConfigurationAuthAlgorithm**)&algo);
            AutoPtr<ArrayOf<String> > arr;
            algo->GetStrings((ArrayOf<String>**)&arr);
            Int32 index = LookupString(val, arr);
            if (0 <= index) {
                AutoPtr<IBitSet> bs;
                config->GetAllowedAuthAlgorithms((IBitSet**)&bs);
                bs->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfigurationPairwiseCipher::VAR_NAME);
    if (!TextUtils::IsEmpty(value)) {
        AutoPtr<ArrayOf<String> > vals;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        for (Int32 i = 0; i < vals->GetLength(); i++) {
            String val = (*vals)[i];
            AutoPtr<IWifiConfigurationPairwiseCipher> paci;
            CWifiConfigurationPairwiseCipher::AcquireSingleton((IWifiConfigurationPairwiseCipher**)&paci);
            AutoPtr<ArrayOf<String> > arr;
            paci->GetStrings((ArrayOf<String>**)&arr);
            Int32 index = LookupString(val, arr);
            if (0 <= index) {
                AutoPtr<IBitSet> bs;
                config->GetAllowedPairwiseCiphers((IBitSet**)&bs);
                bs->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfigurationGroupCipher::VAR_NAME);
    if (!TextUtils::IsEmpty(value)) {
        AutoPtr<ArrayOf<String> > vals;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        for (Int32 i = 0; i < vals->GetLength(); i++) {
            String val = (*vals)[i];
            AutoPtr<IWifiConfigurationGroupCipher> grci;
            CWifiConfigurationGroupCipher::AcquireSingleton((IWifiConfigurationGroupCipher**)&grci);
            AutoPtr<ArrayOf<String> > arr;
            grci->GetStrings((ArrayOf<String>**)&arr);
            Int32 index = LookupString(val, arr);
            if (0 <= index) {
                AutoPtr<IBitSet> bs;
                config->GetAllowedGroupCiphers((IBitSet**)&bs);
                bs->Set(index);
            }
        }
    }

    AutoPtr<IWifiEnterpriseConfig> enterpriseConfig;
    config->GetEnterpriseConfig((IWifiEnterpriseConfig**)&enterpriseConfig);
    if (enterpriseConfig == NULL) {
        CWifiEnterpriseConfig::New((IWifiEnterpriseConfig**)&enterpriseConfig);
    }
    AutoPtr<IHashMap> enterpriseFields;
    enterpriseConfig->GetFields((IHashMap**)&enterpriseFields);
    for (Int32 i = 0; i < ENTERPRISE_CONFIG_SUPPLICANT_KEYS->GetLength(); i++) {
        String key = (*ENTERPRISE_CONFIG_SUPPLICANT_KEYS)[i];
        value = mWifiNative->GetNetworkVariable(netId, key);
        if (!TextUtils::IsEmpty(value)) {
            if (!EnterpriseConfigKeyShouldBeQuoted(key)) {
                value = RemoveDoubleQuotes(value);
            }
            enterpriseFields->Put(CoreUtils::Convert(key), CoreUtils::Convert(value));
        }
        else {
            enterpriseFields->Put(CoreUtils::Convert(key), CoreUtils::Convert(EMPTY_VALUE));
        }
    }

    Boolean bTemp;
    if (MigrateOldEapTlsNative(enterpriseConfig, netId, &bTemp), bTemp) {
        Boolean bRes;
        SaveConfig(&bRes);
    }

    MigrateCerts(enterpriseConfig);
    // initializeSoftwareKeystoreFlag(config.enterpriseConfig, mKeyStore);
}

String WifiConfigStore::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    Int32 length = string.GetLength();
    if ((length > 1) && (string.GetChar(0) == '"')
            && (string.GetChar(length - 1) == '"')) {
        return string.Substring(1, length - 1);
    }
    return string;
}

String WifiConfigStore::MakeString(
    /* [in] */ IBitSet* inSet,
    /* [in] */ ArrayOf<String>* strings)
{
    AutoPtr<StringBuffer> buf = new StringBuffer();
    Int32 nextSetBit = -1;

    /* Make sure all set bits are in [0, strings.length) to avoid
     * going out of bounds on strings.  (Shouldn't happen, but...) */
    AutoPtr<IBitSet> newSet;
    inSet->Get(0, strings->GetLength(), (IBitSet**)&newSet);

    while ((newSet->NextSetBit(nextSetBit + 1, &nextSetBit), nextSetBit) != -1) {
        String str = (*strings)[nextSetBit].Replace('_', '-');
        buf->Append(str);
        buf->Append(String(" "));
    }

    // remove trailing space
    Int32 num = 0;
    newSet->Cardinality(&num);
    Int32 len = 0;
    buf->GetLength(&len);
    if (num > 0) {
        //buf->SetLength(len - 1);
        len -= 1;
    }

    String result;
    buf->Substring(0, len, &result);
    return result;
}

Int32 WifiConfigStore::LookupString(
    /* [in] */ const String& _string,
    /* [in] */ ArrayOf<String>* strings)
{
    Int32 size = _string.GetLength();

    String string = _string.Replace('-', '_');

    for (Int32 i = 0; i < size; i++)
        if (string.Equals((*strings)[i]))
            return i;

    // if we ever get here, we should probably add the
    // value to WifiConfiguration to reflect that it's
    // supported by the WPA supplicant
    String er("Failed to look-up a string: ");
    er += string;
    Loge(er);

    return -1;
}

Int32 WifiConfigStore::ConfigKey(
    /* [in] */ IWifiConfiguration* config)
{
    String key;
    config->ConfigKey(&key);
    return key.GetHashCode();
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s)
{
    if (mLocalLog != NULL) {
        mLocalLog->Log(s);
    }
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s,
    /* [in] */ Boolean force)
{
    LocalLog(s);
    if (force) Loge(s);
}

void WifiConfigStore::LocalLog(
    /* [in] */ const String& s,
    /* [in] */ Int32 netId)
{
    if (mLocalLog == NULL) {
        return;
    }

    AutoPtr<IWifiConfiguration> config;
    {    AutoLock syncLock(mConfiguredNetworks);
        AutoPtr<IInterface> p;
        mConfiguredNetworks->Get(CoreUtils::Convert(netId), (IInterface**)&p);
        config = IWifiConfiguration::Probe(p);
    }

    if (config != NULL) {
        StringBuilder str(s);
        str += " ";
        String ss;
        config->GetPrintableSsid(&ss);
        str += ss;
        str += " "; str += netId;
        str += " status=";
        Int32 status = 0;
        config->GetStatus(&status);
        str += status;
        str += " key=";
        String ck;
        config->ConfigKey(&ck);
        str += ck;
        mLocalLog->Log(str.ToString());
    }
    else {
        StringBuilder str(s);
        str += " ";
        str += netId;
        mLocalLog->Log(str.ToString());
    }
}

Boolean WifiConfigStore::PutCertInKeyStore(
    /* [in] */ const String& name,
    /* [in] */ ICertificate* cert)
{
    // try {
    assert(0);//TODO
    //TODO AutoPtr<ICredentialsHelper> hlp;
    //TODO CCredentialsHelper::AcquireSingleton((ICredentialsHelper**)&hlp);
    //TODO AutoPtr<ArrayOf<Byte> > certData;
    //TODO hlp->ConvertToPem(cert, (ArrayOf<Byte>**)&certData);
    //TODO if (DBG) {
    //TODO     String str("putting certificate ");
    //TODO     str += name;
    //TODO     str += " in keystore";
    //TODO     Logger::D(TAG, str);
    //TODO }
    //TODO return mKeyStore->Put(name, certData, IProcess::WIFI_UID, IKeyStore::FLAG_NONE);

    // } catch (IOException e1) {
    //     return false;
    // } catch (CertificateException e2) {
    //     return false;
    // }
    return FALSE;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
