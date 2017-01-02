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

#include "PlainServerSocketImpl.h"
#include "CBoolean.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(PlainServerSocketImpl, PlainSocketImpl, IPlainServerSocketImpl)

ECode PlainServerSocketImpl::constructor()
{
    return PlainSocketImpl::constructor();
}

ECode PlainServerSocketImpl::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    return PlainSocketImpl::constructor(fd);
}

ECode PlainServerSocketImpl::Create(
    /* [in] */ Boolean isStreaming)
{
    ECode ec = NOERROR;
    ec = PlainSocketImpl::Create(isStreaming);
    FAIL_RETURN(ec)

    if (isStreaming) {
        AutoPtr<IBoolean> obj;
        CBoolean::New(TRUE, (IBoolean**)&obj);
        SetOption(ISocketOptions::_SO_REUSEADDR, obj.Get());
    }
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
