
#ifndef __ORG_XML_SAX_HELPERS_CLOCATORIMPL_H__
#define __ORG_XML_SAX_HELPERS_CLOCATORIMPL_H__

#include "_Org_Xml_Sax_Helpers_CLocatorImpl.h"
#include "LocatorImpl.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CLocatorImpl), public LocatorImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CLOCATORIMPL_H__
