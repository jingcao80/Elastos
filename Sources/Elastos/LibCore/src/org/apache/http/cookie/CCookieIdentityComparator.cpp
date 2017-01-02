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

#include "org/apache/http/cookie/CCookieIdentityComparator.h"

using Elastos::Core::EIID_IComparator;
using Elastos::IO::EIID_ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

const Int64 CCookieIdentityComparator::serialVersionUID;

CAR_INTERFACE_IMPL_2(CCookieIdentityComparator, Object, IComparator, ISerializable)

CAR_OBJECT_IMPL(CCookieIdentityComparator)

ECode CCookieIdentityComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICookie> c1 = ICookie::Probe(lhs);
    AutoPtr<ICookie> c2 = ICookie::Probe(rhs);
    String c1Name, c2Name;
    c1->GetName(&c1Name);
    c2->GetName(&c2Name);
    Int32 res = c1Name.Compare(c2Name);
    if (res == 0) {
        // do not differentiate empty and null domains
        String d1;
        c1->GetDomain(&d1);
        if (d1.IsNull()) {
            d1 = String("");
        }
        String d2;
        c2->GetDomain(&d2);
        if (d2.IsNull()) {
            d2 = String("");
        }
        res = d1.CompareIgnoreCase(d2);
    }
    *result = res;
    return NOERROR;
}

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
