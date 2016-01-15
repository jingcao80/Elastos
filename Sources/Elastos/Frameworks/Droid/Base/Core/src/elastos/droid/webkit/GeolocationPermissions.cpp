
#include "elastos/droid/webkit/GeolocationPermissions.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(GeolocationPermissions, Object, IGeolocationPermissions);

ECode GeolocationPermissions::constructor()
{
    return NOERROR;
}

AutoPtr<IGeolocationPermissions> GeolocationPermissions::GetInstance()
{
    AutoPtr<IGeolocationPermissions> gp;
    WebViewFactory::GetProvider()->GetGeolocationPermissions((IGeolocationPermissions**)&gp);
    return gp;
}

ECode GeolocationPermissions::GetOrigins(
    /* [in] */ IValueCallback* callback)
{
    return NOERROR;
}

ECode GeolocationPermissions::GetAllowed(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    return NOERROR;
}

ECode GeolocationPermissions::Clear(
    /* [in] */ const String& origin)
{
    return NOERROR;
}

ECode GeolocationPermissions::Allow(
    /* [in] */ const String& origin)
{
    return NOERROR;
}

ECode GeolocationPermissions::ClearAll()
{
    return NOERROR;
}

ECode GeolocationPermissions::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "GeolocationPermissions";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos