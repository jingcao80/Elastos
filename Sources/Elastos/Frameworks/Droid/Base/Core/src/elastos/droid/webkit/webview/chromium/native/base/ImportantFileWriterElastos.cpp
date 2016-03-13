
#include "elastos/droid/webkit/webview/chromium/native/base/ImportantFileWriterElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/ImportantFileWriterElastos_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Write a binary file atomically.
 *
 * This either writes all the data or leaves the file unchanged.
 *
 * @param fileName The complete path of the file to be written
 * @param data The data to be written to the file
 * @return true if the data was written to the file, false if not.
 */
Boolean ImportantFileWriterElastos::WriteFileAtomically(
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Byte>* data)
{
    return NativeWriteFileAtomically(fileName, data);
}

Boolean ImportantFileWriterElastos::NativeWriteFileAtomically(
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Byte>* data)
{
    return Elastos_ImportantFileWriterAndroid_nativeWriteFileAtomically(fileName, data);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
