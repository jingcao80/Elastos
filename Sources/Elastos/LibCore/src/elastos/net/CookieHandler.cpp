
#include "CookieHandler.h"

namespace Elastos {
namespace Net {

AutoPtr<ICookieHandler> CookieHandler::sSystemWideCookieHandler;

CAR_INTERFACE_IMPL(CookieHandler, Object, ICookieHandler)

CookieHandler::CookieHandler()
{
}

CookieHandler::~CookieHandler()
{
}

ECode CookieHandler::GetDefault(
    /* [out] */ ICookieHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = sSystemWideCookieHandler;
    REFCOUNT_ADD(*handler);

    return NOERROR;
}

ECode CookieHandler::SetDefault(
    /* [in] */ ICookieHandler* cHandler)
{
    sSystemWideCookieHandler = cHandler;
    return NOERROR;
}

ECode CookieHandler::Get(
    /* [in] */ IURI* uri,
    /* [in] */ IMap* requestHeaders,
    /* [out]*/ IMap** cookiesMap)
{
    assert(0 && "Sub class must overwrite this method.");
    return E_NOT_IMPLEMENTED;
}

ECode CookieHandler::Put(
    /* [in] */ IURI* uri,
    /* [in] */ IMap* requestHeaders)
{
    assert(0 && "Sub class must overwrite this method.");
    return E_NOT_IMPLEMENTED;
}

} // namespace Net
} // namespace Elastos
