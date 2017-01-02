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

#include "CThreadGroup.h"
#include "Thread.h"

namespace Elastos {
namespace Core {

AutoPtr<IThreadGroup> CThreadGroup::sSystem;
AutoPtr<IThreadGroup> CThreadGroup::sMain;

CAR_OBJECT_IMPL(CThreadGroup)

AutoPtr<IThreadGroup> CThreadGroup::GetSystemThreadGroup()
{
    if (sSystem == NULL) {
        AutoPtr<CThreadGroup> group;
        ASSERT_SUCCEEDED(CThreadGroup::NewByFriend((CThreadGroup**)&group));
        sSystem = (IThreadGroup*)group.Get();
    }
    return sSystem;
}

AutoPtr<IThreadGroup> CThreadGroup::GetMainThreadGroup()
{
    if (sMain == NULL) {
        if (sSystem == NULL) {
            AutoPtr<CThreadGroup> group;
            ASSERT_SUCCEEDED(CThreadGroup::NewByFriend((CThreadGroup**)&group));
            sSystem = (IThreadGroup*)group.Get();
        }
        AutoPtr<CThreadGroup> group;
        ASSERT_SUCCEEDED(CThreadGroup::NewByFriend(sSystem, String("main"), (CThreadGroup**)&group));
        sMain = (IThreadGroup*)group.Get();
    }
    return sMain;
}

} // namespace Core
} // namespace Elastos
