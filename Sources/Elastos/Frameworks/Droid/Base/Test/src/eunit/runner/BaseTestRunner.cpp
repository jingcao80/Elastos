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

#include "eunit/runner/BaseTestRunner.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::CProperties;
using Eunit::Framework::EIID_ITestListener;

namespace Eunit {
namespace Runner {

const String BaseTestRunner::SUITE_METHODNAME("suite");
AutoPtr<IProperties> BaseTestRunner::sPreferences;
Int32 BaseTestRunner::sMaxMessageLength = 500;
Boolean BaseTestRunner::sFilterStack = TRUE;

CAR_INTERFACE_IMPL_2(BaseTestRunner, Object, ITestListener, IBaseTestRunner)

ECode BaseTestRunner::StartTest(
    /* [in] */ ITest* test)
{
    AutoLock lock(mLock);

    String name;
    IObject::Probe(test)->ToString(&name);
    return TestStarted(name);
}

void BaseTestRunner::SetPreferences(
    /* [in] */ IProperties* preferences)
{
    sPreferences = preferences;
}

AutoPtr<IProperties> BaseTestRunner::GetPreferences()
{
    if (sPreferences == NULL) {
        CProperties::New((IProperties**)&sPreferences);
        assert(0);
        // fPreferences.put("loading", "true");
        // fPreferences.put("filterstack", "true");
        ReadPreferences();
    }
    return sPreferences;
}

} // namespace Runner
} // namespace Eunit
