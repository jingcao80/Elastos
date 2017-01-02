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

#include "CJarHandler.h"
#include "UrlUtils.h"
#include "CURL.h"
#include "StringBuilder.h"
#include "CJarURLConnectionImpl.h"

using Elastos::Core::StringBuilder;
using Elastos::Net::CURL;

namespace Libcore {
namespace Net {
namespace Url {

CAR_OBJECT_IMPL(CJarHandler)
CAR_INTERFACE_IMPL(CJarHandler, URLStreamHandler, IJarHandler)

ECode CJarHandler::constructor()
{
    return NOERROR;
}

ECode CJarHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [out] */ IURLConnection** urlConnection)
{
    VALIDATE_NOT_NULL(urlConnection)

    return CJarURLConnectionImpl::New(u, urlConnection);
}

ECode CJarHandler::ParseURL(
    /* [in] */ IURL* url,
    /* [in] */ const String& spec,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String tmpspec;
    String file;
    url->GetFile(&file);
    if (file.IsNull()) {
        file = "";
    }
    if (end > start) {
        tmpspec = spec.Substring(start, end);
    }
    else {
        tmpspec = "";
    }
    if (tmpspec.IndexOf("!/") == -1 && (file.IndexOf("!/") == -1)) {
        // throw new NullPointerException("Cannot find \"!/\"");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (file.IsEmpty()) {
        file = tmpspec;
    }
    else if (tmpspec.GetChar(0) == '/') {
        file = file.Substring(0, file.IndexOf('!') + 1) + tmpspec;
    }
    else {
        Int32 idx = file.IndexOf('!');
        String tmpFile = file.Substring(idx + 1, file.LastIndexOf('/') + 1) + tmpspec;
        tmpFile = UrlUtils::CanonicalizePath(tmpFile, TRUE);
        file = file.Substring(0, idx + 1) + tmpFile;
    }
    // try {
        // check that the embedded url is valid
    AutoPtr<IURL> outurl;
    FAIL_RETURN(CURL::New(file, (IURL**)&outurl));
    // } catch (MalformedURLException e) {
    //     throw new NullPointerException(e.toString());
    // }
    return SetURL(url, String("jar"), String(""), -1, String(NULL), String(NULL), file, String(NULL), String(NULL));
}

ECode CJarHandler::ToExternalForm(
    /* [in] */ IURL* url,
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s)

    StringBuilder sb;
    sb.Append(String("jar:"));
    String file;
    url->GetFile(&file);
    sb.Append(file);
    String ref;
    url->GetRef(&ref);
    if (!ref.IsNull()) {
        sb.Append(ref);
    }
    *s = sb.ToString();
    return NOERROR;
}

} // namespace Url
} // namespace Net
} // namespace Libcore
