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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/crypto/ByteArrayGenerator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::EIID_IInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Crypto {

/**
 * Polls random data to generate the array.
 * @param numBytes Length of the array to generate.
 * @return byte[] containing randomly generated data.
 */
AutoPtr< ArrayOf<Byte> > ByteArrayGenerator::GetBytes(
    /* [in] */ Int32 numBytes)
{
    AutoPtr<IFileInputStream> fis;
    // try {
        CFileInputStream::New(String("/dev/urandom"), (IFileInputStream**)&fis);
        AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(numBytes);
        Int32 readLen;
        AutoPtr<IInputStream> inputStream = IInputStream::Probe(fis);
        inputStream->Read(bytes, &readLen);
        if (bytes->GetLength() != readLen) {
            //throw new GeneralSecurityException("Not enough random data available");
            Logger::E("ByteArrayGenerator::GetBytes", "Not enough random data available");
            assert(0);
        }
    // } finally {
        if (fis != NULL) {
            inputStream->Close();
        }
    // }

    return bytes;
}

} // namespace Crypto
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
