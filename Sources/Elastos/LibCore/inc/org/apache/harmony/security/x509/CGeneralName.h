
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralName.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Utility::IList;

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
        /* [out] */ Int32* pTag);

    CARAPI GetName(
        /* [out] */ IInterface** ppName);

    CARAPI Equals(
        /* [in] */ IInterface* pOther,
        /* [out] */ Boolean* pEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* pResult);

    CARAPI IsAcceptable(
        /* [in] */ IGeneralName* pGname,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI GetAsList(
        /* [out] */ IList** ppList);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI GetEncodedName(
        /* [out, callee] */ ArrayOf<Byte>** ppEncodedName);

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ IOtherName* pName);

    CARAPI constructor(
        /* [in] */ IORAddress* pName1);

    CARAPI constructor(
        /* [in] */ IName* pName2);

    CARAPI constructor(
        /* [in] */ IEDIPartyName* pName3);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pName4);

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte>* pName1);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
