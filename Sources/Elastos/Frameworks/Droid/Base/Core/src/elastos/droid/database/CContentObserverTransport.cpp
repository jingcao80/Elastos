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

#include "elastos/droid/database/CContentObserverTransport.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Database::EIID_IIContentObserver;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_3(CContentObserverTransport, Object, IContentObserverTransport, IIContentObserver, IBinder);

CAR_OBJECT_IMPL(CContentObserverTransport)

CContentObserverTransport::CContentObserverTransport()
    : mBBinder(NULL)
{
}

CContentObserverTransport::~CContentObserverTransport()
{
}

ECode CContentObserverTransport::constructor(
    /* [in] */ IContentObserver* contentObserver)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(contentObserver);
    if (wrs) {
        wrs->GetWeakReference((IWeakReference**)&mWeakContentObserver);
    }
    return NOERROR;
}

ECode CContentObserverTransport::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    if (mWeakContentObserver != NULL) {
        AutoPtr<IContentObserver> co;
        mWeakContentObserver->Resolve(EIID_IContentObserver, (IInterface**)&co);
        if (co) {
            co->DispatchChange(selfChange, uri, userId);
        }
    }
    return NOERROR;
}

ECode CContentObserverTransport::ReleaseContentObserver()
{
    mWeakContentObserver = NULL;
    return NOERROR;
}

ECode CContentObserverTransport::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CContentObserverTransport{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " contentObserver=";
    if (mWeakContentObserver != NULL) {
        AutoPtr<IContentObserver> co;
        mWeakContentObserver->Resolve(EIID_IContentObserver, (IInterface**)&co);
        sb += Object::ToString(co);
    }
    else {
        sb += "NULL";
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
