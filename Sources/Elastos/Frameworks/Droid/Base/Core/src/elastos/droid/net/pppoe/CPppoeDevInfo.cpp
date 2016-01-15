
#include "CPppoeDevInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

ECode CPppoeDevInfo::SetIfName(
    /* [in] */ const String& ifname)
{
    mDev_name = ifname;
    return NOERROR;
}

ECode CPppoeDevInfo::GetIfName(
    /* [out] */ String* ifname)
{
    VALIDATE_NOT_NULL(ifname);
    *ifname = mDev_name;
    return NOERROR;
}

ECode CPppoeDevInfo::SetIpAddress(
    /* [in] */ const String& ip)
{
    mIpaddr = ip;
    return NOERROR;
}

ECode CPppoeDevInfo::GetIpAddress(
    /* [out] */ String* ip)
{
    VALIDATE_NOT_NULL(ip);
    *ip = mIpaddr;
    return NOERROR;
}

ECode CPppoeDevInfo::SetNetMask(
    /* [in] */ const String& ip)
{
    mNetmask = ip;
    return NOERROR;
}

ECode CPppoeDevInfo::GetNetMask(
    /* [out] */ String* netMask)
{
    VALIDATE_NOT_NULL(netMask);
    *netMask = mNetmask;
    return NOERROR;
}

ECode CPppoeDevInfo::SetRouteAddr(
    /* [in] */ const String& route)
{
    mRoute = route;
    return NOERROR;
}

ECode CPppoeDevInfo::GetRouteAddr(
    /* [out] */ String* route)
{
    VALIDATE_NOT_NULL(route);
    *route = mRoute;
    return NOERROR;
}

ECode CPppoeDevInfo::SetDnsAddr(
    /* [in] */ const String& dns)
{
    mDns = dns;
    return NOERROR;
}

ECode CPppoeDevInfo::GetDnsAddr(
    /* [out] */ String* dns)
{
    VALIDATE_NOT_NULL(dns);
    *dns = mDns;
    return NOERROR;
}

ECode CPppoeDevInfo::SetConnectMode(
    /* [in] */ const String& mode,
    /* [out] */ Boolean* result)
{
    if (mMode.Equals(IPppoeDevInfo::PPPOE_CONN_MODE_DHCP) || mMode.Equals(IPppoeDevInfo::PPPOE_CONN_MODE_MANUAL)) {
        mMode = mode;
        *result = TRUE;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPppoeDevInfo::GetConnectMode(
    /* [out] */ String* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mMode;
    return NOERROR;
}

ECode CPppoeDevInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeDevInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mDev_name);
    dest->WriteString(mIpaddr);
    dest->WriteString(mNetmask);
    dest->WriteString(mRoute);
    dest->WriteString(mDns);
    dest->WriteString(mMode);
    return NOERROR;
}

ECode CPppoeDevInfo::constructor()
{
    mDev_name = NULL;
    mIpaddr = NULL;
    mNetmask = NULL;
    mRoute = NULL;
    mDns = NULL;
    mMode = IPppoeDevInfo::PPPOE_CONN_MODE_DHCP;
    return NOERROR;
}

} // Pppoe
} // Net
} // Droid
} // Elastos

