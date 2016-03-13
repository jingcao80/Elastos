
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_IMPORTANTFILEWRITERANDROID_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_IMPORTANTFILEWRITERANDROID_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This class provides an interface to the native class for writing
 * important data files without risking data loss.
 */
//@JNINamespace("base::android")
class ImportantFileWriterElastos
    : public Object
{
public:
    /**
     * Write a binary file atomically.
     *
     * This either writes all the data or leaves the file unchanged.
     *
     * @param fileName The complete path of the file to be written
     * @param data The data to be written to the file
     * @return true if the data was written to the file, false if not.
     */
    static CARAPI_(Boolean) WriteFileAtomically(
        /* [in] */ const String& fileName,
        /* [in] */ ArrayOf<Byte>* data);

private:
    static CARAPI_(Boolean) NativeWriteFileAtomically(
        /* [in] */ const String& fileName,
        /* [in] */ ArrayOf<Byte>* data);
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_IMPORTANTFILEWRITERANDROID_H__
