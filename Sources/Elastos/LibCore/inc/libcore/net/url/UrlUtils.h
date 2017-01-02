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

#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_URLUTILS_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_URLUTILS_H__

#include "Object.h"

namespace Libcore {
namespace Net {
namespace Url {

class UrlUtils
{
public:
    static String CanonicalizePath(
        /* [in] */ const String& path,
        /* [in] */ Boolean discardRelativePrefix);

    static String AuthoritySafePath(
        /* [in] */ const String& authority,
        /* [in] */ const String& path);

    static String GetSchemePrefix(
        /* [in] */ const String& spec);

    static Boolean IsValidSchemeChar(
        /* [in] */ Int32 index,
        /* [in] */ Byte c);

    static Int32 FindFirstOf(
        /* [in] */ const String& string,
        /* [in] */ const String& chars,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

private:
    UrlUtils();
    UrlUtils(const UrlUtils&);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_URL_LIBCORE_NET_URI_URLUTILS_H__
