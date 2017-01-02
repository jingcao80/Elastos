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

#ifndef __ELASTOS_DROID_OS_CMESSAGEHELPER_H__
#define __ELASTOS_DROID_OS_CMESSAGEHELPER_H__

#include "_Elastos_Droid_Os_CMessageHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessageHelper)
    , public Singleton
    , public IMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IMessage* orig,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    CARAPI Obtain(
        /* [in] */ IHandler* target,
        /* [in] */ IRunnable* callback,
        /* [out] */ IMessage** msg);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__CMESSAGEHELPER_H__
