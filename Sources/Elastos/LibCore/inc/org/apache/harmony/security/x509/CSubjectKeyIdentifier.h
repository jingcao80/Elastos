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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_X509_CSubjectKeyIdentifier.h"
#include "ExtensionValue.h"

using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CSubjectKeyIdentifier)
    , public ExtensionValue
    , public ISubjectKeyIdentifier
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI GetKeyIdentifier(
        /* [out, callee] */ ArrayOf<Byte>** ppKeyIdentifier);

    /**
     * Creates an object on the base of its encoded form.
     */
    static CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ ISubjectKeyIdentifier** result) /*throws IOException*/;

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pKeyIdentifier);

private:
    // the value of key identifier
    AutoPtr<ArrayOf<Byte> > mKeyIdentifier;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CSUBJECTKEYIDENTIFIER_H__
