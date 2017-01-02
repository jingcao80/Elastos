//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
