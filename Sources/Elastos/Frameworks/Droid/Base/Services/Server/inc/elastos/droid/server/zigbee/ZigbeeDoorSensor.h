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

#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDOORSENSOR_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDOORSENSOR_H__

#include "zigbee/ZigbeeDevice.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeDoorSensor : public ZigbeeDevice
{
public:
    ZigbeeDoorSensor(
        /* [in] */ const String& name,
        /* [in] */ Int32 status,
        /* [in] */ Int32 shortAddr,
        /* [in] */ Int64 macAddr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 zoneType);

    //short-term solution for history record display (5 recently records only)
    CARAPI_(void) AddHistoryRecord(
        /* [in] */ IDate* date,
        /* [in] */ Int32 doorState);

    //short-term solution for history record display
    CARAPI_(AutoPtr<List<String> >) GetHistoryTime();

    CARAPI_(AutoPtr<List<Int32> >) GetHistoryDoorState();

    CARAPI_(Boolean) IsDoorWarning(
        /* [in] */ Int32 doorState);

    CARAPI_(void) ResetLastAlert();

    // time format: yyyy-MM-dd-H-mm-ss
    CARAPI_(void) SetLastAlertTime(
        /* [in] */ const String& time);

    CARAPI_(String) GetLastAlertTime();

    CARAPI_(void) SetLastAlertState(
        /* [in] */ Int32 state);

    CARAPI_(Int32) GetLastAlertState();

    CARAPI_(void) SetWarningState(
        /* [in] */ Int32 warningState);

    CARAPI_(Int32) GetWarningState();

    CARAPI_(void) SetWarningSetting(
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetWarningSetting();

    CARAPI_(void) SetHistorySetting(
        /* [in] */ Boolean setting);

    CARAPI_(Boolean) GetHistorySetting();

    // returns: 0 - success, 1 - fail
    CARAPI_(Int32) SetHistoryFileName(
        /* [in] */ const String& filename);

    CARAPI_(String) GetHistoryFileName();

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

    Boolean mWarningEnable;
    Int32 mWarningState;  //warning if door open or close
    Boolean mHistoryRecordEnable;
    String mHistoryRecordFileName;

    /* [short-term solution]for remote client warning , due to remote client can't get callback warning message */
    String mLastAlertTime;
    Int32 mLastAlertState;

    //short-term solution for history record display
    AutoPtr<List<String> > mHistoryTime;
    AutoPtr<List<Int32> > mHistoryDoorState;
};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDOORSENSOR_H__
