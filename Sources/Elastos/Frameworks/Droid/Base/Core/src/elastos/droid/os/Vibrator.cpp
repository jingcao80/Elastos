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
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/os/Vibrator.h"
#include "elastos/droid/os/Process.h"

using Elastos::Droid::App::CActivityThread;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(Vibrator, Object, IVibrator)

Vibrator::Vibrator()
{}

ECode Vibrator::constructor()
{
    mPackageName = CActivityThread::GetCurrentPackageName();
    return NOERROR;
}

ECode Vibrator::constructor(
    /* [in] */ IContext* context)
{
    return context->GetOpPackageName(&mPackageName);
}

ECode Vibrator::Vibrate(
    /* [in] */ Int64 milliseconds)
{
    return Vibrate(milliseconds, NULL);
}

ECode Vibrator::Vibrate(
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* attributes)
{
    return Vibrate(Process::MyUid(), mPackageName, milliseconds, attributes);
}

ECode Vibrator::Vibrate(
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat)
{
    return Vibrate(pattern, repeat, NULL);
}

ECode Vibrator::Vibrate(
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IAudioAttributes* attributes)
{
    return Vibrate(Process::MyUid(), mPackageName, pattern, repeat, attributes);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
