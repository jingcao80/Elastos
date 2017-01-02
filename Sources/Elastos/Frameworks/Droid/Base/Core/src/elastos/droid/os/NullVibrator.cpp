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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/NullVibrator.h"

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<NullVibrator> sInstance;

NullVibrator::NullVibrator()
{}


AutoPtr<NullVibrator> NullVibrator::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new NullVibrator();
        sInstance->constructor();
    }
    return sInstance;
}

ECode NullVibrator::HasVibrator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode NullVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* attributes)
{
    return Vibrate(milliseconds);
}

ECode NullVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IAudioAttributes* attributes)
{
    if (repeat >= pattern->GetLength()) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return NOERROR;
}

ECode NullVibrator::Cancel()
{
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
