
#ifndef __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_H__
#define __ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_View_CWindowManagerGlobal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IIWindowSessionCallback;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace View {

class ViewRootImpl;

CarClass(CWindowManagerGlobal)
    , public Object
    , public IWindowManagerGlobal
{
private:
    class SystemPropertyUpdaterRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SystemPropertyUpdaterRunnable(
            /* [in] */ CWindowManagerGlobal* owner);

        CARAPI Run();

    private:
        CWindowManagerGlobal* mOwner;
    };

    class InnerWindowSessionCallbackStub
        : public Object
        , public IIWindowSessionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerWindowSessionCallbackStub();

        CARAPI OnAnimatorScaleChanged(
            /* [in] */ Float scale);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWindowManagerGlobal();

    CARAPI constructor();

    static AutoPtr<IWindowManagerGlobal> GetInstance();

    static AutoPtr<IIWindowManager> GetWindowManagerService();

    static AutoPtr<IWindowSession> GetWindowSession();

    static AutoPtr<IWindowSession> PeekWindowSession();

    CARAPI GetWindowManagerService(
        /* [out] */ IIWindowManager** windowManager);

    CARAPI GetWindowSession(
        /* [out] */ IWindowSession** windowSession);

    CARAPI PeekWindowSession(
        /* [out] */ IWindowSession** windowSession);

    CARAPI GetViewRootNames(
        /* [out, callee] */ ArrayOf<String>** resultz);

    CARAPI GetRootView(
        /* [in] */ const String& name,
        /* [out] */ IView** view);

    CARAPI AddView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params,
        /* [in] */ IDisplay* display,
        /* [in] */ IWindow* parentWindow);

    CARAPI UpdateViewLayout(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI RemoveView(
        /* [in] */ IView* view,
        /* [in] */ Boolean immediate);

    CARAPI CloseAll(
        /* [in] */ IBinder* token,
        /* [in] */ const String& who,
        /* [in] */ const String& what);

    CARAPI SetStoppedState(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean stopped);

    CARAPI ReportNewConfiguration(
        /* [in] */ IConfiguration* config);

    CARAPI DoRemoveView(
        /* [in] */ IViewRootImpl* root);

    CARAPI ChangeCanvasOpacity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean opaque);

    static CARAPI_(void) TrimForeground();

    static CARAPI_(Boolean) ShouldDestroyEglContext(
        /* [in] */ Int32 trimLevel);

private:
    CARAPI_(void) RemoveViewLocked(
        /* [in] */ Int32 index,
        /* [in] */ Boolean immediate);

    ECode FindViewLocked(
        /* [in] */ IView* view,
        /* [in] */ Boolean required,
        /* [out] */ Int32* result);

    CARAPI TrimMemory(
        /* [in] */ Int32 level);

    CARAPI_(void) DoTrimForeground();

    CARAPI DumpGfxInfo(
        /* [in] */ IFileDescriptor* fd);

    static String GetWindowName(
        /* [in] */ IViewRootImpl* root);

private:
    static const char* TAG;

    static AutoPtr<IWindowManagerGlobal> sDefaultWindowManager;
    static Object sDefaultWindowManagerLock;
    static AutoPtr<IIWindowManager> sWindowManagerService;
    static AutoPtr<IWindowSession> sWindowSession;

    Object mLock;

    AutoPtr<IArrayList> mViews;
    AutoPtr<IArrayList> mRoots;
    AutoPtr<IArrayList> mParams;
    AutoPtr<IArrayList> mDyingViews;
    AutoPtr<IRunnable> mSystemPropertyUpdater;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CWINDOWMANAGERGLOBAL_H__
