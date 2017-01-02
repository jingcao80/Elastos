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

#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEGROUP_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEGROUP_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeGroup : public ElRefBase
{
public:
    ZigbeeGroup(
        /* [in] */ const String& groupName,
        /* [in] */ Int32 groupType,
        /* [in] */ Int32 groupZoneType);

    ZigbeeGroup(
        /* [in] */ const String& groupName,
        /* [in] */ Int32 groupType,
        /* [in] */ Int32 groupZoneType,
        /* [in] */ List<String>* deviceList);

    CARAPI_(Int32) SetName(
        /* [in] */ const String& name);

    CARAPI_(String) GetName();

    //return value: 0 -success, 1 - fail/illegal device type
    CARAPI_(Int32) SetType(
        /* [in] */ Int32 type);

    CARAPI_(Int32) GetType();

    //Returns: 0 -success, 1 - fail/illegal group zone type
    CARAPI_(Int32) SetZoneType(
        /* [in] */ Int32 zoneType);

    CARAPI_(Int32) GetZoneType();

    /*
     *  Change device name in the group
     *  Parameters: old device name, new device name
     *  Retruns: void
    */
    CARAPI_(void) SetDeviceName(
        /* [in] */ const String& oldName,
        /* [in] */ const String& newName);

    CARAPI_(void) AddDevice(
        /* [in] */ const String& deviceName);

    CARAPI_(void) RemoveDevice(
        /* [in] */ const String& deviceName);

    CARAPI_(Int32) GetDeviceNumber();

    CARAPI_(void) RemoveAllDevices();

    CARAPI_(AutoPtr<List<String> >) GetAllDevices();

    //returns: true - device is in the group, false - device is not in the group
    CARAPI_(Boolean) IsContainDevice(
        /* [in] */ const String& deviceName);

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

    String mName;   //group name
    Int32 mType;  //group type
    Int32 mZoneType;  //group zone type
    AutoPtr<List<String> > mDeviceNames;   //devices' name in the group
};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEGROUP_H__
