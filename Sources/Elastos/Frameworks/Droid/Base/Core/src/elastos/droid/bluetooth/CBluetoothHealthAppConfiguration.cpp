
#include "elastos/droid/bluetooth/CBluetoothHealthAppConfiguration.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothHealthAppConfiguration, Object, IBluetoothHealthAppConfiguration, IParcelable);

CAR_OBJECT_IMPL(CBluetoothHealthAppConfiguration);

CBluetoothHealthAppConfiguration::CBluetoothHealthAppConfiguration()
    : mDataType(0)
    , mRole(0)
    , mChannelType(0)
{}

ECode CBluetoothHealthAppConfiguration::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal)
    AutoPtr<IBluetoothHealthAppConfiguration> config = IBluetoothHealthAppConfiguration::Probe(obj);
    if (config) {
        // config.getName() can never be NULL
        String name;
        config->GetName(&name);
        Int32 type, role, channelType;
        config->GetDataType(&type);
        config->GetRole(&role);
        config->GetChannelType(&channelType);
        *equal = mName.Equals(name) && mDataType == type && mRole == role && mChannelType == channelType;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    Int32 result = 17;
    result = 31 * result + (mName.IsNull() ? 0 : mName.GetHashCode());
    result = 31 * result + mDataType;
    result = 31 * result + mRole;
    result = 31 * result + mChannelType;
    return result;
}

ECode CBluetoothHealthAppConfiguration::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s)
    StringBuilder sb("BluetoothHealthAppConfiguration [mName = ");
    sb += mName;
    sb += ",mDataType = ";
    sb += mDataType;
    sb += ", mRole = ";
    sb += mRole;
    sb += ",mChannelType = ";
    sb += mChannelType;
    sb += "]";
    *s = sb.ToString();
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::GetDataType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mDataType;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::GetRole(
    /* [out] */ Int32* role)
{
    VALIDATE_NOT_NULL(role)
    *role = mRole;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::GetChannelType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mChannelType;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadInt32(&mDataType);
    source->ReadInt32(&mRole);
    source->ReadInt32(&mChannelType);
    return NOERROR;

}

ECode CBluetoothHealthAppConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteInt32(mDataType);
    dest->WriteInt32(mRole);
    dest->WriteInt32(mChannelType);
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::constructor()
{
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 dataType)
{
    mName = name;
    mDataType = dataType;
    mRole = IBluetoothHealth::SINK_ROLE;
    mChannelType = IBluetoothHealth::CHANNEL_TYPE_ANY;
    return NOERROR;
}

ECode CBluetoothHealthAppConfiguration::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 dataType,
    /* [in] */ Int32 role,
    /* [in] */ Int32 channelType)
{
    mName = name;
    mDataType = dataType;
    mRole = role;
    mChannelType = channelType;
    return NOERROR;
}

}
}
}

