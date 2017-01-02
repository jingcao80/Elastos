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

#include "org/apache/http/client/utils/URIUtils.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/net/CURI.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Net::CURI;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Utils {

ECode URIUtils::CreateURI(
    /* [in] */ const String& scheme,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& path,
    /* [in] */ const String& query,
    /* [in] */ const String& fragment,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)

    StringBuilder buffer;
    if (!host.IsNull()) {
        if (!scheme.IsNull()) {
            buffer.Append(scheme);
            buffer.Append("://");
        }
        buffer.Append(host);
        if (port > 0) {
            buffer.AppendChar(':');
            buffer.Append(port);
        }
    }
    if (path.IsNull() || !path.StartWith("/")) {
        buffer.AppendChar('/');
    }
    if (!path.IsNull()) {
        buffer.Append(path);
    }
    if (!query.IsNull()) {
        buffer.AppendChar('?');
        buffer.Append(query);
    }
    if (!fragment.IsNull()) {
        buffer.AppendChar('#');
        buffer.Append(fragment);
    }
    return CURI::New(buffer.ToString(), uri);
}

ECode URIUtils::RewriteURI(
    /* [in] */ IURI* inUri,
    /* [in] */ IHttpHost* target,
    /* [in] */ Boolean dropFragment,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    if (inUri == NULL) {
        Logger::E("URIUtils", "URI may nor be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (target != NULL) {
        String schemeName, hostName, path, query;
        target->GetSchemeName(&schemeName);
        target->GetHostName(&hostName);
        inUri->GetRawPath(&path);
        inUri->GetRawQuery(&query);
        Int32 port;
        target->GetPort(&port);
        String fragment(NULL);
        if (!dropFragment) {
            inUri->GetRawFragment(&fragment);
        }
        return CreateURI(schemeName, hostName, port, path, query, fragment, uri);
    }
    else {
        String path, query;
        inUri->GetRawPath(&path);
        inUri->GetRawQuery(&query);
        String fragment(NULL);
        if (!dropFragment) {
            inUri->GetRawFragment(&fragment);
        }
        return CreateURI(String(NULL), String(NULL), -1, path, query, fragment, uri);
    }
}

ECode URIUtils::RewriteURI(
    /* [in] */ IURI* inUri,
    /* [in] */ IHttpHost* target,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    return RewriteURI(inUri, target, FALSE, uri);
}

ECode URIUtils::Resolve(
    /* [in] */ IURI* baseURI,
    /* [in] */ const String& reference,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    AutoPtr<IURI> iuri;
    CURI::Create(reference, (IURI**)&iuri);
    return Resolve(baseURI, iuri, uri);
}

ECode URIUtils::Resolve(
    /* [in] */ IURI* baseURI,
    /* [in] */ IURI* inReference,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    if (baseURI == NULL) {
        Logger::E("URIUtils", "Base URI may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (inReference == NULL) {
        Logger::E("URIUtils", "Reference URI may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IURI> reference = inReference;
    String referenceStr;
    IObject::Probe(reference)->ToString(&referenceStr);
    Boolean emptyReference = referenceStr.GetLength() == 0;
    if (emptyReference) {
        reference = NULL;
        CURI::Create(String("#"), (IURI**)&reference);
    }
    AutoPtr<IURI> resolved;
    baseURI->Resolve(reference, (IURI**)&resolved);
    if (emptyReference) {
        String resolvedString;
        IObject::Probe(resolved)->ToString(&resolvedString);
        resolved = NULL;
        CURI::Create(resolvedString.Substring(0, resolvedString.IndexOf('#')), (IURI**)&resolved);
    }
    *uri = resolved;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

} // namespace Utils
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org