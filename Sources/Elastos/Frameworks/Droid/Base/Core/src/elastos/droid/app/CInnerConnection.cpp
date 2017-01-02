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

#include "elastos/droid/app/CInnerConnection.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CInnerConnection, Object, IIServiceConnection, IBinder)

CAR_OBJECT_IMPL(CInnerConnection)

ECode CInnerConnection::constructor(
    /* [in] */ IServiceDispatcher* sd)
{
    mDispatcher = sd;
    return NOERROR;
}

ECode CInnerConnection::Connected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (mDispatcher != NULL) {
        mDispatcher->Connected(name, service);
    }
    return NOERROR;
}

ECode CInnerConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ServiceDispatcher::InnerConnection");
    return NOERROR;
}

}
}
}