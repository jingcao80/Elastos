
#include "org/apache/harmony/xml/dom/CDocumentImplHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_SINGLETON_IMPL(CDocumentImplHelper)

CAR_INTERFACE_IMPL(CDocumentImplHelper, Singleton, IDocumentImplHelper)

ECode CDocumentImplHelper::IsXMLIdentifier(
    /* [in] */ const String& s,
    /* [out] */ Boolean * pResult)
{
    VALIDATE_NOT_NULL(pResult);
    *pResult = FALSE;
    if (s.GetLength() == 0) {
        return NOERROR;
    }

    if (!IsXMLIdentifierStart(s.GetChar(0))) {
        return NOERROR;
    }

    for (Int32 i = 1; i < s.GetLength(); i++) {
        if (!IsXMLIdentifierPart(s.GetChar(i))) {
            return NOERROR;
        }
    }

    *pResult = TRUE;
    return NOERROR;
}

Boolean CDocumentImplHelper::IsXMLIdentifierStart(
    /* [in] */ Char32 c)
{
    assert(0);
    //return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
    return FALSE;
}

Boolean CDocumentImplHelper::IsXMLIdentifierPart(
    /* [in] */ Char32 c)
{
    assert(0);
    //return isXMLIdentifierStart(c) || (c >= '0' && c <= '9') || (c == '-') || (c == '.');
    return FALSE;
}

}
}
}
}
}

