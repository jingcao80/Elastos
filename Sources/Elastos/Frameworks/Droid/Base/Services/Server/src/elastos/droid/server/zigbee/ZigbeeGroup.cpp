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

#include "zigbee/ZigbeeGroup.h"
#include "zigbee/ZigbeeDevice.h"
#include <elastos/etl_algo.h>

using Elastos::Utility::Find;
using Elastos::Droid::Os::IZigbeeManager;
using Elastos::Droid::Os::CZigbeeManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeGroup::TAG("ZigbeeGroup");
const Boolean ZigbeeGroup::DBG = TRUE;

ZigbeeGroup::ZigbeeGroup(
    /* [in] */ const String& groupName,
    /* [in] */ Int32 groupType,
    /* [in] */ Int32 groupZoneType)
{
    mName = groupName;
    mType = groupType;
    mZoneType = groupZoneType;
    mDeviceNames = new List<String>();
}

ZigbeeGroup::ZigbeeGroup(
    /* [in] */ const String& groupName,
    /* [in] */ Int32 groupType,
    /* [in] */ Int32 groupZoneType,
    /* [in] */ List<String>* deviceList)
{
    mName = groupName;
    mType = groupType;
    mZoneType = groupZoneType;
    mDeviceNames = deviceList;
}

Int32 ZigbeeGroup::SetName(
    /* [in] */ const String& name)
{
    if (!ZigbeeDevice::IsValidName(name))
        return 1;

    mName = name;
    return 0;
}

String ZigbeeGroup::GetName()
{
    return mName;
}

//return value: 0 -success, 1 - fail/illegal device type
Int32 ZigbeeGroup::SetType(
    /* [in] */ Int32 type)
{
    AutoPtr<IZigbeeManager> zm;
    CZigbeeManager::AcquireSingleton((IZigbeeManager**)&zm);
    Boolean isValidType;
    if (zm->IsValidType(type, &isValidType), isValidType) {
        mType = type;
        return 0;
    }
    return 1;
}

Int32 ZigbeeGroup::GetType()
{
    return mType;
}

//Returns: 0 -success, 1 - fail/illegal group zone type
Int32 ZigbeeGroup::SetZoneType(
    /* [in] */ Int32 zoneType)
{
    mZoneType = zoneType;
    return 0;
}

Int32 ZigbeeGroup::GetZoneType()
{
    return mZoneType;
}

/*
 *  Change device name in the group
 *  Parameters: old device name, new device name
 *  Retruns: void
*/
void ZigbeeGroup::SetDeviceName(
    /* [in] */ const String& oldName,
    /* [in] */ const String& newName)
{
    List<String>::Iterator find = Find(mDeviceNames->Begin(), mDeviceNames->End(), oldName);
    if (find != mDeviceNames->End()) {
        *find = newName;
    }
}

void ZigbeeGroup::AddDevice(
    /* [in] */ const String& deviceName)
{
    if(!IsContainDevice(deviceName)){
        mDeviceNames->PushBack(deviceName);
    }
}

void ZigbeeGroup::RemoveDevice(
    /* [in] */ const String& deviceName)
{
    mDeviceNames->Remove(deviceName);
}

Int32 ZigbeeGroup::GetDeviceNumber()
{
    return mDeviceNames->GetSize();
}

void ZigbeeGroup::RemoveAllDevices()
{
    mDeviceNames->Clear();
}

AutoPtr<List<String> > ZigbeeGroup::GetAllDevices()
{
    return mDeviceNames;
}

//returns: true - device is in the group, false - device is not in the group
Boolean ZigbeeGroup::IsContainDevice(
    /* [in] */ const String& deviceName)
{
    if (Find(mDeviceNames->Begin(), mDeviceNames->End(), deviceName) != mDeviceNames->End()) {
        return TRUE;
    }
    else{
        return FALSE;
    }
}

} // Zigbee
} // Server
} // Droid
} // Elastos
