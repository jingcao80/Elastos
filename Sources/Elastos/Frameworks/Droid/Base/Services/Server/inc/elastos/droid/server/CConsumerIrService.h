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

#ifndef __ELASTOS_DROID_SERVER_CCONSUMERIR_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CCONSUMERIR_SERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CConsumerIrService.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::IIConsumerIrService;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CConsumerIrService)
    , public Object
    , public IIConsumerIrService
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CConsumerIrService();

    virtual ~CConsumerIrService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI HasIrEmitter(
        /* [out] */ Boolean* result);

    CARAPI ThrowIfNoIrEmitter();


    //@Override
    CARAPI Transmit(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 carrierFrequency,
        /* [in] */ ArrayOf<Int32>* pattern);

    //@Override
    CARAPI GetCarrierFrequencies(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI ToString(
        /* [out] */ String* str);
private:

    static Int64 HalOpen();

    static Int32 HalTransmit(
        /* [in] */ Int64 halObject,
        /* [in] */ Int32 carrierFrequency,
        /* [in] */ ArrayOf<Int32>* pattern);

    static AutoPtr<ArrayOf<Int32> >  HalGetCarrierFrequencies(
        /* [in] */ Int64 halObject);

private:
    static const String TAG;

    static const Int32 MAX_XMIT_TIME; /* in microseconds */

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    Int64 mNativeHal;
    Object mHalLock;
};


} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__
