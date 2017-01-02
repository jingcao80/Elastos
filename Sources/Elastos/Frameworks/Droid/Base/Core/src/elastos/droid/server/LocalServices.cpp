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

#include "elastos/droid/server/LocalServices.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

Object LocalServices::sLock;
HashMap<InterfaceID, AutoPtr<IInterface> > LocalServices::sLocalServiceObjects;

AutoPtr<IInterface> LocalServices::GetService(
    /* [in] */ const InterfaceID& type)
{
    AutoPtr<IInterface> service;
    {    AutoLock syncLock(sLock);
        HashMap<InterfaceID, AutoPtr<IInterface> >::Iterator it;
        it = sLocalServiceObjects.Find(type);
        if (it != sLocalServiceObjects.End()) {
            service = it->mSecond;
        }
    }
    return service;
}

ECode LocalServices::AddService(
    /* [in] */ const InterfaceID& type,
    /* [in] */ IInterface* service)
{
    {    AutoLock syncLock(sLock);
        HashMap<InterfaceID, AutoPtr<IInterface> >::Iterator it;
        it = sLocalServiceObjects.Find(type);
        if (it != sLocalServiceObjects.End()) {
            Slogger::E("LocalServices", "Overriding service registration");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        AutoPtr<IInterface> obj = service;
        sLocalServiceObjects[type] = obj;
    }
    return NOERROR;
}


} // namespace Server
} // namespace Droid
} // namespace Elastos
