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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "eunit/textui/ResultPrinter.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Eunit::Framework::EIID_ITestListener;

namespace Eunit {
namespace Textui {

CAR_INTERFACE_IMPL_2(ResultPrinter, Object, ITestListener, IResultPrinter)

void ResultPrinter::Print(
    /* [in] */ ITestResult* result,
    /* [in] */ Int64 runTime)
{
    AutoLock lock(&mLock);
    PrintHeader(runTime);
    PrintErrors(result);
    PrintFailures(result);
    PrintFooter(result);
}

void ResultPrinter::PrintWaitPrompt()
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Println();
    writer->Println(String("<RETURN> to continue"));
}

void ResultPrinter::PrintHeader(
    /* [in] */ Int64 runTime)
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Println();
    writer->Println(String("Time: ") + ElapsedTimeAsString(runTime));
}

void ResultPrinter::PrintErrors(
    /* [in] */ ITestResult* result)
{
    AutoPtr<IEnumeration> errors;
    result->Errors((IEnumeration**)&errors);
    Int32 number;
    result->ErrorCount(&number);
    PrintDefects(errors, number, String("error"));
}

void ResultPrinter::PrintFailures(
    /* [in] */ ITestResult* result)
{
    AutoPtr<IEnumeration> failures;
    result->Failures((IEnumeration**)&failures);
    Int32 number;
    result->FailureCount(&number);
    PrintDefects(failures, number, String("failure"));
}

void ResultPrinter::PrintDefects(
    /* [in] */ IEnumeration* booBoos,
    /* [in] */ Int32 count,
    /* [in] */ const String& type)
{
    if (count == 0) return;
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    if (count == 1) {
        StringBuilder sb;
        sb.Append("There was ");
        sb.Append(count);
        sb.Append(" ");
        sb.Append(type);
        sb.Append(":");
        writer->Println(sb.ToString());
    }
    else {
        StringBuilder sb;
        sb.Append("There were ");
        sb.Append(count);
        sb.Append(" ");
        sb.Append(type);
        sb.Append("s:");
        writer->Println(sb.ToString());
    }
    Boolean hasMore;
    for (Int32 i = 1; (booBoos->HasMoreElements(&hasMore), hasMore); i++) {
        AutoPtr<IInterface> e;
        booBoos->GetNextElement((IInterface**)&e);
        assert(ITestFailure::Probe(e) != NULL);
        PrintDefect(ITestFailure::Probe(e), i);
    }
}

ECode ResultPrinter::PrintDefect(
    /* [in] */ ITestFailure* booBoo,
    /* [in] */ Int32 count)
{
    PrintDefectHeader(booBoo, count);
    PrintDefectTrace(booBoo);
    return NOERROR;
}

void ResultPrinter::PrintDefectHeader(
    /* [in] */ ITestFailure* booBoo,
    /* [in] */ Int32 count)
{
    // I feel like making this a println, then adding a line giving the throwable a chance to print something
    // before we get to the stack trace.
    AutoPtr<ITest> test;
    booBoo->FailedTest((ITest**)&test);
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Print(StringUtils::ToString(count) + ") " + testStr);
}

void ResultPrinter::PrintDefectTrace(
    /* [in] */ ITestFailure* booBoo)
{
    assert(0);
    // getWriter().print(BaseTestRunner.getFilteredTrace(booBoo.trace()));
}

void ResultPrinter::PrintFooter(
    /* [in] */ ITestResult* result)
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    Boolean successful;
    if (result->WasSuccessful(&successful), successful) {
        writer->Println();
        writer->Print(String("OK"));
        Int32 number;
        result->RunCount(&number);
        StringBuilder sb;
        sb.Append(" (");
        sb.Append(number);
        sb.Append(" test");
        if (number == 1) sb.Append("");
        else sb.Append("s");
        sb.Append(")");
        writer->Println(sb.ToString());
    }
    else{
        writer->Println();
        writer->Println(String("FAILURES!!!"));
        StringBuilder sb;
        sb.Append("Tests run: ");
        Int32 number;
        result->RunCount(&number);
        sb.Append(number);
        sb.Append(",  Failures: ");
        result->FailureCount(&number);
        sb.Append(number);
        sb.Append(",  Errors: ");
        result->ErrorCount(&number);
        sb.Append(number);
        writer->Println(sb.ToString());
    }
    writer->Println();
}

String ResultPrinter::ElapsedTimeAsString(
    /* [in] */ Int64 runTime)
{
    // android-changed
    // The following line was altered for compatibility with
    // Android libraries.
    return StringUtils::ToString((Double)runTime / 1000);
}

ECode ResultPrinter::GetWriter(
    /* [out] */ IPrintStream** writer)
{
    VALIDATE_NOT_NULL(writer);
    *writer = mWriter;
    REFCOUNT_ADD(*writer);
    return NOERROR;
}

ECode ResultPrinter::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Print(String("E"));
    return NOERROR;
}

ECode ResultPrinter::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Print(String("F"));
    return NOERROR;
}

ECode ResultPrinter::EndTest(
    /* [in] */ ITest* test)
{
    return NOERROR;
}

ECode ResultPrinter::StartTest(
    /* [in] */ ITest* test)
{
    AutoPtr<IPrintStream> writer;
    GetWriter((IPrintStream**)&writer);
    writer->Print(String("."));
    if (mColumn++ >= 40) {
        writer->Println();
        mColumn = 0;
    }
    return NOERROR;
}

} // namespace Textui
} // namespace Eunit
