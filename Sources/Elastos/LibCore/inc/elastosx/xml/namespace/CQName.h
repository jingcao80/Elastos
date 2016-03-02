
#ifndef __ELASTOSX_XML_NAMESPACE_CQNAME_H__
#define __ELASTOSX_XML_NAMESPACE_CQNAME_H__

#include "Object.h"
#include "_Elastosx_Xml_Namespace_CQName.h"

using Elastos::Core::Object;

namespace Elastosx {
namespace Xml {
namespace Namespace {

CarClass(CQName)
    , public Object
    , public IQName
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetNamespaceURI(
        /* [out] */ String * pUri);

    CARAPI GetLocalPart(
        /* [out] */ String * pPart);

    CARAPI GetPrefix(
        /* [out] */ String * pPrefix);

    CARAPI Equals(
        /* [in] */ IInterface * pObjectToTest,
        /* [out] */ Boolean * pIsEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI constructor(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localPart);

    CARAPI constructor(
        /* [in] */ const String& namespaceURI,
        /* [in] */ const String& localPart,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ const String& localPart);

    static CARAPI ValueOf(
        /* [in] */ const String& qNameAsString,
        /* [out] */  IQName** qName);
private:

    /**
     * <p>Namespace URI of this <code>QName</code>.</p>
     */
    String mNamespaceURI;

    /**
     * <p>local part of this <code>QName</code>.</p>
     */
    String mLocalPart;

    /**
     * <p>prefix of this <code>QName</code>.</p>
     */
    String mPrefix;

    /**
     * <p><code>String</code> representation of this <code>QName</code>.</p>
     */
    String mQNameAsString;
};

}
}
}

#endif // __CQNAME_H__
