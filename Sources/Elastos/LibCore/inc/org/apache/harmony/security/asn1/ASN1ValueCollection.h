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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__

#include <elastos.h>
#include "ASN1Constructed.h"

_ELASTOS_NAMESPACE_USING

using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

/**
 * This abstract class represents ASN.1 type that is a collection of ASN.1 types.
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1ValueCollection
    : public ASN1Constructed
    , public IASN1ValueCollection
{
public:
    CAR_INTERFACE_DECL();

    ASN1ValueCollection();

    CARAPI constructor(
        /* [in] */ Int32 tagNumber,
        /* [in] */ IASN1Type* type);

    /**
     * Provides an object's values to be encoded
     *
     * Derived classes should override this method to provide
     * encoding for a selected class of objects.
     *
     * @param - an object to be encoded
     * @return - a collection of object's values to be encoded
     */
    virtual CARAPI GetValues(
        /* [in] */ IInterface* object,
        /* [out] */ ICollection** values);

    CARAPI SetType(
        /* [in] */ IASN1Type* type);

    CARAPI GetType(
        /* [out] */ IASN1Type** type);

public:
    /** A value collection of this ASN.1 type */
    AutoPtr<IASN1Type> mType;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1VALUECOLLECTION_H__
