
#ifndef __ORG_XML_SAX_HELPERS_CATTRIBUTESIMPL_H__
#define __ORG_XML_SAX_HELPERS_CATTRIBUTESIMPL_H__

#include "_Org_Xml_Sax_Helpers_CAttributesImpl.h"
#include "AttributesImpl.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CAttributesImpl)
    , public AttributesImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CATTRIBUTESIMPL_H__
