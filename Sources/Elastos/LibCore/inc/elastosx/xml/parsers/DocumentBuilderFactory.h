#ifndef __ELASTOSX_XML_PARSERS_DOCUMENTBUILDERFACTORY_H__
#define __ELASTOSX_XML_PARSERS_DOCUMENTBUILDERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class DocumentBuilderFactory
    : public Object
    , public IDocumentBuilderFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

    CARAPI SetIgnoringElementContentWhitespace(
        /* [in] */ Boolean whitespace);

    CARAPI SetExpandEntityReferences(
        /* [in] */ Boolean expandEntityRef);

    CARAPI SetIgnoringComments(
        /* [in] */ Boolean ignoreComments);

    CARAPI SetCoalescing(
        /* [in] */ Boolean coalescing);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean* isAware);

    CARAPI IsValidating(
        /* [out] */ Boolean* isValidating);

    CARAPI IsIgnoringElementContentWhitespace(
        /* [out] */ Boolean* isIgnoringElementContentWhiteSpace);

    CARAPI IsExpandEntityReferences(
        /* [out] */ Boolean* isExpandEntityReferences);

    CARAPI IsIgnoringComments(
        /* [out] */ Boolean* comments);

    CARAPI IsCoalescing(
        /* [out] */ Boolean* isCoalescing);

    CARAPI GetSchema(
        /* [out] */ ISchema** schema);

    CARAPI SetSchema(
        /* [in] */ ISchema* schema);

    CARAPI SetXIncludeAware(
        /* [in] */ Boolean state);

    CARAPI IsXIncludeAware(
        /* [out] */ Boolean* isXIncludeAware);

    static CARAPI NewInstance(
        /* [out] */ IDocumentBuilderFactory** instance);

    static CARAPI NewInstance(
        /* [in] */ String factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ IDocumentBuilderFactory** instance);

protected:
    DocumentBuilderFactory();

private:
    Boolean mValidating;
    Boolean mNamespaceAware;
    Boolean mWhitespace;
    Boolean mExpandEntityRef;
    Boolean mIgnoreComments;
    Boolean mCoalescing;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif
