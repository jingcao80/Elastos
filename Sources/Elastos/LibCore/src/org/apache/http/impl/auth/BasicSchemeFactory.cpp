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

#include "org/apache/http/impl/auth/BasicSchemeFactory.h"
#include "org/apache/http/impl/auth/BasicScheme.h"

using Org::Apache::Http::Auth::EIID_IAuthSchemeFactory;
using Org::Apache::Http::Auth::EIID_IAuthScheme;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

CAR_INTERFACE_IMPL(BasicSchemeFactory, Object, IAuthSchemeFactory)

ECode BasicSchemeFactory::NewInstance(
    /* [in] */ IHttpParams* params,
    /* [out] */ IAuthScheme** scheme)
{
    VALIDATE_NOT_NULL(scheme)
    AutoPtr<BasicScheme> s = new BasicScheme();
    *scheme = (IAuthScheme*)s->Probe(EIID_IAuthScheme);
    REFCOUNT_ADD(*scheme)
    return NOERROR;
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org