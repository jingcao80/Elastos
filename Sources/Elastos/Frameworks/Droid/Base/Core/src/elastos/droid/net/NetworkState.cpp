
#include "elastos/droid/net/NetworkState.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkState, Object, IParcelable, INetworkState)

ECode NetworkState::constructor(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities)
{
    return constructor(networkInfo, linkProperties, networkCapabilities, String(NULL), String(NULL));
}

ECode NetworkState::constructor(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ const String& subscriberId,
    /* [in] */ const String& networkId)
{
    mNetworkInfo = networkInfo;
    mLinkProperties = linkProperties;
    mNetworkCapabilities = networkCapabilities;
    mSubscriberId = subscriberId;
    mNetworkId = networkId;
    return NOERROR;
}

ECode NetworkState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mNetworkInfo = INetworkInfo::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&mLinkProperties);
    mLinkProperties = ILinkProperties::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&mNetworkCapabilities);
    mNetworkCapabilities = INetworkCapabilities::Probe(obj);
    source->ReadString(&mSubscriberId);
    source->ReadString(&mNetworkId);
    return NOERROR;
}

ECode NetworkState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mNetworkInfo);
    dest->WriteInterfacePtr(mLinkProperties);
    dest->WriteInterfacePtr(mNetworkCapabilities);
    dest->WriteString(mSubscriberId);
    dest->WriteString(mNetworkId);
    return NOERROR;
}

ECode NetworkState::GetNetworkInfo(
    /* [out] */ INetworkInfo** networkInfo)
{
    VALIDATE_NOT_NULL(networkInfo);

    *networkInfo = mNetworkInfo;
    REFCOUNT_ADD(*networkInfo);
    return NOERROR;
}

ECode NetworkState::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);

    *linkProperties = mLinkProperties;
    REFCOUNT_ADD(*linkProperties);
    return NOERROR;
}

ECode NetworkState::GetNetworkCapabilities(
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetworkCapabilities;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NetworkState::GetSubscriberId(
    /* [out] */ String* subscriberId)
{
    VALIDATE_NOT_NULL(subscriberId);

    *subscriberId = mSubscriberId;
    return NOERROR;
}

ECode NetworkState::GetNetworkId(
    /* [out] */ String* networkId)
{
    VALIDATE_NOT_NULL(networkId);

    *networkId = mNetworkId;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
