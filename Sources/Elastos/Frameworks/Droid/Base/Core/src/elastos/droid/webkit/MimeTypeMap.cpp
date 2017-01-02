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

#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/MimeTypeMap.h"
#include "elastos/droid/webkit/URLUtil.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Droid::Text::TextUtils;
using Libcore::Net::CMimeUtils;
using Libcore::Net::IMimeUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {

AutoPtr<MimeTypeMap> MimeTypeMap::sMimeTypeMap = new MimeTypeMap();

CAR_INTERFACE_IMPL(MimeTypeMap, Object, IMimeTypeMap);

MimeTypeMap::MimeTypeMap()
{
}

/**
 * Returns the file extension or an empty string iff there is no
 * extension. This method is a convenience method for obtaining the
 * extension of a url and has undefined results for other Strings.
 * @param url
 * @return The file extension of the given url.
 */
String MimeTypeMap::GetFileExtensionFromUrl(
    /* [in] */ const String& _url)
{
    String url = _url;
    if (!TextUtils::IsEmpty(url)) {
        Int32 fragment = url.LastIndexOf('#');
        if (fragment > 0) {
            url = url.Substring(0, fragment);
        }

        Int32 query = url.LastIndexOf('?');
        if (query > 0) {
            url = url.Substring(0, query);
        }

        Int32 filenamePos = url.LastIndexOf('/');
        String filename =
            0 <= filenamePos ? url.Substring(filenamePos + 1) : url;

        // if the filename contains special characters, we don't
        // consider it valid for our matching purposes:
        AutoPtr<IPatternHelper> patternHelper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
        Boolean matches = FALSE;
        if (!filename.IsEmpty() &&
            (patternHelper->Matches(String("[a-zA-Z_0-9\\.\\-\\(\\)\\%]+"), filename, &matches), matches)) {
            Int32 dotPos = filename.LastIndexOf('.');
            if (0 <= dotPos) {
                return filename.Substring(dotPos + 1);
            }
        }
    }

    return String("");
}

/**
 * Return true if the given MIME type has an entry in the map.
 * @param mimeType A MIME type (i.e. text/plain)
 * @return True iff there is a mimeType entry in the map.
 */
ECode MimeTypeMap::HasMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMimeUtils> utils;
    CMimeUtils::AcquireSingleton((IMimeUtils**)&utils);
    return utils->HasMineType(mimeType, result);
}

/**
 * Return the MIME type for the given extension.
 * @param extension A file extension without the leading '.'
 * @return The MIME type for the given extension or null iff there is none.
 */
ECode MimeTypeMap::GetMimeTypeFromExtension(
    /* [in] */ const String& extension,
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType)
    AutoPtr<IMimeUtils> utils;
    CMimeUtils::AcquireSingleton((IMimeUtils**)&utils);
    return utils->GuessMimeTypeFromExtension(extension, mimeType);
}

/**
 * Return true if the given extension has a registered MIME type.
 * @param extension A file extension without the leading '.'
 * @return True iff there is an extension entry in the map.
 */
ECode MimeTypeMap::HasExtension(
    /* [in] */ const String& extension,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMimeUtils> utils;
    CMimeUtils::AcquireSingleton((IMimeUtils**)&utils);
    return utils->HasExtension(extension, result);
}

/**
 * Return the registered extension for the given MIME type. Note that some
 * MIME types map to multiple extensions. This call will return the most
 * common extension for the given MIME type.
 * @param mimeType A MIME type (i.e. text/plain)
 * @return The extension for the given MIME type or null iff there is none.
 */
ECode MimeTypeMap::GetExtensionFromMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ String* extension)
{
    VALIDATE_NOT_NULL(extension);
    AutoPtr<IMimeUtils> utils;
    CMimeUtils::AcquireSingleton((IMimeUtils**)&utils);
    return utils->GuessExtensionFromMimeType(mimeType, extension);
}

/**
 * If the given MIME type is null, or one of the "generic" types (text/plain
 * or application/octet-stream) map it to a type that Android can deal with.
 * If the given type is not generic, return it unchanged.
 *
 * @param mimeType MIME type provided by the server.
 * @param url URL of the data being loaded.
 * @param contentDisposition Content-disposition header given by the server.
 * @return The MIME type that should be used for this data.
 */
/* package */
String MimeTypeMap::RemapGenericMimeType(
    /* [in] */ const String& _mimeType,
    /* [in] */ const String& _url,
    /* [in] */ const String& contentDisposition)
{
    String mimeType = _mimeType;
    String url = _url;
    // If we have one of "generic" MIME types, try to deduce
    // the right MIME type from the file extension (if any):
    if (mimeType.Equals("text/plain") ||
            mimeType.Equals("application/octet-stream")) {
        // for attachment, use the filename in the Content-Disposition
        // to guess the mimetype
        String filename;
        if (!contentDisposition.IsNullOrEmpty()) {
            filename = URLUtil::ParseContentDisposition(contentDisposition);
        }

        if (!filename.IsNullOrEmpty()) {
            url = filename;
        }

        String extension = GetFileExtensionFromUrl(url);
        String newMimeType;
        GetMimeTypeFromExtension(extension, &newMimeType);
        if (!newMimeType.IsNullOrEmpty()) {
            mimeType = newMimeType;
        }
    }
    else if (mimeType.Equals("text/vnd.wap.wml")) {
        // As we don't support wml, render it as plain text
        mimeType = "text/plain";
    }
    else {
        // It seems that xhtml+xml and vnd.wap.xhtml+xml mime
        // subtypes are used interchangeably. So treat them the same.
        if (mimeType.Equals("application/vnd.wap.xhtml+xml")) {
            mimeType = "application/xhtml+xml";
        }
    }

    return mimeType;
}

/**
 * Get the singleton instance of MimeTypeMap.
 * @return The singleton instance of the MIME-type map.
 */
AutoPtr<MimeTypeMap> MimeTypeMap::GetSingleton()
{
    return sMimeTypeMap;
}

// Static method called by jni.
String MimeTypeMap::MimeTypeFromExtension(
    /* [in] */ const String& extension)
{
    AutoPtr<IMimeUtils> utils;
    CMimeUtils::AcquireSingleton((IMimeUtils**)&utils);
    String mimeType;
    utils->GuessMimeTypeFromExtension(extension, &mimeType);
    return mimeType;
}

ECode MimeTypeMap::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "MimeTypeMap";
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
