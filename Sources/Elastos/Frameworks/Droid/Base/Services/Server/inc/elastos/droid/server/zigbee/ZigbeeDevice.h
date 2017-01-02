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

#ifndef __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDEVICE_H__
#define __ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDEVICE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

class ZigbeeDevice : public ElRefBase
{
public:
    ZigbeeDevice();

    ZigbeeDevice(
        /* [in] */ const String& name,
        /* [in] */ Int32 status,
        /* [in] */ Int32 shortAddr,
        /* [in] */ Int64 macAddr,
        /* [in] */ Int32 type,
        /* [in] */ Int32 zoneType,
        /* [in] */ const String& className = String("ZigbeeDevice"));

    Int32 SetName(
        /* [in] */ const String& name);

    String GetName();

    //return value: 0 -success, 1 - fail/illegal device type
    Int32 SetType(
        /* [in] */ Int32 type);

    Int32 GetType();

    //return value: 0 -success, 1 - fail/illegal zone type
    Int32 SetZoneType(
        /* [in] */ Int32 zoneType);

    Int32 GetZoneType();

    //return value: 0 -success, 1 - fail/illegal online status
    Int32 SetOnlineStatus(
        /* [in] */ Int32 status);

    Int32 GetOnlineStatus();

    void SetShortAddr(
        /* [in] */ Int32 ShortAddr);

    Int32 GetShortAddr();

    void SetMacAddr(
        /* [in] */ Int64 macAddr);

    Int64 GetMacAddr();

    // Compare short address
    // return value: true-the same, false-not the same
    Boolean CompareShortAddr(
        /* [in] */ Int32 shortAddr);

    String GetClass();

    static Boolean IsValidName(
        /* [in] */ const String& name);

private:
    static const String TAG;
    //debug
    static const Boolean DBG;

protected:
    static const String NOT_AVAILABLE;
    static const String sFileNameSpliter;
    static const Int32 sIllegalFilenameCharLength = 9;
    static const Char32 sIllegalFilenameChar[sIllegalFilenameCharLength];

private:
    String mName;           //device name
    Int32 mOnlineStatus;    //device online status
    Int32 mShortAddr;       //device short address
    Int64 mMacAddr;         //device MAC address
    Int32 mType;            //device type
    Int32 mZoneType;        //IAS zone type
    String mClassName;
};

} // Zigbee
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ZIGBEE_ZIGBEEDEVICE_H__
