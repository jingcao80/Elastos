//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEHUMIDITYSENSOR_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEHUMIDITYSENSOR_H__

#include "zigbee/ZigbeeDevice.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeHumiditySensor : public ZigbeeDevice
{
public:
    ZigbeeHumiditySensor(
        /* [in] */ const String& name,
        /* [in] */ Int32 status,
        /* [in] */ Int32 shortAddr,
        /* [in] */ Int64 macAddr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 zoneType);

    //short-term solution for history record display (360 recently records only - 3 hours[humidity sensor report every 30 seconds])
    CARAPI_(void) AddHistoryRecord(
        /* [in] */ IDate* date,
        /* [in] */ Int32 humidity);

    //short-term solution for history record display
    CARAPI_(Int32) GetHistoryMaxHumidity();

    CARAPI_(Int32) GetHistoryMinHumidity();

    CARAPI_(Int32) GetHistoryAvgHumidity();

    CARAPI_(String) GetHistoryMaxHumidityTime();

    CARAPI_(String) GetHistoryMinHumidityTime();

    CARAPI_(AutoPtr<List<String> >) GetHistoryTime();

    CARAPI_(AutoPtr<List<Int32> >) GetHistoryHumidity();

    CARAPI_(void) ResetLastAlert();

    // time format: yyyy-MM-dd-H-mm-ss
    CARAPI_(void) SetLastAlertTime(
        /* [in] */ const String& time);

    CARAPI_(String) GetLastAlertTime();

    CARAPI_(void) SetLastAlertHumidity(
        /* [in] */ Int32 humidity);

    CARAPI_(Int32) GetLastAlertHumidity();

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

    CARAPI_(void) SetWarningHumidityLowerLimit(
        /* [in] */ Int32 humidity);

    CARAPI_(Int32) GetWarningHumidityLowerLimit();

    CARAPI_(void) SetWarningHumidityUpperLimit(
        /* [in] */ Int32 humidity);

    CARAPI_(Int32) GetWarningHumidityUpperLimit();

    CARAPI_(Boolean) IsWarning(
        /* [in] */ Int32 humidity);

    CARAPI_(Boolean) IsUpperLimitWarning(
        /* [in] */ Int32 humidity);

    CARAPI_(Boolean) IsLowerLimitWarning(
        /* [in] */ Int32 humidity);

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

    Boolean mUpperLimitWarningEnable;
    Boolean mLowerLimitWarningEnable;
    Int32 mLowerLimitWarningHumidity;
    Int32 mUpperLimitWarningHumidity;
    String mWarningMethod;  //how to show the warning
    String mWarningMethod_Email;

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    String mLastAlertTime;
    Int32 mLastAlertHumidity;

    Boolean mHistoryRecordEnable;
    String mHistoryRecordFileName;
    //Int32 mHistoryRecordInterval;
    //Int32 mHistoryRecordDuration;

    //short-term solution for history record display
    AutoPtr<List<String> > mHistoryTime;    // format: yyyy-MM-dd-H-mm-ss
    AutoPtr<List<Int32> > mHistoryHumidity;
    String mHistoryRecordTime_Max;
    String mHistoryRecordTime_Min;
    Int32 mHistoryRecordHumidity_Max;
    Int32 mHistoryRecordHumidity_Min;
    Int32 mHistoryRecordHumidity_Avg;

};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEHUMIDITYSENSOR_H__
