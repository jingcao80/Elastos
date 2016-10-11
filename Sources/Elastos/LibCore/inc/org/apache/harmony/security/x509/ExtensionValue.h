
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_EXTENSIONVALUE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_EXTENSIONVALUE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

class ExtensionValue
    : public Object
    , public IExtensionValue
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

protected:
    /** Encoded form of the extension. */
    AutoPtr<ArrayOf<Byte> > mEncoding;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_EXTENSIONVALUE_H__
