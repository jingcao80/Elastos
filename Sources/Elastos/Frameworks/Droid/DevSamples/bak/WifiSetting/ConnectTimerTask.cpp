
#include "ConnectTimerTask.h"
#include "stdio.h"
#include "WifiFun.h"
#include "CWifiSetting.h"
#include <unistd.h>
#include "Ethernet.h"

Boolean ConnectTimerTask::mIsChecked = FALSE;
CONNECTWAY ConnectTimerTask::mConnectWay = CONNECTWAY_WIFI;
ScanResult* ConnectTimerTask::mScanResult = NULL;
AutoPtr<ICharSequence> ConnectTimerTask::mPassword = NULL;

ConnectTimerTask::ConnectTimerTask(
    /* [in]  */ CWifiSetting* host,
    /* [in]  */ CONNECTWAY way)
    : mHost(host)
    , mIsStarted(FALSE)
{
    mIsChecked = TRUE;
    mConnectWay = way;
}

ConnectTimerTask::ConnectTimerTask(
    /* [in]  */ CWifiSetting* host,
    /* [in]  */ CONNECTWAY way,
    /* [in]  */ ScanResult* res,
    /* [in]  */ ICharSequence* password)
    : mHost(host)
    , mIsStarted(FALSE)
{
    mIsChecked = TRUE;
    mConnectWay = way;

    mScanResult = res;
    mPassword = password;
}

void ConnectTimerTask::StatusChanged(
    /* [in]  */ CONNECTWAY way,
    /* [in] */ Boolean isChecked)
{
    mIsChecked = isChecked;
    mConnectWay = way;
}

ECode ConnectTimerTask::Run()
{
    if (mIsChecked) {
        if (mConnectWay == CONNECTWAY_WIFI) {
            if (!mIsStarted) {
                mIsStarted = TRUE;
                //Wifi Start.
                ElWifi_Start();
            }

            //Get Wifi nodes.
            std::vector<ScanResult*>* scanList = ElWifi_ScanResults();

            //Add Wifi hot points to list.
            AutoLock lock(mHost->mScanListLock);
            void (STDCALL CWifiSetting::*pHandlerFunc)(std::vector<ScanResult*>*);
            pHandlerFunc = &CWifiSetting::AddWifiNodesByVector;
            AutoPtr<IParcel> params;
            CCallbackParcel::New((IParcel**)&params);
            params->WriteInt32((Handle32)(Handle32*)scanList);

            mHost->mApartment->PostCppCallbackDelayed(
                (Handle32)mHost, *(Handle32*)&pHandlerFunc,
                params, 0, 0);
        }
        else if (mConnectWay == CONNECTWAY_ETH) {
            Boolean connected = Ethernet::RunDhcpEthe();

            void (STDCALL CWifiSetting::*pHandlerFunc)(Boolean);
            pHandlerFunc = &CWifiSetting::EthStateChanged;
            AutoPtr<IParcel> params;
            CCallbackParcel::New((IParcel**)&params);
            params->WriteBoolean(connected);

            mHost->mApartment->PostCppCallbackDelayed(
                (Handle32)mHost, *(Handle32*)&pHandlerFunc,
                params, 0, 0);

printf("============================= Start Ethernet : %d.\n", connected);
            if (connected) {
                Boolean c = FALSE;
                Cancel(&c);
            }
        }
    }
    else {
        if (mConnectWay == CONNECTWAY_WIFI) {
            //Wifi Stop.
            if (!ElWifi_Stop()) {
        printf("============================= stop wifi failed.\n");
            }
            else {
        printf("============================= stop wifi success.\n");
            }
        }
        else if (mConnectWay == CONNECTWAY_ETH) {
            Ethernet::StopDhcpEthe();
        }
    }

    return NOERROR;
}
