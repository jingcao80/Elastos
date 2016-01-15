
#ifndef __ELASTOS_DROID_WEBKIT_BASE_JAVAHANDLERTHREAD_H__
#define __ELASTOS_DROID_WEBKIT_BASE_JAVAHANDLERTHREAD_H__

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * This class is an internal detail of the native counterpart.
 * It is instantiated and owned by the native object.
 */
//@JNINamespace("base::android")
class CppHandlerThread
    : public Object
{
private:
    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        InnerRunnable(
            /* [in] */ CppHandlerThread* owner,
            /* [in] */ const Int64 nativeThread,
            /* [in] */ const Int64 nativeEvent);

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI Run();

    private:
        CppHandlerThread* mOwner;
        const Int64 mNativeThread;
        const Int64 mNativeEvent;
    };

public:
    static CARAPI_(void*) ElaCppHandlerThreadCallback_Init();

private:
    static CARAPI_(void) Start(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeThread,
        /* [in] */ Int64 nativeEvent);

    CppHandlerThread(
        /* [in] */ const String& name);

    //@CalledByNative return CppHandlerThread
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ const String& name);

    //@CalledByNative
    CARAPI_(void) Start(
        /* [in] */ const Int64 nativeThread,
        /* [in] */ const Int64 nativeEvent);

    CARAPI_(void) NativeInitializeThread(
        /* [in] */ Int64 nativeCppHandlerThread,
        /* [in] */ Int64 nativeEvent);

public:
    AutoPtr<IHandlerThread> mThread;
};

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_BASE_JAVAHANDLERTHREAD_H__
