
#ifndef __WIFINATIVE_H__
#define __WIFINATIVE_H__

#include <elastos.h>

using namespace Elastos;

/**
 * Native calls for sending requests to the supplicant daemon, and for
 * receiving asynchronous events. All methods of the form "xxxxCommand()"
 * must be single-threaded, to avoid requests and responses initiated
 * from multiple threads from being intermingled.
 * <p/>
 * Note that methods whose names are not of the form "xxxCommand()" do
 * not talk to the supplicant daemon.
 * Also, note that all WifiNative calls should happen in the
 * WifiStateTracker class except for waitForEvent() call which is
 * on a separate monitor channel for WifiMonitor
 *
 * {@hide}
 */
class WifiNative
{
public:
    static CARAPI_(String) GetErrorString(
        /* [in] */ Int32 errorCode);

    static CARAPI_(Boolean) LoadDriver();

    static CARAPI_(Boolean) UnloadDriver();

    static CARAPI_(Boolean) StartSupplicant();

    /* Sends a kill signal to supplicant. To be used when we have lost connection
       or when the supplicant is hung */
    static CARAPI_(Boolean) StopSupplicant(
        /* [in] */ Boolean p2pSupported);

    static CARAPI_(Boolean) ConnectToSupplicant();

    static CARAPI_(void) CloseSupplicantConnection();

    static CARAPI_(Boolean) PingCommand();

    static CARAPI_(Boolean) ScanCommand(
        /* [in] */ Boolean forceActive);

    static CARAPI_(Boolean) SetScanModeCommand(
        /* [in] */ Boolean setActive);

    static CARAPI_(String) ListNetworksCommand();

    static CARAPI_(Int32) AddNetworkCommand();

    static CARAPI_(Boolean) SetNetworkVariableCommand(
        /* [in] */ Int32 netId,
        /* [in] */ CString name,
        /* [in] */ CString value);

    static CARAPI_(String) GetNetworkVariableCommand(
        /* [in] */ Int32 netId,
        /* [in] */ CString name);

    static CARAPI_(Boolean) RemoveNetworkCommand(
        /* [in] */ Int32 netId);

    static CARAPI_(Boolean) EnableNetworkCommand(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers);

    static CARAPI_(Boolean) DisableNetworkCommand(
        /* [in] */ Int32 netId);

    static CARAPI_(Boolean) ReconnectCommand();

    static CARAPI_(Boolean) ReassociateCommand();

    static CARAPI_(Boolean) DisconnectCommand();

    static CARAPI_(String) StatusCommand();

    static CARAPI_(Int32) GetRssiCommand();

    static CARAPI_(Int32) GetRssiApproxCommand();

    static CARAPI_(Int32) GetLinkSpeedCommand();

    static CARAPI_(String) GetMacAddressCommand();

    static CARAPI_(String) ScanResultsCommand();

    static CARAPI_(Boolean) StartDriverCommand();

    static CARAPI_(Boolean) StopDriverCommand();

    /**
     * Start filtering out multicast packets, to reduce battery consumption
     * that would result from processing them, only to discard them.
     * @return {@code true} if the operation succeeded, {@code false} otherwise
     */
    static CARAPI_(Boolean) StartPacketFiltering();

    /**
     * Stop filtering out multicast packets.
     * @return {@code true} if the operation succeeded, {@code false} otherwise
     */
    static CARAPI_(Boolean) StopPacketFiltering();

    static CARAPI_(Boolean) SetPowerModeCommand(
        /* [in] */ Int32 mode);

    static CARAPI_(Int32) GetPowerModeCommand();

    static CARAPI_(Boolean) SetNumAllowedChannelsCommand(
        /* [in] */ Int32 numChannels);

    static CARAPI_(Int32) GetNumAllowedChannelsCommand();

    /**
     * Sets the bluetooth coexistence mode.
     *
     * @param mode One of {@link #BLUETOOTH_COEXISTENCE_MODE_DISABLED},
     *            {@link #BLUETOOTH_COEXISTENCE_MODE_ENABLED}, or
     *            {@link #BLUETOOTH_COEXISTENCE_MODE_SENSE}.
     * @return Whether the mode was successfully set.
     */
    static CARAPI_(Boolean) SetBluetoothCoexistenceModeCommand(
        /* [in] */ Int32 mode);

    /**
     * Enable or disable Bluetooth coexistence scan mode. When this mode is on,
     * some of the low-level scan parameters used by the driver are changed to
     * reduce interference with A2DP streaming.
     *
     * @param isSet whether to enable or disable this mode
     * @return {@code true} if the command succeeded, {@code false} otherwise.
     */
    static CARAPI_(Boolean) SetBluetoothCoexistenceScanModeCommand(
        /* [in] */ Boolean setCoexScanMode);

    static CARAPI_(Boolean) SaveConfigCommand();

    static CARAPI_(Boolean) ReloadConfigCommand();

    static CARAPI_(Boolean) SetScanResultHandlingCommand(
        /* [in] */ Int32 mode);

    static CARAPI_(Boolean) AddToBlacklistCommand(
        /* [in] */ CString bssid);

    static CARAPI_(Boolean) ClearBlacklistCommand();

    // static CARAPI_(Boolean) DoDhcpRequest(DhcpInfo results);

    static CARAPI_(String) GetDhcpError();

    static CARAPI_(Boolean) SetSuspendOptimizationsCommand(
        /* [in] */ Boolean enabled);

    /**
     * Wait for the supplicant to send an event, returning the event string.
     * @return the event string sent by the supplicant.
     */
    static CARAPI_(String) WaitForEvent();

    static Boolean RunDhcp();

    static Boolean StopDhcp();

    static Boolean SetInterfaceDown();

private:
    static CARAPI_(Int32) GetRssiHelper(
        /* [in] */ CString cmd);

public:
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_ENABLED = 0;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_DISABLED = 1;
    static const Int32 BLUETOOTH_COEXISTENCE_MODE_SENSE = 2;

    static String sInterface;
};

#endif //__WIFINATIVE_H__
