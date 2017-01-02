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

#ifndef __ELASTOS_DROID_OS_REGISTRANT_H__
#define __ELASTOS_DROID_OS_REGISTRANT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/os/AsyncResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

class RegistrantList;

/** @hide */
class Registrant
    : public Object
    , public IRegistrant
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI Clear();

    CARAPI NotifyRegistrant();

    CARAPI NotifyResult(
        /* [in] */ IInterface* result);

    CARAPI NotifyException(
        /* [in] */ IThrowable* exception);

    CARAPI NotifyRegistrant(
        /* [in] */ IAsyncResult* ar);

    /*package*/
    void InternalNotifyRegistrant(
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* exception);

    /**
     * NOTE: May return null if weak reference has been collected
     */
    CARAPI MessageForRegistrant(
        /* [out] */ IMessage** result);

    CARAPI GetHandler(
        /* [out] */ IHandler** result);

private:
    AutoPtr<IWeakReference>     mRefH;
    Int32                       mWhat;
    AutoPtr<IInterface>         mUserObj;
    friend class RegistrantList;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REGISTRANT_H__
