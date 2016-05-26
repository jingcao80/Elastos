
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralName.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralName)
    , public Object
    , public IGeneralName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTag(
        /* [out] */ Int32 * pTag);

    CARAPI GetName(
        /* [out] */ IInterface ** ppName);

    CARAPI Equals(
        /* [in] */ IInterface * pOther,
        /* [out] */ Boolean * pEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pResult);

    CARAPI IsAcceptable(
        /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralName * pGname,
        /* [out] */ Boolean * pIsAcceptable);

    CARAPI GetAsList(
        /* [out] */ Elastos::Utility::IList ** ppList);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI GetEncodedName(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncodedName);

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IOtherName * pName);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IORAddress * pName1);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X501::IName * pName2);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IEDIPartyName * pName3);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pName4);

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte> * pName1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
