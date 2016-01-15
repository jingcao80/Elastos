
#ifndef __ORG_XML_SAX_HELPERS_CATTRIBUTELISTIMPL_H__
#define __ORG_XML_SAX_HELPERS_CATTRIBUTELISTIMPL_H__

#include "_Org_Xml_Sax_Helpers_CAttributeListImpl.h"
#include "Object.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CAttributeListImpl)
    , public Object
    , public IAttributeList
    , public IAttributeListImpl
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAttributeListImpl();

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI GetName(
        /* [in] */ Int32 i,
        /* [out] */ String* name);

    CARAPI GetType(
        /* [in] */ Int32 i,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */ Int32 i,
        /* [out] */ String* value);

    CARAPI GetType(
        /* [in] */ const String& name,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ String* value);

    CARAPI SetAttributeList(
        /* [in] */ IAttributeList* atts);

    CARAPI AddAttribute(
        /* [in] */ const String& name,
        /* [in] */ const String& type,
        /* [in] */ const String& value);

    CARAPI RemoveAttribute(
        /* [in] */ const String& name);

    CARAPI Clear();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IAttributeList* atts);

private:
    List<String> mNames;
    List<String> mTypes;
    List<String> mValues;

    CARAPI_(Int32) GetListIndex(
        /* [in] */ List<String>* list,
        /* [in] */ const String& value);
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CATTRIBUTELISTIMPL_H__
