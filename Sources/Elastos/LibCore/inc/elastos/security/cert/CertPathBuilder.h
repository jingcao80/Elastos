
#ifndef __ELASTOS_SECURITY_CERT_CERTPATHBUILDER_H__
#define __ELASTOS_SECURITY_CERT_CERTPATHBUILDER_H__

#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * This class implements the functionality of a builder for an unverified
 * <i>Certification Path</i>s from a specified certificate to a trust anchor.
 */
class CertPathBuilder
    : public Object
    , public ICertPathBuilder
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ICertPathBuilder **builder);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ICertPathBuilder **builder);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider *provider,
        /* [out] */ ICertPathBuilder **builder);

    static CARAPI GetDefaultType(
        /* [out] */ String *type);

    CARAPI GetAlgorithm(
        /* [out] */ String *algorithm);

    CARAPI GetProvider(
        /* [out] */ IProvider **provider);

    CARAPI Build(
        /* [in] */ ICertPathParameters* params,
        /* [out] */ ICertPathBuilderResult** result);

protected:
    CertPathBuilder(
        /* [in] */ ICertPathBuilderSpi *builderSpi,
        /* [in] */ IProvider *provider,
        /* [in] */ const String& algorithm);

private:
    // Store CertPathBuilder service name
    static const String sSERVICE;

    //apache...todo
    // Used to access common engine functionality
    //static const Engine ENGINE = new Engine(SERVICE);

    // Store default property name
    static const String sPROPERTY_NAME;

    // Default value of CertPathBuilder type.
    static const String sDEFAULT_PROPERTY;

    // Store used provider
    const AutoPtr<IProvider> mProvider;

    // Store spi implementation
    const AutoPtr<ICertPathBuilderSpi> mSpiImpl;

    // Store algorithm name
    const String mAlgorithm;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTPATHBUILDER_H__
