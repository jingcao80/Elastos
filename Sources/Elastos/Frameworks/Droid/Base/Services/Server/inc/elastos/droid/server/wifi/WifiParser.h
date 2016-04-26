#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFIPARSER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFIPARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IBitSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
  * Describes information about a detected access point. In addition
  * to the attributes described here, the supplicant keeps track of
  * {@code quality}, {@code noise}, and {@code maxbitrate} attributes,
  * but does not currently report them to external clients.
  */
class WifiParser
    : public Object
{
public:
    /*
      * {@hide}
      */
    class IE : public Object
    {
    public:
        Int32 id;
        AutoPtr<ArrayOf<Byte> > data;
    };

public:
    WifiParser();

    //WFA WPA vendor IE OUI/type
    /*
      * parse beacon or probe response frame and build the capabilities
      * {@hide}
      *
      * This function is called so as to build the capabilities string of the scan result, hence it is called
      * by AutoJoin controller when handling scan results that are coming from WifiScanner.
      *
      * It parses the ieee beacon's capability field, WPA and RSNE IE as per spec, but build the
      * ScanResult.capabilities String in a way that mirror the values returned by wpa_supplicant.
      *
      * Once the capabilities string is build, the ScanResult can be used be the system as if it was coming from supplicant.
      */
    /* @hide
      * */
    static CARAPI_(String) Parse_akm(
        /* [in] */ ArrayOf<IE*>* full_IE,
        /* [in] */ IBitSet* ieee_cap);

private:
    static const Int32 VENDOR_SPECIFIC_IE = 221;
    static const Int32 IEEE_RSN_IE = 48;
    //IEEE 2012 8.4.2.27
    static const Int32 WPA_IE_VENDOR_TYPE = 0x0050f201;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFIPARSER_H__

