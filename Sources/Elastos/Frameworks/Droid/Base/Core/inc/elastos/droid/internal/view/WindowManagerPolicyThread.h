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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IThread;
using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class WindowManagerPolicyThread
{
public:
    static CARAPI Set(
        /* [in] */ IThread* thread,
        /* [in] */ ILooper* looper);

    static CARAPI_(AutoPtr<IThread>) GetThread();

    static CARAPI_(AutoPtr<ILooper>) GetLooper();

private:
    WindowManagerPolicyThread();

private:
    static AutoPtr<IThread> sThread;
    static AutoPtr<ILooper> sLooper;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_WINDOWMANAGERPOLICYTHREAD_H__
