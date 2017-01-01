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

#include "org/alljoyn/bus/AboutIcon.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

const Int32 AboutIcon::MAX_CONTENT_LENGTH;

CAR_INTERFACE_IMPL(AboutIcon, Object, IAboutIcon)

/**
 * Set the content for an icon.  The content must fit into a MsgArg so the
 * largest size for the icon data is MAX_CONTENT_LENGTH bytes.
 *
 * Note as long as the MIME type matches it is possible to set both icon content
 * and icon URL
 *
 * If an error is returned the icon content will remain unchanged.
 *
 * @param mimeType a MIME type indicating the icon image type. Typical
 *                     value will be `image/jpeg` or `image/png`
 * @param content  an array of bytes that represent an icon
 *
 * @throws BusException if unable to create AboutIcon
 * <ul>
 *   <li>"MAX_CONTENT_LENGTH exceeded" if icon is too large</li>
 * </ul>
 */
ECode AboutIcon::constructor(
    /* [in] */ const String& mimeType,
    /* [in] */ ArrayOf<Byte>* content)
{
    if (content->GetLength() > MAX_CONTENT_LENGTH) {
        Logger::E("AboutIcon", "MAX_CONTENT_LENGTH exceeded");
        return E_BUS_EXCEPTION;
    }
    mMimeType = mimeType;
    mContent = content;
    mUrl = "";
    return NOERROR;
}

/**
 * Set a URL that contains the icon for the application.
 *
 * @param mimeType a MIME type indicating the icon image type. Typical
 *                     value will be `image/jpeg` or `image/png`
 * @param url      A URL that contains the location of the icon hosted in
 *                     the cloud.
 *
 * @throws BusException if unable to create AboutIcon
 */
ECode AboutIcon::constructor(
    /* [in] */ const String& mimeType,
    /* [in] */ const String& url)
{
    mMimeType = mimeType;
    mContent = NULL;
    mUrl = url;
    return NOERROR;
}

/**
 * Set the URL and content for an icon.  The content must fit into a MsgArg so the
 * largest size for the icon data is MAX_CONTENT_LENGTH bytes.
 *
 * Note the MIME type for the content and the URL must match
 *
 * If an error is returned the icon content will remain unchanged.
 *
 * @param mimeType a MIME type indicating the icon image type. Typical
 *                     value will be `image/jpeg` or `image/png`
 * @param url      A URL that contains the location of the icon hosted in
 *                 the cloud.
 * @param content  an array of bytes that represent an icon
 *
 * @throws BusException if unable to create AboutIcon
 * <ul>
 *   <li>"MAX_CONTENT_LENGTH exceeded" if icon is too large</li>
 * </ul>
 */
ECode AboutIcon::constructor(
    /* [in] */ const String& mimeType,
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* content)
{
    if (content->GetLength() > MAX_CONTENT_LENGTH) {
        Logger::E("AboutIcon", "MAX_CONTENT_LENGTH exceeded");
        return E_BUS_EXCEPTION;
    }
    mMimeType = mimeType;
    mUrl = url;
    mContent = content;
    return NOERROR;
}

/**
 * @return A string indicating the image MIME type
 */
ECode AboutIcon::GetMimeType(
    /* [out] */ String* mimetype)
{
    VALIDATE_NOT_NULL(mimetype)
    *mimetype = mMimeType;
    return NOERROR;
}

/**
 * @return A URL indicating the cloud location of the icon
 */
ECode AboutIcon::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url)
    *url = mUrl;
    return NOERROR;
}

/**
 * @return an array of bytes containing the binary content for the icon
 */
ECode AboutIcon::GetContent(
    /* [out, callee] */ ArrayOf<Byte>** content)
{
    VALIDATE_NOT_NULL(content)
    *content = mContent;
    REFCOUNT_ADD(*content)
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
