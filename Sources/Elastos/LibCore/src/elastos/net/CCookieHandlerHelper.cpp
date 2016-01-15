
#include "CCookieHandlerHelper.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CCookieHandlerHelper, Singleton, ICookieHandlerHelper)

CAR_SINGLETON_IMPL(CCookieHandlerHelper)

ECode CCookieHandlerHelper::GetDefault(
    /* [out] */ ICookieHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mSystemWideCookieHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode CCookieHandlerHelper::SetDefault(
    /* [in] */ ICookieHandler* handler)
{
    mSystemWideCookieHandler = handler;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
