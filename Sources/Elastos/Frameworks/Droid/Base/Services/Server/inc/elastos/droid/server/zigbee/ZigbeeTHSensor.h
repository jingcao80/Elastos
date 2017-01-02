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

#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETHSENSOR_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETHSENSOR_H__

#include "zigbee/ZigbeeTemperatureSensor.h"
#include "zigbee/ZigbeeHumiditySensor.h"

using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeTHSensor : public ZigbeeDevice
{
public:
    ZigbeeTHSensor(
        /* [in] */ const String& name,
        /* [in] */ Int32 status,
        /* [in] */ Int32 shortAddr,
        /* [in] */ Int64 macAddr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 zoneType);

    CARAPI_(AutoPtr<ZigbeeTemperatureSensor>) GetTemperatureSensor();

    CARAPI_(AutoPtr<ZigbeeHumiditySensor>) GetHumiditySensor();

    CARAPI_(void) AddHistoryRecord(
        /* [in] */ Int32 target,
        /* [in] */ IDate* date,
        /* [in] */ Int32 data);

    //short-term solution for history record display
    CARAPI_(Int32) GetHistoryMaxData(
        /* [in] */ Int32 target);

    CARAPI_(Int32) GetHistoryMinData(
        /* [in] */ Int32 target);

    CARAPI_(Int32) GetHistoryAvgData(
        /* [in] */ Int32 target);

    CARAPI_(String) GetHistoryMaxDataTime(
        /* [in] */ Int32 target);

    CARAPI_(String) GetHistoryMinDataTime(
        /* [in] */ Int32 target);

    CARAPI_(AutoPtr<List<String> >) GetHistoryTime(
        /* [in] */ Int32 target);

    CARAPI_(AutoPtr<List<Int32> >) GetHistoryData(
        /* [in] */ Int32 target);

    // time format: yyyy-MM-dd-H-mm-ss
    CARAPI_(void) SetLastAlertTime(
        /* [in] */ Int32 target,
        /* [in] */ const String& time);

    CARAPI_(String) GetLastAlertTime(
        /* [in] */ Int32 target);

    CARAPI_(void) SetLastAlertData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

    CARAPI_(Int32) GetLastAlertData(
        /* [in] */ Int32 target);

    CARAPI_(void) SetUpperLimitWarningSetting(
        /* [in] */ Int32 target,
        /* [in] */ Boolean setting);

    CARAPI_(void) SetLowerLimitWarningSetting(
        /* [in] */ Int32 target,
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetUpperLimitWarningSetting(
        /* [in] */ Int32 target);

    CARAPI_(Boolean) GetLowerLimitWarningSetting(
        /* [in] */ Int32 target);

    CARAPI_(void) SetHistorySetting(
        /* [in] */ Int32 target,
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetHistorySetting(
        /* [in] */ Int32 target);

    // returns: 0 - success, 1 - fail
    CARAPI_(Int32) SetHistoryFileName(
        /* [in] */ Int32 target,
        /* [in] */ const String& filename);

    CARAPI_(String) GetHistoryFileName(
        /* [in] */ Int32 target);

    CARAPI_(void) SetWarningDataLowerLimit(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

    CARAPI_(Int32) GetWarningDataLowerLimit(
        /* [in] */ Int32 target);

    CARAPI_(void) SetWarningDataUpperLimit(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

    CARAPI_(Int32) GetWarningDataUpperLimit(
        /* [in] */ Int32 target);

    CARAPI_(Boolean) IsWarning(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

    CARAPI_(Boolean) IsUpperLimitWarning(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

    CARAPI_(Boolean) IsLowerLimitWarning(
        /* [in] */ Int32 target,
        /* [in] */ Int32 data);

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

    AutoPtr<ZigbeeTemperatureSensor> mTemperatureSensor;
    AutoPtr<ZigbeeHumiditySensor> mHumiditySensor;
};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEETHSENSOR_H__
