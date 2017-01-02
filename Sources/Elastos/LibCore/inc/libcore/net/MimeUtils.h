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

#ifndef __LIBCORE_NET_MIMEUTILS_H__
#define __LIBCORE_NET_MIMEUTILS_H__

#include "Elastos.CoreLibrary.h"
#include "coredef.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IInputStream;

namespace Libcore {
namespace Net {

class MimeUtils
{
public:
    static CARAPI HasMineType(
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean *result);

    static CARAPI GuessMimeTypeFromExtension(
        /* [in] */ const String& extension,
        /* [out] */ String* result);

    static CARAPI HasExtension(
        /* [in] */ const String& extension,
        /* [out] */ Boolean* result);

    static CARAPI GuessExtensionFromMimeType(
        /* [in] */ const String& mimeType,
        /* [out] */ String* result);

    static CARAPI_(Boolean) HasMineType(
        /* [in] */ const String& mimeType);

    static CARAPI_(String) GuessMimeTypeFromExtension(
        /* [in] */ const String& extension);

    static CARAPI_(Boolean) HasExtension(
        /* [in] */ const String& extension);

    static CARAPI_(String) GuessExtensionFromMimeType(
        /* [in] */ const String& mimeType);

private:
    MimeUtils();
    ~MimeUtils();

private:
    static void Add(
        /* [in] */ const String& mimeType,
        /* [in] */ const String& extension);

    static CARAPI ApplyOverrides();

    static void Init();

    static CARAPI GetContentTypesPropertiesStream(
        /* [out] */IInputStream** result);

    static CARAPI Load(
        /* [in] */ IInputStream* stream,
        /* [in] */ HashMap<String, String>& map);

private:
    static HashMap<String, String> mMimeTypeToExtensionMap;
    static HashMap<String, String> mExtensionToMimeTypeMap;
    static Boolean sInited;
};

} // namespace Net
} // namespace Libcore

#endif // __LIBCORE_NET_MIMEUTILS_H__
