
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/test/TestFilePrinter.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Eunit::Framework::EIID_ITestListener;

namespace Elastos {
namespace Droid {
namespace Test {

const String TestFilePrinter::PATH("/data/data/com.elastos.runtime/elastos/data/");

TestFilePrinter::TestFilePrinter(
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean onlyFailures)
    : mTag(tag)
    , mOnlyFailures(onlyFailures)
{
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    Int32 pid;
    os->Getpid(&pid);
    StringBuilder sb;
    sb += "\nI/";
    sb += mTag;
    sb += "(";
    sb += pid;
    sb += "): ";
    mPrefix = sb.ToString();
}

TestFilePrinter::~TestFilePrinter()
{
    if (mFileOut != NULL) {
        ICloseable::Probe(mFileOut)->Close();
    }
}

CAR_INTERFACE_IMPL_2(TestFilePrinter, Object, ITestListener, ITestRunnerListener)

ECode TestFilePrinter::Started(
    /* [in] */ const String& className)
{
    if (mOutput == NULL) return NOERROR;
    if (!mOnlyFailures) {
        mOutput->WriteUTF(mPrefix + "started: " + className);
    }
    return NOERROR;
}

ECode TestFilePrinter::Finished(
    /* [in] */ const String& className)
{
    if (mOutput == NULL) return NOERROR;
    if (!mOnlyFailures) {
        mOutput->WriteUTF(mPrefix + "finished: " + className);
    }
    return NOERROR;
}

ECode TestFilePrinter::Performance(
    /* [in] */ const String& className,
    /* [in] */ Int64 itemTimeNS,
    /* [in] */ Int32 iterations,
    /* [in] */ IList* itermediates)
{
    if (mOutput == NULL) return NOERROR;
    mPrefix.AppendFormat("perf: %s = %lldns/op (done %d times)",
            className.string(), itemTimeNS, iterations);
    mOutput->WriteUTF(mPrefix);
    if (itermediates != NULL) {
        Int32 N;
        itermediates->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            assert(0);
            // TestRunner.IntermediateTime time = intermediates.get(i);
            // Log.i(mTag, "  intermediate: " + time.name + " = "
            //         + time.timeInNS + "ns");
        }
    }
    return NOERROR;
}

ECode TestFilePrinter::Passed(
    /* [in] */ const String& className)
{
    if (mOutput == NULL) return NOERROR;
    if (!mOnlyFailures) {
        mOutput->WriteUTF(mPrefix + "passed: " + className);
        mOutput->WriteUTF(String("\n"));
    }
    return NOERROR;
}

ECode TestFilePrinter::Failed(
    /* [in] */ const String& className,
    /* [in] */ ITestFailure* failure)
{
    if (mOutput == NULL) return NOERROR;
    mOutput->WriteUTF(mPrefix + "failed: " + className);
    String message;
    failure->ExceptionMessage(&message);
    mOutput->WriteUTF(mPrefix + message);
    mOutput->WriteUTF(mPrefix + String("----- begin exception -----"));
    String trace;
    failure->Trace(&trace);
    AutoPtr< ArrayOf<String> > lines;
    StringUtils::Split(trace, "\n", (ArrayOf<String>**)&lines);
    for (Int32 i = 0; i < lines->GetLength(); ++i) {
        mOutput->WriteUTF(mPrefix + (*lines)[i]);
    }
    mOutput->WriteUTF(mPrefix + String("----- end exception -----"));
    return NOERROR;
}

void TestFilePrinter::Failed(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    mFailedTests.Insert(testStr);
    Failed(testStr, failure);
}

ECode TestFilePrinter::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    assert(0);
    // Failed(test, ec);
    return NOERROR;
}

ECode TestFilePrinter::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    Failed(test, failure);
    return NOERROR;
}

ECode TestFilePrinter::EndTest(
    /* [in] */ ITest* test)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    Finished(testStr);
    if (mFailedTests.Find(testStr) == mFailedTests.End()) {
        Passed(testStr);
    }
    mFailedTests.Erase(testStr);
    if (mOutput != NULL) {
        IFlushable::Probe(mOutput)->Flush();
        ICloseable::Probe(mFileOut)->Close();
        mFileOut = NULL;
    }
    return NOERROR;
}

ECode TestFilePrinter::StartTest(
    /* [in] */ ITest* test)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);

    Int32 startIndex = testStr.LastIndexOf("(");
    Int32 endIndex = testStr.LastIndexOf(")");
    String className = testStr.Substring(startIndex + 1, endIndex);
    endIndex = startIndex;
    String methodName = testStr.Substring(0, endIndex);
    String path = PATH + className + "_" + methodName + ".out";
    ECode ec = CFileOutputStream::New(path, (IFileOutputStream**)&mFileOut);
    if (SUCCEEDED(ec)) {
        CDataOutputStream::New(IOutputStream::Probe(mFileOut), (IDataOutput**)&mOutput);
    }
    else {
        Logger::E(mTag, "Create file %s failed(ec = 0x%08x).", path.string(), ec);
    }
    return Started(testStr);
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
