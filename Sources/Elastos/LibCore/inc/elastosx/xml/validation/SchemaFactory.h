#ifndef __ELASTOSX_XML_VALIDATION_SCHEMAFACTORY_H__
#define __ELASTOSX_XML_VALIDATION_SCHEMAFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::IO::IFile;
using Elastos::Net::IURL;
using Org::Xml::Sax::IErrorHandler;
using Org::W3c::Dom::Ls::ILSResourceResolver;
using Elastosx::Xml::Transform::ISource;

namespace Elastosx {
namespace Xml {
namespace Validation {

class SchemaFactory
    : public Object
    , public ISchemaFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* feature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* object);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** property);

    CARAPI NewSchema(
        /* [in] */ ISource* schema,
        /* [out] */ ISchema** instance);

    CARAPI NewSchema(
        /* [in] */ IFile* schema,
        /* [out] */ ISchema** instance);

    CARAPI NewSchema(
        /* [in] */ IURL* schema,
        /* [out] */ ISchema** instance);

    using ISchemaFactory::NewSchema;

    static CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    static CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ ISchemaFactory** factory);
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
#endif