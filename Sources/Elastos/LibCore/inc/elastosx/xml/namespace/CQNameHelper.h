
#ifndef __ELASTOSX_XML_NAMESPACE_CQNAMEHELPER_H__
#define __ELASTOSX_XML_NAMESPACE_CQNAMEHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Namespace_CQNameHelper.h"

using Elastos::Core::Singleton;

namespace Elastosx {
namespace Xml {
namespace Namespace {

CarClass(CQNameHelper)
    , public Singleton
    , public IQNameHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ const String& qNameAsString,
        /* [out] */ Elastosx::Xml::Namespace::IQName ** ppName);
};

} // namespace Namespace
} // namespace Xml
} // namespace Elastosx

#endif // __CQNAMEHELPER_H__
