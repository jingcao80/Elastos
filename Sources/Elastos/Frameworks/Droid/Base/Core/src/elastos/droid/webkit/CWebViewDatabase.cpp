
#include "elastos/droid/webkit/CWebViewDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

ECode CWebViewDatabase::HasUsernamePassword(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::ClearUsernamePassword()
{
    WebViewDatabase::ClearUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::HasHttpAuthUsernamePassword(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasHttpAuthUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::ClearHttpAuthUsernamePassword()
{
    WebViewDatabase::ClearHttpAuthUsernamePassword();
    return NOERROR;
}

ECode CWebViewDatabase::HasFormData(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = WebViewDatabase::HasFormData();
    return NOERROR;
}

ECode CWebViewDatabase::ClearFormData()
{
    WebViewDatabase::ClearFormData();
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

