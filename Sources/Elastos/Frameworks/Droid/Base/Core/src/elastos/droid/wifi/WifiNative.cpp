
#include "elastos/droid/wifi/WifiNative.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
//#include <wifi.h>
#include <stdlib.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;

#define WIFI_PKG_NAME "android/net/wifi/WifiNative"
#define BUF_SIZE 256
#define EVENT_BUF_SIZE 2048

static const Boolean DBG = FALSE;

namespace Elastos {
namespace Droid {
namespace Wifi {

static int doCommand(const char *ifname, const char *cmd, char *replybuf, int replybuflen)
{
    size_t reply_len = replybuflen - 1;

    if (::wifi_command(ifname, cmd, replybuf, &reply_len) != 0) {
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

static int doIntCommand(const char *ifname, const char* fmt, ...)
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
    if (doCommand(ifname, buf, reply, sizeof(reply)) != 0) {
        return -1;
    }
    return static_cast<int>(atoi(reply));
}

static Boolean doBooleanCommand(const char *ifname, const char* expect, const char* fmt, ...)
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
    if (doCommand(ifname, buf, reply, sizeof(reply)) != 0) {
        return FALSE;
    }
    return (strcmp(reply, expect) == 0);
}

// Send a command to the supplicant, and return the reply as a String
static String doStringCommand(const char *ifname, const char* fmt, ...)
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
    if (doCommand(ifname, buf, reply, sizeof(reply)) != 0) {
        return String(NULL);
    }
    return String(reply);
}

const Int32 WifiNative::DEFAULT_GROUP_OWNER_INTENT;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_ENABLED;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_DISABLED;
const Int32 WifiNative::BLUETOOTH_COEXISTENCE_MODE_SENSE;

WifiNative::WifiNative(
    /* [in] */ const String& iface)
    : mInterface(iface)
    , mSuspendOptEnabled(FALSE)
{
    mTAG = String("WifiNative-") + iface;
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

Boolean WifiNative::ConnectToSupplicant(
    /* [in] */ const String& iface)
{
    return (::wifi_connect_to_supplicant(iface.string()) == 0);
}

void WifiNative::CloseSupplicantConnection(
    /* [in] */ const String& iface)
{
    ::wifi_close_supplicant_connection(iface.string());
}

String WifiNative::WaitForEvent(
    /* [in] */ const String& iface)
{
    char buf[EVENT_BUF_SIZE];
    int nread = ::wifi_wait_for_event(iface.string(), buf, sizeof buf);
    if (nread > 0) {
        return String(buf);
    }
    else {
        return String(NULL);
    }
}

Boolean WifiNative::DoBooleanCommand(
    /* [in] */ const String& iface,
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return FALSE;
    }

    return doBooleanCommand(iface.string(), "OK", "%s", command.string());
}

Int32 WifiNative::DoIntCommand(
    /* [in] */ const String& iface,
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return -1;
    }

    return doIntCommand(iface.string(), "%s", command.string());
}

String WifiNative::DoStringCommand(
    /* [in] */ const String& iface,
    /* [in] */ const String& command)
{
    if (command.IsNull()) {
        return String(NULL);
    }

    return doStringCommand(iface.string(), "%s", command.string());
}

Boolean WifiNative::ConnectToSupplicant()
{
    return ConnectToSupplicant(mInterface);
}

void WifiNative::CloseSupplicantConnection()
{
    CloseSupplicantConnection(mInterface);
}

String WifiNative::WaitForEvent()
{
    return WaitForEvent(mInterface);
}

Boolean WifiNative::DoBooleanCommand(
    /* [in] */ const String& command)
{
    if (DBG) Logger::D(mTAG, "doBoolean: [%s]", command.string());
    return DoBooleanCommand(mInterface, command);
}

Int32 WifiNative::DoIntCommand(
    /* [in] */ const String& command)
{
    if (DBG) Logger::D(mTAG, "doInt: [%s]", command.string());
    return DoIntCommand(mInterface, command);
}

String WifiNative::DoStringCommand(
    /* [in] */ const String& command)
{
    if (DBG) {
        if (!command.Equals("CMD_RSSI_POLL") && !command.Equals("SIGNAL_POLL"))
            Logger::D(mTAG, "doString: [%s]", command.string());
    }
    return DoStringCommand(mInterface, command);
}

Boolean WifiNative::Ping()
{
    String pong = DoStringCommand(String("PING"));
    return (!pong.IsNull() && pong.Equals("PONG"));
}

Boolean WifiNative::Scan()
{
   return DoBooleanCommand(String("SCAN"));
}

Boolean WifiNative::SetScanMode(
    /* [in] */ Boolean setActive)
{
    if (setActive) {
        return DoBooleanCommand(String("DRIVER SCAN-ACTIVE"));
    }
    else {
        return DoBooleanCommand(String("DRIVER SCAN-PASSIVE"));
    }
}

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
    command.AppendCStr("SET_NETWORK ");
    command.AppendInt32(netId);
    command.AppendCStr(" ");
    command.AppendString(name);
    command.AppendCStr(" ");
    command.AppendString(value);
    return DoBooleanCommand(command.ToString());
}

String WifiNative::GetNetworkVariable(
    /* [in] */ Int32 netId,
    /* [in] */ const String& name)
{
    if (name.IsNullOrEmpty()) return String(NULL);
    StringBuilder command;
    command.AppendCStr("GET_NETWORK ");
    command.AppendInt32(netId);
    command.AppendCStr(" ");
    command.AppendString(name);
    return DoStringCommand(command.ToString());
}

Boolean WifiNative::RemoveNetwork(
    /* [in] */ Int32 netId)
{
    return DoBooleanCommand(String("REMOVE_NETWORK ") + StringUtils::Int32ToString(netId));
}

Boolean WifiNative::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers)
{
    if (disableOthers) {
        return DoBooleanCommand(String("SELECT_NETWORK ") + StringUtils::Int32ToString(netId));
    }
    else {
        return DoBooleanCommand(String("ENABLE_NETWORK ") + StringUtils::Int32ToString(netId));
    }
}

Boolean WifiNative::DisableNetwork(
    /* [in] */ Int32 netId)
{
    return DoBooleanCommand(String("DISABLE_NETWORK ") + StringUtils::Int32ToString(netId));
}

Boolean WifiNative::Reconnect()
{
    return DoBooleanCommand(String("RECONNECT"));
}

Boolean WifiNative::Reassociate()
{
    return DoBooleanCommand(String("REASSOCIATE"));
}

Boolean WifiNative::Disconnect()
{
    return DoBooleanCommand(String("DISCONNECT"));
}

String WifiNative::Status()
{
    return DoStringCommand(String("STATUS"));
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

String WifiNative::ScanResults()
{
    return DoStringCommand(String("BSS RANGE=ALL MASK=0x1986"));
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
    return DoBooleanCommand(String("DRIVER SETBAND ") + StringUtils::Int32ToString(band));
}

Boolean WifiNative::SetBluetoothCoexistenceMode(
    /* [in] */ Int32 mode)
{
    return DoBooleanCommand(String("DRIVER BTCOEXMODE ") + StringUtils::Int32ToString(mode));
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

Boolean WifiNative::SaveConfig()
{
    // Make sure we never write out a value for AP_SCAN other than 1
    return DoBooleanCommand(String("AP_SCAN 1")) && DoBooleanCommand(String("SAVE_CONFIG"));
}

Boolean WifiNative::SetScanResultHandling(
    /* [in] */ Int32 mode)
{
    return DoBooleanCommand(String("AP_SCAN ") + StringUtils::Int32ToString(mode));
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
    if (mSuspendOptEnabled == enabled) return TRUE;
    mSuspendOptEnabled = enabled;
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
    return DoBooleanCommand(String("DRIVER COUNTRY ") + countryCode);
}

void WifiNative::EnableBackgroundScan(
    /* [in] */ Boolean enable)
{
    if (enable) {
        DoBooleanCommand(String("SET pno 1"));
    }
    else {
        DoBooleanCommand(String("SET pno 0"));
    }
}

void WifiNative::SetScanInterval(
    /* [in] */ Int32 scanInterval)
{
    DoBooleanCommand(String("SCAN_INTERVAL ") + StringUtils::Int32ToString(scanInterval));
}

String WifiNative::SignalPoll()
{
    return DoStringCommand(String("SIGNAL_POLL"));
}

String WifiNative::PktcntPoll()
{
    return DoStringCommand(String("PKTCNT_POLL"));
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
    if (bssid.IsNullOrEmpty()) {
        return DoBooleanCommand(String("WPS_PBC interface=") + iface);
    }
    else {
        StringBuilder command;
        command.AppendCStr("WPS_PBC interface=");
        command.AppendString(iface);
        command.AppendCStr(" ");
        command.AppendString(bssid);
        return DoBooleanCommand(command.ToString());
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
    StringBuilder command;
    command.AppendCStr("WPS_PIN interface=");
    command.AppendString(iface);
    command.AppendCStr(" any ");
    command.AppendString(pin);
    return DoBooleanCommand(command.ToString());
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
    if (bssid.IsNullOrEmpty()) {
        StringBuilder command;
        command.AppendCStr("WPS_PIN interface=");
        command.AppendString(iface);
        command.AppendCStr(" any");
        return DoStringCommand(command.ToString());
    }
    else {
        StringBuilder command;
        command.AppendCStr("WPS_PIN interface=");
        command.AppendString(iface);
        command.AppendCStr(" ");
        command.AppendString(bssid);
        return DoStringCommand(command.ToString());
    }
}

Boolean WifiNative::StartWpsRegistrar(
    /* [in] */ const String& bssid,
    /* [in] */ const String& pin)
{
    if (bssid.IsNullOrEmpty() || pin.IsNullOrEmpty()) return FALSE;
    StringBuilder command;
    command.AppendCStr("WPS_REG ");
    command.AppendString(bssid);
    command.AppendCStr(" ");
    command.AppendString(pin);
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
    return DoBooleanCommand(String("SET persistent_reconnect ") + StringUtils::Int32ToString(value));
}

Boolean WifiNative::SetGroupOwnerPsk(
    /* [in] */ const String& name)
{
    return DoBooleanCommand(String("SET p2p_go_swpsk ") + name);
}

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
    StringBuilder command;
    command.AppendCStr("SET interface=");
    command.AppendString(iface);
    command.AppendCStr(" p2p_group_idle ");
    command.AppendInt32(time);
    return DoBooleanCommand(command.ToString());
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
    if (enabled) {
        StringBuilder command;
        command.AppendCStr("P2P_SET interface=");
        command.AppendString(iface);
        command.AppendCStr(" ps 1");
        return DoBooleanCommand(String("P2P_SET interface=") + iface + String(" ps 1"));
    }
    else {
        StringBuilder command;
        command.AppendCStr("P2P_SET interface=");
        command.AppendString(iface);
        command.AppendCStr(" ps 0");
        return DoBooleanCommand(String("P2P_SET interface=") + iface + String(" ps 0"));
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
    return DoBooleanCommand(String("P2P_FIND ") + StringUtils::Int32ToString(timeout));
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
    return DoBooleanCommand(String("P2P_LISTEN ") + StringUtils::Int32ToString(timeout));
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
    return DoBooleanCommand(String("P2P_GROUP_ADD persistent=") + StringUtils::Int32ToString(netId));
}

Boolean WifiNative::P2pGroupRemove(
    /* [in] */ const String& iface)
{
    if (iface.IsNullOrEmpty()) return FALSE;
    return DoBooleanCommand(String("P2P_GROUP_REMOVE ") + iface);
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
    command.AppendCStr("P2P_INVITE persistent=");
    command.AppendInt32(netId);
    command.AppendCStr(" peer=");
    command.AppendString(deviceAddress);
    return DoBooleanCommand(command.ToString());
}

String WifiNative::P2pGetSsid(
    /* [in] */ const String& deviceAddress)
{
    return P2pGetParam(deviceAddress, String("oper_ssid"));
}

String WifiNative::P2pGetDeviceAddress()
{
    String status = Status();
    if (status.IsNull()) return String("");

    AutoPtr< ArrayOf<String> > tokens;
    StringUtils::Split(status, String("\n"), (ArrayOf<String>**)&tokens);
    if (tokens == NULL) return String("");
    for (Int32 i = 0; i < tokens->GetLength(); ++i) {
        String token = (*tokens)[i];
        if (token.StartWith("p2p_device_address=")) {
            AutoPtr< ArrayOf<String> > nameValue;
            StringUtils::Split(token, String("="), (ArrayOf<String>**)&nameValue);
            if (nameValue->GetLength() != 2) break;
            return (*nameValue)[1];
        }
    }
    return String("");
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
        ECode ec = NOERROR;
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            String token = (*tokens)[i];
            if (token.StartWith("group_capab=")) {
                AutoPtr<ArrayOf<String> > nameValue;
                StringUtils::Split(token, "=", (ArrayOf<String>**)&nameValue);
                if (nameValue == NULL || nameValue->GetLength() != 2) break;

                ec = StringUtils::ParseInt32((*nameValue)[1], 10, &gc);
                if (FAILED(ec)) {
                    gc = 0;
                }

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

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
