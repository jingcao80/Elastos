
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CCryptoProvider.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CCryptoProvider)
    , public Object
    , public ICryptoProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ String * pStr);

    CARAPI GetPropertyEx(
        /* [in] */ const String& name,
        /* [in] */ const String& defaultValue,
        /* [out] */ String * pStr);

    CARAPI List(
        /* [in] */ Elastos::IO::IPrintStream * pOutstream);

    CARAPI ListEx(
        /* [in] */ Elastos::IO::IPrintWriter * pOutwriter);

    CARAPI Load(
        /* [in] */ Elastos::IO::IInputStream * pInstream);

    CARAPI LoadEx(
        /* [in] */ Elastos::IO::IReader * pInreader);

    CARAPI PropertyNames(
        /* [out] */ IInterface ** ppInter);

    CARAPI StringPropertyNames(
        /* [out] */ Elastos::Utility::IObjectStringMap ** ppStrmap);

    CARAPI Save(
        /* [in] */ Elastos::IO::IOutputStream * pOutstream,
        /* [in] */ const String& comment);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [out] */ IObject ** ppObj);

    CARAPI Store(
        /* [in] */ Elastos::IO::IOutputStream * pOutstream,
        /* [in] */ const String& comment);

    CARAPI StoreEx(
        /* [in] */ Elastos::IO::IWriter * pWriter,
        /* [in] */ const String& comment);

    CARAPI LoadFromXML(
        /* [in] */ Elastos::IO::IInputStream * pInstream);

    CARAPI StoreToXML(
        /* [in] */ Elastos::IO::IOutputStream * pOs,
        /* [in] */ const String& comment);

    CARAPI StoreToXMLEx(
        /* [in] */ Elastos::IO::IOutputStream * pOs,
        /* [in] */ const String& comment,
        /* [in] */ const String& encoding);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetVersion(
        /* [out] */ Double * pVersionNumber);

    CARAPI GetInfo(
        /* [out] */ String * pInfo);

    CARAPI GetService(
        /* [in] */ const String& type,
        /* [in] */ const String& algorithm,
        /* [out] */ Elastos::Security::IProviderService ** ppService);

    CARAPI GetServices(
        /* [out] */ Elastos::Utility::ISet ** ppServices);

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
