
#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETEMPERATURESENSOR_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETEMPERATURESENSOR_H__

#include "zigbee/ZigbeeDevice.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeTemperatureSensor : public ZigbeeDevice
{
public:
    ZigbeeTemperatureSensor(
        /* [in] */ const String& name,
        /* [in] */ Int32 status,
        /* [in] */ Int32 shortAddr,
        /* [in] */ Int64 macAddr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 zoneType);

    //short-term solution for history record display (360 recently records only - 3 hours[temperature sensor report every 30 seconds])
    CARAPI_(void) AddHistoryRecord(
        /* [in] */ IDate* date,
        /* [in] */ Int32 temperature);

    //short-term solution for history record display
    CARAPI_(Int32) GetHistoryMaxTemperature();

    CARAPI_(Int32) GetHistoryMinTemperature();

    CARAPI_(Int32) GetHistoryAvgTemperature();

    CARAPI_(String) GetHistoryMaxTemperatureTime();

    CARAPI_(String) GetHistoryMinTemperatureTime();

    CARAPI_(AutoPtr<List<String> >) GetHistoryTime();

    CARAPI_(AutoPtr<List<Int32> >) GetHistoryTemperature();

    CARAPI_(void) ResetLastAlert();

    // time format: yyyy-MM-dd-H-mm-ss
    CARAPI_(void) SetLastAlertTime(
        /* [in] */ const String& time);

    CARAPI_(String) GetLastAlertTime();

    CARAPI_(void) SetLastAlertTemperature(
        /* [in] */ Int32 temperature);

    CARAPI_(Int32) GetLastAlertTemperature();

    CARAPI_(void) SetUpperLimitWarningSetting(
        /* [in] */ Boolean setting);

    CARAPI_(void) SetLowerLimitWarningSetting(
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetUpperLimitWarningSetting();

    CARAPI_(Boolean) GetLowerLimitWarningSetting();

    CARAPI_(void) SetHistorySetting(
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetHistorySetting();

    // returns: 0 - success, 1 - fail
    CARAPI_(Int32) SetHistoryFileName(
        /* [in] */ const String& filename);

    CARAPI_(String) GetHistoryFileName();

    CARAPI_(void) SetWarningTemperatureLowerLimit(
        /* [in] */ Int32 temperature);

    CARAPI_(Int32) GetWarningTemperatureLowerLimit();

    CARAPI_(void) SetWarningTemperatureUpperLimit(
        /* [in] */ Int32 temperature);

    CARAPI_(Int32) GetWarningTemperatureUpperLimit();

    CARAPI_(Boolean) IsWarning(
        /* [in] */ Int32 temperature);

    CARAPI_(Boolean) IsUpperLimitWarning(
        /* [in] */ Int32 temperature);

    CARAPI_(Boolean) IsLowerLimitWarning(
        /* [in] */ Int32 temperature);

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

    Boolean mUpperLimitWarningEnable;
    Boolean mLowerLimitWarningEnable;
    Int32 mLowerLimitWarningTemperature;
    Int32 mUpperLimitWarningTemperature;
    String mWarningMethod;  //how to show the warning
    String mWarningMethod_Email;

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    String mLastAlertTime;
    Int32 mLastAlertTemperature;

    Boolean mHistoryRecordEnable;
    String mHistoryRecordFileName;
    //Int32 mHistoryRecordInterval;
    //Int32 mHistoryRecordDuration;

    //short-term solution for history record display
    AutoPtr<List<String> > mHistoryTime;    // format: yyyy-MM-dd-H-mm-ss
    AutoPtr<List<Int32> > mHistoryTemperature;
    String mHistoryRecordTime_Max;
    String mHistoryRecordTime_Min;
    Int32 mHistoryRecordTemperature_Max;
    Int32 mHistoryRecordTemperature_Min;
    Int32 mHistoryRecordTemperature_Avg;
};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETEMPERATURESENSOR_H__
