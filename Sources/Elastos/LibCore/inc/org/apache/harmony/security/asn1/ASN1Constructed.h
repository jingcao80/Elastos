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

#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__

#include "ASN1Type.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


/**
 * This abstract class is the super class for all constructed ASN.1 types
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
class ASN1Constructed
    : public ASN1Type
{
public:
    /**
     * Tests provided identifier.
     *
     * @param identifier - identifier to be verified
     * @return - true if identifier correspond to constructed identifier of
     *           this ASN.1 type, otherwise false
     */
    CARAPI CheckTag(
        /* [in] */ Int32 identifier,
        /* [ou] */ Boolean* result);

    CARAPI EncodeASN(
        /* [in] */ IBerOutputStream* out);

protected:
    CARAPI constructor(
        /* [in] */ Int32 tagNumber);

    CARAPI constructor(
        /* [in] */ Int32 tagClass,
        /* [in] */ Int32 tagNumber);
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_ASN1CONSTRUCTED_H__
