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

#include "CSSLServerSocketFactoryHelper.h"
#include "SSLServerSocketFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLServerSocketFactoryHelper, Singleton, ISSLServerSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLServerSocketFactoryHelper)

ECode CSSLServerSocketFactoryHelper::GetDefault(
    /* [out] */ IServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return SSLServerSocketFactory::GetDefault(factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
