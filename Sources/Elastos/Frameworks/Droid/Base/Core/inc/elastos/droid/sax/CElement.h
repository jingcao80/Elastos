#ifndef __ELASTOS_DROID_SAX_CELEMENT_H__
#define __ELASTOS_DROID_SAX_CELEMENT_H__

#include "_Elastos_Droid_Sax_CElement.h"
#include "elastos/droid/sax/Element.h"

namespace Elastos {
namespace Droid {
namespace Sax {

/**
 * An XML element. Provides access to child elements and hooks to listen
 * for events related to this element.
 *
 * @see RootElement
 */
CarClass(CElement)
    , public Element
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SAX_CELEMENT_H__
