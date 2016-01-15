#include "elastos/droid/webkit/native/android_webview/AwFormDatabase.h"
#include "elastos/droid/webkit/native/android_webview/api/AwFormDatabase_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
