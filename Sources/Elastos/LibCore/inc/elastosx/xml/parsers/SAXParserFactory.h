#ifndef __ELASTOSX_XML_PARSERS_SAXPARSERFACTORY_H__
#define __ELASTOSX_XML_PARSERS_SAXPARSERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastosx::Xml::Validation::ISchema;

namespace Elastosx {
namespace Xml {
namespace Parsers {

class SAXParserFactory
    : public Object
    , public ISAXParserFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetNamespaceAware(
        /* [in]*/ Boolean awareness);

    CARAPI SetValidating(
        /* [in]*/ Boolean validating);

    CARAPI IsNamespaceAware(
        /* [out]*/ Boolean* isAware);

    CARAPI IsValidating(
        /* [out]*/ Boolean* isValidating);

    CARAPI GetSchema(
        /* [out]*/ ISchema** schema);

    CARAPI SetSchema(
        /* [in]*/ ISchema* schema);

    CARAPI SetXIncludeAware(
        /* [in]*/ Boolean state);

    CARAPI IsXIncludeAware(
        /* [out]*/ Boolean* isAware);

    static CARAPI NewInstance(
        /* [out]*/ ISAXParserFactory** instance);

    static CARAPI NewInstance(
        /* [in]*/ const String& factoryClassName,
        /* [in]*/ IClassLoader* classLoader,
        /* [out]*/ ISAXParserFactory** instance);

protected:
    SAXParserFactory();

private:
    Boolean mValidating;
    Boolean mNamespaceAware;
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
#endif
