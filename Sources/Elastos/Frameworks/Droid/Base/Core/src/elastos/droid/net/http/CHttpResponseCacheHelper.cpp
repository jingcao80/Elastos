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
#include "elastos/droid/net/http/CHttpResponseCacheHelper.h"
#include "elastos/droid/net/http/HttpResponseCache.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CHttpResponseCacheHelper, Singleton, IHttpResponseCacheHelper)

CAR_SINGLETON_IMPL(CHttpResponseCacheHelper)

ECode CHttpResponseCacheHelper::GetInstalled(
    /* [out] */ IHttpResponseCache** result)
{
    return HttpResponseCache::GetInstalled(result);
}

ECode CHttpResponseCacheHelper::Install(
    /* [in] */ IFile* directory,
    /* [in] */ Int64 maxSize,
    /* [out] */ IHttpResponseCache** result)
{
    return HttpResponseCache::Install(directory, maxSize, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
