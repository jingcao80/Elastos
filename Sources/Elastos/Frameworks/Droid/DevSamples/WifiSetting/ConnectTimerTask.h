
#ifndef __CONNECTTIMERTASK_H__
#define __CONNECTTIMERTASK_H__

#include <Elastos.Core.h>
#include <Elastos.Core.h>
#include <elastos/TimerTask.h>
#include <elautoptr.h>
#include "WifiFun.h"

class CWifiSetting;

enum CONNECTWAY {
    CONNECTWAY_WIFI,
    CONNECTWAY_ETH
};


class ConnectTimerTask : public TimerTask
{
public:
    ConnectTimerTask(
        /* [in]  */ CWifiSetting* host,
        /* [in]  */ CONNECTWAY way);

    ConnectTimerTask(
        /* [in]  */ CWifiSetting* host,
        /* [in]  */ CONNECTWAY way,
        /* [in]  */ ScanResult* res,
        /* [in]  */ ICharSequence* password);

    static CARAPI_(void) StatusChanged(
        /* [in]  */ CONNECTWAY way,
        /* [in] */ Boolean isChecked);

    CARAPI Run();

private:
    CWifiSetting*  mHost;
    Boolean mIsStarted;
    static Boolean mIsChecked;
    static CONNECTWAY mConnectWay;
    static ScanResult* mScanResult;
    static AutoPtr<ICharSequence> mPassword;
};

#endif //__CONNECTTIMERTASK_H__
