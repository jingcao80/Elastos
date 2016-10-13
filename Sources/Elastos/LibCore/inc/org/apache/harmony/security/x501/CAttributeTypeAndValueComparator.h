
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeTypeAndValueComparator.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;
using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Org::Apache::Harmony::Security::Utils::IObjectIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

/**
 * AttributeTypeAndValue comparator
 *
 */
CarClass(CAttributeTypeAndValueComparator)
    , public Object
    , public IComparator
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * compares two AttributeTypeAndValues
     *
     * @param atav1
     *            first AttributeTypeAndValue
     * @param atav2
     *            second AttributeTypeAndValue
     * @return -1 of first AttributeTypeAndValue "less" than second
     *         AttributeTypeAndValue 1 otherwise, 0 if they are equal
     */
    CARAPI Compare(
        /* [in] */ IInterface * atav1,
        /* [in] */ IInterface * atav2,
        /* [out] */ Int32 * result);

private:
    /**
     * compares two Object identifiers
     *
     * @param oid1
     *            first OID
     * @param oid2
     *            second OID
     * @return -1 of first OID "less" than second OID 1 otherwise, 0 if they are
     *         equal
     */
    static CARAPI_(Int32) CompateOids(
        /* [in] */ IObjectIdentifier* oid1,
        /* [in] */ IObjectIdentifier* oid2);

private:
    // private static final long serialVersionUID = -1286471842007103132L;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__
