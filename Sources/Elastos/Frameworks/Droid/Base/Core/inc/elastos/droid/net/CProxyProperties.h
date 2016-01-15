
#ifndef __ELASTOS_DROID_NET_CPROXYPROPERTIES_H__
#define __ELASTOS_DROID_NET_CPROXYPROPERTIES_H__

#include "_Elastos_Droid_Net_CProxyProperties.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Net::IUri;

using Elastos::Core::CString;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Net::IInetSocketAddress;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CProxyProperties)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& exclList);

    CARAPI constructor(
       /* [in] */ IProxyProperties* source);

    CARAPI GetSocketAddress(
        /* [out] */ IInetSocketAddress** address);

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI GetExclusionList(
        /* [out] */ String* exList);

    CARAPI GetParsedExclusionList(
        /*[out, callee]*/ ArrayOf<String>** parsedExclusionList);

    CARAPI IsExcluded(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI MakeProxy(
        /* [out] */ Elastos::Net::IProxy** proxy);

    //@Override
    CARAPI ToString(
        /* [out] */ String* output);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IProxyProperties* o,
        /* [out] */ Boolean* result);
    /*
    * generate hashcode based on significant fields
    */
    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);
    /**
    * Implement the Parcelable interface.
    * @hide
    */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    String mHost;
    Int32 mPort;
    String mExclusionList;
    AutoPtr< ArrayOf<String> > mParsedExclusionList;

    CARAPI_(void) SetExclusionList(
        /* [in] */ const String& exclusionList);
};

} //namespace Net
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_NET_CPROXYPROPERTIES_H__
