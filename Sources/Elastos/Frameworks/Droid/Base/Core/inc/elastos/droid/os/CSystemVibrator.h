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

#ifndef __ELASTOS_DROID_OS_CSYSTEMVIBRATOR_H__
#define __ELASTOS_DROID_OS_CSYSTEMVIBRATOR_H__

#include "_Elastos_Droid_Os_CSystemVibrator.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/Vibrator.h"

using Elastos::Droid::Os::IIVibratorService;
using Elastos::Droid::Media::IAudioAttributes;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Vibrator implementation that controls the main system vibrator.
 *
 * @hide
 */
CarClass(CSystemVibrator)
    , public Vibrator
{
public:
    CAR_OBJECT_DECL()

    CSystemVibrator();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI HasVibrator(
        /* [out] */ Boolean* result);

    using Vibrator::Vibrate;

    //@Override
    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ Int64 milliseconds,
        /* [in] */ IAudioAttributes* attributes);

    //@Override
    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes);

    //@Override
    CARAPI Cancel();

private:
    static Int32 UsageForAttributes(
        /* [in] */ IAudioAttributes* attributes);

private:
    static const String TAG;

    AutoPtr<IIVibratorService> mService;
    AutoPtr<IBinder> mToken;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSYSTEMVIBRATOR_H__
