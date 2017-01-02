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

#include "org/apache/http/CHttpVersionHelper.h"
#include "org/apache/http/CHttpVersion.h"

namespace Org {
namespace Apache {
namespace Http {

CAR_INTERFACE_IMPL(CHttpVersionHelper, Object, IHttpVersionHelper)

CAR_SINGLETON_IMPL(CHttpVersionHelper)

ECode CHttpVersionHelper::GerHttpVersion09(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_0_9;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode CHttpVersionHelper::GerHttpVersion10(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_1_0;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode CHttpVersionHelper::GerHttpVersion11(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_1_1;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
