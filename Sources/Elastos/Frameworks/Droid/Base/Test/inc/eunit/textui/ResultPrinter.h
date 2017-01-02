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

#ifndef __EUNIT_TEXTUI_RESULTPRINTER_H__
#define __EUNIT_TEXTUI_RESULTPRINTER_H__

#include <elastos/coredef.h>
#include <_Eunit.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IPrintStream;
using Elastos::Utility::IEnumeration;
using Eunit::Framework::ITestListener;
using Eunit::Framework::ITestResult;
using Eunit::Framework::ITestFailure;
using Eunit::Framework::ITest;

namespace Eunit {
namespace Textui {

class ECO_PUBLIC ResultPrinter
    : public Object
    , public ITestListener
    , public IResultPrinter
{
public:
    ResultPrinter(
        /* [in] */ IPrintStream* writer)
        : mWriter(writer)
        , mColumn(0)
    {}

    CAR_INTERFACE_DECL()

    /* API for use by textui.TestRunner
     */
    virtual CARAPI_(void) Print(
        /* [in] */ ITestResult* result,
        /* [in] */ Int64 runTime);

    virtual CARAPI_(void) PrintWaitPrompt();

    CARAPI PrintDefect(
        /* [in] */ ITestFailure* booBoo,
        /* [in] */ Int32 count);

    CARAPI GetWriter(
        /* [out] */ IPrintStream** writer);

    CARAPI AddError(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    CARAPI AddFailure(
        /* [in] */ ITest* test,
        /* [in] */ ITestFailure* failure);

    CARAPI EndTest(
        /* [in] */ ITest* test);

    CARAPI StartTest(
        /* [in] */ ITest* test);

protected:
    virtual CARAPI_(void) PrintHeader(
        /* [in] */ Int64 runTime);

    virtual CARAPI_(void) PrintErrors(
        /* [in] */ ITestResult* result);

    virtual CARAPI_(void) PrintFailures(
        /* [in] */ ITestResult* result);

    virtual CARAPI_(void) PrintDefects(
        /* [in] */ IEnumeration* booBoos,
        /* [in] */ Int32 count,
        /* [in] */ const String& type);

    virtual CARAPI_(void) PrintDefectHeader(
        /* [in] */ ITestFailure* booBoo,
        /* [in] */ Int32 count);

    virtual CARAPI_(void) PrintDefectTrace(
        /* [in] */ ITestFailure* booBoo);

    virtual CARAPI_(void) PrintFooter(
        /* [in] */ ITestResult* result);

    /**
     * Returns the formatted string of the elapsed time.
     * Duplicated from BaseTestRunner. Fix it.
     */
    virtual CARAPI_(String) ElapsedTimeAsString(
        /* [in] */ Int64 runTime);

public:
    AutoPtr<IPrintStream> mWriter;
    Int32 mColumn;
    Object mLock;
};

} // namespace Textui
} // namespace Eunit

#endif //__EUNIT_TEXTUI_RESULTPRINTER_H__
