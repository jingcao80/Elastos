
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
