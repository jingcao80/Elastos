
#ifndef __ORG_XML_SAX_HELPERS_CDEFAULTHANDLER_H__
#define __ORG_XML_SAX_HELPERS_CDEFAULTHANDLER_H__

#include "_Org_Xml_Sax_Helpers_CDefaultHandler.h"
#include "DefaultHandler.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CDefaultHandler)
    , public DefaultHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CDEFAULTHANDLER_H__
