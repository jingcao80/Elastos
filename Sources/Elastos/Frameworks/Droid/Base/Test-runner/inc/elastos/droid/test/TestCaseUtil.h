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

#ifndef __TESTCASEUTIL_H__
#define __TESTCASEUTIL_H__

#include "_Eunit.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Eunit::Framework::ITest;

namespace Elastos {
namespace Droid {
namespace Test {

/**
 * @hide - This is part of a framework that is under development and should not be used for
 * active development.
 */
class TestCaseUtil
{
public:
    static CARAPI_(AutoPtr<IList>) GetTests(
        /* [in] */ ITest* test,
        /* [in] */ Boolean flatten);

    static CARAPI_(String) GetTestName(
        /* [in] */ ITest* test);

private:
    static CARAPI_(AutoPtr<IList>) GetTests(
        /* [in] */ ITest* test,
        /* [in] */ Boolean flatten,
        /* [in] */ ISet* seen);

private:
    TestCaseUtil() {}
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__TESTCASEUTIL_H__
