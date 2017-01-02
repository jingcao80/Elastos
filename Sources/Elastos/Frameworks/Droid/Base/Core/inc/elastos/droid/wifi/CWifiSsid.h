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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISSID_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISSID_H__

#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Wifi_CWifiSsid.h"
#include <elastos/core/Object.h>

using Elastos::IO::IByteArrayOutputStream;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiSsid)
    , public Object
    , public IWifiSsid
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetOctets(
        /* [out] */ IByteArrayOutputStream** result);

    CARAPI IsHidden(
        /* [out] */ Boolean* result);

    CARAPI GetOctets(
        /* [out, callee] */ ArrayOf<Byte>** octets);

    CARAPI GetHexString(
        /* [out] */ String* hexString);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    static CARAPI CreateFromAsciiEncoded(
        /* [in] */ const String& asciiEncoded,
        /* [out] */ IWifiSsid** wifiSsid);

    static CARAPI CreateFromHex(
        /* [in] */ const String& hexStr,
        /* [out] */ IWifiSsid** wifiSsid);

private:
    /* This function is equivalent to printf_decode() at src/utils/common.c in
     * the supplicant */
    CARAPI_(void) ConvertToBytes(
        /* [in] */ const String& asciiEncoded);

    CARAPI_(Boolean) IsArrayAllZeroes(
        /* [in] */ const ArrayOf<Byte>* ssidBytes);

private:
    AutoPtr<IByteArrayOutputStream> mOctets;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWIFISSID_H__
