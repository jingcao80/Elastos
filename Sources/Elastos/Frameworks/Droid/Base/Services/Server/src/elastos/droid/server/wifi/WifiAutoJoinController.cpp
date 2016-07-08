
#include "elastos/droid/server/wifi/WifiAutoJoinController.h"
#include "elastos/droid/server/wifi/WifiParser.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::INetworkKey;
using Elastos::Droid::Net::CNetworkKey;
using Elastos::Droid::Net::IWifiKey;
using Elastos::Droid::Net::CWifiKey;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Wifi::IWifiConfigurationHelper;
using Elastos::Droid::Wifi::CWifiConfigurationHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::CInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                        WifiAutoJoinController
//=====================================================================
Int32 WifiAutoJoinController::mScanResultMaximumAge = 40000;
const Int32 WifiAutoJoinController::AUTO_JOIN_IDLE;
const Int32 WifiAutoJoinController::AUTO_JOIN_ROAMING;
const Int32 WifiAutoJoinController::AUTO_JOIN_EXTENDED_ROAMING;
const Int32 WifiAutoJoinController::AUTO_JOIN_OUT_OF_NETWORK_ROAMING;
const Int32 WifiAutoJoinController::HIGH_THRESHOLD_MODIFIER;
const String WifiAutoJoinController::TAG("WifiAutoJoinController ");
Boolean WifiAutoJoinController::DBG = FALSE;
Boolean WifiAutoJoinController::VDBG = FALSE;
const Boolean WifiAutoJoinController::mStaStaSupported = FALSE;
const Int32 WifiAutoJoinController::SCAN_RESULT_CACHE_SIZE;
const Int64 WifiAutoJoinController::loseBlackListHardMilli;
const Int64 WifiAutoJoinController::loseBlackListSoftMilli;

WifiAutoJoinController::WifiAutoJoinController(
    /* [in] */ IContext* c,
    /* [in] */ WifiStateMachine* w,
    /* [in] */ WifiConfigStore* s,
    /* [in] */ IWifiConnectionStatistics* st,
    /* [in] */ WifiNative* n)
{
    CHashMap::New((IHashMap**)&mScanResultCache);

    mContext = c;
    mWifiStateMachine = w;
    mWifiConfigStore = s;
    mWifiNative = n;
    mNetworkScoreCache = NULL;
    mWifiConnectionStatistics = st;
    AutoPtr<IInterface> _scoreManager;
    mContext->GetSystemService(IContext::NETWORK_SCORE_SERVICE, (IInterface**)&_scoreManager);
    mScoreManager = INetworkScoreManager::Probe(_scoreManager);
    if (mScoreManager == NULL) {
        // String str("Registered scoreManager NULL ");
        // str += " service ";
        // str += IContext::NETWORK_SCORE_SERVICE;
        // LogDbg(str);
    }

    if (mScoreManager != NULL) {
        mNetworkScoreCache = new WifiNetworkScoreCache();
        mNetworkScoreCache->constructor(mContext);
        mScoreManager->RegisterNetworkScoreCache(INetworkKey::TYPE_WIFI, mNetworkScoreCache);
    }
    else {
        // String str("No network score service: Couldnt register as a WiFi score Manager, type=");
        // str += StringUtils::ToString(INetworkKey::TYPE_WIFI);
        // str += " service ";
        // str += IContext::NETWORK_SCORE_SERVICE;
        // LogDbg(str);
        mNetworkScoreCache = NULL;
    }
    CArrayList::New((IArrayList**)&mBlacklistedBssids);
}

ECode WifiAutoJoinController::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0 ) {
        DBG = TRUE;
        VDBG = TRUE;
    }
    else {
        DBG = FALSE;
        VDBG = FALSE;
    }
    return NOERROR;
}

ECode WifiAutoJoinController::AddToScanCache(
    /* [in] */ IList* scanList,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(scanList);
    VALIDATE_NOT_NULL(result);

    Int32 numScanResultsKnown = 0; // Record number of scan results we knew about
    AutoPtr<IWifiConfiguration> associatedConfig;
    Boolean didAssociate = FALSE;

    AutoPtr<IArrayList> unknownScanResults;
    CArrayList::New((IArrayList**)&unknownScanResults);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs = 0;
    sys->GetCurrentTimeMillis(&nowMs);
    AutoPtr<IIterator> it;
    scanList->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IScanResult> result = IScanResult::Probe(p);
        String sid;
        result->GetSSID(&sid);
        if (sid.IsNull()) continue;

        // Make sure we record the last time we saw this result
        result->SetSeen(nowMs);

        // Fetch the previous instance for this result
        String bsid;
        result->GetBSSID(&bsid);
        AutoPtr<IInterface> _sr;
        mScanResultCache->Get(CoreUtils::Convert(bsid), (IInterface**)&_sr);
        AutoPtr<IScanResult> sr = IScanResult::Probe(_sr);
        if (sr != NULL) {
            // If there was a previous cache result for this BSSID, average the RSSI values
            Int32 level = 0;
            Int64 seen = 0;
            sr->GetLevel(&level);
            sr->GetSeen(&seen);
            result->AverageRssi(level, seen, mScanResultMaximumAge);

            // Remove the previous Scan Result - this is not necessary
            mScanResultCache->Remove(CoreUtils::Convert(bsid));
        }

        Boolean bRes = FALSE;
        mNetworkScoreCache->IsScoredNetwork(result, &bRes);
        if (!bRes) {
            AutoPtr<IWifiKey> wkey;
            // Quoted SSIDs are the only one valid at this stage
            // try {
            String str("\"");
            str += sid;
            str += String("\"");
            CWifiKey::New(str, bsid, (IWifiKey**)&wkey);
            // } catch (IllegalArgumentException e) {
            //     logDbg("AutoJoinController: received badly encoded SSID=[" + result.SSID +
            //             "] ->skipping this network");
            //     wkey = null;
            // }
            if (wkey != NULL) {
                AutoPtr<INetworkKey> nkey;
                CNetworkKey::New(wkey, (INetworkKey**)&nkey);
                //if we don't know this scan result then request a score from the scorer
                unknownScanResults->Add(nkey);
            }
            if (VDBG) {
                // String cap("");
                // result->GetCapabilities(&cap);
                // Int32 level = 0;
                // result->GetLevel(&level);
                // String str(sid);
                // str += String(" ");
                // str += bsid;
                // str += " rssi="; str += StringUtils::ToString(level);
                // str += " cap "; str += cap;
                // str += " is not scored";
                // LogDbg(str);
            }
        }
        else {
            if (VDBG) {
                // String cap("");
                // result->GetCapabilities(&cap);
                // Int32 score = 0;
                // mNetworkScoreCache->GetNetworkScore(result, &score);
                // Int32 level = 0;
                // result->GetLevel(&level);
                // String str(sid);
                // str += " ";
                // str += bsid;
                // str += " rssi="; str += level;
                // str += " cap "; str += cap;
                // str += " is scored :";
                // str += score;
                // LogDbg(str);
            }
        }

        // scanResultCache.put(result.BSSID, new ScanResult(result));
        mScanResultCache->Put(CoreUtils::Convert(bsid), result);
        // Add this BSSID to the scanResultCache of a Saved WifiConfiguration
        mWifiConfigStore->UpdateSavedNetworkHistory(result, &didAssociate);

        // If not successful, try to associate this BSSID to an existing Saved WifiConfiguration
        if (!didAssociate) {
            // We couldn't associate the scan result to a Saved WifiConfiguration
            // Hence it is untrusted
            result->SetUntrusted(TRUE);
            mWifiConfigStore->AssociateWithConfiguration(result, (IWifiConfiguration**)&associatedConfig);
            String configSID;
            if (associatedConfig != NULL && !(associatedConfig->GetSSID(&configSID), configSID.IsNull())) {
                if (VDBG) {
                    // String str("addToScanCache save associated config ");
                    // str += configSID;
                    // str += " with ";
                    // str += sid;
                    // LogDbg(str);
                }
                mWifiStateMachine->SendMessage(
                         WifiStateMachine::CMD_AUTO_SAVE_NETWORK, associatedConfig);
                didAssociate = TRUE;
            }
        }
        else {
            // If the scan result has been blacklisted fir 18 hours -> unblacklist
            Int64 now = 0;
            sys->GetCurrentTimeMillis(&now);
            Int64 blackListTimestamp = 0;
            result->GetBlackListTimestamp(&blackListTimestamp);
            if ((now - blackListTimestamp) > loseBlackListHardMilli) {
                result->SetAutoJoinStatus(IScanResult::ENABLED);
            }
        }
        if (didAssociate) {
            numScanResultsKnown++;
            Int32 isAutoJoinCandidate = 0;
            result->GetIsAutoJoinCandidate(&isAutoJoinCandidate);
            result->SetIsAutoJoinCandidate(++isAutoJoinCandidate);
        }
        else {
            result->SetIsAutoJoinCandidate(0);
        }
    }

    Int32 size = 0;
    unknownScanResults->GetSize(&size);
    if (size != 0) {
        AutoPtr<ArrayOf<IInterface*> > p = ArrayOf<IInterface*>::Alloc(size);
        AutoPtr<ArrayOf<IInterface*> > newInterfaceKeys;
        unknownScanResults->ToArray(p.Get(), (ArrayOf<IInterface*>**)&newInterfaceKeys);
        // Kick the score manager, we will get updated scores asynchronously
        AutoPtr<ArrayOf<INetworkKey*> > newKeys = ArrayOf<INetworkKey*>::Alloc(newInterfaceKeys->GetLength());
        for(Int32 i = 0; i < newInterfaceKeys->GetLength(); ++i) {
            AutoPtr<INetworkKey> nwk = INetworkKey::Probe((*newInterfaceKeys)[i]);
            newKeys->Set(i, nwk);
        }
        Boolean b = FALSE;
        mScoreManager->RequestScores(newKeys, &b);
    }
    *result = numScanResultsKnown;
    return NOERROR;
}

ECode WifiAutoJoinController::LogDbg(
    /* [in] */ const String& message)
{
    LogDbg(message, FALSE);
    return NOERROR;
}

ECode WifiAutoJoinController::LogDbg(
    /* [in] */ const String& message,
    /* [in] */ Boolean stackTrace)
{
    //Int64 now = SystemClock::GetElapsedRealtimeNanos();
    if (stackTrace) {
        AutoPtr<IThread> th = Thread::GetCurrentThread();
        // Logger::E(TAG, message + " stack:"
        //         + Thread.currentThread().getStackTrace()[2].getMethodName() + " - "
        //         + Thread.currentThread().getStackTrace()[3].getMethodName() + " - "
        //         + Thread.currentThread().getStackTrace()[4].getMethodName() + " - "
        //         + Thread.currentThread().getStackTrace()[5].getMethodName());
    }
    else {
        Logger::E(TAG, message);
    }
    return NOERROR;
}

ECode WifiAutoJoinController::NewSupplicantResults(
    /* [in] */ Boolean doAutoJoin,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 numScanResultsKnown = 0;
    AutoPtr<IList> scanList;
    mWifiStateMachine->GetScanResultsListNoCopyUnsync((IList**)&scanList);
    AddToScanCache(scanList, &numScanResultsKnown);
    AgeScanResultsOut(mScanResultMaximumAge);
    if (DBG) {
        // String str("newSupplicantResults size=");
        // Int32 size = 0;
        // mScanResultCache->GetSize(&size);
        // str += StringUtils::ToString(size);
        // str += " known=";
        // str += StringUtils::ToString(numScanResultsKnown);
        // str += " ";
        // str += StringUtils::ToString(doAutoJoin);
        // LogDbg(str);
    }
    if (doAutoJoin) {
        AttemptAutoJoin();
    }
    mWifiConfigStore->WriteKnownNetworkHistory();
    *result = numScanResultsKnown;
    return NOERROR;
}

ECode WifiAutoJoinController::NewHalScanResults()
{
    AutoPtr<IList> scanList;//mWifiScanner.syncGetScanResultsList();
    String akm = WifiParser::Parse_akm(NULL, NULL);
    LogDbg(akm);
    Int32 size = 0;
    AddToScanCache(scanList, &size);
    AgeScanResultsOut(0);
    AttemptAutoJoin();
    mWifiConfigStore->WriteKnownNetworkHistory();
    return NOERROR;
}

ECode WifiAutoJoinController::LinkQualitySignificantChange()
{
    AttemptAutoJoin();
    return NOERROR;
}

ECode WifiAutoJoinController::UpdateConfigurationHistory(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean userTriggered,
    /* [in] */ Boolean connect)
{
    AutoPtr<IWifiConfiguration> selected;
    mWifiConfigStore->GetWifiConfiguration(netId, (IWifiConfiguration**)&selected);
    if (selected == NULL) {
        // String str("updateConfigurationHistory nid=");
        // str += netId;
        // str += " no selected configuration!";
        // LogDbg(str);
        return NOERROR;
    }

    String sid;
    selected->GetSSID(&sid);
    if (sid.IsNull()) {
        // String str("updateConfigurationHistory nid=");
        // str += netId;
        // str += " no SSID in selected configuration!";
        // LogDbg(str);
        return NOERROR;
    }

    if (userTriggered) {
        // Reenable autojoin for this network,
        // since the user want to connect to this configuration
        selected->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
        selected->SetSelfAdded(FALSE);
        selected->SetDirty(TRUE);
    }

    if (DBG && userTriggered) {
        AutoPtr<IHashMap> hm;
        selected->GetConnectChoices((IHashMap**)&hm);
        if (hm != NULL) {
            // String str("updateConfigurationHistory will update ");
            // str += netId;
            // str += " now: ";
            // Int32 size = 0;
            // hm->GetSize(&size);
            // str += size;
            // str += " uid=";
            // Int32 uid = 0;
            // selected->GetCreatorUid(&uid);
            // str += uid;
            // LogDbg(str, TRUE);
        }
        else {
            // String str("updateConfigurationHistory will update ");
            // str += netId;
            // str += " uid=";
            // Int32 uid = 0;
            // selected->GetCreatorUid(&uid);
            // str += uid;
            // LogDbg(str, TRUE);
        }
    }

    if (connect && userTriggered) {
        Boolean found = FALSE;
        Int32 choice = 0;
        Int32 size = 0;

        // Reset the triggered disabled count, because user wanted to connect to this
        // configuration, and we were not.
        selected->SetNumUserTriggeredWifiDisableLowRSSI(0);
        selected->SetNumUserTriggeredWifiDisableBadRSSI(0);
        selected->SetNumUserTriggeredWifiDisableNotHighRSSI(0);
        Int32 numUserTriggeredJoinAttempts = 0;
        selected->GetNumUserTriggeredJoinAttempts(&numUserTriggeredJoinAttempts);
        selected->SetNumUserTriggeredJoinAttempts(++numUserTriggeredJoinAttempts);

        AutoPtr<IList> networks;
        mWifiConfigStore->GetRecentConfiguredNetworks(12000, FALSE, (IList**)&networks);
        if (networks != NULL) networks->GetSize(&size);
        // String dbgStr("updateConfigurationHistory found ");
        // dbgStr += size;
        // dbgStr += " networks";
        // LogDbg(dbgStr);
        if (networks != NULL) {
            AutoPtr<IIterator> itNW;
            networks->GetIterator((IIterator**)&itNW);
            Boolean bHasNxt = FALSE;
            while ((itNW->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                itNW->GetNext((IInterface**)&p);
                AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(p);
                if (DBG) {
                    // String sid;
                    // config->GetSSID(&sid);
                    // Int32 id = 0;
                    // config->GetNetworkId(&id);
                    // String str("updateConfigurationHistory got ");
                    // str += sid;
                    // str += " nid=";
                    // str += id;
                    // LogDbg(str);
                }

                String strSelet, strConfig;
                selected->ConfigKey(TRUE, &strSelet);
                config->ConfigKey(TRUE, &strConfig);
                if (strSelet.Equals(strConfig)) {
                    found = TRUE;
                    continue;
                }

                // Compare RSSI values so as to evaluate the strength of the user preference
                Int32 order = 0;
                CompareWifiConfigurationsRSSI(config, selected, String(NULL), &order);

                if (order < -30) {
                    // Selected configuration is worse than the visible configuration
                    // hence register a strong choice so as autojoin cannot override this
                    // for instance, the user has select a network
                    // with 1 bar over a network with 3 bars...
                    choice = 60;
                }
                else if (order < -20) {
                    choice = 50;
                }
                else if (order < -10) {
                    choice = 40;
                }
                else if (order < 20) {
                    // Selected configuration is about same or has a slightly better RSSI
                    // hence register a weaker choice, here a difference of at least +/-30 in
                    // RSSI comparison triggered by autoJoin will override the choice
                    choice = 30;
                }
                else {
                    // Selected configuration is better than the visible configuration
                    // hence we do not know if the user prefers this configuration strongly
                    choice = 20;
                }

                // The selected configuration was preferred over a recently seen config
                // hence remember the user's choice:
                // add the recently seen config to the selected's connectChoices array

                AutoPtr<IHashMap> selHM;
                selected->GetConnectChoices((IHashMap**)&selHM);
                if (selHM == NULL) {
                    CHashMap::New((IHashMap**)&selHM);
                    selected->SetConnectChoices(selHM);
                }

                // dbgStr = "updateConfigurationHistory add a choice ";
                // dbgStr += strSelet;
                // dbgStr += " over ";
                // dbgStr += strConfig;
                // dbgStr += " choice ";
                // dbgStr += choice;
                // LogDbg(dbgStr);

                AutoPtr<IInterface> _currentChoice;
                selHM->Get(CoreUtils::Convert(strConfig), (IInterface**)&_currentChoice);
                AutoPtr<IInteger32> currentChoice = IInteger32::Probe(_currentChoice);
                if (currentChoice != NULL) {
                    // User has made this choice multiple time in a row, so bump up a lot
                    Int32 val = 0;
                    currentChoice->GetValue(&val);
                    choice += val;
                }
                // Add the visible config to the selected's connect choice list
                selHM->Put(CoreUtils::Convert(strConfig), CoreUtils::Convert(choice));

                AutoPtr<IHashMap> cfhm;
                config->GetConnectChoices((IHashMap**)&cfhm);
                if (cfhm != NULL) {
                    if (VDBG) {
                        // dbgStr = "updateConfigurationHistory will remove ";
                        // dbgStr += strSelet;
                        // dbgStr += " from ";
                        // dbgStr += strConfig;
                        // LogDbg(dbgStr);
                    }
                    // Remove the selected from the recently seen config's connectChoice list
                    cfhm->Remove(CoreUtils::Convert(strSelet));

                    AutoPtr<IHashMap> linkedConfigurations;
                    selected->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
                    if (linkedConfigurations != NULL) {
                        // Remove the selected's linked configuration from the
                        // recently seen config's connectChoice list
                        AutoPtr<ISet> st;
                        linkedConfigurations->GetKeySet((ISet**)&st);
                        AutoPtr<IIterator> it;
                        st->GetIterator((IIterator**)&it);
                        Boolean bHasNxt = FALSE;
                        while ((it->HasNext(&bHasNxt), bHasNxt)) {
                            AutoPtr<IInterface> key;
                            it->GetNext((IInterface**)&key);
                           cfhm->Remove(key);
                        }
                    }
                }
            }
            if (found == FALSE) {
                // We haven't found the configuration that the user just selected in our
                // scan cache.
                // In that case we will need a new scan before attempting to connect to this
                // configuration anyhow and thus we can process the scan results then.
                // dbgStr = "updateConfigurationHistory try to connect to an old network!! : ";
                // String strSelet;
                // selected->ConfigKey(TRUE, &strSelet);
                // dbgStr += strSelet;
                // LogDbg(dbgStr);
            }

            AutoPtr<IHashMap> shm;
            selected->GetConnectChoices((IHashMap**)&shm);
            if (shm != NULL) {
                if (VDBG) {
                    // Int32 size = 0;
                    // shm->GetSize(&size);
                    // dbgStr = "updateConfigurationHistory ";
                    // dbgStr += netId;
                    // dbgStr += " now: ";
                    // dbgStr += size;
                    // LogDbg(dbgStr);
                }
            }
        }
    }

    // TODO: write only if something changed
    if (userTriggered || connect) {
        mWifiConfigStore->WriteKnownNetworkHistory();
    }
    return NOERROR;
}

ECode WifiAutoJoinController::GetConnectChoice(
    /* [in] */ IWifiConfiguration* source,
    /* [in] */ IWifiConfiguration* target,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(source);
    VALIDATE_NOT_NULL(target);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInteger32> choice;
    if (source == NULL || target == NULL) {
        *result = 0;
        return NOERROR;
    }

    AutoPtr<IHashMap> hm;
    source->GetConnectChoices((IHashMap**)&hm);
    String strTarget;
    target->ConfigKey(TRUE, &strTarget);
    AutoPtr<IHashMap> linkedConfigurations;
    source->GetLinkedConfigurations((IHashMap**)&linkedConfigurations);
    if (hm != NULL) {
        Boolean bConKey = FALSE;
        hm->ContainsKey(CoreUtils::Convert(strTarget), &bConKey);
        AutoPtr<IInterface> _choice;
        hm->Get(CoreUtils::Convert(strTarget), (IInterface**)&_choice);
        choice = IInteger32::Probe(_choice);
    }
    else if (linkedConfigurations != NULL) {
        AutoPtr<ISet> st;
        linkedConfigurations->GetKeySet((ISet**)&st);
        AutoPtr<IIterator> it;
        st->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            String key;
            ICharSequence::Probe(p)->ToString(&key);
            AutoPtr<IWifiConfiguration> config;
            mWifiConfigStore->GetWifiConfiguration(key, (IWifiConfiguration**)&config);
            if (config != NULL) {
                AutoPtr<IHashMap> configHM;
                config->GetConnectChoices((IHashMap**)&configHM);
                if (configHM != NULL) {
                    AutoPtr<IInterface> p;
                    configHM->Get(CoreUtils::Convert(strTarget), (IInterface**)&p);
                    choice = IInteger32::Probe(p);
                }
            }
        }
    }

    if (choice == NULL) {
        //We didn't find the connect choice
        *result = 0;
        return NOERROR;
    }
    else {
        Int32 val = 0;
        choice->GetValue(&val);
        if (val < 0) {
            CInteger32::New(20, (IInteger32**)&choice); // Compatibility with older files
        }
        choice->GetValue(&val);
        *result = val;
        return NOERROR;
    }
    return NOERROR;
}

ECode WifiAutoJoinController::GetScoreFromVisibility(
    /* [in] */ IWifiConfigurationVisibility* visibility,
    /* [in] */ Int32 rssiBoost,
    /* [in] */ const String& dbg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(visibility);
    VALIDATE_NOT_NULL(result);

    Int32 rssiBoost5 = 0;
    Int32 score = 0;

    /**
     * Boost RSSI value of 5GHz bands iff the base value is better than threshold
     * This implements band preference where we prefer 5GHz if RSSI5 is good enough, whereas
     * we prefer 2.4GHz otherwise.
     * Note that 2.4GHz doesn't need a boost since at equal power the RSSI is typically
     * at least 6-10 dB higher
     */
    Int32 rssi5 = 0;
    visibility->GetRssi5(&rssi5);
    RssiBoostFrom5GHzRssi(rssi5, dbg + "->", &rssiBoost5);
    Int32 rssi24 = 0;
    visibility->GetRssi24(&rssi24);

    // Select which band to use so as to score a
    if (rssi5 + rssiBoost5 > rssi24) {
        // Prefer a's 5GHz
        score = rssi5 + rssiBoost5 + rssiBoost;
    }
    else {
        // Prefer a's 2.4GHz
        score = rssi24 + rssiBoost;
    }

    *result = score;
    return NOERROR;
}

ECode WifiAutoJoinController::CompareWifiConfigurationsRSSI(
    /* [in] */ IWifiConfiguration* a,
    /* [in] */ IWifiConfiguration* b,
    /* [in] */ const String& currentConfiguration,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(a);
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(result);

    Int32 order = 0;

    // Boost used so as to favor current config
    Int32 aRssiBoost = 0;
    Int32 bRssiBoost = 0;

    Int32 scoreA = 0;
    Int32 scoreB = 0;

    // Retrieve the visibility
    AutoPtr<IWifiConfigurationVisibility> astatus;
    a->GetVisibility((IWifiConfigurationVisibility**)&astatus);
    AutoPtr<IWifiConfigurationVisibility> bstatus;
    b->GetVisibility((IWifiConfigurationVisibility**)&bstatus);;
    if (astatus == NULL || bstatus == NULL) {
        // Error visibility wasn't set
        LogDbg(String("    compareWifiConfigurations NULL band status!"));
        *result = 0;
        return NOERROR;
    }

    // Apply Hysteresis, boost RSSI of current configuration
    String aCK, bCK;
    a->ConfigKey(&aCK);
    b->ConfigKey(&bCK);
    if (NULL != currentConfiguration) {
        if (aCK.Equals(currentConfiguration)) {
            aRssiBoost = +10;
        }
        else if (bCK.Equals(currentConfiguration)) {
            bRssiBoost = +10;
        }
    }

    if (VDBG) {
        // Int32 arssi5 = 0, brssi5 = 0, arssi24 = 0, brssi24 = 0;
        // astatus->GetRssi5(&arssi5);
        // astatus->GetRssi24(&arssi24);
        // bstatus->GetRssi5(&brssi5);
        // bstatus->GetRssi24(&brssi24);
        // StringBuilder str("    compareWifiConfigurationsRSSI: ");
        // str += aCK;
        // str += " ";
        // str += arssi24;
        // str += ",";
        // str += arssi5;
        // str += " boost=";
        // str += aRssiBoost;
        // str += " ";
        // str += bCK;
        // str += " ";
        // str += brssi24;
        // str += ",";
        // str += brssi5;
        // str += " boost=";
        // str += bRssiBoost;
        // LogDbg(str.ToString());
    }

    GetScoreFromVisibility(astatus, aRssiBoost, aCK, &scoreA);
    GetScoreFromVisibility(bstatus, bRssiBoost, bCK, &scoreB);

    // Compare a and b
    // If a score is higher then a > b and the order is descending (negative)
    // If b score is higher then a < b and the order is ascending (positive)
    order = scoreB - scoreA;

    // Normalize the order to [-50, +50]
    if (order > 50) {
        order = 50;
    }
    else if (order < -50) {
        order = -50;
    }

    if (VDBG) {
        // String prefer(" = ");
        // if (order > 0) {
        //     prefer = " < "; // Ascending
        // }
        // else if (order < 0) {
        //     prefer = " > "; // Descending
        // }
        // Int32 arssi5 = 0, brssi5 = 0, arssi24 = 0, brssi24 = 0;
        // astatus->GetRssi5(&arssi5);
        // astatus->GetRssi24(&arssi24);
        // bstatus->GetRssi5(&brssi5);
        // bstatus->GetRssi24(&brssi24);
        // Int32 anum5 = 0, anum24 = 0, bnum5 = 0, bnum24 = 0;
        // astatus->GetNum5(&anum5);
        // astatus->GetNum24(&anum24);
        // bstatus->GetNum5(&bnum5);
        // bstatus->GetNum24(&bnum24);
        // StringBuilder str("    compareWifiConfigurationsRSSI ");
        // str += aCK;
        // str += " rssi=("; str += arssi24;
        // str += ","; str += arssi5;
        // str += ") num=("; str += anum24;
        // str += ","; str += anum5;
        // str += ")";
        // str += " scorea="; str += scoreA;
        // str += prefer;
        // str += bCK;
        // str += " rssi=("; str += brssi24; str += ",";
        // str += brssi5; str += ") num=(";
        // str += bnum24; str += ",";
        // str += bnum5; str += ")";
        // str += " scoreb="; str += scoreB;
        // str += " -> "; str += order;
        // LogDbg(str.ToString());
    }

    *result = order;
    return NOERROR;
}

ECode WifiAutoJoinController::CompareWifiConfigurationsWithScorer(
    /* [in] */ IWifiConfiguration* a,
    /* [in] */ IWifiConfiguration* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(a);
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(result);

    Int32 aRssiBoost = 0;
    Int32 bRssiBoost = 0;

    // Apply Hysteresis : boost RSSI of current configuration before
    // looking up the score
    String aConKey, bConKey;
    a->ConfigKey(&aConKey);
    b->ConfigKey(&bConKey);
    if (!mCurrentConfigurationKey.IsNull()) {
        if (aConKey.Equals(mCurrentConfigurationKey)) {
            aRssiBoost += 20;
        }
        else if (bConKey.Equals(mCurrentConfigurationKey)) {
            bRssiBoost += 20;
        }
    }
    Int32 scoreA = 0;
    GetConfigNetworkScore(a, 3000, aRssiBoost, &scoreA);
    Int32 scoreB = 0;
    GetConfigNetworkScore(b, 3000, bRssiBoost, &scoreB);

    // Both configurations need to have a score for the scorer to be used
    // ...and the scores need to be different:-)
    if (scoreA == WifiNetworkScoreCache::INVALID_NETWORK_SCORE
            || scoreB == WifiNetworkScoreCache::INVALID_NETWORK_SCORE) {
        if (VDBG)  {
            String str("    compareWifiConfigurationsWithScorer no-scores: ");
            str += aConKey;
            str += " ";
            str += bConKey;
            LogDbg(str);
        }
        *result = 0;
        return NOERROR;
    }

    if (VDBG) {
        String prefer(" = ");
        if (scoreA < scoreB) {
            prefer = " < ";
        }
        if (scoreA > scoreB) {
            prefer = " > ";
        }
        AutoPtr<IWifiConfigurationVisibility> avisibility;
        a->GetVisibility((IWifiConfigurationVisibility**)&avisibility);
        AutoPtr<IWifiConfigurationVisibility> bvisibility;
        b->GetVisibility((IWifiConfigurationVisibility**)&bvisibility);
        Int32 arssi5 = 0, brssi5 = 0, arssi24 = 0, brssi24 = 0;
        avisibility->GetRssi5(&arssi5);
        avisibility->GetRssi24(&arssi24);
        bvisibility->GetRssi5(&brssi5);
        bvisibility->GetRssi24(&brssi24);
        Int32 anum5 = 0, anum24 = 0, bnum5 = 0, bnum24 = 0;
        avisibility->GetNum5(&anum5);
        avisibility->GetNum24(&anum24);
        bvisibility->GetNum5(&bnum5);
        bvisibility->GetNum24(&bnum24);
        // StringBuilder str("    compareWifiConfigurationsWithScorer ");
        // str += aConKey;
        // str += " rssi=("; str += arssi24;
        // str += ","; str += arssi5;
        // str += ") num=("; str += anum24;
        // str += ","; str += anum5; str += ")";
        // str += " sc="; str += scoreA;
        // str += prefer; str += bConKey;
        // str += " rssi=("; str += brssi24;
        // str += ","; str += brssi5;
        // str += ") num=("; str += bnum24;
        // str += ","; str += bnum5; str += ")";
        // str += " sc="; str += scoreB;
        // str += " -> "; str += scoreB - scoreA;
        // LogDbg(str.ToString());
    }

    // If scoreA > scoreB, the comparison is descending hence the return value is negative
    *result = scoreB - scoreA;
    return NOERROR;
}

ECode WifiAutoJoinController::CompareWifiConfigurations(
    /* [in] */ IWifiConfiguration* a,
    /* [in] */ IWifiConfiguration* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(a);
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(result);

    Int32 order = 0;
    String lastSelectedConfiguration;
    mWifiConfigStore->GetLastSelectedConfiguration(&lastSelectedConfiguration);
    Boolean linked = FALSE;

    AutoPtr<IHashMap> alinkedConfigurations;
    a->GetLinkedConfigurations((IHashMap**)&alinkedConfigurations);
    AutoPtr<IHashMap> blinkedConfigurations;
    b->GetLinkedConfigurations((IHashMap**)&blinkedConfigurations);
    Int32 aAutoJoinStatus = 0, bAutoJoinStatus = 0;
    a->GetAutoJoinStatus(&aAutoJoinStatus);
    b->GetAutoJoinStatus(&bAutoJoinStatus);
    String aConKey, bConKey;
    a->ConfigKey(TRUE, &aConKey);
    b->ConfigKey(TRUE, &bConKey);
    if ((alinkedConfigurations != NULL) && (blinkedConfigurations != NULL)
            && (aAutoJoinStatus == IWifiConfiguration::AUTO_JOIN_ENABLED)
            && (bAutoJoinStatus == IWifiConfiguration::AUTO_JOIN_ENABLED)) {
        AutoPtr<IInterface> pA, pB;
        alinkedConfigurations->Get(CoreUtils::Convert(bConKey), (IInterface**)&pA);
        blinkedConfigurations->Get(CoreUtils::Convert(aConKey), (IInterface**)&pB);
        if ((pA != NULL)
            && (pB != NULL)) {
            linked = TRUE;
        }
    }

    Boolean aEphemeral = FALSE, bEphemeral = FALSE;
    if (aEphemeral && bEphemeral == FALSE) {
        if (VDBG) {
            String str("    compareWifiConfigurations ephemeral and prefers ");
            str += bConKey;
            str += " over ";
            str += aConKey;
            LogDbg(str);
        }
        *result = 1; // b is of higher priority - ascending
        return NOERROR;
    }
    if (bEphemeral && aEphemeral == FALSE) {
        if (VDBG) {
            String str("    compareWifiConfigurations ephemeral and prefers ");
            str += aConKey;
            str += " over ";
            str += bConKey;
            LogDbg(str);
        }
        *result = -1; // a is of higher priority - descending
        return NOERROR;
    }

    // Apply RSSI, in the range [-5, +5]
    // after band adjustment, +n difference roughly corresponds to +10xn dBm
    Int32 rss = 0;
    CompareWifiConfigurationsRSSI(a, b, mCurrentConfigurationKey, &rss);
    order = order + rss;

    // If the configurations are not linked, compare by user's choice, only a
    // very high RSSI difference can then override the choice
    if (!linked) {
        Int32 choice = 0;
        GetConnectChoice(a, b, &choice);
        if (choice > 0) {
            // a is of higher priority - descending
            order = order - choice;
            if (VDBG) {
                // String str("    compareWifiConfigurations prefers ");
                // str += aConKey; str += " over ";
                // str += bConKey; str += " due to user choice of ";
                // str += choice; str += " order -> ";
                // str += order;
                // LogDbg(str);
            }
        }

        GetConnectChoice(b, a, &choice);
        if (choice > 0) {
            // a is of lower priority - ascending
            order = order + choice;
            if (VDBG) {
                // String str("    compareWifiConfigurations prefers ");
                // str += bConKey; str += " over ";
                // str += aConKey; str += " due to user choice of ";
                // str += choice; str += " order ->";
                // str += order;
                // LogDbg(str);
            }
        }
    }

    if (order == 0) {
        // We don't know anything - pick the last seen i.e. K behavior
        // we should do this only for recently picked configurations
        Int32 aPriority = 0, bPriority = 0;
        if (aPriority > bPriority) {
            // a is of higher priority - descending
            if (VDBG) {
                // String str("    compareWifiConfigurations prefers -1 ");
                // str += aConKey; str += " over ";
                // str += bConKey; str += " due to priority";
                // LogDbg(str);
            }

            order = -1;
        }
        else if (aPriority < bPriority) {
            // a is of lower priority - ascending
            if (VDBG) {
                // String str("    compareWifiConfigurations prefers +1 ");
                // str += bConKey; str += " over ";
                // str += aConKey; str += " due to priority";
                // LogDbg(str);
            }
            order = 1;
        }
    }

    String sorder(" == ");
    if (order > 0) {
        sorder = " < ";
    }
    else if (order < 0) {
        sorder = " > ";
    }

    if (VDBG) {
        // String str("compareWifiConfigurations: ");
        // str += aConKey; str += sorder;
        // str += bConKey; str += " order ";
        // str += order;
        // LogDbg(str);
    }

    *result = order;
    return NOERROR;
}

ECode WifiAutoJoinController::IsBadCandidate(
    /* [in] */ Int32 rssi5,
    /* [in] */ Int32 rssi24,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (rssi5 < -80 && rssi24 < -90);
    return NOERROR;
}

ECode WifiAutoJoinController::CompareWifiConfigurationsTop(
    /* [in] */ IWifiConfiguration* a,
    /* [in] */ IWifiConfiguration* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(a);
    VALIDATE_NOT_NULL(b);
    VALIDATE_NOT_NULL(result);

    Int32 scorerOrder = 0;
    CompareWifiConfigurationsWithScorer(a, b, &scorerOrder);
    Int32 order = 0;
    CompareWifiConfigurations(a, b, &order);

    if (scorerOrder * order < 0) {
        if (VDBG) {
            // String str("    -> compareWifiConfigurationsTop: ");
            // str += "scorer override "; str += scorerOrder;
            // str += " "; str += order;
            // LogDbg(str);
        }
        // For debugging purpose, remember that an override happened
        // during that autojoin Attempt
        didOverride = TRUE;
        Int32 aNumScorerOverride = 0, bNumScorerOverride = 0;
        a->GetNumScorerOverride(&aNumScorerOverride);
        a->SetNumScorerOverride(++aNumScorerOverride);
        b->GetNumScorerOverride(&bNumScorerOverride);
        b->SetNumScorerOverride(++bNumScorerOverride);
    }

    if (scorerOrder != 0) {
        // If the scorer came up with a result then use the scorer's result, else use
        // the order provided by the base comparison function
        order = scorerOrder;
    }
    *result = order;
    return NOERROR;
}

ECode WifiAutoJoinController::RssiBoostFrom5GHzRssi(
    /* [in] */ Int32 rssi,
    /* [in] */ const String& dbg,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (!mWifiConfigStore->enable5GHzPreference) {
        *result = 0;
        return NOERROR;
    }
    if (rssi > mWifiConfigStore->bandPreferenceBoostThreshold5) {
        // Boost by 2 dB for each point
        //    Start boosting at -65
        //    Boost by 20 if above -55
        //    Boost by 40 if abore -45
        Int32 boost = mWifiConfigStore->bandPreferenceBoostFactor5
                *(rssi - mWifiConfigStore->bandPreferenceBoostThreshold5);
        if (boost > 50) {
            // 50 dB boost is set so as to overcome the hysteresis of +20 plus a difference of
            // 25 dB between 2.4 and 5GHz band. This allows jumping from 2.4 to 5GHz
            // consistently
            boost = 50;
        }
        if (VDBG && !dbg.IsNull()) {
            // String str("        ");
            // str += dbg; str += ":    rssi5 ";
            // str += rssi; str += " boost ";
            // str += boost;
            // LogDbg(str);
        }
        *result = boost;
        return NOERROR;
    }

    if (rssi < mWifiConfigStore->bandPreferencePenaltyThreshold5) {
        // penalize if < -75
        Int32 boost = mWifiConfigStore->bandPreferencePenaltyFactor5
                *(rssi - mWifiConfigStore->bandPreferencePenaltyThreshold5);
        *result = boost;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode WifiAutoJoinController::AttemptRoam(
    /* [in] */ IScanResult* a,
    /* [in] */ IWifiConfiguration* current,
    /* [in] */ Int32 age,
    /* [in] */ const String& currentBSSID,
    /* [out] */ IScanResult** result)
{
    VALIDATE_NOT_NULL(a);
    VALIDATE_NOT_NULL(current);
    VALIDATE_NOT_NULL(result);

    if (current == NULL) {
        if (VDBG) {
            LogDbg(String("attemptRoam not associated"));
        }
        AutoPtr<IScanResult> _a = a;
        *result = _a;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IHashMap> scanResultCache;
    current->GetScanResultCache((IHashMap**)&scanResultCache);
    if (scanResultCache == NULL) {
        if (VDBG) {
            LogDbg(String("attemptRoam no scan cache"));
        }
        AutoPtr<IScanResult> _a = a;
        *result = _a;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    Int32 size = 0;
    scanResultCache->GetSize(&size);
    if (size > 6) {
        if (VDBG) {
            // String str("attemptRoam scan cache size ");
            // str += size;
            // str += " --> bail";
            // LogDbg(str);
        }
        // Implement same SSID roaming only for configurations
        // that have less than 4 BSSIDs
        AutoPtr<IScanResult> _a = a;
        *result = _a;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    String bssid;
    current->GetBSSID(&bssid);
    if (!bssid.IsNull() && !bssid.Equals("any")) {
        if (DBG) {
            String str("attemptRoam() BSSID is set ");
            str += bssid;
            str += " -> bail";
            LogDbg(str);
        }
        AutoPtr<IScanResult> _a = a;
        *result = _a;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // Determine which BSSID we want to associate to, taking account
    // relative strength of 5 and 2.4 GHz BSSIDs
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs = 0;
    sys->GetCurrentTimeMillis(&nowMs);
    AutoPtr<ICollection> cls;
    scanResultCache->GetValues((ICollection**)&cls);
    AutoPtr<IIterator> it;
    cls->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IScanResult> b = IScanResult::Probe(p);
        Int32 bRssiBoost5 = 0;
        Int32 aRssiBoost5 = 0;
        Int32 bRssiBoost = 0;
        Int32 aRssiBoost = 0;
        Int64 bSeen = 0;
        b->GetSeen(&bSeen);
        String bBSSID;
        b->GetBSSID(&bBSSID);
        Int32 bAutoJoinStatus = 0;
        b->GetAutoJoinStatus(&bAutoJoinStatus);
        Int32 bNumIpConfigFailures = 0;
        b->GetNumIpConfigFailures(&bNumIpConfigFailures);
        if ((bSeen == 0) || bBSSID.IsNull()
                || ((nowMs - bSeen) > age)
                || bAutoJoinStatus != IScanResult::ENABLED
                || bNumIpConfigFailures > 8) {
            continue;
        }

        // Pick first one
        if (a == NULL) {
            a = b;
            continue;
        }

        Int32 aNumIpConfigFailures = 0;
        a->GetNumIpConfigFailures(&aNumIpConfigFailures);
        String aBSSID;
        a->GetBSSID(&aBSSID);
        Int32 aLevel = 0, bLevel = 0;
        a->GetLevel(&aLevel);
        b->GetLevel(&bLevel);
        Int32 aFrequency = 0, bFrequency = 0;
        a->GetFrequency(&aFrequency);
        b->GetFrequency(&bFrequency);
        if (bNumIpConfigFailures < (aNumIpConfigFailures - 1)) {
            // Prefer a BSSID that doesn't have less number of Ip config failures
            // String str("attemptRoam: ");
            // str += bBSSID; str += " rssi="; str += bLevel;
            // str += " ipfail="; str += bNumIpConfigFailures;
            // str += " freq="; str += bFrequency;
            // str += " > "; str += aBSSID; str += " rssi=";
            // str += aLevel; str += " ipfail="; str += aNumIpConfigFailures;
            // str += " freq="; str += aFrequency;
            // LogDbg(str);
            a = b;
            continue;
        }

        // Apply hysteresis: we favor the currentBSSID by giving it a boost
        if (currentBSSID != NULL && currentBSSID.Equals(bBSSID)) {
            // Reduce the benefit of hysteresis if RSSI <= -75
            if (bLevel <= mWifiConfigStore->bandPreferencePenaltyThreshold5) {
                bRssiBoost = mWifiConfigStore->associatedHysteresisLow;
            }
            else {
                bRssiBoost = mWifiConfigStore->associatedHysteresisHigh;
            }
        }
        if (currentBSSID != NULL && currentBSSID.Equals(aBSSID)) {
            if (aLevel <= mWifiConfigStore->bandPreferencePenaltyThreshold5) {
                // Reduce the benefit of hysteresis if RSSI <= -75
                aRssiBoost = mWifiConfigStore->associatedHysteresisLow;
            }
            else {
                aRssiBoost = mWifiConfigStore->associatedHysteresisHigh;
            }
        }

        // Favor 5GHz: give a boost to 5GHz BSSIDs, with a slightly progressive curve
        //   Boost the BSSID if it is on 5GHz, above a threshold
        //   But penalize it if it is on 5GHz and below threshold
        //
        //   With he current threshold values, 5GHz network with RSSI above -55
        //   Are given a boost of 30DB which is enough to overcome the current BSSID
        //   hysteresis (+14) plus 2.4/5 GHz signal strength difference on most cases
        //
        // The "current BSSID" Boost must be added to the BSSID's level so as to introduce
        // some amount of hysteresis
        Boolean bIs5 = FALSE, aIs5 = FALSE;
        a->Is5GHz(&aIs5);
        b->Is5GHz(&bIs5);
        if (bIs5) {
            RssiBoostFrom5GHzRssi(bLevel + bRssiBoost, bBSSID, &bRssiBoost5);
        }
        if (aIs5) {
            RssiBoostFrom5GHzRssi(aLevel + aRssiBoost, aBSSID, &aRssiBoost5);
        }

        if (VDBG)  {
            String comp(" < ");
            if (bLevel + bRssiBoost + bRssiBoost5 > aLevel + aRssiBoost + aRssiBoost5) {
                comp = " > ";
            }
            // String str("attemptRoam: ");
            // str += bBSSID; str += " rssi="; str += bLevel;
            // str += " boost="; str += bRssiBoost; str += "/";
            // str += bRssiBoost5; str += " freq=";
            // str += bFrequency; str += comp; str += aBSSID;
            // str += " rssi="; str += aLevel; str += " boost=";
            // str += aRssiBoost; str += "/"; str += aRssiBoost5;
            // str += " freq="; str += aFrequency;
            // LogDbg(str);
        }

        // Compare the RSSIs after applying the hysteresis boost and the 5GHz
        // boost if applicable
        if (bLevel + bRssiBoost + bRssiBoost5 > aLevel + aRssiBoost + aRssiBoost5) {
            // b is the better BSSID
            a = b;
        }
    }
    if (a != NULL) {
        if (VDBG)  {
            String aBSSID;
            a->GetBSSID(&aBSSID);
            Int32 aLevel = 0;
            a->GetLevel(&aLevel);
            Int32 aFrequency = 0;
            a->GetFrequency(&aFrequency);
            String ck;
            current->ConfigKey(&ck);
            StringBuilder sb;
            sb.Append("attemptRoam: ");
            sb.Append(ck);
            sb.Append(" Found ");
            sb.Append(aBSSID);
            sb.Append(" rssi=");
            sb.Append(aLevel);
            sb.Append(" freq=");
            sb.Append(aFrequency);
            if (currentBSSID != NULL) {
                sb.Append(" Current: ");
                sb.Append(currentBSSID);
            }
            sb.Append("\n");
            LogDbg(sb.ToString());
        }
    }
    AutoPtr<IScanResult> _a = a;
    *result = _a;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode WifiAutoJoinController::GetConfigNetworkScore(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 age,
    /* [in] */ Int32 rssiBoost,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(config);
    VALIDATE_NOT_NULL(result);

    String ck;
    config->ConfigKey(&ck);
    if (mNetworkScoreCache == NULL) {
        if (VDBG) {
            String str("       getConfigNetworkScore for ");
            str += ck;
            str += "  -> no scorer, hence no scores";
            LogDbg(str);
        }
        *result = WifiNetworkScoreCache::INVALID_NETWORK_SCORE;
        return NOERROR;
    }
    AutoPtr<IHashMap> hm;
    config->GetScanResultCache((IHashMap**)&hm);
    if (hm == NULL) {
        if (VDBG) {
            String str("       getConfigNetworkScore for ");
            str += ck;
            str += " -> no scan cache";
            LogDbg(str);
        }
        *result = WifiNetworkScoreCache::INVALID_NETWORK_SCORE;
        return NOERROR;
    }

    // Get current date
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs = 0;
    sys->GetCurrentTimeMillis(&nowMs);

    Int32 startScore = -10000;

    // Run thru all cached scan results
    AutoPtr<ICollection> cls;
    hm->GetValues((ICollection**)&cls);
    AutoPtr<IIterator> it;
    cls->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IScanResult> result = IScanResult::Probe(p);
        Int64 seen = 0;
        result->GetSeen(&seen);
        if ((nowMs - seen) < age) {
            Int32 sc = 0;
            mNetworkScoreCache->GetNetworkScore(result, rssiBoost, &sc);
            if (sc > startScore) {
                startScore = sc;
            }
        }
    }
    if (startScore == -10000) {
        startScore = WifiNetworkScoreCache::INVALID_NETWORK_SCORE;
    }
    if (VDBG) {
        // if (startScore == WifiNetworkScoreCache::INVALID_NETWORK_SCORE) {
        //     String str("    getConfigNetworkScore for ");
        //     str += ck;
        //     str += " -> no available score";
        //     LogDbg(str);
        // }
        // else {
        //     String str("    getConfigNetworkScore for ");
        //     str += ck; str += " boost=";
        //     str += rssiBoost; str += " score = ";
        //     str += startScore;
        //     LogDbg(str);
        // }
    }

    *result = startScore;
    return NOERROR;
}

ECode WifiAutoJoinController::HandleBSSIDBlackList(
    /* [in] */ Boolean enable,
    /* [in] */ const String& bssid,
    /* [in] */ Int32 reason)
{
    if (5 == reason) { // Enable Auto Join for all BSSIDs
        mBlacklistedBssids->Clear();
        return NOERROR;
    }
    Boolean b = FALSE;
    if (!enable) {
        mBlacklistedBssids->Contains(CoreUtils::Convert(bssid), &b);
        if (!b) {
            mBlacklistedBssids->Add(CoreUtils::Convert(bssid));
        }
    }
    else {
        mBlacklistedBssids->Contains(CoreUtils::Convert(bssid), &b);
        if (b) {
            mBlacklistedBssids->Remove(CoreUtils::Convert(bssid));
        }
    }
    return NOERROR;
}

ECode WifiAutoJoinController::IsBlacklistedBSSID(
    /* [in] */ const String& bssid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return mBlacklistedBssids->Contains(CoreUtils::Convert(bssid), result);
}

ECode WifiAutoJoinController::AttemptAutoJoin()
{
    didOverride = FALSE;
    didBailDueToWeakRssi = FALSE;
    Int32 networkSwitchType = AUTO_JOIN_IDLE;

    String lastSelectedConfiguration;
    mWifiConfigStore->GetLastSelectedConfiguration(&lastSelectedConfiguration);

    // Reset the currentConfiguration Key, and set it only if WifiStateMachine and
    // supplicant agree
    mCurrentConfigurationKey = NULL;
    AutoPtr<IWifiConfiguration> currentConfiguration;
    mWifiStateMachine->GetCurrentWifiConfiguration((IWifiConfiguration**)&currentConfiguration);

    AutoPtr<IWifiConfiguration> candidate;

    // Obtain the subset of recently seen networks
    AutoPtr<IList> list;
    mWifiConfigStore->GetRecentConfiguredNetworks(3000, FALSE, (IList**)&list);
    if (list == NULL) {
        if (VDBG) {
            Int32 size = 0;
            mWifiConfigStore->GetconfiguredNetworkSize(&size);
            String str("attemptAutoJoin nothing known=");
            str += StringUtils::ToString(size);
            LogDbg(str);
        }
        return NOERROR;
    }

    // Find the currently connected network: ask the supplicant directly
    String val = mWifiNative->Status(TRUE);
    AutoPtr<ArrayOf<String> > status;
    StringUtils::Split(val, String("\\r?\\n"), (ArrayOf<String>**)&status);
    if (VDBG) {
        String str("attemptAutoJoin() status=");
        str += val; str += " split=";
        str += StringUtils::ToString(status->GetLength());
        LogDbg(str);
    }

    Int32 supplicantNetId = -1;
    for (Int32 i = 0; i < status->GetLength(); i++) {
        String key = (*status)[i];
        if (key.RegionMatches(0, String("id="), 0, 3)) {
            Int32 idx = 3;
            supplicantNetId = 0;
            while (idx < key.GetLength()) {
                Char32 c = key.GetChar(idx);

                if ((c >= 0x30) && (c <= 0x39)) {
                    supplicantNetId *= 10;
                    supplicantNetId += c - 0x30;
                    idx++;
                }
                else {
                    break;
                }
            }
        }
        else if (key.Contains("wpa_state=ASSOCIATING")
                || key.Contains("wpa_state=ASSOCIATED")
                || key.Contains("wpa_state=FOUR_WAY_HANDSHAKE")
                || key.Contains("wpa_state=GROUP_KEY_HANDSHAKE")) {
            if (DBG) {
                String str("attemptAutoJoin: bail out due to sup state ");
                str += key;
                LogDbg(str);
            }
            // After WifiStateMachine ask the supplicant to associate or reconnect
            // we might still obtain scan results from supplicant
            // however the supplicant state in the mWifiInfo and supplicant state tracker
            // are updated when we get the supplicant state change message which can be
            // processed after the SCAN_RESULT message, so at this point the framework doesn't
            // know that supplicant is ASSOCIATING.
            // A good fix for this race condition would be for the WifiStateMachine to add
            // a new transient state where it expects to get the supplicant message indicating
            // that it started the association process and within which critical operations
            // like autojoin should be deleted.

            // This transient state would remove the need for the roam Wathchdog which
            // basically does that.

            // At the moment, we just query the supplicant state synchronously with the
            // mWifiNative.status() command, which allow us to know that
            // supplicant has started association process, even though we didnt yet get the
            // SUPPLICANT_STATE_CHANGE message.
            return NOERROR;
        }
    }

    if (DBG) {
        String conf("");
        String last("");
        if (currentConfiguration != NULL) {
            String ck;
            currentConfiguration->ConfigKey(&ck);
            conf = " current=";
            conf += ck;
        }
        if (!lastSelectedConfiguration.IsNull()) {
            last = " last=";
            last += lastSelectedConfiguration;
        }
        // String str("attemptAutoJoin() num recent config ");
        // Int32 s = 0;
        // list->GetSize(&s);
        // str += StringUtils::ToString(s);
        // str += conf; str += last;
        // str += " ---> suppNetId="; str += StringUtils::ToString(supplicantNetId);
        // LogDbg(str);
    }

    if (currentConfiguration != NULL) {
        Int32 networkId = 0;
        currentConfiguration->GetNetworkId(&networkId);
        if (supplicantNetId != networkId
                //https://b.corp.google.com/issue?id=16484607
                //mark this confition as an error only if the mismatched networkId are valid
                && supplicantNetId != IWifiConfiguration::INVALID_NETWORK_ID
                && networkId != IWifiConfiguration::INVALID_NETWORK_ID) {
            // String str("attemptAutoJoin() ERROR wpa_supplicant out of sync nid=");
            // str += supplicantNetId;
            // str += " WifiStateMachine=";
            // str += networkId;
            // LogDbg(str);
            mWifiStateMachine->DisconnectCommand();
            return NOERROR;
        }
        else {
            currentConfiguration->ConfigKey(&mCurrentConfigurationKey);
        }
    }
    else {
        if (supplicantNetId != IWifiConfiguration::INVALID_NETWORK_ID) {
            // Maybe in the process of associating, skip this attempt
            return NOERROR;
        }
    }

    Int32 currentNetId = -1;
    if (currentConfiguration != NULL) {
        // If we are associated to a configuration, it will
        // be compared thru the compareNetwork function
        currentConfiguration->GetNetworkId(&currentNetId);
    }

    /**
     * Run thru all visible configurations without looking at the one we
     * are currently associated to
     * select Best Network candidate from known WifiConfigurations
     */
    AutoPtr<IIterator> itl;
    list->GetIterator((IIterator**)&itl);
    Boolean bHasNxt = FALSE;
    while ((itl->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        itl->GetNext((IInterface**)&p);
        AutoPtr<IWifiConfiguration> config = IWifiConfiguration::Probe(p);
        String ssid;
        config->GetSSID(&ssid);
        if (ssid.IsNull()) {
            continue;
        }

        String bssid;
        config->GetBSSID(&bssid);
        Boolean b = FALSE;
        IsBlacklistedBSSID(bssid, &b);
        if (b) {
            String str("attemptAutoJoin skip candidate as AP is Blacklisted config.SSID = ");
            str += ssid;
            str += " config.BSSID=";
            str += bssid;
            LogDbg(str);
            continue;
        }

        Int32 autoJoinStatus = 0;
        config->GetAutoJoinStatus(&autoJoinStatus);
        String ckt;
        config->ConfigKey(TRUE, &ckt);
        if (autoJoinStatus >=
                IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE) {
            Int32 disableReason = 0;
            config->GetDisableReason(&disableReason);
            // Avoid networks disabled because of AUTH failure altogether
            if (DBG) {
                String str("attemptAutoJoin skip candidate due to auto join status ");
                str += StringUtils::ToString(autoJoinStatus);
                str += " key "; str += ckt;
                str += " reason "; str += StringUtils::ToString(disableReason);
                LogDbg(str);
            }
            continue;
        }

        // Try to un-blacklist based on elapsed time
        Int64 blackListTimestamp = 0;
        config->GetBlackListTimestamp(&blackListTimestamp);
        if (blackListTimestamp > 0) {
            AutoPtr<ISystem> sys;
            CSystem::AcquireSingleton((ISystem**)&sys);
            Int64 now = 0;
            sys->GetCurrentTimeMillis(&now);
            if (now < blackListTimestamp) {
                /**
                 * looks like there was a change in the system clock since we black listed, and
                 * timestamp is not meaningful anymore, hence lose it.
                 * this event should be rare enough so that we still want to lose the black list
                 */
                config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
            }
            else {
                if ((now - blackListTimestamp) > loseBlackListHardMilli) {
                    // Reenable it after 18 hours, i.e. next day
                    config->SetAutoJoinStatus(IWifiConfiguration::AUTO_JOIN_ENABLED);
                }
                else if ((now - blackListTimestamp) > loseBlackListSoftMilli) {
                    // Lose blacklisting due to bad link
                    config->SetAutoJoinStatus(autoJoinStatus - 8);
                }
            }
        }

        // Try to unblacklist based on good visibility
        AutoPtr<IWifiConfigurationVisibility> visibility;
        config->GetVisibility((IWifiConfigurationVisibility**)&visibility);
        Int32 rssi5 = 0, rssi24 = 0;
        visibility->GetRssi5(&rssi5);
        visibility->GetRssi24(&rssi24);
        Int32 num5 = 0, num24 = 0;
        visibility->GetNum5(&num5);
        visibility->GetNum24(&num24);
        if (rssi5 < mWifiConfigStore->thresholdUnblacklistThreshold5Soft
                && rssi24
                < mWifiConfigStore->thresholdUnblacklistThreshold24Soft) {
            if (DBG) {
                String str("attemptAutoJoin do not unblacklist due to low visibility ");
                str += StringUtils::ToString(autoJoinStatus);
                str += " key "; str += ckt;
                str += " rssi=("; str += StringUtils::ToString(rssi24);
                str += ","; str += StringUtils::ToString(rssi5);
                str += ") num=("; str += StringUtils::ToString(num24);
                str += ","; str += StringUtils::ToString(num5); str += ")";
                LogDbg(str);
            }
        }
        else if (rssi5 < mWifiConfigStore->thresholdUnblacklistThreshold5Hard
                && rssi24
                < mWifiConfigStore->thresholdUnblacklistThreshold24Hard) {
            // If the network is simply temporary disabled, don't allow reconnect until
            // RSSI becomes good enough
            config->SetAutoJoinStatus(autoJoinStatus - 1);
            if (DBG) {
                String str("attemptAutoJoin good candidate seen, bumped soft -> status=");
                str += StringUtils::ToString(autoJoinStatus);
                str += " "; str += ckt; str += " rssi=(";
                str += StringUtils::ToString(rssi24); str += ","; str += StringUtils::ToString(rssi5);
                str += ") num=("; str += StringUtils::ToString(num24);
                str += ","; str += StringUtils::ToString(num5); str += ")";
                LogDbg(str);
            }
        }
        else {
            config->SetAutoJoinStatus(autoJoinStatus - 3);
            if (DBG) {
                String str("attemptAutoJoin good candidate seen, bumped hard -> status=");
                str += StringUtils::ToString(autoJoinStatus);
                str += " "; str += ckt; str += " rssi=(";
                str += StringUtils::ToString(rssi24); str += ","; str += StringUtils::ToString(rssi5);
                str += ") num=("; str += StringUtils::ToString(num24);
                str += ","; str += StringUtils::ToString(num5); str += ")";
                LogDbg(str);
            }
        }

        if (autoJoinStatus >=
                IWifiConfiguration::AUTO_JOIN_TEMPORARY_DISABLED) {
            // Network is blacklisted, skip
            if (DBG) {
                String str("attemptAutoJoin skip blacklisted -> status=");
                str += StringUtils::ToString(autoJoinStatus);
                str += " "; str += ckt; str += " rssi=(";
                str += StringUtils::ToString(rssi24); str += ","; str += StringUtils::ToString(rssi5);
                str += ") num=("; str += StringUtils::ToString(num24);
                str += ","; str += StringUtils::ToString(num5); str += ")";
                LogDbg(str);
            }
            continue;
        }
        Int32 networkId = 0;
        config->GetNetworkId(&networkId);
        if (networkId == currentNetId) {
            if (DBG) {
                String str("attemptAutoJoin skip current candidate  ");
                str += StringUtils::ToString(currentNetId);
                str += " key "; str += ckt;
                LogDbg(str);
            }
            continue;
        }

        Boolean isLastSelected = FALSE;
        if (lastSelectedConfiguration != NULL &&
                ckt.Equals(lastSelectedConfiguration)) {
            isLastSelected = TRUE;
        }

        if (visibility == NULL) {
            continue;
        }
        Int32 boost = 0;
        config->GetAutoJoinUseAggressiveJoinAttemptThreshold(&boost);
        boost += weakRssiBailCount;
        if ((rssi5 + boost)
            < mWifiConfigStore->thresholdInitialAutoJoinAttemptMin5RSSI
            && (rssi24 + boost)
            < mWifiConfigStore->thresholdInitialAutoJoinAttemptMin24RSSI) {
            if (DBG) {
                String str("attemptAutoJoin skip due to low visibility -> status=");
                str += StringUtils::ToString(autoJoinStatus);
                str += " key "; str += ckt; str += " rssi=";
                str += StringUtils::ToString(rssi24); str += ", "; str += StringUtils::ToString(rssi5);
                str += " num="; str += StringUtils::ToString(num24);
                str += ", "; str += StringUtils::ToString(num5);
                LogDbg(str);
            }

            // Don't try to autojoin a network that is too far but
            // If that configuration is a user's choice however, try anyway
            if (!isLastSelected) {
                config->SetAutoJoinBailedDueToLowRssi(TRUE);
                didBailDueToWeakRssi = TRUE;
                continue;
            }
            else {
                // Next time, try to be a bit more aggressive in auto-joining
                Int32 autoJoinUseAggressiveJoinAttemptThreshold = 0;
                config->GetAutoJoinUseAggressiveJoinAttemptThreshold(&autoJoinUseAggressiveJoinAttemptThreshold);
                Boolean autoJoinBailedDueToLowRssi = FALSE;
                config->GetAutoJoinBailedDueToLowRssi(&autoJoinBailedDueToLowRssi);
                AutoPtr<IWifiConfigurationHelper> hlp;
                CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&hlp);
                Int32 boost = 0;
                hlp->GetMAX_INITIAL_AUTO_JOIN_RSSI_BOOST(&boost);
                if (autoJoinUseAggressiveJoinAttemptThreshold
                        < boost
                        && autoJoinBailedDueToLowRssi) {
                    autoJoinUseAggressiveJoinAttemptThreshold += 4;
                    config->SetAutoJoinUseAggressiveJoinAttemptThreshold(autoJoinUseAggressiveJoinAttemptThreshold);
                }
            }
        }
        Boolean noInternetAccess = FALSE;
        config->GetNoInternetAccess(&noInternetAccess);
        if (noInternetAccess && !isLastSelected) {
            // Avoid autojoining this network because last time we used it, it didn't
            // have internet access
            if (DBG) {
                String str("attemptAutoJoin skip candidate due to noInternetAccess flag ");
                str += ckt;
                LogDbg(str);
            }
            continue;
        }

        if (DBG) {
            String cur("");
            if (candidate != NULL) {
                String cck;
                candidate->ConfigKey(&cck);
                cur = " current candidate ";
                cur += cck;
            }
            String str("attemptAutoJoin trying id=");
            str += StringUtils::ToString(networkId); str += " ";
            str += ckt;
            str += " status="; str += StringUtils::ToString(autoJoinStatus);
            str += cur;
            LogDbg(str);
        }

        if (candidate == NULL) {
            candidate = config;
        }
        else {
            if (VDBG)  {
                String cck;
                candidate->ConfigKey(&cck);
                String str("attemptAutoJoin will compare candidate  ");
                str += cck; str += " with ";
                str += ckt;
                LogDbg(str);
            }
            Int32 order = 0;
            CompareWifiConfigurationsTop(candidate, config, &order);

            // The lastSelectedConfiguration is the configuration the user has manually selected
            // thru WifiPicker, or that a 3rd party app asked us to connect to via the
            // enableNetwork with disableOthers=true WifiManager API
            // As this is a direct user choice, we strongly prefer this configuration,
            // hence give +/-100
            String cck;
            candidate->ConfigKey(&cck);
            if ((lastSelectedConfiguration != NULL)
                    && cck.Equals(lastSelectedConfiguration)) {
                // candidate is the last selected configuration,
                // so keep it above connect choices (+/-60) and
                // above RSSI/scorer based selection of linked configuration (+/- 50)
                // by reducing order by -100
                order = order - 100;
                if (VDBG) {
                    String str("     ...and prefers -100 ");
                    str += cck;
                    str += " over "; str += ckt;
                    str += " because it is the last selected -> ";
                    str += StringUtils::ToString(order);
                    LogDbg(str);
                }
            }
            else if ((lastSelectedConfiguration != NULL)
                    && ckt.Equals(lastSelectedConfiguration)) {
                // config is the last selected configuration,
                // so keep it above connect choices (+/-60) and
                // above RSSI/scorer based selection of linked configuration (+/- 50)
                // by increasing order by +100
                order = order + 100;
                if (VDBG) {
                    String str("     ...and prefers +100 ");
                    str += ckt;
                    str += " over "; str += cck;
                    str += " because it is the last selected -> ";
                    str += StringUtils::ToString(order);
                    LogDbg(str);
                }
            }

            if (order > 0) {
                // Ascending : candidate < config
                candidate = config;
            }
        }
    }

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    // Wait for VPN to be available on the system to make use of this code
    // Now, go thru scan result to try finding a better untrusted network
    if (mNetworkScoreCache != NULL) {
        AutoPtr<IWifiConfigurationHelper> hlp;
        CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&hlp);
        Int32 rssi5 = 0;
        Int32 rssi24 = 0;
        hlp->GetINVALID_RSSI(&rssi5);
        hlp->GetINVALID_RSSI(&rssi24);
        AutoPtr<IWifiConfigurationVisibility> visibility;
        if (candidate != NULL) {
            AutoPtr<IWifiConfigurationVisibility> vis;
            candidate->GetVisibility((IWifiConfigurationVisibility**)&vis);
            vis->GetRssi5(&rssi5);
            vis->GetRssi24(&rssi24);
        }

        // Get current date
        Int64 nowMs = 0;
        sys->GetCurrentTimeMillis(&nowMs);
        Int32 currentScore = -10000;
        // The untrusted network with highest score
        AutoPtr<IScanResult> untrustedCandidate;
        // Look for untrusted scored network only if the current candidate is bad
        Boolean b = FALSE;
        IsBadCandidate(rssi24, rssi5, &b);
        if (b) {
            AutoPtr<ICollection> cl;
            mScanResultCache->GetValues((ICollection**)&cl);
            AutoPtr<IIterator> it;
            cl->GetIterator((IIterator**)&it);
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IScanResult> result = IScanResult::Probe(p);
                Int32 rssiBoost = 0;
                // We look only at untrusted networks with a valid SSID
                // A trusted result would have been looked at thru it's Wificonfiguration
                String ssid;
                result->GetSSID(&ssid);
                Boolean untrusted = FALSE;
                result->GetUntrusted(&untrusted);
                Int64 seen = 0;
                result->GetSeen(&seen);
                String bssid;
                result->GetBSSID(&bssid);
                if (TextUtils::IsEmpty(ssid) || !untrusted) {
                    continue;
                }
                if ((nowMs - seen) < 3000) {
                    // Increment usage count for the network
                    mWifiConnectionStatistics->IncrementOrAddUntrusted(ssid, 0, 1);

                    if (lastUntrustedBSSID != NULL
                            && bssid.Equals(lastUntrustedBSSID)) {
                        // Apply a large hysteresis to the untrusted network we are connected to
                        rssiBoost = 25;
                    }
                    Int32 score = 0;
                    mNetworkScoreCache->GetNetworkScore(result, rssiBoost, &score);
                    if (score != WifiNetworkScoreCache::INVALID_NETWORK_SCORE
                            && score > currentScore) {
                        // Highest score: Select this candidate
                        currentScore = score;
                        untrustedCandidate = result;
                        if (VDBG) {
                            String str("AutoJoinController: found untrusted candidate ");
                            str += ssid;
                            Int32 level = 0;
                            result->GetLevel(&level);
                            str += " RSSI="; str += StringUtils::ToString(level);
                            Int32 frequency = 0;
                            result->GetFrequency(&frequency);
                            str += " freq="; str += StringUtils::ToString(frequency);
                            str += " score="; str += StringUtils::ToString(score);
                            LogDbg(str);
                        }
                    }
                }
            }
        }
        if (untrustedCandidate != NULL) {
            String ssid;
            untrustedCandidate->GetSSID(&ssid);
            if (lastUntrustedBSSID == NULL
                    || !ssid.Equals(lastUntrustedBSSID)) {
                // We found a new candidate that we are going to connect to, then
                // increase its connection count
                mWifiConnectionStatistics->IncrementOrAddUntrusted(ssid, 1, 0);
                // Remember which SSID we are connecting to
                lastUntrustedBSSID = ssid;
            }
        }
        // Now we don't have VPN, and thus don't actually connect to the untrusted candidate
        untrustedCandidate = NULL;
    }

    Int64 nowMs = 0;
    sys->GetCurrentTimeMillis(&nowMs);
    Int64 lastUnwanted =
            nowMs - mWifiConfigStore->lastUnwantedNetworkDisconnectTimestamp;
    if (candidate == NULL
            && lastSelectedConfiguration == NULL
            && currentConfiguration == NULL
            && didBailDueToWeakRssi
            && (mWifiConfigStore->lastUnwantedNetworkDisconnectTimestamp == 0
                || lastUnwanted > (1000 * 60 * 60 * 24 * 7))
            ) {
        // We are bailing out of autojoin although we are seeing a weak configuration, and
        // - we didn't find another valid candidate
        // - we are not connected
        // - without a user network selection choice
        // - ConnectivityService has not triggered an unwanted network disconnect
        //       on this device for a week (hence most likely there is no SIM card or cellular)
        // If all those conditions are met, then boost the RSSI of the weak networks
        // that we are seeing so as we will eventually pick one
        if (weakRssiBailCount < 10) {
            weakRssiBailCount += 1;
        }
    }
    else {
        if (weakRssiBailCount > 0) {
            weakRssiBailCount -= 1;
        }
    }

    /**
     *  If candidate is found, check the state of the connection so as
     *  to decide if we should be acting on this candidate and switching over
     */
    Int32 networkDelta = CompareNetwork(candidate, lastSelectedConfiguration);
    if (DBG && candidate != NULL) {
        String doSwitch("");
        String current("");
        if (networkDelta < 0) {
            doSwitch = " -> not switching";
        }
        if (currentConfiguration != NULL) {
            String ck;
            currentConfiguration->ConfigKey(&ck);
            current = " with current ";
            current += ck;
        }
        // String str("attemptAutoJoin networkSwitching candidate ");
        // String cck;
        // candidate->ConfigKey(&cck);
        // str += cck;
        // str += current;
        // str += " linked=";
        // Boolean b = FALSE;
        // str += (currentConfiguration != NULL
        //                 && (currentConfiguration->IsLinked(candidate, &b), b));
        // str += " : delta=";
        // str += StringUtils::ToString(networkDelta);
        // str += " "; str += doSwitch;
        // LogDbg(str);
    }

    /**
     * Ask WifiStateMachine permission to switch :
     * if user is currently streaming voice traffic,
     * then we should not be allowed to switch regardless of the delta
     */
    Boolean b = FALSE;
    mWifiStateMachine->ShouldSwitchNetwork(networkDelta, &b);
    if (b) {
        if (mStaStaSupported) {
            LogDbg(String("mStaStaSupported --> error do nothing now "));
        }
        else {
            Boolean bLinked = FALSE;
            if (currentConfiguration != NULL && (currentConfiguration->IsLinked(candidate, &bLinked), bLinked)) {
                networkSwitchType = AUTO_JOIN_EXTENDED_ROAMING;
            }
            else {
                networkSwitchType = AUTO_JOIN_OUT_OF_NETWORK_ROAMING;
            }
            if (DBG) {
                String ck;
                candidate->ConfigKey(&ck);
                String str("AutoJoin auto connect with netId ");
                Int32 networkId = 0;
                candidate->GetNetworkId(&networkId);
                str += StringUtils::ToString(networkId);
                str += " to ";
                str += ck;
                LogDbg(str);
            }
            if (didOverride) {
                Int32 num = 0;
                candidate->GetNumScorerOverrideAndSwitchedNetwork(&num);
                candidate->SetNumScorerOverrideAndSwitchedNetwork(++num);
            }
            Int32 numAssociation = 0;
            candidate->GetNumAssociation(&numAssociation);
            candidate->SetNumAssociation(++numAssociation);
            Int32 numAutoJoinAttempt = 0;
            mWifiConnectionStatistics->GetNumAutoJoinAttempt(&numAutoJoinAttempt);
            mWifiConnectionStatistics->SetNumAutoJoinAttempt(++numAutoJoinAttempt);

            String cbssid;
            candidate->GetBSSID(&cbssid);
            if (cbssid.IsNull() || cbssid.Equals("any")) {
                // First step we selected the configuration we want to connect to
                // Second step: Look for the best Scan result for this configuration
                // TODO this algorithm should really be done in one step
                String currentBSSID;
                mWifiStateMachine->GetCurrentBSSID(&currentBSSID);
                AutoPtr<IScanResult> roamCandidate;
                AttemptRoam(NULL, candidate, 3000, String(NULL), (IScanResult**)&roamCandidate);
                if (roamCandidate != NULL && !currentBSSID.IsNull()) {
                    String rbssid;
                    roamCandidate->GetBSSID(&rbssid);
                    if ( currentBSSID.Equals(rbssid)) {
                        // Sanity, we were already asociated to that candidate
                        roamCandidate = NULL;
                    }
                }
                Boolean b5GHz = FALSE;
                if (roamCandidate != NULL && (roamCandidate->Is5GHz(&b5GHz), b5GHz)) {
                    // If the configuration hasn't a default BSSID selected, and the best
                    // candidate is 5GHZ, then select this candidate so as WifiStateMachine and
                    // supplicant will pick it first
                    String rbssid;
                    roamCandidate->GetBSSID(&rbssid);
                    candidate->SetAutoJoinBSSID(rbssid);
                    if (VDBG) {
                        Int32 level = 0, frequency = 0;
                        roamCandidate->GetLevel(&level);
                        roamCandidate->GetFrequency(&frequency);
                        String str("AutoJoinController: lock to 5GHz ");
                        str += rbssid;
                        str += " RSSI="; str += StringUtils::ToString(level);
                        str += " freq="; str += StringUtils::ToString(frequency);
                        LogDbg(str);
                    }
                }
                else {
                    // We couldnt find a roam candidate
                    candidate->SetAutoJoinBSSID(String("any"));
                }
            }
            Int32 networkId = 0;
            candidate->GetNetworkId(&networkId);
            mWifiStateMachine->SendMessage(WifiStateMachine::CMD_AUTO_CONNECT,
                         networkId, networkSwitchType, candidate);
        }
    }

    if (networkSwitchType == AUTO_JOIN_IDLE) {
        String currentBSSID;
        mWifiStateMachine->GetCurrentBSSID(&currentBSSID);
        // Attempt same WifiConfiguration roaming
        AutoPtr<IScanResult> roamCandidate;
        AttemptRoam(NULL, currentConfiguration, 3000,
                currentBSSID, (IScanResult**)&roamCandidate);
        /**
         *  TODO: (post L initial release)
         *  consider handling linked configurations roaming (i.e. extended Roaming)
         *  thru the attemptRoam function which makes use of the RSSI roaming threshold.
         *  At the moment, extended roaming is only handled thru the attemptAutoJoin()
         *  function which compare configurations.
         *
         *  The advantage of making use of attemptRoam function is that this function
         *  will looks at all the BSSID of each configurations, instead of only looking
         *  at WifiConfiguration.visibility which keeps trackonly of the RSSI/band of the
         *  two highest BSSIDs.
         */
        // Attempt linked WifiConfiguration roaming
        /* if (currentConfiguration != null
                && currentConfiguration.linkedConfigurations != null) {
            for (String key : currentConfiguration.linkedConfigurations.keySet()) {
                WifiConfiguration link = mWifiConfigStore.getWifiConfiguration(key);
                if (link != null) {
                    roamCandidate = attemptRoam(roamCandidate, link, 3000,
                            currentBSSID);
                }
            }
        }*/
        String rbssid;
        if (roamCandidate != NULL && currentBSSID != NULL
                && (roamCandidate->GetBSSID(&rbssid),currentBSSID.Equals(rbssid))) {
            roamCandidate = NULL;
        }
        b = FALSE;
        // mWifiStateMachine->ShouldSwitchNetwork(999, &b);
        if (roamCandidate != NULL && b) {
            Int32 networkId = 0;
            currentConfiguration->GetNetworkId(&networkId);
            if (DBG) {
                String str("AutoJoin auto roam with netId ");
                str += StringUtils::ToString(networkId);
                str += " ";
                String ck;
                currentConfiguration->ConfigKey(&ck);
                str += ck;
                str += " to BSSID="; str += rbssid;
                str += " freq=";
                Int32 frequency = 0;
                roamCandidate->GetFrequency(&frequency);
                str += StringUtils::ToString(frequency);
                str += " RSSI=";
                Int32 level = 0;
                roamCandidate->GetLevel(&level);
                str += StringUtils::ToString(level);
                LogDbg(str);
            }
            networkSwitchType = AUTO_JOIN_ROAMING;
            Int32 numAutoRoamAttempt = 0;
            mWifiConnectionStatistics->GetNumAutoRoamAttempt(&numAutoRoamAttempt);
            mWifiConnectionStatistics->SetNumAutoRoamAttempt(++numAutoRoamAttempt);

            mWifiStateMachine->SendMessage(WifiStateMachine::CMD_AUTO_ROAM,
                         networkId, 1, roamCandidate);
        }
    }
    if (VDBG) {
        String str("Done attemptAutoJoin status=");
        str += StringUtils::ToString(networkSwitchType);
        LogDbg(str);
    }
    return NOERROR;
}

void WifiAutoJoinController::AgeScanResultsOut(
    /* [in] */ Int32 delay)
{
    if (delay <= 0) {
        delay = mScanResultMaximumAge; // Something sane
    }
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 milli = 0;
    sys->GetCurrentTimeMillis(&milli);
    if (VDBG) {
        String str("ageScanResultsOut delay ");
        str += StringUtils::ToString(delay);
        str += " size ";
        Int32 size = 0;
        mScanResultCache->GetSize(&size);
        str += StringUtils::ToString(size);
        str += " now ";
        str += StringUtils::ToString(milli);
        LogDbg(str);
    }

    AutoPtr<ISet> st;
    mScanResultCache->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> iter;
    st->GetIterator((IIterator**)&iter);
    Boolean bHasNxt = FALSE;
    while ((iter->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        iter->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> val;
        entry->GetValue((IInterface**)&val);
        AutoPtr<IScanResult> result = IScanResult::Probe(val);

        Int64 seen = 0;
        result->GetSeen(&seen);
        if ((seen + delay) < milli) {
            iter->Remove();
        }
    }
}

Int32 WifiAutoJoinController::CompareNetwork(
    /* [in] */ IWifiConfiguration* candidate,
    /* [in] */ const String& lastSelectedConfiguration)
{
    if (candidate == NULL) {
        return -3;
    }

    AutoPtr<IWifiConfiguration> currentNetwork;
    mWifiStateMachine->GetCurrentWifiConfiguration((IWifiConfiguration**)&currentNetwork);
    if (currentNetwork == NULL) {
        // Return any absurdly high score, if we are not connected there is no current
        // network to...
       return 1000;
    }

    String ck, ck2;
    candidate->ConfigKey(TRUE, &ck);
    currentNetwork->ConfigKey(TRUE, &ck2);
    if (ck.Equals(ck2)) {
        return -2;
    }

    String ck3, ck4;
    candidate->ConfigKey(&ck3);
    currentNetwork->ConfigKey(&ck4);
    if (DBG) {
        String str("compareNetwork will compare ");
        str += ck3;
        str += " with current ";
        str += ck4;
        LogDbg(str);
    }
    Int32 order = 0;
    CompareWifiConfigurationsTop(currentNetwork, candidate, &order);

    // The lastSelectedConfiguration is the configuration the user has manually selected
    // thru WifiPicker, or that a 3rd party app asked us to connect to via the
    // enableNetwork with disableOthers=true WifiManager API
    // As this is a direct user choice, we strongly prefer this configuration,
    // hence give +/-100
    if ((lastSelectedConfiguration != NULL)
            && ck4.Equals(lastSelectedConfiguration)) {
        // currentNetwork is the last selected configuration,
        // so keep it above connect choices (+/-60) and
        // above RSSI/scorer based selection of linked configuration (+/- 50)
        // by reducing order by -100
        order = order - 100;
        if (VDBG) {
            String str("     ...and prefers -100 ");
            str += ck4;
            str += " over "; str += ck3;
            str += " because it is the last selected -> ";
            str += StringUtils::ToString(order);
            LogDbg(str);
        }
    }
    else if ((lastSelectedConfiguration != NULL)
            && ck3.Equals(lastSelectedConfiguration)) {
        // candidate is the last selected configuration,
        // so keep it above connect choices (+/-60) and
        // above RSSI/scorer based selection of linked configuration (+/- 50)
        // by increasing order by +100
        order = order + 100;
        if (VDBG) {
            String str("     ...and prefers +100 ");
            str += ck3; str += " over ";
            str += ck4;
            str += " because it is the last selected -> ";
            str += StringUtils::ToString(order);
            LogDbg(str);
        }
    }

    return order;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


