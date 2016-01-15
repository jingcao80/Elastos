
#include "elastos/droid/net/NetworkConfig.h"
#include "elastos/droid/net/Network.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkConfig, Object, INetworkConfig)

ECode NetworkConfig::constructor(
    /* [in] */ const String& init)
{
    AutoPtr<ArrayOf<String> > fragments;
    StringUtils::Split(init, String(","), (ArrayOf<String>**)&fragments);
    mName = (*fragments)[0].Trim().ToLowerCase();
    mType = StringUtils::ParseInt32((*fragments)[1]);
    mRadio = StringUtils::ParseInt32((*fragments)[2]);
    mPriority = StringUtils::ParseInt32((*fragments)[3]);
    mRestoreTime = StringUtils::ParseInt32((*fragments)[4]);
    mDependencyMet = (*fragments)[5].EqualsIgnoreCase("TRUE");
    return NOERROR;
}

ECode NetworkConfig::IsDefault(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mType == mRadio);
    return NOERROR;
}

ECode NetworkConfig::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mName;
    return NOERROR;
}

ECode NetworkConfig::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode NetworkConfig::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mType;
    return NOERROR;
}

ECode NetworkConfig::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode NetworkConfig::GetRadio(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRadio;
    return NOERROR;
}

ECode NetworkConfig::SetRadio(
    /* [in] */ Int32 radio)
{
    mRadio = radio;
    return NOERROR;
}


ECode NetworkConfig::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPriority;
    return NOERROR;
}

ECode NetworkConfig::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}


ECode NetworkConfig::GetDependencyMet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mDependencyMet;
    return NOERROR;
}

ECode NetworkConfig::SetDependencyMet(
    /* [in] */ Boolean dependencyMet)
{
    mDependencyMet = dependencyMet;
    return NOERROR;
}

ECode NetworkConfig::GetRestoreTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRestoreTime;
    return NOERROR;
}

    /**
     * indicates the default restoral timer in seconds
     * if the network is used as a special network feature
     * -1 indicates no restoration of default
     */
ECode NetworkConfig::SetRestoreTime(
    /* [in] */ Int32 restoreTime)
{
    mRestoreTime = restoreTime;
    return NOERROR;
}


} // namespace Net
} // namespace Droid
} // namespace Elastos
