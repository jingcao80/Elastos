
#include "elastos/droid/view/ContextThemeWrapperHolder.h"
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

// CAR_INTERFACE_IMPL(ContextThemeWrapperHolder, ContextThemeWrapper, IContextThemeWrapperHolder)

ContextThemeWrapperHolder::ContextThemeWrapperHolder()
    : mMemoryLeakTarget(FALSE)
    , mMemoryCount(0)
{
    Logger::I("ContextThemeWrapperHolder", " >> Create ContextThemeWrapperHolder: %p", this);
}

ContextThemeWrapperHolder::~ContextThemeWrapperHolder()
{
    if (mMemoryLeakTarget) {
        Logger::I("ContextThemeWrapperHolder", " >> Destroy ContextThemeWrapperHolder: %p", this);
    }
}

PInterface ContextThemeWrapperHolder::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IContextThemeWrapperHolder*)this;
    }
    else if (riid == EIID_IContextThemeWrapperHolder) {
        return (IContextThemeWrapperHolder*)this;
    }
    return ContextThemeWrapper::Probe(riid);
}

ECode ContextThemeWrapperHolder::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (NULL == pIID) return E_INVALID_ARGUMENT;

    if (pObject == (IInterface *)(IContextThemeWrapperHolder *)this) {
        *pIID = EIID_IContextThemeWrapperHolder;
        return NOERROR;
    }

    return ContextThemeWrapper::GetInterfaceID(pObject, pIID);
}

UInt32 ContextThemeWrapperHolder::AddRef()
{
    UInt32 count = Object::AddRef();
    // if (mMemoryLeakTarget) {
    //     android::CallStack stack;
    //     stack.update();
    //     String backtrace(stack.toString("").string());
    //     Logger::I("ContextThemeWrapperHolder", "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    //     Logger::I("ContextThemeWrapperHolder", ">> AddRef: %p, refcount: %d, callstack:\n%s", this, count, backtrace.string());
    //     Logger::I("ContextThemeWrapperHolder", "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    // }
    return count;
}

UInt32 ContextThemeWrapperHolder::Release()
{
    if (mMemoryLeakTarget) {
        Thread::Sleep(10);
        if (GetStrongCount() < 3) {
            Logger::I("ContextThemeWrapperHolder", " >> Release: %p, refcount: %d", this, GetStrongCount());
        }

        // android::CallStack stack;
        // stack.update();
        // String backtrace(stack.toString("").string());
        // Logger::I("ContextThemeWrapperHolder", "-------------------------------------------------------");
        // Logger::I("ContextThemeWrapperHolder", ">> Release: %p, refcount: %d, callstack:\n%s", this, GetStrongCount(), backtrace.string());
        // Logger::I("ContextThemeWrapperHolder", "-------------------------------------------------------");
    }

    Int32 count = Object::Release();
    // if (mMemoryLeakTarget) {
    //     if (count == 1 && (++mMemoryCount == 3)) {
    //         Release();
    //     }
    // }

    return count;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
