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

#include "org/conscrypt/COpenSSLBIOSourceHelper.h"
#include "org/conscrypt/OpenSSLBIOSource.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLBIOSourceHelper)

CAR_INTERFACE_IMPL(COpenSSLBIOSourceHelper, Singleton, IOpenSSLBIOSource)

ECode COpenSSLBIOSourceHelper::Wrap(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IOpenSSLBIOSource** result)
{
    return OpenSSLBIOSource::Wrap(buffer, result);
}

} // namespace Conscrypt
} // namespace Org
