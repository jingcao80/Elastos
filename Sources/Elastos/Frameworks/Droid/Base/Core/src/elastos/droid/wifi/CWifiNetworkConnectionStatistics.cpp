
#include "elastos/droid/wifi/CWifiNetworkConnectionStatistics.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Wifi {

const String CWifiNetworkConnectionStatistics::TAG("CWifiNetworkConnnectionStatistics");

CAR_INTERFACE_IMPL_2(CWifiNetworkConnectionStatistics, Object, IWifiNetworkConnectionStatistics, IParcelable)

CAR_OBJECT_IMPL(CWifiNetworkConnectionStatistics)

CWifiNetworkConnectionStatistics::CWifiNetworkConnectionStatistics()
    : mNumConnection(0)
    , mNumUsage(0)
{
}

ECode CWifiNetworkConnectionStatistics::constructor()
{
    return NOERROR;
}

ECode CWifiNetworkConnectionStatistics::constructor(
    /* [in] */ Int32 connection,
    /* [in] */ Int32 usage)
{
    mNumConnection = connection;
    mNumUsage = usage;
    return NOERROR;
}

ECode CWifiNetworkConnectionStatistics::constructor(
    /* [in] */ IWifiNetworkConnectionStatistics* source)
{
    source->GetNumConnection(&mNumConnection);
    return source->GetNumUsage(&mNumUsage);
}

ECode CWifiNetworkConnectionStatistics::GetNumConnection(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumConnection;
    return NOERROR;
}

ECode CWifiNetworkConnectionStatistics::SetNumConnection(
    /* [in] */ Int32 numConnection)
{
    mNumConnection = numConnection;
    return NOERROR;
}

ECode CWifiNetworkConnectionStatistics::GetNumUsage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumUsage;
    return NOERROR;
}

ECode CWifiNetworkConnectionStatistics::SetNumUsage(
    /* [in] */ Int32 numUsage)
{
    mNumUsage = numUsage;
    return NOERROR;
}

//@Override
ECode CWifiNetworkConnectionStatistics::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sbuf;
    sbuf.Append("c=");
    sbuf.Append(mNumConnection);
    sbuf.Append(" u=");
    sbuf.Append(mNumUsage);
    return sbuf.ToString(str);
}

/** Implement the Parcelable interface */
ECode CWifiNetworkConnectionStatistics::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface */
//@Override
ECode CWifiNetworkConnectionStatistics::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mNumConnection);
    return dest->WriteInt32(mNumUsage);
}

ECode CWifiNetworkConnectionStatistics::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiNetworkConnectionStatistics::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
