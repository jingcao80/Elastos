#ifndef __ELASTOSX_XML_VALIDATION_CSCHEMAFACTORYHELPER_H__
#define __ELASTOSX_XML_VALIDATION_CSCHEMAFACTORYHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Validation_CSchemaFactoryHelper.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Validation {

CarClass(CSchemaFactoryHelper)
    , public Singleton
    , public ISchemaFactoryHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [out] */ ISchemaFactory** factory);

    CARAPI NewInstance(
        /* [in] */ const String& schemaLanguage,
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ ISchemaFactory** factory);
};

} // namespace Validation
} // namespace Xml
} // namespace Elastosx

#endif