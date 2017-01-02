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
