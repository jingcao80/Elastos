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

#ifndef __ELASTOS_DROID_OS_NULLVIBRATOR_H__
#define __ELASTOS_DROID_OS_NULLVIBRATOR_H__

#include "elastos/droid/os/Vibrator.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Vibrator implementation that does nothing.
 *
 * @hide
 */
class NullVibrator
    : public Vibrator
{
private:
    NullVibrator();

public:
    static AutoPtr<NullVibrator> GetInstance();

    CARAPI HasVibrator(
        /* [out] */ Boolean* result);

    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ Int64 milliseconds,
        /* [in] */ IAudioAttributes* attributes);

    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ ArrayOf<Int64>* milliseconds,
        /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes);

    using Vibrator::Vibrate;

    CARAPI Cancel();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_NULLVIBRATOR_H__
