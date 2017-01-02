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

#include <CProxyHelper.h>
#include <CProxy.h>

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CProxyHelper, Singleton, IProxyHelper)

CAR_SINGLETON_IMPL(CProxyHelper)

ECode CProxyHelper::GetNO_PROXY(
    /* [out] */ IProxy** proxy)
{
    return CProxy::GetNO_PROXY(proxy);
}

} // namespace Net
} // namespace Elastos
