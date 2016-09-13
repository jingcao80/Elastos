
#include "security/cert/CCollectionCertStoreParameters.h"
#include "core/StringBuilder.h"
#include "Collections.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Collections;

namespace Elastos {
namespace Security {
namespace Cert {

const AutoPtr<ICollection> CCollectionCertStoreParameters::mDefaultCollection
        = ICollection::Probe(Collections::EMPTY_SET);

CAR_OBJECT_IMPL(CCollectionCertStoreParameters)
CAR_INTERFACE_IMPL_3(CCollectionCertStoreParameters, Object, \
    ICollectionCertStoreParameters, ICertStoreParameters, ICloneable)

ECode CCollectionCertStoreParameters::Clone(
    /* [out] */ IInterface **object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ICollectionCertStoreParameters> obj;
    FAIL_RETURN(CCollectionCertStoreParameters::New((ICollectionCertStoreParameters**)&obj))
    *object = obj.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CCollectionCertStoreParameters::GetCollection(
    /* [out] */ ICollection **collection)
{
    VALIDATE_NOT_NULL(collection)
    *collection = mCollection;
    REFCOUNT_ADD(*collection)
    return NOERROR;
}

ECode CCollectionCertStoreParameters::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("CollectionCertStoreParameters: [\ncollection: ");
    AutoPtr<ICollection> coll;
    GetCollection((ICollection**)&coll);
    String strCol = Object::ToString(coll);
    sb.Append(strCol);
    sb.Append("\n]");
    return sb.ToString(str);
}

ECode CCollectionCertStoreParameters::constructor()
{
    mCollection = mDefaultCollection;
    return NOERROR;
}

ECode CCollectionCertStoreParameters::constructor(
    /* [in] */ ICollection *collection)
{
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCollection = collection;
    return NOERROR;
}

}
}
}
