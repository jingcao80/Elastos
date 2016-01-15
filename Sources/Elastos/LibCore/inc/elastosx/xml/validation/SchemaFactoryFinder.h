#ifndef __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYFINDER_H__
#define __ELASTOSX_XML_VALIDATION_SCHEMAFACTORYFINDER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IIterable;
using Org::Xml::Sax::IErrorHandler;
using Org::W3c::Dom::Ls::ILSResourceResolver;
using Elastos::Utility::IProperties;
using Elastos::IO::IInputStream;

namespace Elastosx {
namespace Xml {
namespace Validation {

class SchemaFactoryFinder
    : public Object
    , public ISchemaFactoryFinder
{
public:
    CAR_INTERFACE_DECL()

    SchemaFactoryFinder(
        /* [in] */ IClassLoader* loader);

    CARAPI NewFactory(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI CreateInstance(
        /* [in] */ const String& className,
        /* [out] */ ISchemaFactory** factory);

private:
    CARAPI _NewFactory(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI CreateServiceFileIterator(
        /* [out] */ IIterable** it);

    CARAPI LoadFromServicesFile(
        /* [in] */ String schemaLanguage,
        /* [in] */ String resourceName,
        /* [in] */ IInputStream* in,
        /* [out] */ ISchemaFactory** factory);

    CARAPI Which(
        /* [in] */ ClassID clazz,
        /* [out] */ String* str);

private:
    /**
     * <p>Cache properties for performance.</p>
     */
    static AutoPtr<IProperties> sCacheProps;

    /**
     * <p>First time requires initialization overhead.</p>
     */
    static Boolean sFirstTime;

    /**
     * Default columns per line.
     */
    static Int32 DEFAULT_LINE_LENGTH;

    static String SERVICE_CLASS;

    static const String W3C_XML_SCHEMA10_NS_URI; // = "http://www.w3.org/XML/XMLSchema/v1.0";
    static const String W3C_XML_SCHEMA11_NS_URI; // = "http://www.w3.org/XML/XMLSchema/v1.1";

    AutoPtr<IClassLoader> mClassLoader;
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif