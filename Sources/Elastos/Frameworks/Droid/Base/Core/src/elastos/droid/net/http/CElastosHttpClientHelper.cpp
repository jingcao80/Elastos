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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CElastosHttpClientHelper.h"
#include "elastos/droid/net/http/CElastosHttpClient.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CElastosHttpClientHelper, Singleton, IElastosHttpClientHelper)

CAR_SINGLETON_IMPL(CElastosHttpClientHelper)

ECode CElastosHttpClientHelper::GetDEFAULT_SYNC_MIN_GZIP_BYTES(
    /* [out] */ Int64* result)
{
    return CElastosHttpClient::GetDEFAULT_SYNC_MIN_GZIP_BYTES(result);
}

ECode CElastosHttpClientHelper::SetDEFAULT_SYNC_MIN_GZIP_BYTES(
    /* [in] */ Int64 DEFAULT_SYNC_MIN_GZIP_BYTES)
{
    return CElastosHttpClient::SetDEFAULT_SYNC_MIN_GZIP_BYTES(DEFAULT_SYNC_MIN_GZIP_BYTES);
}

ECode CElastosHttpClientHelper::NewInstance(
    /* [in] */ const String& userAgent,
    /* [in] */ IContext* context,
    /* [out] */ IElastosHttpClient** result)
{
    return CElastosHttpClient::NewInstance(userAgent, context, result);
}

ECode CElastosHttpClientHelper::NewInstance(
    /* [in] */ const String& userAgent,
    /* [out] */ IElastosHttpClient** result)
{
    return CElastosHttpClient::NewInstance(userAgent, result);
}

ECode CElastosHttpClientHelper::ModifyRequestToAcceptGzipResponse(
    /* [in] */ IHttpRequest* request)
{
    return CElastosHttpClient::ModifyRequestToAcceptGzipResponse(request);
}

ECode CElastosHttpClientHelper::GetUngzippedContent(
    /* [in] */ IHttpEntity* entity,
    /* [out] */ IInputStream** result)
{
    return CElastosHttpClient::GetUngzippedContent(entity, result);
}

ECode CElastosHttpClientHelper::GetCompressedEntity(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IAbstractHttpEntity** result)
{
    return CElastosHttpClient::GetCompressedEntity(data, resolver, result);
}

ECode CElastosHttpClientHelper::GetMinGzipSize(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ Int64* result)
{
    return CElastosHttpClient::GetMinGzipSize(resolver, result);
}

ECode CElastosHttpClientHelper::ParseDate(
    /* [in] */ const String& dateString,
    /* [out] */ Int64* result)
{
    return CElastosHttpClient::ParseDate(dateString, result);
}


} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
