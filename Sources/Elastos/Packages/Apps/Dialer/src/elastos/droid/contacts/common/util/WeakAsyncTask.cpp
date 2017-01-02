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

#include "elastos/droid/contacts/common/util/WeakAsyncTask.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

WeakAsyncTask::WeakAsyncTask(
    /* [in] */ IInterface* target)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(target);
    wrs->GetWeakReference((IWeakReference**)&mTarget);
}

ECode WeakAsyncTask::OnPreExecute()
{
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return OnPreExecute(target);
    }
    return NOERROR;
}

ECode WeakAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return DoInBackground(target, params, result);
    }
    return NOERROR;
}

ECode WeakAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    AutoPtr<IInterface> target;
    mTarget->Resolve(EIID_IInterface, (IInterface**)&target);
    if (target != NULL) {
        return OnPostExecute(target, result);
    }
    return NOERROR;
}

ECode WeakAsyncTask::OnPreExecute(
    /* [in] */ IInterface* target)
{
    // No default action
    return NOERROR;
}

ECode WeakAsyncTask::OnPostExecute(
    /* [in] */ IInterface* target,
    /* [in] */ IInterface* result)
{
    // No default action
    return NOERROR;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
