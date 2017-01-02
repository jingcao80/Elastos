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

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETSTRINGUTIL_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETSTRINGUTIL_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import java.nio.ByteBuffer;
// import java.nio.charset.Charset;
// import java.nio.charset.CharsetDecoder;
// import java.nio.charset.CodingErrorAction;
// import java.text.Normalizer;

using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * Utility functions for converting strings between formats when not built with
  * icu.
  */
// @JNINamespace("net::android")
class NetStringUtil
    : public Object
{
private:
    /**
      * Attempts to convert text in a given character set to a Unicode string.
      * Returns null on failure.
      * @param text ByteBuffer containing the character array to convert.
      * @param charset Character set it's in encoded in.
      * @return: Unicode string on success, null on failure.
      */
    // @CalledByNative
    static CARAPI_(String) ConvertToUnicode(
        /* [in] */ IByteBuffer* text,
        /* [in] */ const String& charset_name);

    /**
      * Attempts to convert text in a given character set to a Unicode string,
      * and normalize it.  Returns null on failure.
      * @param text ByteBuffer containing the character array to convert.
      * @param charset Character set it's in encoded in.
      * @return: Unicode string on success, null on failure.
      */
    // @CalledByNative
    static CARAPI_(String) ConvertToUnicodeAndNormalize(
        /* [in] */ IByteBuffer* text,
        /* [in] */ const String& charset_name);

    /**
      * Convert text in a given character set to a Unicode string.  Any invalid
      * characters are replaced with U+FFFD.  Returns null if the character set
      * is not recognized.
      * @param text ByteBuffer containing the character array to convert.
      * @param charset Character set it's in encoded in.
      * @return: Unicode string on success, null on failure.
      */
    // @CalledByNative
    static CARAPI_(String) ConvertToUnicodeWithSubstitutions(
        /* [in] */ IByteBuffer* text,
        /* [in] */ const String& charset_name);
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETSTRINGUTIL_H_

