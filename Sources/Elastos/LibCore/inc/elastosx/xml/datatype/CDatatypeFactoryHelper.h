#ifndef __ELASTOSX_XML_DATATYPE_CDATATYPEFACTORYHELPER_H__
#define __ELASTOSX_XML_DATATYPE_CDATATYPEFACTORYHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Datatype_CDatatypeFactoryHelper.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CarClass(CDatatypeFactoryHelper)
    , public Singleton
    , public IDatatypeFactoryHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [out] */ IDatatypeFactory** instaces);

    CARAPI NewInstance(
        /* [in] */ const String& factoryClassName,
        /* [in] */ IClassLoader* classLoader,
        /* [out] */ IDatatypeFactory** instaces);
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx

#endif