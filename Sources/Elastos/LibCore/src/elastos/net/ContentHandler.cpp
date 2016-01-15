
#include "ContentHandler.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(ContentHandler, Object, IContentHandler)

ContentHandler::ContentHandler()
{
}

ContentHandler::~ContentHandler()
{
}

ECode ContentHandler::GetContent(
    /* [in] */ IURLConnection* uConn,
    /* [in] */ ArrayOf<InterfaceID> * types,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;
    VALIDATE_NOT_NULL(types)

    AutoPtr<IInterface> content;
    FAIL_RETURN(GetContent(uConn, (IInterface**)&content));
    for (Int32 i = 0; i < types->GetLength(); i++) {
        if (content->Probe((*types)[i]) != NULL) {
            *obj = content;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
    }
    *obj = NULL;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
