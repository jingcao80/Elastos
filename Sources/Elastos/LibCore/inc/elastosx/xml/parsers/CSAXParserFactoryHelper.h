#ifndef __ELASTOSX_XML_PARSERS_CSAXPARSERFACTORYHELPER_H__
#define __ELASTOSX_XML_PARSERS_CSAXPARSERFACTORYHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Parsers_CSAXParserFactoryHelper.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CarClass(CSAXParserFactoryHelper)
    , public Singleton
    , public ISAXParserFactoryHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [out] */ ISAXParserFactory** instaces);

    CARAPI NewInstance(
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ ISAXParserFactory** instaces);
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif