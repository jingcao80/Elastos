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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__

#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Os::AsyncTask;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/** Static methods that are useful for scheduling actions to occur at a later time. */
class WeakAsyncTask
    : public AsyncTask
{
public:
    WeakAsyncTask(
        /* [in] */ IInterface* target);

    virtual ~WeakAsyncTask() {}

protected:
    /** {@inheritDoc} */
    // @Override
    CARAPI OnPreExecute();

    /** {@inheritDoc} */
    // @Override
    CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnPostExecute(
        /* [in] */ IInterface* result);

    virtual CARAPI OnPreExecute(
        /* [in] */ IInterface* target);

    virtual CARAPI DoInBackground(
        /* [in] */ IInterface* target,
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result) = 0;

    virtual CARAPI OnPostExecute(
        /* [in] */ IInterface* target,
        /* [in] */ IInterface* result);

protected:
    AutoPtr<IWeakReference> mTarget;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_WEAKASYNCTASK_H__
