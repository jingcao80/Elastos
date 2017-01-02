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

#ifndef __ELASTOS_CORE_CTHREADGROUP_H__
#define __ELASTOS_CORE_CTHREADGROUP_H__

#include "_Elastos_Core_CThreadGroup.h"
#include "ThreadGroup.h"

namespace Elastos {
namespace Core {

CarClass(CThreadGroup)
    , public ThreadGroup
{
public:
    CAR_OBJECT_DECL()

    static CARAPI_(AutoPtr<IThreadGroup>) GetSystemThreadGroup();

    static CARAPI_(AutoPtr<IThreadGroup>) GetMainThreadGroup();

private:
    // BEGIN android-added
    /* the VM uses these directly; do not rename */
    static AutoPtr<IThreadGroup> sSystem;
    static AutoPtr<IThreadGroup> sMain;
    // END android-added
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CTHREADGROUP_H__
