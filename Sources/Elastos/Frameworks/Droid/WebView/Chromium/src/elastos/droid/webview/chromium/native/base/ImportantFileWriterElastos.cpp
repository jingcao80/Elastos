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
