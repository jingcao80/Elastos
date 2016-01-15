
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/utility/DumpUtils.h"
#include "elastos/droid/internal/utility/CFastPrintWriter.h"

using Elastos::Droid::Internal::Utility::CFastPrintWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

DumpUtils::DumpRunnable::DumpRunnable(
    /* [in] */ IDump* dump,
    /* [in] */ IStringWriter* sw)
    : mDump(dump)
    , mSw(sw)
{
}

ECode DumpUtils::DumpRunnable::Run()
{
    AutoPtr<IPrintWriter> lpw;
    CFastPrintWriter::New(IWriter::Probe(mSw), (IPrintWriter**)&lpw);
    mDump->Dump(lpw);
    ICloseable::Probe(lpw)->Close();
    return NOERROR;
}

ECode DumpUtils::DumpAsync(
    /* [in] */ IHandler* handler,
    /* [in] */ IDump* dump,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 timeout)
{
    AutoPtr<IStringWriter> sw;
    CStringWriter::New((IStringWriter**)&sw);
    AutoPtr<DumpRunnable> runnable = new DumpRunnable(dump, sw);
    Boolean res;
    handler->RunWithScissors(runnable, timeout, &res);
    if (res) {
        String str;
        IObject::Probe(sw)->ToString(&str);
        return pw->Print(str);
    }
    else {
        return pw->Println(String("... timed out"));
    }
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
