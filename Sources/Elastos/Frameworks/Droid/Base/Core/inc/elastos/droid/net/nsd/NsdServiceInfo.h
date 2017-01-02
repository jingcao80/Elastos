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

#ifndef __ELASTOS_DROID_NET_NSD_NSDSERVICEINFO_H__
#define __ELASTOS_DROID_NET_NSD_NSDSERVICEINFO_H__

#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArrayMap;

using Elastos::Net::IInetAddress;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * A class representing service information for network service discovery
 * {@see NsdManager}
 */
class NsdServiceInfo
    : public Object
    , public IParcelable
    , public INsdServiceInfo
{
public:
    CAR_INTERFACE_DECL()

    NsdServiceInfo();

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& sn,
        /* [in] */ const String& rt);

    /** Get the service name */
    CARAPI GetServiceName(
        /* [out] */ String* result);

    /** Set the service name */
    CARAPI SetServiceName(
        /* [in] */ const String& s);

    /** Get the service type */
    CARAPI GetServiceType(
        /* [out] */ String* result);

    /** Set the service type */
    CARAPI SetServiceType(
        /* [in] */ const String& s);

    /** Get the host address. The host address is valid for a resolved service. */
    CARAPI GetHost(
        /* [out] */ IInetAddress** result);

    /** Set the host address */
    CARAPI SetHost(
        /* [in] */ IInetAddress* s);

    /** Get port number. The port number is valid for a resolved service. */
    CARAPI GetPort(
        /* [out] */ Int32* result);

    /** Set port number */
    CARAPI SetPort(
        /* [in] */ Int32 p);

    /** @hide */
    CARAPI SetAttribute(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value);

    /**
     * Add a service attribute as a key/value pair.
     *
     * <p> Service attributes are included as DNS-SD TXT record pairs.
     *
     * <p> The key must be US-ASCII printable characters, excluding the '=' character.  Values may
     * be UTF-8 strings or null.  The total length of key + value must be less than 255 bytes.
     *
     * <p> Keys should be short, ideally no more than 9 characters, and unique per instance of
     * {@link NsdServiceInfo}.  Calling {@link #setAttribute} twice with the same key will overwrite
     * first value.
     */
    CARAPI SetAttribute(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /** Remove an attribute by key */
    CARAPI RemoveAttribute(
        /* [in] */ const String& key);

    /**
     * Retrive attributes as a map of String keys to byte[] values.
     *
     * <p> The returned map is unmodifiable; changes must be made through {@link #setAttribute} and
     * {@link #removeAttribute}.
     */
    CARAPI GetAttributes(
        /* [out] */ IMap** result);

    /** @hide */
    CARAPI GetTxtRecord(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    CARAPI GetTxtRecordSize(
        /* [out] */ Int32* result);

private:
    static const String TAG;

    String mServiceName;

    String mServiceType;

    AutoPtr<IArrayMap> mTxtRecord;

    AutoPtr<IInetAddress> mHost;

    Int32 mPort;
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_NSDSERVICEINFO_H__
