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

#ifndef __CTESTFAILURE_H__
#define __CTESTFAILURE_H__

#include "_Eunit_Framework_CTestFailure.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Eunit {
namespace Framework {

CarClass(CTestFailure)
    , public Object
    , public ITestFailure
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ITest* failedTest,
        /* [in] */ ECode ec,
        /* [in] */ const String& message,
        /* [in] */ const String& backtrace);

    /**
     * Gets the failed test.
     */
    CARAPI FailedTest(
        /* [out] */ ITest** test);

    /**
     * Gets the thrown exception.
     */
    CARAPI ThrownException(
        /* [out] */ ECode* ec);

    CARAPI Trace(
        /* [out] */ String* traceStr);

    CARAPI ExceptionMessage(
        /* [out] */ String* message);

    CARAPI IsFailure(
        /* [out] */ Boolean* result);

protected:
    AutoPtr<IWeakReference> mFailedTest;
    ECode mErrorCode;
    String mMessage;
    String mBacktrace;
};

} // namespace Framework
} // namespace Eunit

#endif //__CTESTFAILURE_H__
