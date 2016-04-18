#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/server/wifi/WifiNative.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include "elastos/core/AutoLock.h"
#include <wifi.h>
#include <wifi_hal.h>
#include <stdlib.h>
#include <ctype.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::CScanResult;
using Elastos::Droid::Wifi::IScanResultInformationElement;
using Elastos::Droid::Wifi::CScanResultInformationElement;
using Elastos::Droid::Wifi::IWifiScannerBssidInfo;
using Elastos::Droid::Wifi::CRttManagerRttResult;
using Elastos::Droid::Wifi::CWifiLinkLayerStats;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::List;

#define WIFI_PKG_NAME "android/net/wifi/WifiNative"
#define BUF_SIZE 256
#define EVENT_BUF_SIZE 2048


namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

static int doCommand(const char *cmd, char *replybuf, int replybuflen)
{
    size_t reply_len = replybuflen - 1;

    if (::wifi_command(cmd, replybuf, &reply_len) != 0) {
        return -1;
    }
    else {
        // Strip off trailing newline
        if (reply_len > 0 && replybuf[reply_len-1] == '\n') {
            replybuf[reply_len-1] = '\0';
        }
        else {
            replybuf[reply_len] = '\0';
        }
        return 0;
    }
}

static int doIntCommand(const char* fmt, ...)
{
    char buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int byteCount = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (byteCount < 0 || byteCount >= BUF_SIZE) {
        return -1;
    }
    char reply[BUF_SIZE];
    if (doCommand(buf, reply, sizeof(reply)) != 0) {
        return -1;
    }
    return static_cast<int>(atoi(reply));
}

static Boolean doBooleanCommand(const char* expect, const char* fmt, ...)
{
    char buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int byteCount = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (byteCount < 0 || byteCount >= BUF_SIZE) {
        return FALSE;
    }
    char reply[BUF_SIZE];
    if (doCommand(buf, reply, sizeof(reply)) != 0) {
        return FALSE;
    }
    return (strcmp(reply, expect) == 0);
}

// Send a command to the supplicant, and return the reply as a String
static String doStringCommand(const char* fmt, ...)
{
    char buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int byteCount = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (byteCount < 0 || byteCount >= BUF_SIZE) {
        return String(NULL);
    }
    char reply[4096];
    if (doCommand(buf, reply, sizeof(reply)) != 0) {
        return String(NULL);
    }
    return String(reply);
}

Boolean WifiNative::DBG = FALSE;
Object WifiNative::mLock;
const Int32 WifiNative::DEFAULT_GROUP_OWNER_INTENT;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_ENABLED;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_DISABLED;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_SENSE;

const Int32 WifiNative::SCAN_WITHOUT_CONNECTION_SETUP;
const Int32 WifiNative::SCAN_WITH_CONNECTION_SETUP;

static AutoPtr<ILocalLog> InitLocalLog()
{
    return NULL;//TODO ILocalLog have not been implemented.
}
AutoPtr<ILocalLog> WifiNative::mLocalLog = InitLocalLog();
Int32 WifiNative::sCmdId;

String WifiNative::TAG("WifiNative-HAL");
Int64 WifiNative::sWifiHalHandle = 0;  /* used by JNI to save wifi_handle */
AutoPtr<ArrayOf<Int64> > WifiNative::sWifiIfaceHandles;  /* used by JNI to save interface handles */
Int32 WifiNative::sWlan0Index = -1;
Int32 WifiNative::sP2p0Index = -1;
Boolean WifiNative::sHalIsStarted = FALSE;
Object WifiNative::mClassLock;
Int32 WifiNative::WIFI_SCAN_BUFFER_FULL = 0;
Int32 WifiNative::WIFI_SCAN_COMPLETE = 1;
Int32 WifiNative::sScanCmdId = 0;
AutoPtr<IWifiNativeScanEventHandler> WifiNative::sScanEventHandler;
AutoPtr<WifiNative::ScanSettings> WifiNative::sScanSettings;
Int32 WifiNative::sHotlistCmdId = 0;
AutoPtr<IWifiNativeHotlistEventHandler> WifiNative::sHotlistEventHandler;
AutoPtr<IWifiNativeSignificantWifiChangeEventHandler> WifiNative::sSignificantWifiChangeHandler;
Int32 WifiNative::sSignificantWifiChangeCmdId;
AutoPtr<IWifiNativeRttEventHandler> WifiNative::sRttEventHandler;
Int32 WifiNative::sRttCmdId;

WifiNative::MonitorThread::MonitorThread(
    /* [in] */ WifiNative* owner)
    :mOwner(owner)
{
}

ECode WifiNative::MonitorThread::Run()
{
    Logger::I(TAG, "Waiting for HAL events mWifiHalHandle=%ld", sWifiHalHandle);
    WifiNative::WaitForHalEventNative();
    return NOERROR;
}

WifiNative::WifiNative(
    /* [in] */ const String& interfaceName)
    : mInterfaceName(interfaceName)
    , mSuspendOptEnabled(FALSE)
{
    mTAG = String("WifiNative-") + mInterfaceName;
    if (!interfaceName.Equals("p2p0")) {
        mInterfacePrefix = String("IFNAME=") + interfaceName + String(" ");
    } else {
        // commands for p2p0 interface don't need prefix
        mInterfacePrefix = "";
    }
}

void WifiNative::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        DBG = TRUE;
    } else {
        DBG = FALSE;
    }
}


Boolean WifiNative::LoadDriver()
{
    return (::wifi_load_driver() == 0);
}

Boolean WifiNative::IsDriverLoaded()
{
    return (::is_wifi_driver_loaded() == 1);
}

Boolean WifiNative::UnloadDriver()
{
    return (::wifi_unload_driver() == 0);
}

Boolean WifiNative::StartSupplicant(
    /* [in] */ Boolean p2pSupported)
{
    return (::wifi_start_supplicant(p2pSupported) == 0);
}

Boolean WifiNative::KillSupplicant(
    /* [in] */ Boolean p2pSupported)
{
    return (::wifi_stop_supplicant(p2pSupported) == 0);
}

Int32 WifiNative::GetNewCmdIdLocked()
{
    return sCmdId++;
}

void WifiNative::LocalLog(
    /* [in] */ const String& s)
{
    if (mLocalLog != NULL)
        mLocalLog->Log(mInterfaceName + String(": ") + s);
}

Boolean WifiNative::ConnectToSupplicantNative()
{
    return (::wifi_connect_to_supplicant() == 0);
}

void WifiNative::CloseSupplicantConnectionNative()
{
    ::wifi_close_supplicant_connection();
}

String WifiNative::WaitForEventNative()
{
    char buf[EVENT_BUF_SIZE];
    int nread = ::wifi_wait_for_event(buf, sizeof buf);
    if (nread > 0) {
        return String(buf);
    }
    else {
        return String(NULL);
    }
}

Boolean WifiNative::DoBooleanCommandNative(
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return FALSE;
    }

    return doBooleanCommand("OK", "%s", command.string());
}

Int32 WifiNative::DoIntCommandNative(
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return -1;
    }

    return doIntCommand("%s", command.string());
}

String WifiNative::DoStringCommandNative(
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return String(NULL);
    }

    return doStringCommand("%s", command.string());
}

AutoPtr<ILocalLog> WifiNative::GetLocalLog()
{
    return mLocalLog;
}

Boolean WifiNative::ConnectToSupplicant()
{
    // No synchronization necessary .. it is implemented in WifiMonitor
    LocalLog(mInterfacePrefix + "connectToSupplicant");
    return ConnectToSupplicantNative();
}

void WifiNative::CloseSupplicantConnection()
{
    LocalLog(mInterfacePrefix + "closeSupplicantConnection");
    CloseSupplicantConnectionNative();
}

String WifiNative::WaitForEvent()
{
    // No synchronization necessary .. it is implemented in WifiMonitor
    return WaitForEventNative();
}

Boolean WifiNative::DoBooleanCommand(
    /* [in] */ const String& command)
{
    if (DBG) Logger::D(mTAG, "doBoolean: [%s]", command.string());
    {
        AutoLock lock(mLock);
        Int32 cmdId = GetNewCmdIdLocked();
        String toLog = StringUtils::ToString(cmdId) + ":" + mInterfacePrefix + command;
        Boolean result = DoBooleanCommandNative(mInterfacePrefix + command);
        LocalLog(toLog + " -> " + StringUtils::BooleanToString(result));
        if (DBG) Logger::D(mTAG, command + ": returned " + StringUtils::BooleanToString(result));
        return result;
    }
}

Int32 WifiNative::DoIntCommand(
    /* [in] */ const String& command)
{
    if (DBG) Logger::D(mTAG, "doInt: [%s]", command.string());
    {
        AutoLock lock(mLock);
        Int32 cmdId = GetNewCmdIdLocked();
        String toLog = StringUtils::ToString(cmdId) + ":" + mInterfacePrefix + command;
        Int32 result = DoIntCommandNative(mInterfacePrefix + command);
        LocalLog(toLog + String(" -> ") + StringUtils::ToString(result));
        if (DBG) Logger::D(mTAG, String("   returned ") + StringUtils::ToString(result));
        return result;
    }
}

String WifiNative::DoStringCommand(
    /* [in] */ const String& command)
{
    if (DBG) {
        //GET_NETWORK commands flood the logs
        if (!command.StartWith("GET_NETWORK")) {
            Logger::D(mTAG, "doString: [%s]", command.string());
        }
    }
    {
        AutoLock lock(mLock);
        Int32 cmdId = GetNewCmdIdLocked();
        String toLog = StringUtils::ToString(cmdId) + ":" + mInterfacePrefix + command;
        String result = DoStringCommandNative(mInterfacePrefix + command);
        if (result.IsNull()) {
            if (DBG) Logger::D(mTAG, "doStringCommandNative no result");
        } else {
            if (!command.StartWith("STATUS-")) {
                LocalLog(toLog + " -> " + result);
            }
            if (DBG) Logger::D(mTAG, "   returned %s", result.string());//.replace("\n", " "));
        }
        return result;
    }
}

String WifiNative::DoStringCommandWithoutLogging(
    /* [in] */ const String& command)
{
    if (DBG) {
        //GET_NETWORK commands flood the logs
        if (!command.StartWith("GET_NETWORK")) {
            Logger::D(mTAG, "doString: [%s]", command.string());
        }
    }
    {
        AutoLock lock(mLock);
        return DoStringCommandNative(mInterfacePrefix + command);
    }
}

Boolean WifiNative::Ping()
{
    String pong = DoStringCommand(String("PING"));
    return (!pong.IsNull() && pong.Equals("PONG"));
}

void WifiNative::SetSupplicantLogLevel(
    /* [in] */ const String& level)
{
    DoStringCommand(String("LOG_LEVEL ") + level);
}

String WifiNative::GetFreqCapability()
{
    return DoStringCommand(String("GET_CAPABILITY freq"));
}

Boolean WifiNative::Scan(
    /* [in] */ Int32 type,
    /* [in] */ const String& freqList)
{
    if (type == SCAN_WITHOUT_CONNECTION_SETUP) {
        if (freqList.IsNull()) return DoBooleanCommand(String("SCAN TYPE=ONLY"));
        else
            return DoBooleanCommand(String("SCAN TYPE=ONLY freq=") + freqList);
    } else if (type == SCAN_WITH_CONNECTION_SETUP) {
        if (freqList.IsNull()) return DoBooleanCommand(String("SCAN"));
        else
            return DoBooleanCommand(String("SCAN freq=") + freqList);
    } else {
        //throw new IllegalArgumentException("Invalid scan type");
        Logger::E(mTAG, "WifiNative::Scan Invalid scan type");
    }
    return FALSE;
}

//Boolean WifiNative::SetScanMode(
//    /* [in] */ Boolean setActive)
//{
//    if (setActive) {
//        return DoBooleanCommand(String("DRIVER SCAN-ACTIVE"));
//    }
//    else {
//        return DoBooleanCommand(String("DRIVER SCAN-PASSIVE"));
//    }
//}

Boolean WifiNative::StopSupplicant()
{
    return DoBooleanCommand(String("TERMINATE"));
}

String WifiNative::ListNetworks()
{
    return DoStringCommand(String("LIST_NETWORKS"));
}

Int32 WifiNative::AddNetwork()
{
    return DoIntCommand(String("ADD_NETWORK"));
}

Boolean WifiNative::SetNetworkVariable(
    /* [in] */ Int32 netId,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (name.IsNullOrEmpty() || value.IsNullOrEmpty()) return FALSE;
    StringBuilder command;
    command.Append("SET_NETWORK ");
    command.Append(netId);
    command.Append(" ");
    command.Append(name);
    command.Append(" ");
    command.Append(value);
    return DoBooleanCommand(command.ToString());
}

String WifiNative::GetNetworkVariable(
    /* [in] */ Int32 netId,
    /* [in] */ const String& name)
{
    if (name.IsNullOrEmpty()) return String(NULL);
    StringBuilder command;
    command.Append("GET_NETWORK ");
    command.Append(netId);
    command.Append(" ");
    command.Append(name);
    return DoStringCommandWithoutLogging(command.ToString());
}

Boolean WifiNative::RemoveNetwork(
    /* [in] */ Int32 netId)
{
    return DoBooleanCommand(String("REMOVE_NETWORK ") + StringUtils::ToString(netId));
}

void WifiNative::LogDbg(
    /* [in] */ const String& debug)
{
    //long now = SystemClock.elapsedRealtimeNanos();
    //String ts = String.format("[%,d us] ", now/1000);
    //Log.e("WifiNative: ", ts+debug+ " stack:"
    //        + Thread.currentThread().getStackTrace()[2].getMethodName() +" - "
    //        + Thread.currentThread().getStackTrace()[3].getMethodName() +" - "
    //        + Thread.currentThread().getStackTrace()[4].getMethodName() +" - "
    //        + Thread.currentThread().getStackTrace()[5].getMethodName()+" - "
    //        + Thread.currentThread().getStackTrace()[6].getMethodName());
}

Boolean WifiNative::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers)
{
    if (DBG) LogDbg(String("enableNetwork nid=") + StringUtils::ToString(netId)
                            + " disableOthers=" + StringUtils::BooleanToString(disableOthers));
    if (disableOthers) {
        return DoBooleanCommand(String("SELECT_NETWORK ") + StringUtils::ToString(netId));
    }
    else {
        return DoBooleanCommand(String("ENABLE_NETWORK ") + StringUtils::ToString(netId));
    }
}

Boolean WifiNative::DisableNetwork(
    /* [in] */ Int32 netId)
{
    if (DBG) LogDbg(String("disableNetwork nid=") + StringUtils::ToString(netId));
    return DoBooleanCommand(String("DISABLE_NETWORK ") + StringUtils::ToString(netId));
}

Boolean WifiNative::Reconnect()
{
    if (DBG) LogDbg(String("RECONNECT "));
    return DoBooleanCommand(String("RECONNECT"));
}

Boolean WifiNative::Reassociate()
{
    if (DBG) LogDbg(String("REASSOCIATE "));
    return DoBooleanCommand(String("REASSOCIATE"));
}

Boolean WifiNative::Disconnect()
{
    if (DBG) LogDbg(String("DISCONNECT "));
    return DoBooleanCommand(String("DISCONNECT"));
}

String WifiNative::Status()
{
    return Status(FALSE);
}

String WifiNative::Status(
    /* [in] */ Boolean noEvents)
{
    if (noEvents) {
        return DoStringCommand(String("STATUS-NO_EVENTS"));
    } else {
        return DoStringCommand(String("STATUS"));
    }
}

String WifiNative::GetMacAddress()
{
    //Macaddr = XX.XX.XX.XX.XX.XX
    String ret = DoStringCommand(String("DRIVER MACADDR"));
    if (!ret.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > tokens;
        StringUtils::Split(ret, String(" = "), (ArrayOf<String>**)&tokens);
        if (tokens != NULL && tokens->GetLength() == 2) return (*tokens)[1];
    }
    return String(NULL);
}

String WifiNative::ScanResults(
    /* [in] */ Int32 sid)
{
    return DoStringCommandWithoutLogging(String("BSS RANGE=") + StringUtils::ToString(sid) + "- MASK=0x21987");
}

String WifiNative::ScanResult(
    /* [in] */ const String& bssid)
{
    return DoStringCommand(String("BSS ") + bssid);
}

String WifiNative::SetBatchedScanSettings(
    /* [in] */ IBatchedScanSettings* settings)
{
    if (settings == NULL) {
        return DoStringCommand(String("DRIVER WLS_BATCHING STOP"));
    }
    Int32 scanIntervalSec;
    settings->GetScanIntervalSec(&scanIntervalSec);
    Int32 scansPerBatch;
    settings->GetMaxScansPerBatch(&scansPerBatch);
    String cmd = String("DRIVER WLS_BATCHING SET SCANFREQ=") + StringUtils::ToString(scanIntervalSec);
    cmd += String(" MSCAN=") + StringUtils::ToString(scansPerBatch);
    Int32 maxApPerScan;
    settings->GetMaxApPerScan(&maxApPerScan);
    if (maxApPerScan != IBatchedScanSettings::UNSPECIFIED) {
        cmd += String(" BESTN=") + StringUtils::ToString(maxApPerScan);
    }
    AutoPtr<ICollection> channelSet;
    settings->GetChannelSet((ICollection**)&channelSet);
    Boolean isEmpty;
    if (channelSet != NULL && !(channelSet->IsEmpty(&isEmpty), isEmpty)) {
        cmd += " CHANNEL=<";
        AutoPtr<ArrayOf<IInterface*> > array;
        channelSet->ToArray((ArrayOf<IInterface*>**)&array);
        Int32 size = array->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj = (*array)[i];
            String channel;
            ICharSequence::Probe(obj)->ToString(&channel);
            cmd += String(i > 0 ? "," : "") + channel;
        }
        cmd += ">";
    }
    Int32 maxApForDistance;
    settings->GetMaxApForDistance(&maxApForDistance);
    if (maxApForDistance != IBatchedScanSettings::UNSPECIFIED) {
        cmd += " RTT=" + maxApForDistance;
    }
    return DoStringCommand(cmd);
}

String WifiNative::GetBatchedScanResults()
{
    return DoStringCommand(String("DRIVER WLS_BATCHING GET"));
}

Boolean WifiNative::StartDriver()
{
    return DoBooleanCommand(String("DRIVER START"));
}

Boolean WifiNative::StopDriver()
{
    return DoBooleanCommand(String("DRIVER STOP"));
}

Boolean WifiNative::StartFilteringMulticastV4Packets()
{
    return DoBooleanCommand(String("DRIVER RXFILTER-STOP"))
        && DoBooleanCommand(String("DRIVER RXFILTER-REMOVE 2"))
        && DoBooleanCommand(String("DRIVER RXFILTER-START"));
}

Boolean WifiNative::StopFilteringMulticastV4Packets()
{
    return DoBooleanCommand(String("DRIVER RXFILTER-STOP"))
        && DoBooleanCommand(String("DRIVER RXFILTER-ADD 2"))
        && DoBooleanCommand(String("DRIVER RXFILTER-START"));
}

Boolean WifiNative::StartFilteringMulticastV6Packets()
{
    return DoBooleanCommand(String("DRIVER RXFILTER-STOP"))
        && DoBooleanCommand(String("DRIVER RXFILTER-REMOVE 3"))
        && DoBooleanCommand(String("DRIVER RXFILTER-START"));
}

Boolean WifiNative::StopFilteringMulticastV6Packets()
{
    return DoBooleanCommand(String("DRIVER RXFILTER-STOP"))
        && DoBooleanCommand(String("DRIVER RXFILTER-ADD 3"))
        && DoBooleanCommand(String("DRIVER RXFILTER-START"));
}

Int32 WifiNative::GetBand()
{
    String ret = DoStringCommand(String("DRIVER GETBAND"));
    if (!ret.IsNullOrEmpty()) {
        //reply is "BAND X" where X is the band
        AutoPtr< ArrayOf<String> > tokens;
        StringUtils::Split(ret, String(" "), (ArrayOf<String>**)&tokens);
        //try {
        if (tokens != NULL && tokens->GetLength() == 2)
            return StringUtils::ParseInt32((*tokens)[1]);
        //} catch (NumberFormatException e) {
        //    return -1;
        //}
    }
    return -1;
}

Boolean WifiNative::SetBand(
    /* [in] */ Int32 band)
{
    return DoBooleanCommand(String("DRIVER SETBAND ") + StringUtils::ToString(band));
}

Boolean WifiNative::SetBluetoothCoexistenceMode(
    /* [in] */ Int32 mode)
{
    return DoBooleanCommand(String("DRIVER BTCOEXMODE ") + StringUtils::ToString(mode));
}

Boolean WifiNative::SetBluetoothCoexistenceScanMode(
    /* [in] */ Boolean setCoexScanMode)
{
    if (setCoexScanMode) {
        return DoBooleanCommand(String("DRIVER BTCOEXSCAN-START"));
    }
    else {
        return DoBooleanCommand(String("DRIVER BTCOEXSCAN-STOP"));
    }
}

void WifiNative::EnableSaveConfig()
{
    DoBooleanCommand(String("SET update_config 1"));
}

Boolean WifiNative::SaveConfig()
{
    return DoBooleanCommand(String("SAVE_CONFIG"));
}

Boolean WifiNative::AddToBlacklist(
    /* [in] */ const String& bssid)
{
    if (bssid.IsNullOrEmpty()) return FALSE;
    return DoBooleanCommand(String("BLACKLIST ") + bssid);
}

Boolean WifiNative::ClearBlacklist()
{
    return DoBooleanCommand(String("BLACKLIST clear"));
}

Boolean WifiNative::SetSuspendOptimizations(
    /* [in] */ Boolean enabled)
{
    //if (mSuspendOptEnabled == enabled) return TRUE;
    mSuspendOptEnabled = enabled;
    Logger::E("native", "do suspend %d", enabled);
    if (enabled) {
        return DoBooleanCommand(String("DRIVER SETSUSPENDMODE 1"));
    }
    else {
        return DoBooleanCommand(String("DRIVER SETSUSPENDMODE 0"));
    }
}

Boolean WifiNative::SetCountryCode(
    /* [in] */ const String& countryCode)
{
    return DoBooleanCommand(String("DRIVER COUNTRY ") + countryCode/*.toUpperCase(Locale.ROOT)*/);
}

Boolean WifiNative::EnableBackgroundScan(
    /* [in] */ Boolean enable)
{
    if (enable) {
        Logger::E(mTAG, "doBoolean: enable");
        return DoBooleanCommand(String("SET pno 1"));
    }
    else {
        Logger::E(mTAG, "doBoolean: disable");
        return DoBooleanCommand(String("SET pno 0"));
    }
}

void WifiNative::EnableAutoConnect(
    /* [in] */ Boolean enable) {
    if (enable) {
        DoBooleanCommand(String("STA_AUTOCONNECT 1"));
    } else {
        DoBooleanCommand(String("STA_AUTOCONNECT 0"));
    }
}

void WifiNative::SetScanInterval(
    /* [in] */ Int32 scanInterval)
{
    DoBooleanCommand(String("SCAN_INTERVAL ") + StringUtils::ToString(scanInterval));
}

void WifiNative::StartTdls(
    /* [in] */ const String& macAddr,
    /* [in] */ Boolean enable)
{
    if (enable) {
        DoBooleanCommand(String("TDLS_DISCOVER ") + macAddr);
        DoBooleanCommand(String("TDLS_SETUP ") + macAddr);
    } else {
        DoBooleanCommand(String("TDLS_TEARDOWN ") + macAddr);
    }
}

String WifiNative::SignalPoll()
{
    return DoStringCommandWithoutLogging(String("SIGNAL_POLL"));
}

String WifiNative::PktcntPoll()
{
    return DoStringCommand(String("PKTCNT_POLL"));
}

void WifiNative::BssFlush() {
    DoBooleanCommand(String("BSS_FLUSH 0"));
}

Boolean WifiNative::StartWpsPbc(
    /* [in] */ const String& bssid)
{
    if (bssid.IsNullOrEmpty()) {
        return DoBooleanCommand(String("WPS_PBC"));
    }
    else {
        return DoBooleanCommand(String("WPS_PBC ") + bssid);
    }
}

Boolean WifiNative::StartWpsPbc(
    /* [in] */ const String& iface,
    /* [in] */ const String& bssid)
{
    {
        AutoLock lock(mLock);
        if (bssid.IsNullOrEmpty()) {
            return DoBooleanCommandNative(String("IFNAME=") + iface + " WPS_PBC");
        }
        else {
            StringBuilder command;
            command.Append("IFNAME=");
            command.Append(iface);
            command.Append(" WPS_PBC");
            command.Append(bssid);
            return DoBooleanCommandNative(command.ToString());
        }
    }
}

Boolean WifiNative::StartWpsPinKeypad(
    /* [in] */ const String& pin)
{
    if (pin.IsNullOrEmpty()) return FALSE;
    return DoBooleanCommand(String("WPS_PIN any ") + pin);
}

Boolean WifiNative::StartWpsPinKeypad(
    /* [in] */ const String& iface,
    /* [in] */ const String& pin)
{
    if (pin.IsNullOrEmpty()) return FALSE;
    {
        AutoLock lock(mLock);
        StringBuilder command;
        command.Append("IFNAME=");
        command.Append(iface);
        command.Append(" WPS_PIN any ");
        command.Append(pin);
        return DoBooleanCommand(command.ToString());
    }
}

String WifiNative::StartWpsPinDisplay(
    /* [in] */ const String& bssid)
{
    if (bssid.IsNullOrEmpty()) {
        return DoStringCommand(String("WPS_PIN any"));
    }
    else {
        return DoStringCommand(String("WPS_PIN ") + bssid);
    }
}

String WifiNative::StartWpsPinDisplay(
    /* [in] */ const String& iface,
    /* [in] */ const String& bssid)
{
    {
        AutoLock lock(mLock);
        if (bssid.IsNullOrEmpty()) {
            StringBuilder command;
            command.Append("IFNAME=");
            command.Append(iface);
            command.Append(" WPS_PIN any");
            return DoStringCommandNative(command.ToString());
        }
        else {
            StringBuilder command;
            command.Append("IFNAME=");
            command.Append(iface);
            command.Append(" WPS_PIN ");
            command.Append(bssid);
            return DoStringCommandNative(command.ToString());
        }
    }
}

Boolean WifiNative::SetExternalSim(
    /* [in] */ Boolean external)
{
    {
        AutoLock lock(mLock);
        String value = external ? String("1") : String("0");
        Logger::D(TAG, "Setting external_sim to %s", value.string());
        return DoBooleanCommand(String("SET external_sim ") + value);
    }
}

Boolean WifiNative::SimAuthResponse(
    /* [in] */ Int32 id,
    /* [in] */ const String& response)
{
    {
        AutoLock lock(mLock);
        return DoBooleanCommand(String("CTRL-RSP-SIM-") + StringUtils::ToString(id) + ":GSM-AUTH" + response);
    }
}

Boolean WifiNative::StartWpsRegistrar(
    /* [in] */ const String& bssid,
    /* [in] */ const String& pin)
{
    if (bssid.IsNullOrEmpty() || pin.IsNullOrEmpty()) return FALSE;
    StringBuilder command;
    command.Append("WPS_REG ");
    command.Append(bssid);
    command.Append(" ");
    command.Append(pin);
    return DoBooleanCommand(command.ToString());
}

Boolean WifiNative::CancelWps()
{
    return DoBooleanCommand(String("WPS_CANCEL"));
}

Boolean WifiNative::SetPersistentReconnect(
    /* [in] */ Boolean enabled)
{
    Int32 value = (enabled == TRUE) ? 1 : 0;
    return DoBooleanCommand(String("SET persistent_reconnect ") + StringUtils::ToString(value));
}

//Boolean WifiNative::SetGroupOwnerPsk(
//    /* [in] */ const String& name)
//{
//    return DoBooleanCommand(String("SET p2p_go_swpsk ") + name);
//}

Boolean WifiNative::SetDeviceName(
    /* [in] */ const String& name)
{
    return DoBooleanCommand(String("SET device_name ") + name);
}

Boolean WifiNative::SetDeviceType(
    /* [in] */ const String& type)
{
    return DoBooleanCommand(String("SET device_type ") + type);
}

Boolean WifiNative::SetConfigMethods(
    /* [in] */ const String& cfg)
{
    return DoBooleanCommand(String("SET config_methods ") + cfg);
}

Boolean WifiNative::SetManufacturer(
    /* [in] */ const String& value)
{
    return DoBooleanCommand(String("SET manufacturer ") + value);
}

Boolean WifiNative::SetModelName(
    /* [in] */ const String& value)
{
    return DoBooleanCommand(String("SET model_name ") + value);
}

Boolean WifiNative::SetModelNumber(
    /* [in] */ const String& value)
{
    return DoBooleanCommand(String("SET model_number ") + value);
}

Boolean WifiNative::SetSerialNumber(
    /* [in] */ const String& value)
{
    return DoBooleanCommand(String("SET serial_number ") + value);
}

Boolean WifiNative::SetP2pSsidPostfix(
    /* [in] */ const String& postfix)
{
    return DoBooleanCommand(String("SET p2p_ssid_postfix ") + postfix);
}

Boolean WifiNative::SetP2pGroupIdle(
    /* [in] */ const String& iface,
    /* [in] */ Int32 time)
{
    {
        AutoLock lock(mLock);
        StringBuilder command;
        command.Append("IFNAME=");
        command.Append(iface);
        command.Append(" SET p2p_group_idle ");
        command.Append(time);
        return DoBooleanCommandNative(command.ToString());
    }
}

void WifiNative::SetPowerSave(
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        DoBooleanCommand(String("SET ps 1"));
    }
    else {
        DoBooleanCommand(String("SET ps 0"));
    }
}

Boolean WifiNative::SetP2pPowerSave(
    /* [in] */ const String& iface,
    /* [in] */ Boolean enabled)
{
    {
        AutoLock lock(mLock);
        if (enabled) {
            StringBuilder command;
            command.Append("P2P_SET interface=");
            command.Append(iface);
            command.Append(" ps 1");
            return DoBooleanCommandNative(String("IFNAME=") + iface + String(" P2P_SET ps 1"));
        }
        else {
            StringBuilder command;
            command.Append("P2P_SET interface=");
            command.Append(iface);
            command.Append(" ps 0");
            return DoBooleanCommandNative(String("IFNAME=") + iface + String(" P2P_SET ps 0"));
        }
    }
}

Boolean WifiNative::SetWfdEnable(
    /* [in] */ Boolean enable)
{
    return DoBooleanCommand(String("SET wifi_display ") + (enable ? String("1") : String("0")));
}

Boolean WifiNative::SetWfdDeviceInfo(
    /* [in] */ const String& hex)
{
    return DoBooleanCommand(String("WFD_SUBELEM_SET 0 ") + hex);
}

Boolean WifiNative::SetConcurrencyPriority(
    /* [in] */ const String& s)
{
    return DoBooleanCommand(String("P2P_SET conc_pref ") + s);
}

Boolean WifiNative::P2pFind()
{
    return DoBooleanCommand(String("P2P_FIND"));
}

Boolean WifiNative::P2pFind(
    /* [in] */ Int32 timeout)
{
    if (timeout <= 0) {
        return P2pFind();
    }
    return DoBooleanCommand(String("P2P_FIND ") + StringUtils::ToString(timeout));
}

Boolean WifiNative::P2pStopFind()
{
   return DoBooleanCommand(String("P2P_STOP_FIND"));
}

Boolean WifiNative::P2pListen()
{
    return DoBooleanCommand(String("P2P_LISTEN"));
}

Boolean WifiNative::P2pListen(
    /* [in] */ Int32 timeout)
{
    if (timeout <= 0) {
        return P2pListen();
    }
    return DoBooleanCommand(String("P2P_LISTEN ") + StringUtils::ToString(timeout));
}

Boolean WifiNative::P2pExtListen(
    /* [in] */ Boolean enable,
    /* [in] */ Int32 period,
    /* [in] */ Int32 interval)
{
    if (enable && interval < period) {
        return FALSE;
    }
    return DoBooleanCommand(String("P2P_EXT_LISTEN")
            + (enable
                ? (String(" ") + StringUtils::ToString(period) + String(" ") + StringUtils::ToString(interval))
                : String("")));
}

Boolean WifiNative::P2pSetChannel(
    /* [in] */ Int32 lc,
    /* [in] */ Int32 oc)
{
    if (DBG) Logger::D(mTAG, "p2pSetChannel: lc=%d, oc=%d", lc, oc);

    if (lc >=1 && lc <= 11) {
        if (!DoBooleanCommand(String("P2P_SET listen_channel ") + StringUtils::ToString(lc))) {
            return FALSE;
        }
    } else if (lc != 0) {
        return FALSE;
    }

    if (oc >= 1 && oc <= 165 ) {
        Int32 freq = (oc <= 14 ? 2407 : 5000) + oc * 5;
        return DoBooleanCommand(String("P2P_SET disallow_freq 1000-")
                + StringUtils::ToString(freq - 5) + "," + StringUtils::ToString(freq + 5) + "-6000");
    } else if (oc == 0) {
        /* oc==0 disables "P2P_SET disallow_freq" (enables all freqs) */
        return DoBooleanCommand(String("P2P_SET disallow_freq \"\""));
    }

    return FALSE;
}

Boolean WifiNative::P2pFlush()
{
    return DoBooleanCommand(String("P2P_FLUSH"));
}

String WifiNative::P2pConnect(
    /* [in] */ IWifiP2pConfig* config,
    /* [in] */ Boolean joinExistingGroup)
{
    if (config == NULL) return String(NULL);

    List<String> args;
    AutoPtr<IWpsInfo> wps;
    config->GetWps((IWpsInfo**)&wps);

    String addr;
    config->GetDeviceAddress(&addr);
    args.PushBack(addr);

    Int32 setup;
    wps->GetSetup(&setup);
    String pin;
    wps->GetPin(&pin);
    switch (setup) {
        case IWpsInfo::PBC:
            args.PushBack(String("pbc"));
            break;
        case IWpsInfo::DISPLAY:
            if (pin.IsNullOrEmpty()) {
                args.PushBack(String("pin"));
            }
            else {
                args.PushBack(pin);
            }
            args.PushBack(String("display"));
            break;
        case IWpsInfo::KEYPAD:
            args.PushBack(pin);
            args.PushBack(String("keypad"));
            break;
        case IWpsInfo::LABEL:
            args.PushBack(pin);
            args.PushBack(String("label"));
        default:
            break;
    }

    Int32 netId;
    config->GetNetId(&netId);
    if (netId == IWifiP2pGroup::PERSISTENT_NET_ID) {
        args.PushBack(String("persistent"));
    }

    if (joinExistingGroup) {
        args.PushBack(String("join"));
    }
    else {
        //TODO: This can be adapted based on device plugged in state and
        //device battery state
        Int32 groupOwnerIntent;
        config->GetGroupOwnerIntent(&groupOwnerIntent);
        if (groupOwnerIntent < 0 || groupOwnerIntent > 15) {
            groupOwnerIntent = DEFAULT_GROUP_OWNER_INTENT;
        }

        StringBuilder sb("go_intent=");
        sb.Append(groupOwnerIntent);
        args.PushBack(sb.ToString());
    }

    StringBuilder command("P2P_CONNECT ");
    List<String>::Iterator it = args.Begin();
    for (; it != args.End(); ++it) {
        command += *it;
        command += " ";
    }

    return DoStringCommand(command.ToString());
}

Boolean WifiNative::P2pCancelConnect()
{
    return DoBooleanCommand(String("P2P_CANCEL"));
}

Boolean WifiNative::P2pProvisionDiscovery(
    /* [in] */ IWifiP2pConfig* config)
{
    if (config == NULL) return FALSE;

    AutoPtr<IWpsInfo> wps;
    config->GetWps((IWpsInfo**)&wps);
    Int32 setup;
    wps->GetSetup(&setup);
    String addr;
    config->GetDeviceAddress(&addr);

    switch (setup) {
        case IWpsInfo::PBC:
            return DoBooleanCommand(String("P2P_PROV_DISC ") + addr + String(" pbc"));
        case IWpsInfo::DISPLAY:
            //We are doing display, so provision discovery is keypad
            return DoBooleanCommand(String("P2P_PROV_DISC ") + addr + String(" keypad"));
        case IWpsInfo::KEYPAD:
            //We are doing keypad, so provision discovery is display
            return DoBooleanCommand(String("P2P_PROV_DISC ") + addr + String(" display"));
        default:
            break;
    }

    return FALSE;
}

Boolean WifiNative::P2pGroupAdd(
    /* [in] */ Boolean persistent)
{
    if (persistent) {
        return DoBooleanCommand(String("P2P_GROUP_ADD persistent"));
    }
    return DoBooleanCommand(String("P2P_GROUP_ADD"));
}

Boolean WifiNative::P2pGroupAdd(
    /* [in] */ Int32 netId)
{
    return DoBooleanCommand(String("P2P_GROUP_ADD persistent=") + StringUtils::ToString(netId));
}

Boolean WifiNative::P2pGroupRemove(
    /* [in] */ const String& iface)
{
    if (iface.IsNullOrEmpty()) return FALSE;
    {
        AutoLock lock(mLock);
        return DoBooleanCommandNative(String("IFNAME=") + iface + " P2P_GROUP_REMOVE " + iface);
    }
}

Boolean WifiNative::P2pReject(
    /* [in] */ const String& deviceAddress)
{
    return DoBooleanCommand(String("P2P_REJECT ") + deviceAddress);
}

Boolean WifiNative::P2pInvite(
    /* [in] */ IWifiP2pGroup* group,
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress.IsNullOrEmpty()) return FALSE;

    if (group == NULL) {
        return DoBooleanCommand(String("P2P_INVITE peer=") + deviceAddress);
    }
    else {
        String str;
        group->GetInterface(&str);
        AutoPtr<IWifiP2pDevice> owner;
        group->GetOwner((IWifiP2pDevice**)&owner);
        String addr;
        owner->GetDeviceAddress(&addr);

        StringBuilder sb("P2P_INVITE group=");
        sb += str;
        sb += " peer=";
        sb += deviceAddress;
        sb += " go_dev_addr=";
        sb += addr;

        return DoBooleanCommand(sb.ToString());
    }
}

/* Reinvoke a persistent connection */
Boolean WifiNative::P2pReinvoke(
    /* [in] */ Int32 netId,
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress.IsNullOrEmpty() || netId < 0) return FALSE;

    StringBuilder command;
    command.Append("P2P_INVITE persistent=");
    command.Append(netId);
    command.Append(" peer=");
    command.Append(deviceAddress);
    return DoBooleanCommand(command.ToString());
}

String WifiNative::P2pGetSsid(
    /* [in] */ const String& deviceAddress)
{
    return P2pGetParam(deviceAddress, String("oper_ssid"));
}

String WifiNative::P2pGetDeviceAddress()
{
    Logger::D(TAG, "p2pGetDeviceAddress");

    String status;
    /* Explicitly calling the API without IFNAME= prefix to take care of the devices that
       don't have p2p0 interface. Supplicant seems to be returning the correct address anyway. */

    {
        AutoLock lock(mLock);
        status = DoStringCommandNative(String("STATUS"));
    }

    String result("");
    if (!status.IsNull()) {
        AutoPtr< ArrayOf<String> > tokens;
        StringUtils::Split(status, String("\n"), (ArrayOf<String>**)&tokens);
        if (tokens == NULL) return String("");
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            String token = (*tokens)[i];
            if (token.StartWith("p2p_device_address=")) {
                AutoPtr< ArrayOf<String> > nameValue;
                StringUtils::Split(token, String("="), (ArrayOf<String>**)&nameValue);
                if (nameValue->GetLength() != 2)
                    break;
                result = (*nameValue)[1];
            }
        }
    }

    Logger::D(TAG, "p2pGetDeviceAddress returning %s", result.string());
    return result;
}

Int32 WifiNative::GetGroupCapability(
    /* [in] */ const String& deviceAddress)
{
    Int32 gc = 0;
    if (deviceAddress.IsNullOrEmpty()) return gc;
    String peerInfo = P2pPeer(deviceAddress);
    if (peerInfo.IsNullOrEmpty()) return gc;

    AutoPtr<ArrayOf<String> > tokens;
    StringUtils::Split(peerInfo, "\n", (ArrayOf<String>**)&tokens);

    if (tokens != NULL) {
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            String token = (*tokens)[i];
            if (token.StartWith("group_capab=")) {
                AutoPtr<ArrayOf<String> > nameValue;
                StringUtils::Split(token, "=", (ArrayOf<String>**)&nameValue);
                if (nameValue == NULL || nameValue->GetLength() != 2) break;

                gc = StringUtils::ParseInt32((*nameValue)[1], 10);

                return gc;
            }
        }
    }

    return gc;
}

String WifiNative::P2pPeer(
    /* [in] */ const String& deviceAddress)
{
    return DoStringCommand(String("P2P_PEER ") + deviceAddress);
}

String WifiNative::P2pGetParam(
    /* [in] */ const String& deviceAddress,
    /* [in] */ const String& inKey)
{
    if (deviceAddress.IsNull()) return String(NULL);

    String peerInfo = P2pPeer(deviceAddress);
    if (peerInfo.IsNull()) return String(NULL);

    String key(inKey);
    key += "=";

    AutoPtr<ArrayOf<String> > tokens;
    StringUtils::Split(peerInfo, "\n", (ArrayOf<String>**)&tokens);

    if (tokens != NULL) {
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            String token = (*tokens)[i];
            AutoPtr<ArrayOf<String> > nameValue;
            StringUtils::Split(token, "=", (ArrayOf<String>**)&nameValue);
            if (nameValue == NULL || nameValue->GetLength() != 2) break;
            return (*nameValue)[1];
        }
    }

    return String(NULL);
}

Boolean WifiNative::P2pServiceAdd(
    /* [in] */ IWifiP2pServiceInfo* servInfo)
{
    /*
     * P2P_SERVICE_ADD bonjour <query hexdump> <RDATA hexdump>
     * P2P_SERVICE_ADD upnp <version hex> <service>
     *
     * e.g)
     * [Bonjour]
     * # IP Printing over TCP (PTR) (RDATA=MyPrinter._ipp._tcp.local.)
     * P2P_SERVICE_ADD bonjour 045f697070c00c000c01 094d795072696e746572c027
     * # IP Printing over TCP (TXT) (RDATA=txtvers=1,pdl=application/postscript)
     * P2P_SERVICE_ADD bonjour 096d797072696e746572045f697070c00c001001
     *  09747874766572733d311a70646c3d6170706c69636174696f6e2f706f7374736372797074
     *
     * [UPnP]
     * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012
     * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012::upnp:rootdevice
     * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012::urn:schemas-upnp
     * -org:device:InternetGatewayDevice:1
     * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9322-123456789012::urn:schemas-upnp
     * -org:service:ContentDirectory:2
     */

    AutoPtr<ArrayOf<String> > list;
    servInfo->GetSupplicantQueryList((ArrayOf<String>**)&list);

    if (list) {
        String s;
        for (Int32 i = 0; i < list->GetLength(); ++i) {
            s = (*list)[i];
            String command("P2P_SERVICE_ADD ");
            command += s;
            if (!DoBooleanCommand(command)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

Boolean WifiNative::P2pServiceDel(
    /* [in] */ IWifiP2pServiceInfo* servInfo)
{
    /*
     * P2P_SERVICE_DEL bonjour <query hexdump>
     * P2P_SERVICE_DEL upnp <version hex> <service>
     */
    assert(0);

    AutoPtr<ArrayOf<String> > list;
    servInfo->GetSupplicantQueryList((ArrayOf<String>**)&list);

    if (list) {
        String s;
        for (Int32 i = 0; i < list->GetLength(); ++i) {
            s = (*list)[i];
            AutoPtr<ArrayOf<String> > data;
            StringUtils::Split(s, " ", (ArrayOf<String>**)&data);
            if (data == NULL || data->GetLength() < 2) {
                return FALSE;
            }

            String command("P2P_SERVICE_DEL ");
            if ((*data)[0].Equals("upnp")) {
                command += s;
            }
            else if ((*data)[0].Equals("bonjour")) {
                command += (*data)[0];
                command += " ";
                command += (*data)[1];
            }
            else {
                return FALSE;
            }

            if (!DoBooleanCommand(command)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

Boolean WifiNative::P2pServiceFlush()
{
    return DoBooleanCommand(String("P2P_SERVICE_FLUSH"));
}

String WifiNative::P2pServDiscReq(
    /* [in] */ const String& addr,
    /* [in] */ const String& query)
{
    String command("P2P_SERV_DISC_REQ");
    command += (String(" ") + addr);
    command += (String(" ") + query);

    return DoStringCommand(command);
}

Boolean WifiNative::P2pServDiscCancelReq(
    /* [in] */ const String& id)
{
    return DoBooleanCommand(String("P2P_SERV_DISC_CANCEL_REQ ") + id);
}

void WifiNative::SetMiracastMode(
    /* [in] */ Int32 mode)
{
    // Note: optional feature on the driver. It is ok for this to fail.
    DoBooleanCommand(String("DRIVER MIRACAST ") + StringUtils::ToString(mode));
}

Boolean WifiNative::GetModeCapability(
    /* [in] */ const String& mode)
{
    String ret = DoStringCommand(String("GET_CAPABILITY modes"));
    if (!TextUtils::IsEmpty(ret)) {
        AutoPtr<ArrayOf<String> > tokens;
        StringUtils::Split(ret, " ", (ArrayOf<String>**)&tokens);

        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            String t = (*tokens)[i];
            if (t.Compare(mode) == 0)
                return TRUE;
        }
    }
    return FALSE;
}

Boolean WifiNative::FetchAnqp(
    /* [in] */ const String& bssid,
    /* [in] */ const String& subtypes)
{
    return DoBooleanCommand(String("ANQP_GET ") + bssid + " " + subtypes);
}

Boolean WifiNative::SetMode(
    /* [in] */ Int32 mode)
{
    return (::wifi_set_mode(mode) == 0);
}

Boolean WifiNative::StartHalNative()
{
    wifi_handle halHandle = (wifi_handle)sWifiHalHandle;
    if (halHandle == NULL) {
        wifi_error res = wifi_initialize(&halHandle);
        if (res == WIFI_SUCCESS) {
            //setStaticLongField(env, cls, WifiHandleVarName, (jlong)halHandle);
            sWifiHalHandle = (Int64)(halHandle);
            Logger::D(TAG, "Did set static halHandle = %p", halHandle);
        }
        //env->GetJavaVM(&mVM);
        //mCls = (jclass) env->NewGlobalRef(cls);
        Logger::D(TAG, "halHandle = %p", halHandle);
        return res == WIFI_SUCCESS;
    } else {
        return TRUE;
    }
}

static void elastos_net_wifi_hal_cleaned_up_handler(wifi_handle handle) {
    Logger::D("WifiNative", "In wifi cleaned up handler");
    WifiNative::SetWifiHalHandle(0);
}

void WifiNative::StopHalNative()
{
    Logger::D(TAG, "In wifi stop Hal");
    wifi_handle halHandle = (wifi_handle)sWifiHalHandle;
    wifi_cleanup(halHandle, elastos_net_wifi_hal_cleaned_up_handler);
}

void WifiNative::WaitForHalEventNative()
{
    //ALOGD("waitForHalEvents called, vm = %p, obj = %p, env = %p", mVM, mCls, env);
    wifi_handle halHandle = (wifi_handle)sWifiHalHandle;
    wifi_event_loop(halHandle);
}

Boolean WifiNative::StartHal() {

    AutoLock classLock(mClassLock);
    Logger::I(TAG, "startHal");
    {
        AutoLock lock(mLock);
        if (sHalIsStarted)
            return TRUE;
        if (StartHalNative()) {
            GetInterfaces();
            AutoPtr<MonitorThread> mt = new MonitorThread(NULL);
            mt->Start();
            sHalIsStarted = TRUE;
            return TRUE;
        } else {
            Logger::I(TAG, "Could not start hal");
            sHalIsStarted = FALSE;
            return FALSE;
        }
    }
}

void WifiNative::StopHal()
{
    StopHalNative();
}

Int32 WifiNative::GetInterfacesNative()
{
    Int32 n = 0;
    wifi_handle halHandle = (wifi_handle)sWifiHalHandle;
    wifi_interface_handle *ifaceHandles = NULL;
    Int32 result = wifi_get_ifaces(halHandle, &n, &ifaceHandles);
    if (result < 0) {
        return result;
    }

    if (n < 0) {
        Logger::E("WifiNative::GetInterfacesNative", "android_net_wifi_getInterfaces no interfaces");
        return 0;
    }

    if (ifaceHandles == NULL) {
        //THROW(env, "android_net_wifi_getInterfaces null interface array");
        Logger::E("WifiNative::GetInterfacesNative", "android_net_wifi_getInterfaces null interface array");
        return 0;
    }

    if (n > 8) {
        //THROW(env, "Too many interfaces");
        Logger::E("WifiNative::GetInterfacesNative", "Too many interfaces");
        return 0;
    }

    sWifiIfaceHandles = ArrayOf<Int64>::Alloc(n);

    for (int i = 0; i < n; i++) {
        (*sWifiIfaceHandles)[i] = reinterpret_cast<Int64>(ifaceHandles[i]);
    }

    return (result < 0) ? result : n;
}

Int32 WifiNative::GetInterfaces() {
    AutoLock classLock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sWifiIfaceHandles == NULL) {
            Int32 num = GetInterfacesNative();
            Int32 wifi_num = 0;
            for (Int32 i = 0; i < num; i++) {
                String name = GetInterfaceNameNative(i);
                Logger::I(TAG, "interface[%d] = %s", i, name.string());
                if (name.Equals("wlan0")) {
                    sWlan0Index = i;
                    wifi_num++;
                } else if (name.Equals("p2p0")) {
                    sP2p0Index = i;
                    wifi_num++;
                }
            }
            return wifi_num;
        } else {
            return sWifiIfaceHandles->GetLength();
        }
    }
}

String WifiNative::GetInterfaceNameNative(
    /* [in] */ Int32 index)
{
    char buf[EVENT_BUF_SIZE];

    Int64 value = (*sWifiIfaceHandles)[index];
    wifi_interface_handle handle = (wifi_interface_handle) value;
    Int32 result = ::wifi_get_iface_name(handle, buf, sizeof(buf));
    if (result < 0) {
            return String(NULL);
    }
    else {
            return String(buf);
    }
}

String WifiNative::GetInterfaceName(
    /* [in] */ Int32 index)
{
    AutoLock classLock(mClassLock);
    return GetInterfaceNameNative(index);
}

Boolean WifiNative::GetScanCapabilities(
    /* [in] */ ScanCapabilities* capabilities)
{
    return GetScanCapabilitiesNative(sWlan0Index, capabilities);
}

Boolean WifiNative::GetScanCapabilitiesNative(
    /* [in] */ Int32 iface,
    /* [in] */ ScanCapabilities* capabilities)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "getting scan capabilities on interface[%d] = %p", iface, handle);

    wifi_gscan_capabilities c;
    memset(&c, 0, sizeof(c));
    Int32 result = wifi_get_gscan_capabilities(handle, &c);
    if (result != WIFI_SUCCESS) {
        Logger::D(TAG, "failed to get capabilities : %d", result);
        return FALSE;
    }

    capabilities->max_scan_cache_size = c.max_scan_cache_size;
    capabilities->max_scan_buckets = c.max_scan_buckets;
    capabilities->max_ap_cache_per_scan = c.max_ap_cache_per_scan;
    capabilities->max_rssi_sample_size = c.max_rssi_sample_size;
    capabilities->max_scan_reporting_threshold = c.max_scan_reporting_threshold;
    capabilities->max_hotlist_aps = c.max_hotlist_aps;
    capabilities->max_significant_wifi_change_aps = c.max_significant_wifi_change_aps;

    return TRUE;
}

static void onScanResultsAvailable(wifi_request_id id, unsigned num_results) {
    Logger::D("WifiNative", "onScanResultsAvailable called");

    //reportEvent(env, mCls, "onScanResultsAvailable", "(I)V", id);
    WifiNative::OnScanResultsAvailable(id);
}

static void onScanEvent(wifi_scan_event event, unsigned status) {

    Logger::D("WifiNative", "onScanStatus called");

    //reportEvent(env, mCls, "onScanStatus", "(I)V", status);
    WifiNative::OnScanStatus(status);
}

static void onFullScanResult(wifi_request_id id, wifi_scan_result *result) {

    Logger::D("WifiNative", "onFullScanResult called");

    //jobject scanResult = createScanResult(env, result);
    AutoPtr<IScanResult> scanResult;
    CScanResult::New((IScanResult**)&scanResult);

    scanResult->SetSSID(String(result->ssid));

    char bssid[32];
    snprintf(bssid, 32, "%02x:%02x:%02x:%02x:%02x:%02x", result->bssid[0], result->bssid[1],
                    result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5]);

    scanResult->SetBSSID(String(bssid));

    scanResult->SetLevel(result->rssi);
    scanResult->SetFrequency(result->channel);
    scanResult->SetTimestamp(result->ts);


    Logger::D("WifiNative", "Creating a byte array of length %d", result->ie_length);

    AutoPtr<ArrayOf<Byte> > elements = ArrayOf<Byte>::Alloc(result->ie_length);

    Logger::D("WifiNative", "Setting byte array");

    Byte *bytes = (Byte*)&(result->ie_data[0]);
    elements->Copy(0, bytes, result->ie_length);

    Logger::D("WifiNative", "Returning result");

    //reportEvent(env, mCls, "onFullScanResult", "(ILandroid/net/wifi/ScanResult;[B)V", id, scanResult, elements);
    WifiNative::OnFullScanResult(id, scanResult, elements);
}

Boolean WifiNative::StartScanNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id,
    /* [in] */ ScanSettings* settings)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "starting scan on interface[%d] = %p", iface, handle);

    wifi_scan_cmd_params params;
    memset(&params, 0, sizeof(params));

    params.base_period = settings->base_period_ms;
    params.max_ap_per_scan = settings->max_ap_per_scan;
    params.report_threshold = settings->report_threshold;

    Logger::D(TAG, "Initialized common fields %d, %d, %d", params.base_period,
            params.max_ap_per_scan, params.report_threshold);

    //const char *bucket_array_type = "[Lcom/android/server/wifi/WifiNative$BucketSettings;";
    //const char *channel_array_type = "[Lcom/android/server/wifi/WifiNative$ChannelSettings;";

    //jobjectArray buckets = (jobjectArray)getObjectField(env, settings, "buckets", bucket_array_type);
    AutoPtr<ArrayOf<BucketSettings*> > buckets = settings->buckets;
    params.num_buckets = settings->num_buckets;

    Logger::D(TAG, "Initialized num_buckets to %d", params.num_buckets);

    for (int i = 0; i < params.num_buckets; i++) {
        //jobject bucket = getObjectArrayField(env, settings, "buckets", bucket_array_type, i);
        AutoPtr<BucketSettings> bucket = (*buckets)[i];

        params.buckets[i].bucket = bucket->bucket;
        params.buckets[i].band = (wifi_band) bucket->band;
        params.buckets[i].period = bucket->period_ms;

        Logger::D(TAG, "Initialized common bucket fields %d:%d:%d", params.buckets[i].bucket,
                params.buckets[i].band, params.buckets[i].period);

        Int32 report_events = bucket->report_events;
        params.buckets[i].report_events = report_events;

        Logger::D(TAG, "Initialized report events to %d", params.buckets[i].report_events);

        //jobjectArray channels = (jobjectArray)getObjectField(env, bucket, "channels", channel_array_type);
        AutoPtr<ArrayOf<ChannelSettings*> > channels = bucket->channels;

        params.buckets[i].num_channels = bucket->num_channels;
        Logger::D(TAG, "Initialized num_channels to %d", params.buckets[i].num_channels);

        for (int j = 0; j < params.buckets[i].num_channels; j++) {
            //jobject channel = getObjectArrayField(env, bucket, "channels", channel_array_type, j);
            AutoPtr<ChannelSettings> channel = (*channels)[j];

            params.buckets[i].channels[j].channel = channel->frequency;
            params.buckets[i].channels[j].dwellTimeMs = channel->dwell_time_ms;

            Boolean passive = channel->passive;
            params.buckets[i].channels[j].passive = (passive ? 1 : 0);

            Logger::D(TAG, "Initialized channel %d", params.buckets[i].channels[j].channel);
        }
    }

    Logger::D(TAG, "Initialized all fields");

    wifi_scan_result_handler handler;
    memset(&handler, 0, sizeof(handler));
    handler.on_scan_results_available = &onScanResultsAvailable;
    handler.on_full_scan_result = &onFullScanResult;
    handler.on_scan_event = &onScanEvent;

    return wifi_start_gscan(id, handle, params, handler) == WIFI_SUCCESS;
}

Boolean WifiNative::StopScanNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "stopping scan on interface[%d] = %p", iface, handle);

    return wifi_stop_gscan(id, handle)  == WIFI_SUCCESS;
}

AutoPtr<ArrayOf<IScanResult*> > WifiNative::GetScanResultsNative(
    /* [in] */ Int32 iface,
    /* [in] */ Boolean flush)
{
    wifi_scan_result results[256];
    Int32 num_results = 256;

    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "getting scan results on interface[%d] = %p", iface, handle);

    Int32 result = wifi_get_cached_gscan_results(handle, 1, num_results, results, &num_results);
    if (result == WIFI_SUCCESS) {
        AutoPtr<ArrayOf<IScanResult*> > scanResults = ArrayOf<IScanResult*>::Alloc(num_results);

        for (Int32 i = 0; i < num_results; i++) {

            AutoPtr<IScanResult> scanResult;
            CScanResult::New((IScanResult**)&scanResult);

            scanResult->SetSSID(String(results[i].ssid));

            char bssid[32];
            snprintf(bssid, 32, "%02x:%02x:%02x:%02x:%02x:%02x", results[i].bssid[0],
                    results[i].bssid[1], results[i].bssid[2], results[i].bssid[3],
                    results[i].bssid[4], results[i].bssid[5]);

            scanResult->SetBSSID(String(bssid));

            scanResult->SetLevel(results[i].rssi);
            scanResult->SetFrequency(results[i].channel);
            scanResult->SetTimestamp(results[i].ts);

            scanResults->Set(i, scanResult);
        }

        return scanResults;
    } else {
        return NULL;
    }
}

static wifi_iface_stat link_stat;
static wifi_radio_stat radio_stat; // L release has support for only one radio

static void onLinkStatsResults(wifi_request_id id, wifi_iface_stat *iface_stat,
     int num_radios, wifi_radio_stat *radio_stats)
{
    if (iface_stat != 0) {
        memcpy(&link_stat, iface_stat, sizeof(wifi_iface_stat));
    } else {
        memset(&link_stat, 0, sizeof(wifi_iface_stat));
    }

    if (num_radios > 0 && radio_stats != 0) {
        memcpy(&radio_stat, radio_stats, sizeof(wifi_radio_stat));
    } else {
        memset(&radio_stat, 0, sizeof(wifi_radio_stat));
    }
}

AutoPtr<IWifiLinkLayerStats> WifiNative::GetWifiLinkLayerStatsNative(
    /* [in] */ Int32 iface)
{
    wifi_stats_result_handler handler;
    memset(&handler, 0, sizeof(handler));
    handler.on_link_stats_results = &onLinkStatsResults;
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Int32 result = wifi_get_link_stats(0, handle, handler);
    if (result < 0) {
        Logger::D(TAG, "android_net_wifi_getLinkLayerStats: failed to get link statistics\n");
        return NULL;
    }

    AutoPtr<IWifiLinkLayerStats> wifiLinkLayerStats;
    CWifiLinkLayerStats::New((IWifiLinkLayerStats**)&wifiLinkLayerStats);

    wifiLinkLayerStats->SetBeacon_rx(link_stat.beacon_rx);
    wifiLinkLayerStats->SetRssi_mgmt(link_stat.rssi_mgmt);
    wifiLinkLayerStats->SetRxmpdu_be(link_stat.ac[WIFI_AC_BE].rx_mpdu);
    wifiLinkLayerStats->SetRxmpdu_bk(link_stat.ac[WIFI_AC_BK].rx_mpdu);
    wifiLinkLayerStats->SetRxmpdu_vi(link_stat.ac[WIFI_AC_VI].rx_mpdu);
    wifiLinkLayerStats->SetRxmpdu_vo(link_stat.ac[WIFI_AC_VO].rx_mpdu);
    wifiLinkLayerStats->SetTxmpdu_be(link_stat.ac[WIFI_AC_BE].tx_mpdu);
    wifiLinkLayerStats->SetTxmpdu_bk(link_stat.ac[WIFI_AC_BK].tx_mpdu);
    wifiLinkLayerStats->SetTxmpdu_vi(link_stat.ac[WIFI_AC_VI].tx_mpdu);
    wifiLinkLayerStats->SetTxmpdu_vo(link_stat.ac[WIFI_AC_VO].tx_mpdu);
    wifiLinkLayerStats->SetLostmpdu_be(link_stat.ac[WIFI_AC_BE].mpdu_lost);
    wifiLinkLayerStats->SetLostmpdu_bk(link_stat.ac[WIFI_AC_BK].mpdu_lost);
    wifiLinkLayerStats->SetLostmpdu_vi(link_stat.ac[WIFI_AC_VI].mpdu_lost);
    wifiLinkLayerStats->SetLostmpdu_vo(link_stat.ac[WIFI_AC_VO].mpdu_lost);
    wifiLinkLayerStats->SetRetries_be(link_stat.ac[WIFI_AC_BE].retries);
    wifiLinkLayerStats->SetRetries_bk(link_stat.ac[WIFI_AC_BK].retries);
    wifiLinkLayerStats->SetRetries_vi(link_stat.ac[WIFI_AC_VI].retries);
    wifiLinkLayerStats->SetRetries_vo(link_stat.ac[WIFI_AC_VO].retries);


    wifiLinkLayerStats->SetOn_time(radio_stat.on_time);
    wifiLinkLayerStats->SetTx_time(radio_stat.tx_time);
    wifiLinkLayerStats->SetRx_time(radio_stat.rx_time);
    wifiLinkLayerStats->SetOn_time_scan(radio_stat.on_time_scan);

    return wifiLinkLayerStats;
}

void WifiNative::OnScanResultsAvailable(
    /* [in] */ Int32 id)
{
    AutoLock classlock(mClassLock);
    if (sScanEventHandler != NULL) {
        sScanEventHandler->OnScanResultsAvailable();
    }
}

void WifiNative::OnScanStatus(
    /* [in] */ Int32 status)
{
    AutoLock classlock(mClassLock);
    Logger::I(TAG, "Got a scan status changed event, status = %d", status);

    if (status == WIFI_SCAN_BUFFER_FULL) {
        /* we have a separate event to take care of this */
    } else if (status == WIFI_SCAN_COMPLETE) {
        if (sScanEventHandler  != NULL) {
            sScanEventHandler->OnSingleScanComplete();
        }
    }
}

void WifiNative::OnFullScanResult(
    /* [in] */ Int32 id,
    /* [in] */ IScanResult* result,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoLock classlock(mClassLock);
    if (DBG) {
        String ssid;
        result->GetSSID(&ssid);
        Logger::I(TAG, "Got a full scan results event, ssid = %s, num = %d", ssid.string(), bytes->GetLength());
    }

    Int32 num = 0;
    for (Int32 i = 0; i < bytes->GetLength(); ) {
        num++;
        Int32 type  = (Int32)(*bytes)[i] & 0xFF;
        Int32 len = (Int32) (*bytes)[i + 1] & 0xFF;
        if (len < 0) {
            Logger::E(TAG, "bad length; returning");
            return;
        }
        i += len + 2;
        if (DBG) Logger::I(TAG, "bytes[%d] = [%d, %d], next = %d", i, type, len, i);
    }

    AutoPtr<ArrayOf<IScanResultInformationElement*> > elements = ArrayOf<IScanResultInformationElement*>::Alloc(num);
    for (Int32 i = 0, index = 0; i < num; i++) {
        Int32 type  = (Int32) (*bytes)[index] & 0xFF;
        Int32 len = (Int32) (*bytes)[index + 1] & 0xFF;
        if (DBG) Logger::I(TAG, "index = %d, type = %d, len = %d", index, type, len);

        AutoPtr<IScanResultInformationElement> elem;
        CScanResultInformationElement::New((IScanResultInformationElement**)&elem);
        elem->SetId(type);
        AutoPtr<ArrayOf<Byte> > elemBytes = ArrayOf<Byte>::Alloc(len);
        for (Int32 j = 0; j < len; j++) {
            (*elemBytes)[j] = (*bytes)[index + j + 2];
        }
        elem->SetBytes(elemBytes);
        elements->Set(i, elem);
        index += (len + 2);
    }

    result->SetInformationElements(elements);
    if (sScanEventHandler  != NULL) {
        sScanEventHandler->OnFullScanResult(result);
    }
}

Boolean WifiNative::StartScan(
    /* [in] */ ScanSettings* settings,
    /* [in] */ IWifiNativeScanEventHandler* eventHandler)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);

        if (sScanCmdId != 0) {
            StopScan();
        } else if (sScanSettings != NULL || sScanEventHandler != NULL) {
            /* current scan is paused; no need to stop it */
        }

        sScanCmdId = GetNewCmdIdLocked();

        sScanSettings = settings;
        sScanEventHandler = eventHandler;

        if (StartScanNative(sWlan0Index, sScanCmdId, settings) == FALSE) {
            sScanEventHandler = NULL;
            sScanSettings = NULL;
            return FALSE;
        }

        return TRUE;
    }
}

void WifiNative::StopScan()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        StopScanNative(sWlan0Index, sScanCmdId);
        sScanSettings = NULL;
        sScanEventHandler = NULL;
        sScanCmdId = 0;
    }
}

void WifiNative::PauseScan()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sScanCmdId != 0 && sScanSettings != NULL && sScanEventHandler != NULL) {
            Logger::D(TAG, "Pausing scan");
            StopScanNative(sWlan0Index, sScanCmdId);
            sScanCmdId = 0;
            sScanEventHandler->OnScanPaused();
        }
    }
}

void WifiNative::RestartScan()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sScanCmdId == 0 && sScanSettings != NULL && sScanEventHandler != NULL) {
            Logger::D(TAG, "Restarting scan");
            StartScan(sScanSettings, sScanEventHandler);
            sScanEventHandler->OnScanRestarted();
        }
    }
}

AutoPtr<ArrayOf<IScanResult*> > WifiNative::GetScanResults()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        return GetScanResultsNative(sWlan0Index, /* flush = */ FALSE);
    }
}

static Byte parseHexChar(char ch) {
    if (isdigit(ch))
        return ch - '0';
    else if ('A' <= ch && ch <= 'F')
        return ch - 'A' + 10;
    else if ('a' <= ch && ch <= 'f')
        return ch - 'a' + 10;
    else {
        Logger::E("WifiNative", "invalid character in bssid %c", ch);
    }
    return 0;
}

static Byte parseHexByte(const char * &str) {
    Byte b = parseHexChar(str[0]);
    if (str[1] == ':' || str[1] == '\0') {
        str += 2;
        return b;
    } else {
        b = b << 4 | parseHexChar(str[1]);
        str += 3;
        return b;
    }
}

static void parseMacAddress(const char *str, mac_addr addr) {
    addr[0] = parseHexByte(str);
    addr[1] = parseHexByte(str);
    addr[2] = parseHexByte(str);
    addr[3] = parseHexByte(str);
    addr[4] = parseHexByte(str);
    addr[5] = parseHexByte(str);
}

static void onHotlistApFound(wifi_request_id id,
        unsigned num_results, wifi_scan_result *results) {

    Logger::D("WifiNative", "onHotlistApFound called, num_results = %d", num_results);

    AutoPtr<ArrayOf<IScanResult*> > scanResults = ArrayOf<IScanResult*>::Alloc(num_results);

    for (unsigned i = 0; i < num_results; i++) {

        AutoPtr<IScanResult> scanResult;
        CScanResult::New((IScanResult**)&scanResult);
        if (scanResult == NULL) {
            Logger::E("WifiNative", "Error in creating scan result");
            return;
        }

        scanResult->SetSSID(String(results[i].ssid));

        char bssid[32];
        snprintf(bssid, 32, "%02x:%02x:%02x:%02x:%02x:%02x", results[i].bssid[0], results[i].bssid[1],
                results[i].bssid[2], results[i].bssid[3], results[i].bssid[4], results[i].bssid[5]);

        scanResult->SetBSSID(String(bssid));

        scanResult->SetLevel(results[i].rssi);
        scanResult->SetFrequency(results[i].channel);
        scanResult->SetTimestamp(results[i].ts);

        scanResults->Set(i, scanResult);

        Logger::D("WifiNative", "Found AP %32s %s", results[i].ssid, bssid);
    }

    //reportEvent(env, mCls, "onHotlistApFound", "(I[Landroid/net/wifi/ScanResult;)V", id, scanResults);
    WifiNative::OnHotlistApFound(id, scanResults);
}

Boolean WifiNative::SetHotlistNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id,
    /* [in] */ IWifiScannerHotlistSettings* settings)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "setting hotlist on interface[%d] = %p", iface, handle);

    wifi_bssid_hotlist_params params;
    memset(&params, 0, sizeof(params));

    AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > array;
    settings->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&array);
    params.num_ap = array->GetLength();

    if (params.num_ap == 0) {
        Logger::E(TAG, "Error in accesing array");
        return FALSE;
    }

    for (Int32 i = 0; i < params.num_ap; i++) {
        AutoPtr<IWifiScannerBssidInfo> objAp = (*array)[i];

        String macAddrString;
        objAp->GetBssid(&macAddrString);
        if (macAddrString.IsNull()) {
            Logger::E(TAG, "Error getting bssid field");
            return FALSE;
        }

        const char *bssid = macAddrString.string();
        if (bssid == NULL) {
            Logger::E(TAG, "Error getting bssid");
            return false;
        }
        parseMacAddress(bssid, params.ap[i].bssid);

        mac_addr addr;
        memcpy(addr, params.ap[i].bssid, sizeof(mac_addr));

        char bssidOut[32];
        snprintf(bssidOut, 32, "%0x:%0x:%0x:%0x:%0x:%0x", addr[0], addr[1],
                addr[2], addr[3], addr[4], addr[5]);

        Logger::D("WifiNative", "Added bssid %s", bssidOut);

        Int32 low, high;
        objAp->GetLow(&low);
        objAp->GetHigh(&high);
        params.ap[i].low = low;
        params.ap[i].high = high;
    }

    wifi_hotlist_ap_found_handler handler;
    memset(&handler, 0, sizeof(handler));

    handler.on_hotlist_ap_found = &onHotlistApFound;
    return wifi_set_bssid_hotlist(id, handle, params, handler) == WIFI_SUCCESS;
}

Boolean WifiNative::ResetHotlistNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "resetting hotlist on interface[%d] = %p", iface, handle);

    return wifi_reset_bssid_hotlist(id, handle) == WIFI_SUCCESS;
}

Boolean WifiNative::SetHotlist(
    /* [in] */ IWifiScannerHotlistSettings* settings,
    /* [in] */ IWifiNativeHotlistEventHandler* eventHandler)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sHotlistCmdId != 0) {
            return FALSE;
        } else {
            sHotlistCmdId = GetNewCmdIdLocked();
        }

        sHotlistEventHandler = eventHandler;
        if (SetHotlistNative(sWlan0Index, sScanCmdId, settings) == FALSE) {
            sHotlistEventHandler = NULL;
            return FALSE;
        }

        return TRUE;
    }
}

void WifiNative::ResetHotlist()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sHotlistCmdId != 0) {
            ResetHotlistNative(sWlan0Index, sHotlistCmdId);
            sHotlistCmdId = 0;
            sHotlistEventHandler = NULL;
        }
    }
}

void WifiNative::OnHotlistApFound(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sHotlistCmdId != 0) {
            sHotlistEventHandler->OnHotlistApFound(results);
        } else {
            /* this can happen because of race conditions */
            Logger::D(TAG, "Ignoring hotlist AP found change");
        }
    }
}

static void onSignificantWifiChange(wifi_request_id id,
        unsigned num_results, wifi_significant_change_result **results) {
    AutoPtr<ArrayOf<IScanResult*> > scanResults = ArrayOf<IScanResult*>::Alloc(num_results);
    if (scanResults == NULL) {
        Logger::E("WifiNative", "Error in allocating array");
        return;
    }

    for (unsigned i = 0; i < num_results; i++) {

        wifi_significant_change_result result = *(results[i]);

        AutoPtr<IScanResult> scanResult;
        CScanResult::New((IScanResult**)&scanResult);
        if (scanResult == NULL) {
            Logger::E("WifiNative", "Error in creating scan result");
            return;
        }

        char bssid[32];
        snprintf(bssid, 32, "%02x:%02x:%02x:%02x:%02x:%02x", result.bssid[0], result.bssid[1],
                result.bssid[2], result.bssid[3], result.bssid[4], result.bssid[5]);

        scanResult->SetBSSID(String(bssid));

        scanResult->SetLevel(result.rssi[0]);
        scanResult->SetFrequency(result.channel);

        scanResults->Set(i, scanResult);
    }

    //reportEvent(env, mCls, "OnSignificantWifiChange", "(I[Landroid/net/wifi/ScanResult;)V", id, scanResults);
    WifiNative::OnSignificantWifiChange(id, scanResults);

}

Boolean WifiNative::TrackSignificantWifiChangeNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id,
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "tracking significant wifi change on interface[%d] = %p", iface, handle);

    wifi_significant_change_params params;
    memset(&params, 0, sizeof(params));

    Int32 rssiSampleSize;
    settings->GetLostApSampleSize(&rssiSampleSize);
    params.rssi_sample_size = rssiSampleSize;
    Int32 lostApSampleSize;
    settings->GetLostApSampleSize(&lostApSampleSize);
    params.lost_ap_sample_size = lostApSampleSize;
    Int32 minApsBreachingThreshold;
    settings->GetMinApsBreachingThreshold(&minApsBreachingThreshold);
    params.min_breaching = minApsBreachingThreshold;

    //const char *bssid_info_array_type = "[Landroid/net/wifi/WifiScanner$BssidInfo;";
    //jobjectArray bssids = (jobjectArray)getObjectField(
    //        env, settings, "bssidInfos", bssid_info_array_type);
    AutoPtr<ArrayOf<IWifiScannerBssidInfo*> > bssids;
    settings->GetBssidInfos((ArrayOf<IWifiScannerBssidInfo*>**)&bssids);
    params.num_ap = bssids->GetLength();

    if (params.num_ap == 0) {
        Logger::E(TAG, "Error in accessing array");
        return FALSE;
    }

    Logger::D(TAG, "Initialized common fields %d, %d, %d, %d", params.rssi_sample_size,
            params.lost_ap_sample_size, params.min_breaching, params.num_ap);

    for (Int32 i = 0; i < params.num_ap; i++) {
        //jobject objAp = env->GetObjectArrayElement(bssids, i);
        AutoPtr<IWifiScannerBssidInfo> objAp = (*bssids)[i];

        //jstring macAddrString = (jstring) getObjectField(
        //        env, objAp, "bssid", "Ljava/lang/String;");
        String macAddrString;
        objAp->GetBssid(&macAddrString);
        if (macAddrString.IsNull()) {
            Logger::E(TAG, "Error getting bssid field");
            return FALSE;
        }

        const char *bssid = macAddrString.string();
        if (bssid == NULL) {
            Logger::E(TAG, "Error getting bssid");
            return FALSE;
        }

        mac_addr addr;
        parseMacAddress(bssid, addr);
        memcpy(params.ap[i].bssid, addr, sizeof(mac_addr));

        char bssidOut[32];
        snprintf(bssidOut, 32, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1],
                addr[2], addr[3], addr[4], addr[5]);

        Int32 low, high;
        objAp->GetLow(&low);
        objAp->GetHigh(&high);

        params.ap[i].low = low;
        params.ap[i].high = high;

        Logger::D(TAG, "Added bssid %s, [%04d, %04d]", bssidOut, params.ap[i].low, params.ap[i].high);
    }

    Logger::D(TAG, "Added %d bssids", params.num_ap);

    wifi_significant_change_handler handler;
    memset(&handler, 0, sizeof(handler));

    handler.on_significant_change = &onSignificantWifiChange;
    return wifi_set_significant_change_handler(id, handle, params, handler) == WIFI_SUCCESS;
}

Boolean WifiNative::UntrackSignificantWifiChangeNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "resetting significant wifi change on interface[%d] = %p", iface, handle);

    return wifi_reset_significant_change_handler(id, handle) == WIFI_SUCCESS;
}

Boolean WifiNative::TrackSignificantWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings,
    /* [in] */ IWifiNativeSignificantWifiChangeEventHandler* handler)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sSignificantWifiChangeCmdId != 0) {
            return FALSE;
        } else {
            sSignificantWifiChangeCmdId = GetNewCmdIdLocked();
        }

        sSignificantWifiChangeHandler = handler;
        if (TrackSignificantWifiChangeNative(sWlan0Index, sScanCmdId, settings) == FALSE) {
            sSignificantWifiChangeHandler = NULL;
            return FALSE;
        }

        return TRUE;
    }
}

void WifiNative::UntrackSignificantWifiChange()
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sSignificantWifiChangeCmdId != 0) {
            UntrackSignificantWifiChangeNative(sWlan0Index, sSignificantWifiChangeCmdId);
            sSignificantWifiChangeCmdId = 0;
            sSignificantWifiChangeHandler = NULL;
        }
    }
}

void WifiNative::OnSignificantWifiChange(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IScanResult*>* results)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sSignificantWifiChangeCmdId != 0) {
            sSignificantWifiChangeHandler->OnChangesFound(results);
        } else {
            /* this can happen because of race conditions */
            Logger::D(TAG, "Ignoring significant wifi change");
        }
    }
}

AutoPtr<IWifiLinkLayerStats> WifiNative::GetWifiLinkLayerStats(
    /* [in] */ const String& iface)
{
    AutoLock classlock(mClassLock);
    // TODO: use correct iface name to Index translation
    if (iface.IsNull()) return NULL;
    {
        AutoLock lock(mLock);
        if (!sHalIsStarted)
            StartHal();
        if (sHalIsStarted)
            return GetWifiLinkLayerStatsNative(sWlan0Index);
    }
    return NULL;
}

String WifiNative::GetNfcWpsConfigurationToken(
    /* [in] */ Int32 netId)
{
    return DoStringCommand(String("WPS_NFC_CONFIG_TOKEN WPS ") + StringUtils::ToString(netId));
}

String WifiNative::GetNfcHandoverRequest()
{
    return DoStringCommand(String("NFC_GET_HANDOVER_REQ NDEF P2P-CR"));
}

String WifiNative::GetNfcHandoverSelect()
{
    return DoStringCommand(String("NFC_GET_HANDOVER_SEL NDEF P2P-CR"));
}

Boolean WifiNative::InitiatorReportNfcHandover(
    /* [in] */ const String& selectMessage)
{
    return DoBooleanCommand(String("NFC_REPORT_HANDOVER INIT P2P 00 ") + selectMessage);
}

Boolean WifiNative::ResponderReportNfcHandover(
    /* [in] */ const String& requestMessage)
{
    return DoBooleanCommand(String("NFC_REPORT_HANDOVER RESP P2P ") + requestMessage + " 00");
}

Int32 WifiNative::GetSupportedFeatureSetNative(
    /* [in] */ Int32 iface)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    feature_set set = 0;

    wifi_error result = WIFI_SUCCESS;
    /*
       set = WIFI_FEATURE_INFRA
       | WIFI_FEATURE_INFRA_5G
       | WIFI_FEATURE_HOTSPOT
       | WIFI_FEATURE_P2P
       | WIFI_FEATURE_SOFT_AP
       | WIFI_FEATURE_GSCAN
       | WIFI_FEATURE_PNO
       | WIFI_FEATURE_TDLS
       | WIFI_FEATURE_EPR;
     */

    result = wifi_get_supported_feature_set(handle, &set);
    if (result == WIFI_SUCCESS) {
        /* Temporary workaround for RTT capability */
        set = set | WIFI_FEATURE_D2AP_RTT;
        Logger::D(TAG, "wifi_get_supported_feature_set returned set = 0x%x", set);
        return set;
    } else {
        Logger::D(TAG, "wifi_get_supported_feature_set returned error = 0x%x", result);
        return 0;
    }
}

Int32 WifiNative::GetSupportedFeatureSet()
{
    AutoLock classlock(mClassLock);
    return GetSupportedFeatureSetNative(sWlan0Index);
}

void WifiNative::OnRttResults(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IRttManagerRttResult*>* results)
{
    AutoLock classlock(mClassLock);
    if (id == sRttCmdId) {
        Logger::D(TAG, "Received %d rtt results", results->GetLength());
        sRttEventHandler->OnRttResults(results);
        sRttCmdId = 0;
    } else {
        Logger::D(TAG, "Received event for unknown cmd = %d, current id = %d",  id, sRttCmdId);
    }
}

const static Int32 MaxRttConfigs = 16;

static void onRttResults(wifi_request_id id, unsigned num_results, wifi_rtt_result results[]) {

    AutoPtr<ArrayOf<IRttManagerRttResult*> > rttResults = ArrayOf<IRttManagerRttResult*>::Alloc(num_results);
    if (rttResults == NULL) {
        Logger::E("WifiNative", "Error in allocating array");
        return;
    }

    for (unsigned i = 0; i < num_results; i++) {

        wifi_rtt_result& result = results[i];

        AutoPtr<IRttManagerRttResult> rttResult;
        CRttManagerRttResult::New((IRttManagerRttResult**)&rttResult);
        if (rttResult == NULL) {
            Logger::E("WifiNative", "Error in creating rtt result");
            return;
        }

        char bssid[32];
        snprintf(bssid, 32, "%02x:%02x:%02x:%02x:%02x:%02x", result.addr[0], result.addr[1],
                result.addr[2], result.addr[3], result.addr[4], result.addr[5]);

        rttResult->SetBssid(String(bssid));
        rttResult->SetStatus(result.status);
        rttResult->SetRequestType(result.type);
        rttResult->SetTs(result.ts);
        rttResult->SetRssi(result.rssi);
        rttResult->SetRssi_spread(result.rssi_spread);
        rttResult->SetTx_rate(result.tx_rate.bitrate);
        rttResult->SetRtt_ns(result.rtt);
        rttResult->SetRtt_sd_ns(result.rtt_sd);
        rttResult->SetRtt_spread_ns(result.rtt_spread);
        rttResult->SetDistance_cm(result.distance);
        rttResult->SetDistance_sd_cm(result.distance_sd);
        rttResult->SetDistance_spread_cm(result.distance_spread);

        rttResults->Set(i, rttResult);
    }

    //reportEvent(env, mCls, "OnRttResults", "(I[Landroid/net/wifi/RttManager$RttResult;)V", id, rttResults);
    WifiNative::OnRttResults(id, rttResults);
}


Boolean WifiNative::RequestRangeNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D("WifiNative", "sending rtt request [%d] = %p", id, handle);

    wifi_rtt_config configs[MaxRttConfigs];
    memset(&configs, 0, sizeof(configs));

    Int32 len = params->GetLength();
    if (len > MaxRttConfigs) {
        return FALSE;
    }

    for (Int32 i = 0; i < len; i++) {

        //jobject param = env->GetObjectArrayElement((jobjectArray)params, i);
        AutoPtr<IRttManagerRttParams> param = (*params)[i];
        if (param == NULL) {
            Logger::D("WifiNative", "could not get element %d", i);
            continue;
        }

        wifi_rtt_config &config = configs[i];

        String bssid;
        param->GetBssid(&bssid);
        parseMacAddress(bssid.string(), config.addr);
        Int32 requestType;
        param->GetRequestType(&requestType);
        config.type = (wifi_rtt_type)requestType;
        Int32 deviceType;
        param->GetDeviceType(&deviceType);
        config.peer = (wifi_peer_type)deviceType ;
        Int32 frequency;
        param->GetFrequency(&frequency);
        config.channel.center_freq = frequency;
        Int32 channelWidth;
        param->GetChannelWidth(&channelWidth);
        config.channel.width = (wifi_channel_width)channelWidth;
        Int32 num_samples;
        param->GetNum_samples(&num_samples);
        config.num_samples_per_measurement = num_samples;
        Int32 num_retries;
        param->GetNum_retries(&num_retries);
        config.num_retries_per_measurement = num_retries;
    }

    wifi_rtt_event_handler handler;
    handler.on_rtt_results = &onRttResults;

    return wifi_rtt_range_request(id, handle, len, configs, handler) == WIFI_SUCCESS;

}

Boolean WifiNative::CancelRangeRequestNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "cancelling rtt request [%d] = %p", id, handle);

    mac_addr addrs[MaxRttConfigs];
    memset(&addrs, 0, sizeof(addrs));

    Int32 len = params->GetLength();
    if (len > MaxRttConfigs) {
        return FALSE;
    }

    for (Int32 i = 0; i < len; i++) {

        AutoPtr<IRttManagerRttParams> param = (*params)[i];
        if (param == NULL) {
            Logger::D(TAG, "could not get element %d", i);
            continue;
        }

        String bssid;
        param->GetBssid(&bssid);
        parseMacAddress(bssid.string(), addrs[i]);
    }

    return wifi_rtt_range_cancel(id, handle, len, addrs) == WIFI_SUCCESS;
}

Boolean WifiNative::RequestRtt(
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params,
    /* [in] */ IWifiNativeRttEventHandler* handler)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sRttCmdId != 0) {
            return FALSE;
        } else {
            sRttCmdId = GetNewCmdIdLocked();
        }
        sRttEventHandler = handler;
        return RequestRangeNative(sWlan0Index, sRttCmdId, params);
    }
}

Boolean WifiNative::CancelRtt(
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (sRttCmdId == 0) {
            return FALSE;
        }

        if (CancelRangeRequestNative(sWlan0Index, sRttCmdId, params)) {
            sRttEventHandler = NULL;
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

Boolean WifiNative::SetScanningMacOui(
    /* [in] */ ArrayOf<Byte>* oui)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (StartHal()) {
            return SetScanningMacOuiNative(sWlan0Index, oui);
        } else {
            return FALSE;
        }
    }
}

AutoPtr<ArrayOf<Int32> > WifiNative::GetChannelsForBand(
    /* [in] */ Int32 band)
{
    AutoLock classlock(mClassLock);
    {
        AutoLock lock(mLock);
        if (StartHal()) {
            return GetChannelsForBandNative(sWlan0Index, band);
        } else {
            return NULL;
        }
    }
}

Boolean WifiNative::Disable5GHzFrequencies(
    /* [in] */ Boolean disable)
{
    if (disable) {
        return DoBooleanCommand(String("P2P_SET disallow_freq 2485-6000"));
    } else {
        //Empty set means,it will enable all frequences
        return DoBooleanCommand(String("P2P_SET disallow_freq \"\""));
    }
}

Boolean WifiNative::SetScanningMacOuiNative(
    /* [in] */ Int32 iface,
    /* [in] */ ArrayOf<Byte>* oui)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "setting scan oui %p", handle);

    static const UInt32 oui_len = 3;          /* OUI is upper 3 bytes of mac_address */
    Int32 len = oui->GetLength();
    if (len != oui_len) {
        Logger::E(TAG, "invalid oui length %d", len);
        return FALSE;
    }

    Byte* bytes = oui->GetPayload();
    if (bytes == NULL) {
        Logger::E(TAG, "failed to get array");
        return FALSE;
    }

    return wifi_set_scanning_mac_oui(handle, (byte *)bytes) == WIFI_SUCCESS;
}

AutoPtr<ArrayOf<Int32> > WifiNative::GetChannelsForBandNative(
    /* [in] */ Int32 iface,
    /* [in] */ Int32 band)
{
    wifi_interface_handle handle = (wifi_interface_handle)((*sWifiIfaceHandles)[iface]);
    Logger::D(TAG, "getting valid channels %p", handle);

    static const Int32 MaxChannels = 64;
    wifi_channel channels[64];
    Int32 num_channels = 0;
    wifi_error result = wifi_get_valid_channels(handle, band, MaxChannels,
            channels, &num_channels);

    if (result == WIFI_SUCCESS) {
        AutoPtr<ArrayOf<Int32> > channelArray = ArrayOf<Int32>::Alloc(num_channels);
        if (channelArray == NULL) {
            Logger::E(TAG, "failed to allocate channel list");
            return NULL;
        }

        channelArray->Copy(0, channels, num_channels);
        return channelArray;
    } else {
        Logger::E(TAG, "failed to get channel list : %d", result);
        return NULL;
    }
}

void WifiNative::SetWifiHalHandle(
    /* [in] */ Int64 handle)
{
    sWifiHalHandle = handle;
}


} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
