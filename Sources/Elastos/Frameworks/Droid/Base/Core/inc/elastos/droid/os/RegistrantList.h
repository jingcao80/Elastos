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

#ifndef __ELASTOS_DROID_OS_REGISTRANTLIST_H__
#define __ELASTOS_DROID_OS_REGISTRANTLIST_H__

#include "_Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Registrant.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Os {

/** @hide */
class RegistrantList
    : public Object
    , public IRegistrantList
{
public:
    CAR_INTERFACE_DECL()

    RegistrantList();

    //synchronized
    CARAPI_(void) Add(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //synchronized
    CARAPI_(void) AddUnique(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    //synchronized
    CARAPI_(void) Add(
        /* [in] */ IRegistrant* r);

    //synchronized
    CARAPI_(void) RemoveCleared();

    //synchronized
    CARAPI_(Int32) GetSize();

    //synchronized
    CARAPI_(AutoPtr<IInterface>) Get(
        /* [in] */ Int32 index);

    CARAPI_(void) NotifyRegistrants();

    CARAPI_(void) NotifyException(
        /* [in] */ IThrowable* exception);

    CARAPI_(void) NotifyResult(
        /* [in] */ IInterface* result);

    CARAPI_(void) NotifyRegistrants(
        /* [in] */ AsyncResult* ar);

    // synchronized
    CARAPI_(void) Remove(
        /* [in] */ IHandler* h);

private:
    // synchronized
    CARAPI_(void) InternalNotifyRegistrants(
        /* [in] */ IInterface* result,
        /* [in] */ IThrowable* exception);

private:
    AutoPtr<IArrayList> mRegistrants;      // of Registrant
    Object mLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REGISTRANTLIST_H__
