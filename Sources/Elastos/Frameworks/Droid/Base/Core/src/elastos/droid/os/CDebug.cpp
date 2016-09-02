
#include "elastos/droid/os/CDebug.h"
#include "elastos/droid/os/Debug.h"
#include <Elastos.CoreLibrary.IO.h>

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CDebug, Singleton, IDebug)

CAR_SINGLETON_IMPL(CDebug)

ECode CDebug::DumpHeap(
    /* [in] */ IFileDescriptor* fd)
{
    return Debug::DumpHeap(fd);
}

ECode CDebug::GetMemInfo(
    /* [out] */ ArrayOf<Int64>* outSizes)
{
    return Debug::GetMemInfo(outSizes);
}

ECode CDebug::GetMemoryInfo(
    /* [out] */ IDebugMemoryInfo** memoryInfo)
{
    return Debug::GetMemoryInfo(memoryInfo);
}

ECode CDebug::GetMemoryInfo(
    /* [in] */ Int32 pid,
    /* [out] */ IDebugMemoryInfo** memoryInfo)
{
    return Debug::GetMemoryInfo(pid, memoryInfo);
}

ECode CDebug::GetPss(
    /* [out] */ Int64* pss)
{
    return Debug::GetPss(pss);
}

ECode CDebug::GetPss(
    /* [in] */ Int32 pid,
    /* [out, callee] */ ArrayOf<Int64>** outUss,
    /* [out] */ Int64* pss)
{
    return Debug::GetPss(pid, outUss, pss);
}

ECode CDebug::DumpNativeBacktraceToFile(
    /* [in] */ Int32 pid,
    /* [in] */ const String& file)
{
    return Debug::DumpNativeBacktraceToFile(pid, file);
}

ECode CDebug::WaitForDebugger()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::WaitForDebugger(
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::IsDebuggerConnected(
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ChangeDebugPort(
    /* [in] */ Int32 port)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartNativeTracing()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StopNativeTracing()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::EnableEmulatorTraceOutput()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracing()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracing(
    /* [in] */ const String& traceName)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracing(
    /* [in] */ const String& traceName,
    /* [in] */ Int32 bufferSize)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracing(
    /* [in] */ const String& traceName,
    /* [in] */ Int32 bufferSize,
    /* [in] */ Int32 flags)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracingSampling(
    /* [in] */ const String& traceName,
    /* [in] */ Int32 bufferSize,
    /* [in] */ Int32 intervalUs)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartMethodTracing(
    /* [in] */ const String& traceName,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 bufferSize,
    /* [in] */ Int32 flags)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StopMethodTracing()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ThreadCpuTimeNanos(
    /* [out] */ Int64* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StartAllocCounting()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::StopAllocCounting()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalAllocCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalAllocCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalAllocSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalAllocSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalFreedCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalFreedCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalFreedSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalFreedSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalGcInvocationCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalGcInvocationCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalClassInitCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalClassInitCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalClassInitTime(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalClassInitTime()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalExternalAllocCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalExternalAllocCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalExternalAllocSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalExternalAllocSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalExternalFreedCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalExternalFreedCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetGlobalExternalFreedSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetGlobalExternalFreedSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetThreadAllocCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetThreadAllocCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetThreadAllocSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetThreadAllocSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetThreadExternalAllocCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetThreadExternalAllocCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetThreadExternalAllocSize(
    /* [out] */ Int32* size)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetThreadExternalAllocSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetThreadGcInvocationCount(
    /* [out] */ Int32* count)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::ReSetThreadGcInvocationCount()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetNativeHeapSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetNativeHeapAllocatedSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetNativeHeapFreeSize()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::SetAllocationLimit(
    /* [in] */ Int32 limit,
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::SetGlobalAllocationLimit(
    /* [in] */ Int32 limit,
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::PrintLoadedClasses(
    /* [in] */ Int32 flags)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetLoadedClassCount(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::DumpHprofData(
    /* [in] */ const String& fileName)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::DumpHprofData(
    /* [in] */ const String& fileName,
    /* [in] */ IFileDescriptor* fd)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetBinderSentTransactions(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetBinderReceivedTransactions(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetBinderLocalObjectCount(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetBinderProxyObjectCount(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::GetBinderDeathObjectCount(
    /* [out] */ Int32* result)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::SetFieldsOn(
    /* [in] */ const InterfaceID& cl,
    /* [in] */ Boolean partial)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CDebug::DumpService(
    /* [in] */ const String& name,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<String>* args,
    /* [out] */ Boolean * result)
{
    assert(0 && "TODO");
    return NOERROR;
}

}
}
}

