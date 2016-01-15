
#ifndef __ORG_XML_SAX_CHANDLERBASE_H__
#define __ORG_XML_SAX_CHANDLERBASE_H__

#include "_Org_Xml_Sax_CHandlerBase.h"
#include "HandlerBase.h"

namespace Org {
namespace Xml {
namespace Sax {

CarClass(CHandlerBase)
    , public HandlerBase
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_CHANDLERBASE_H__
