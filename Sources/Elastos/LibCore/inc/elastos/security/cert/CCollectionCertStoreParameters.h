
#ifndef __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__

#include "_Elastos_Security_Cert_CCollectionCertStoreParameters.h"

using Elastos::Utility::ICollection;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CCollectionCertStoreParameters)
    : public Object
    , public ICollectionCertStoreParameters
    , public ICertStoreParameters
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface **object);

    CARAPI GetCollection(
        /* [out] */ ICollection **collection);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICollection *collection);

private:
    // Default empty and immutable collection.
    // Used if <code>CollectionCertStoreParameters</code>instance
    // created by the no arg constructor
    static const AutoPtr<ICollection> mDefaultCollection;
    // A <code>Collection</code> of <code>Certificate</code>s
    // and <code>CRL</code>s
    const AutoPtr<ICollection> mCollection;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CCOLLECTIONCERTSTOREPARAMETERS_H__
