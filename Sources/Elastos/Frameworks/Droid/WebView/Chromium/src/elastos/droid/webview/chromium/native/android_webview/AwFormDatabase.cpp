#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwFormDatabase.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwFormDatabase_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

Boolean AwFormDatabase::HasFormData()
{
    return NativeHasFormData();
}

void AwFormDatabase::ClearFormData()
{
    NativeClearFormData();
}

//--------------------------------------------------------------------------------------------
//  Native methods
//--------------------------------------------------------------------------------------------
Boolean AwFormDatabase::NativeHasFormData()
{
    return Elastos_AwFormDatabase_nativeHasFormData();
}

void AwFormDatabase::NativeClearFormData()
{
    Elastos_AwFormDatabase_nativeClearFormData();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
