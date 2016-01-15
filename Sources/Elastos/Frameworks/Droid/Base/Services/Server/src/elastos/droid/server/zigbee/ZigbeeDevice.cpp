
#include "zigbee/ZigbeeDevice.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IZigbeeManager;
using Elastos::Droid::Os::CZigbeeManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Zigbee {

const String ZigbeeDevice::TAG("ZigbeeDevice");
const Boolean ZigbeeDevice::DBG = TRUE;
const String ZigbeeDevice::NOT_AVAILABLE = String("NA");
const String ZigbeeDevice::sFileNameSpliter = String("_");
const Int32 ZigbeeDevice::sIllegalFilenameCharLength;
const Char32 ZigbeeDevice::sIllegalFilenameChar[ZigbeeDevice::sIllegalFilenameCharLength]
    = {'\\', '/', ':', '*', '?', '\"', '<', '>', '|'};

ZigbeeDevice::ZigbeeDevice()
    : mName("init")
    , mOnlineStatus(-1)
    , mShortAddr(0)
    , mMacAddr(0)
    , mType(-1)
    , mZoneType(-1)
{
}

ZigbeeDevice::ZigbeeDevice(
    /* [in] */ const String& name,
    /* [in] */ Int32 status,
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int64 macAddr,
    /* [in] */ Int32 type,
    /* [in] */ Int32 zoneType,
    /* [in] */ const String& className)
    : mName(name)
    , mOnlineStatus(status)
    , mShortAddr(shortAddr)
    , mMacAddr(macAddr)
    , mZoneType(zoneType)
    , mClassName(className)
{
    SetType(type);
}

Int32 ZigbeeDevice::SetName(
    /* [in] */ const String& name)
{
    if (!IsValidName(name))
        return 1;

    mName = name;
    return 0;
}

String ZigbeeDevice::GetName()
{
    return mName;
}

Int32 ZigbeeDevice::SetType(
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

Int32 ZigbeeDevice::GetType()
{
    return mType;
}

Int32 ZigbeeDevice::SetZoneType(
    /* [in] */ Int32 zoneType)
{
    mZoneType = zoneType;
    return 0;
}

Int32 ZigbeeDevice::GetZoneType()
{
    return mZoneType;
}

Int32 ZigbeeDevice::SetOnlineStatus(
    /* [in] */ Int32 status)
{
    if (status == IZigbeeManager::STATUS_ONLINE || status == IZigbeeManager::STATUS_OFFLINE) {
        mOnlineStatus = status;
        return 0;
    }
    return 1;
}

Int32 ZigbeeDevice::GetOnlineStatus()
{
    return mOnlineStatus;
}

void ZigbeeDevice::SetShortAddr(
    /* [in] */ Int32 ShortAddr)
{
    mShortAddr = ShortAddr;
}

Int32 ZigbeeDevice::GetShortAddr()
{
    return mShortAddr;
}

void ZigbeeDevice::SetMacAddr(
    /* [in] */ Int64 macAddr)
{
    mMacAddr = macAddr;
}

Int64 ZigbeeDevice::GetMacAddr()
{
    return mMacAddr;
}

Boolean ZigbeeDevice::CompareShortAddr(
    /* [in] */ Int32 shortAddr)
{
    Boolean result = FALSE;
    if(mShortAddr == shortAddr){
        result = TRUE;
    }

    return result;
}

String ZigbeeDevice::GetClass()
{
    return mClassName;
}

Boolean ZigbeeDevice::IsValidName(
        /* [in] */ const String& name)
{
    for (Int32 i = 0; i < sIllegalFilenameCharLength; i++){
        if (name.IndexOf(sIllegalFilenameChar[i]) >= 0) {
            SLOGGERE(TAG, "IsValidName(%s): contains \"%c\" => illegal char", name.string(), (char)sIllegalFilenameChar[i]);
            return FALSE;
        }
    }

    return TRUE;
}

} // Zigbee
} // Server
} // Droid
} // Elastos
