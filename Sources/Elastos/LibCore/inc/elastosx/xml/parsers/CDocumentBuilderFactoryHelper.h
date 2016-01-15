#ifndef __ELASTOSX_XML_PARSERS_CDOCUMENTBUILDERFACTORYHELPER_H__
#define __ELASTOSX_XML_PARSERS_CDOCUMENTBUILDERFACTORYHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Parsers_CDocumentBuilderFactoryHelper.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CarClass(CDocumentBuilderFactoryHelper)
    , public Singleton
    , public IDocumentBuilderFactoryHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [out] */ IDocumentBuilderFactory** instaces);

    CARAPI NewInstance(
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ IDocumentBuilderFactory** instaces);
};

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx

#endif