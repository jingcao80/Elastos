
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/CValueAnimator.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/internal/utility/CFastPrintWriter.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/view/CWindowManagerGlobalSessionCallback.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/view/HardwareRenderer.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/view/ViewRootImpl.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Internal::Utility::CFastPrintWriter;
using Elastos::Droid::Internal::Utility::IFastPrintWriter;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::View::EIID_IIWindowSessionCallback;
using Elastos::Droid::View::CWindowManagerGlobalSessionCallback;
using Elastos::Droid::View::HardwareRenderer;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const char* CWindowManagerGlobal::TAG = "WindowManager";

//===========================================================================
//          CWindowManagerGlobal::SystemPropertyUpdaterRunnable
//===========================================================================
CAR_INTERFACE_IMPL(CWindowManagerGlobal::SystemPropertyUpdaterRunnable, Object, IRunnable)

CWindowManagerGlobal::SystemPropertyUpdaterRunnable::SystemPropertyUpdaterRunnable(
    /* [in] */ CWindowManagerGlobal* owner)
    : mOwner(owner)
{
}

ECode CWindowManagerGlobal::SystemPropertyUpdaterRunnable::Run()
{
    AutoLock lock(mOwner->mLock);
    assert(NULL != mOwner);
    assert(NULL != mOwner->mRoots);
    Int32 count = 0;
    mOwner->mRoots->GetSize(&count);
    for (Int32 i = count - 1; i >= 0; --i) {
        AutoPtr<IInterface> temp;
        mOwner->mRoots->Get(i, (IInterface**)&temp);
        AutoPtr<IViewRootImpl> impl = IViewRootImpl::Probe(temp);
        impl->LoadSystemProperties();
    }
    return NOERROR;
}

//===========================================================================
//                          CWindowManagerGlobal
//===========================================================================
AutoPtr<IWindowManagerGlobal> CWindowManagerGlobal::sDefaultWindowManager;
Object CWindowManagerGlobal::sDefaultWindowManagerLock;
AutoPtr<IIWindowManager> CWindowManagerGlobal::sWindowManagerService;
AutoPtr<IWindowSession> CWindowManagerGlobal::sWindowSession;

CAR_INTERFACE_IMPL(CWindowManagerGlobal, Object, IWindowManagerGlobal)

CAR_OBJECT_IMPL(CWindowManagerGlobal)

CWindowManagerGlobal::CWindowManagerGlobal()
{
    CArrayList::New((IArrayList**)&mViews);
    CArrayList::New((IArrayList**)&mRoots);
    CArrayList::New((IArrayList**)&mParams);
    CArrayList::New((IArrayList**)&mDyingViews);
}

ECode CWindowManagerGlobal::constructor()
{
    return NOERROR;
}

AutoPtr<IWindowManagerGlobal> CWindowManagerGlobal::GetInstance()
{
    AutoLock lock(sDefaultWindowManagerLock);
    if (sDefaultWindowManager == NULL) {
        CWindowManagerGlobal::New((IWindowManagerGlobal**)&sDefaultWindowManager);
    }
    return sDefaultWindowManager;
}

AutoPtr<IIWindowManager> CWindowManagerGlobal::GetWindowManagerService()
{
    AutoLock lock(sDefaultWindowManagerLock);
    if (sWindowManagerService == NULL) {
        AutoPtr<IServiceManager> sm;
        CServiceManager::AcquireSingleton((IServiceManager**)&sm);
        AutoPtr<IInterface> obj;
        sm->GetService(String("window"), (IInterface**)&obj);
        sWindowManagerService = IIWindowManager::Probe(obj);
    }
    return sWindowManagerService;
}

AutoPtr<IWindowSession> CWindowManagerGlobal::GetWindowSession()
{
    AutoLock lock(sDefaultWindowManagerLock);
    if (sWindowSession == NULL) {
        //try {
            AutoPtr<IInputMethodManager> imm = CInputMethodManager::GetInstance();
            AutoPtr<IIWindowManager> wm = GetWindowManagerService();

            AutoPtr<IInputMethodClient> client;
            imm->GetClient((IInputMethodClient**)&client);
            AutoPtr<IIInputContext> ctx;
            imm->GetInputContext((IIInputContext**)&ctx);
            AutoPtr<IIWindowSessionCallback> cb;
            CWindowManagerGlobalSessionCallback::New((IIWindowSessionCallback**)&cb);
            wm->OpenSession(cb, client, ctx, (IWindowSession**)&sWindowSession);

            Float animatorScale;
            wm->GetCurrentAnimatorScale(&animatorScale);

            AutoPtr<IValueAnimatorHelper> helper;
            CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
            helper->SetDurationScale(animatorScale);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Failed to open window session", e);
        // }
    }
    return sWindowSession;
}

AutoPtr<IWindowSession> CWindowManagerGlobal::PeekWindowSession()
{
    AutoLock lock(sDefaultWindowManagerLock);
    return sWindowSession;
}

ECode CWindowManagerGlobal::GetViewRootNames(
    /* [out, callee] */ ArrayOf<String>** result)
{
    AutoLock lock(sDefaultWindowManagerLock);
    VALIDATE_NOT_NULL(result);

    Int32 numRoots = 0;
    mRoots->GetSize(&numRoots);
    *result = ArrayOf<String>::Alloc(numRoots);
    for (Int32 i = 0; i < numRoots; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mRoots->Get(i, (IInterface**)&interfaceTmp);
        IViewRootImpl* impl = IViewRootImpl::Probe(interfaceTmp);
        (**result)[i] = GetWindowName(impl);
    }

    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWindowManagerGlobal::GetRootView(
    /* [in] */ const String& name,
    /* [out] */ IView** result)
{
    AutoLock lock(sDefaultWindowManagerLock);
    VALIDATE_NOT_NULL(result);

    Int32 rootSize = 0;
    mRoots->GetSize(&rootSize);
    for (Int32 i = rootSize - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mRoots->Get(i, (IInterface**)&interfaceTmp);
        IViewRootImpl* root = IViewRootImpl::Probe(interfaceTmp);
        if (name.Equals(GetWindowName(root))) {
            root->GetView(result);
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CWindowManagerGlobal::GetWindowManagerService(
    /* [out] */ IIWindowManager** windowManager)
{
    VALIDATE_NOT_NULL(windowManager);
    AutoPtr<IIWindowManager> temp = GetWindowManagerService();
    *windowManager = temp;
    REFCOUNT_ADD(*windowManager);
    return NOERROR;
}

ECode CWindowManagerGlobal::GetWindowSession(
    /* [out] */ IWindowSession** windowSession)
{
    VALIDATE_NOT_NULL(windowSession);
    AutoPtr<IWindowSession> temp = GetWindowSession();
    *windowSession = temp;
    REFCOUNT_ADD(*windowSession);
    return NOERROR;
}

ECode CWindowManagerGlobal::PeekWindowSession(
    /* [out] */ IWindowSession** windowSession)
{
    VALIDATE_NOT_NULL(windowSession);
    AutoPtr<IWindowSession> temp = PeekWindowSession();
    *windowSession = temp;
    REFCOUNT_ADD(*windowSession);
    return NOERROR;
}

ECode CWindowManagerGlobal::AddView(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params,
    /* [in] */ IDisplay* display,
    /* [in] */ IWindow* parentWindow)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(params);
    VALIDATE_NOT_NULL(display);
    VALIDATE_NOT_NULL(parentWindow);

    if (view == NULL) {
        Logger::E(TAG, "view must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (display == NULL) {
        Logger::E(TAG, "display must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (IWindowManagerLayoutParams::Probe(params) == NULL) {
        Logger::E(TAG, "Params must be WindowManager.LayoutParams");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IWindowManagerLayoutParams* wmlp = IWindowManagerLayoutParams::Probe(params);
    CWindowManagerLayoutParams* wparams =(CWindowManagerLayoutParams*)wmlp;
    if (parentWindow != NULL) {
        if (wparams->mType == IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR) {
            // TODO: TYPE_NAVIGATION_BAR
            AutoPtr<IBinder> binder;
            view->GetWindowToken((IBinder**)&binder);
            wparams->SetToken(binder);
        }
        else {
            parentWindow->AdjustLayoutParamsForSubWindow(wmlp);
        }
    }
    else {
        // If there's no parent and we're running on L or above (or in the
        // system context), assume we want hardware acceleration.
        AutoPtr<IContext> context;
        view->GetContext((IContext**)&context);
        if (context != NULL) {
            AutoPtr<IApplicationInfo> info;
            context->GetApplicationInfo((IApplicationInfo**)&info);
            Int32 version;
            info->GetTargetSdkVersion(&version);
            if (version >= Build::VERSION_CODES::LOLLIPOP) {
                wparams->mFlags |= IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
            }
        }
    }

    AutoPtr<IViewRootImpl> root;
    AutoPtr<IView> panelParentView;
    {
        AutoLock lock(mLock);

        // Start watching for system property changes.
        if (mSystemPropertyUpdater == NULL) {
            mSystemPropertyUpdater = new SystemPropertyUpdaterRunnable(this);
            AutoPtr<ISystemProperties> properties;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&properties);
            properties->AddChangeCallback(mSystemPropertyUpdater);
        }

        Int32 index;
        FindViewLocked(view, FALSE, &index);
        if (index >= 0) {
            Boolean contains;
            mDyingViews->Contains(view, &contains);
            if (contains) {
                // Don't wait for MSG_DIE to make it's way through root's queue.
                AutoPtr<IInterface> temp;
                mRoots->Get(index, (IInterface**)&temp);
                IViewRootImpl::Probe(temp)->DoDie();
            }
            else {
                Logger::E(TAG, "View 0x%08x has already been added to the window manager.", view);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            // The previous removeView() had not completed executing. Now it has.
        }

        // If this is a panel window, then find the window it is being
        // attached to for future reference.
        if (wparams->mType >= IWindowManagerLayoutParams::FIRST_SUB_WINDOW &&
            wparams->mType <= IWindowManagerLayoutParams::LAST_SUB_WINDOW) {
            Int32 count = 0;
            mRoots->GetSize(&count);
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IInterface> temp;
                mRoots->Get(i, (IInterface**)&temp);
                ViewRootImpl* impl = (ViewRootImpl*)IViewRootImpl::Probe(temp);
                if (IBinder::Probe(impl->mWindow) == wparams->mToken) {
                    AutoPtr<IInterface> it;
                    mViews->Get(i, (IInterface**)&it);
                    panelParentView = IView::Probe(it);;
                }
            }
        }

        AutoPtr<IContext> context;
        view->GetContext((IContext**)&context);
        AutoPtr<ViewRootImpl> vri = new ViewRootImpl();
        vri->constructor(context, display);
        root = (IViewRootImpl*)vri.Get();

        view->SetLayoutParams(params);

        mViews->Add(view);
        mRoots->Add(root);
        mParams->Add(wmlp);
    }

    // do this last because it fires off messages to start doing things
    ECode ec = root->SetView(view, wmlp, panelParentView);
    if (FAILED(ec)) {
        // BadTokenException or InvalidDisplayException, clean up.
        AutoLock lock(mLock);
        Int32 index;
        FindViewLocked(view, FALSE, &index);
        if (index >= 0) {
            RemoveViewLocked(index, TRUE);
        }
    }
    return ec;
}

ECode CWindowManagerGlobal::UpdateViewLayout(
    /* [in] */ IView* view,
    /* [in] */ IViewGroupLayoutParams* params)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(params);

    if (view == NULL) {
        Logger::E(TAG, "view must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!IWindowManagerLayoutParams::Probe(params)) {
        Logger::E(TAG, "Params must be WindowManager.LayoutParams");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IWindowManagerLayoutParams* wparams = IWindowManagerLayoutParams::Probe(params);
    view->SetLayoutParams(IViewGroupLayoutParams::Probe(wparams));

    AutoLock lock(mLock);
    Int32 index;
    FAIL_RETURN(FindViewLocked(view, TRUE, &index));

    AutoPtr<IInterface> temp;
    mRoots->Get(index, (IInterface**)&temp);
    IViewRootImpl* impl = IViewRootImpl::Probe(temp);
    mParams->Remove(index);
    mParams->Add(index, TO_IINTERFACE(wparams));
    impl->SetLayoutParams(wparams, FALSE);
    return NOERROR;
}

ECode CWindowManagerGlobal::RemoveView(
    /* [in] */ IView* view,
    /* [in] */ Boolean immediate)
{
    VALIDATE_NOT_NULL(view);
    if (view == NULL) {
        Logger::E(TAG, "view must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mLock);
    Int32 index;
    FAIL_RETURN(FindViewLocked(view, TRUE, &index));
    AutoPtr<IInterface> temp;
    mRoots->Get(index, (IInterface**)&temp);
    IViewRootImpl* impl = IViewRootImpl::Probe(temp);
    AutoPtr<IView> curView;
    impl->GetView((IView**)&curView);
    RemoveViewLocked(index, immediate);
    if (curView.Get() == view) {
        return NOERROR;
    }

    Logger::E(TAG, "Calling with view 0x%08x but the ViewAncestor"
        " is attached to 0x%08x", view, curView.Get());
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CWindowManagerGlobal::CloseAll(
    /* [in] */ IBinder* token,
    /* [in] */ const String& who,
    /* [in] */ const String& what)
{
    VALIDATE_NOT_NULL(token);
    AutoLock lock(mLock);
    Logger::I(TAG, "Closing all windows of %p", token);

    Int32 count = 0;
    mViews->GetSize(&count);
    for (Int32 i = count - 1; i >= 0; --i) {
        //Logger::I("foo", "@ " + i + " token " + (*mParams)[i].token
        //        + " view " + (*mRoots)[i].getView());
        AutoPtr<IInterface> temp;
        mParams->Get(i, (IInterface**)&temp);
        IWindowManagerLayoutParams* param = IWindowManagerLayoutParams::Probe(temp);
        AutoPtr<IBinder> paramToken;
        param->GetToken((IBinder**)&paramToken);
        if (NULL == token || token == paramToken) {
            temp = NULL;
            mRoots->Get(i, (IInterface**)&temp);
            IViewRootImpl* root = IViewRootImpl::Probe(temp);

            Logger::I("foo", "Force closing %d" + (Int32)root);
            if (who != NULL) {
                // WindowLeaked leak = new WindowLeaked(
                //         what + " " + who + " has leaked window "
                //         + root.getView() + " that was originally added here");
                // leak.setStackTrace(root.getLocation().getStackTrace());
                // Log.e(TAG, leak.getMessage(), leak);
                Logger::E(TAG, "%s %s has leaked window ", what.string(), who.string());
            }

            RemoveViewLocked(i, FALSE);
        }
    }
    return NOERROR;
}

void CWindowManagerGlobal::RemoveViewLocked(
    /* [in] */ Int32 index,
    /* [in] */ Boolean immediate)
{
    AutoPtr<IInterface> temp;
    mRoots->Get(index, (IInterface**)&temp);
    IViewRootImpl* root = IViewRootImpl::Probe(temp);
    AutoPtr<IView> view;
    root->GetView((IView**)&view);

    if (view != NULL) {
        AutoPtr<IInputMethodManager> imm = CInputMethodManager::GetInstance();
        if (imm != NULL) {
            temp = NULL;
            mViews->Get(index, (IInterface**)&temp);
            IView* v = IView::Probe(temp);
            AutoPtr<IBinder> binder;
            v->GetWindowToken((IBinder**)&binder);
            imm->WindowDismissed(binder);
        }
    }

    Boolean deferred;
    root->Die(immediate, &deferred);

    if (view != NULL) {
        View* v = (View*)view.Get();
        v->AssignParent(NULL);
        if (deferred) {
            mDyingViews->Add(view.Get());
        }
    }
}

ECode CWindowManagerGlobal::DoRemoveView(
    /* [in] */ IViewRootImpl* root)
{
    VALIDATE_NOT_NULL(root);
    {
        AutoLock lock(mLock);
        Int32 index = 0;
        mRoots->IndexOf(root, &index);
        if (index >= 0) {
            mRoots->Remove(index);
            mParams->Remove(index);
            AutoPtr<IInterface> temp;
            mViews->Remove(index, (IInterface**)&temp);
            mDyingViews->Remove(IView::Probe(temp));
        }
    }

    Boolean available;
    if (HardwareRenderer::sTrimForeground && (HardwareRenderer::IsAvailable(&available), available)) {
        DoTrimForeground();
    }
    return NOERROR;
}

ECode CWindowManagerGlobal::FindViewLocked(
    /* [in] */ IView* view,
    /* [in] */ Boolean required,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 index = 0;
    mViews->IndexOf(view, &index);
    if (required && index < 0) {
        Logger::D("CWindowManagerGlobal", "view = %d not attached to window manager", (Int32)view);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = index;
    return NOERROR;
}

Boolean CWindowManagerGlobal::ShouldDestroyEglContext(
    /* [in] */ Int32 trimLevel)
{
    // On low-end gfx devices we trim when memory is moderate;
    // on high-end devices we do this when low.
    if (trimLevel >= IComponentCallbacks2::TRIM_MEMORY_COMPLETE) {
        return TRUE;
    }
    if (trimLevel >= IComponentCallbacks2::TRIM_MEMORY_MODERATE
            && !CActivityManager::IsHighEndGfx()) {
        return TRUE;
    }
    return FALSE;
}

ECode CWindowManagerGlobal::TrimMemory(
    /* [in] */ Int32 level)
{
    Boolean available;
    if (HardwareRenderer::IsAvailable(&available), available) {
        if (ShouldDestroyEglContext(level)) {
            // Destroy all hardware surfaces and resources associated to
            // known windows
            {
                AutoLock lock(mLock);
                Int32 count;
                mRoots->GetSize(&count);
                for (int i = count - 1; i >= 0; --i) {
                    AutoPtr<IInterface> temp;
                    mRoots->Get(i, (IInterface**)&temp);
                    IViewRootImpl::Probe(temp)->DestroyHardwareResources();
                }
            }
            // Force a full memory flush
            level = IComponentCallbacks2::TRIM_MEMORY_COMPLETE;
        }

        HardwareRenderer::TrimMemory(level);
        if (HardwareRenderer::sTrimForeground) {
            DoTrimForeground();
        }
    }
    return NOERROR;
}

void CWindowManagerGlobal::TrimForeground()
{
    Boolean available;
    if (HardwareRenderer::sTrimForeground && (HardwareRenderer::IsAvailable(&available), available)) {
        AutoPtr<IWindowManagerGlobal> wm = GetInstance();
        ((CWindowManagerGlobal*)wm.Get())->DoTrimForeground();
    }
}

void CWindowManagerGlobal::DoTrimForeground()
{
    Boolean hasVisibleWindows = FALSE;
    {
        AutoLock lock(mLock);
        Int32 size;
        mRoots->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; --i) {
            AutoPtr<IInterface> temp;
            mRoots->Get(i, (IInterface**)&temp);
            IViewRootImpl* root = IViewRootImpl::Probe(temp);
            ViewRootImpl* impl = (ViewRootImpl*)root;

            Int32 visiblity = 0;
            root->GetHostVisibility(&visiblity);

            if (NULL != impl->mView && visiblity == IView::VISIBLE
                    && NULL != impl->mAttachInfo->mHardwareRenderer) {
                hasVisibleWindows = TRUE;
            }
            else {
                root->DestroyHardwareResources();
            }
        }
    }

    if (!hasVisibleWindows) {
        HardwareRenderer::TrimMemory(IComponentCallbacks2::TRIM_MEMORY_COMPLETE);
    }
}

ECode CWindowManagerGlobal::DumpGfxInfo(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IFileOutputStream> fout;
    CFileOutputStream::New(fd, (IFileOutputStream**)&fout);

    AutoPtr<IFastPrintWriter> fastPrintWriter;
    IOutputStream* outputStream = IOutputStream::Probe(fout);
    CFastPrintWriter::New(outputStream, (IFastPrintWriter**)&fastPrintWriter);
    IPrintWriter* pw = IPrintWriter::Probe(fastPrintWriter);
    //try {
        {
            AutoLock lock(mLock);
            if (mViews != NULL) {
                pw->Println(String("Profile data in ms:"));
                Int32 count = 0;
                mViews->GetSize(&count);
                String name;
                for (Int32 i = 0; i < count; ++i) {
                    AutoPtr<IInterface> interfaceTmp;
                    mRoots->Get(i, (IInterface**)&interfaceTmp);
                    IViewRootImpl* root = IViewRootImpl::Probe(interfaceTmp);
                    name = GetWindowName(root);

                    AutoPtr<ICharSequence> charSequenceTmp;
                    CString::New(name, (ICharSequence**)&charSequenceTmp);
                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                    args->Set(0, TO_IINTERFACE(charSequenceTmp));
                    pw->Printf(String("\n\t%s"), args);

                    AutoPtr<IView> viewTmp;
                    root->GetView((IView**)&viewTmp);
                    View* viewTmp1 = (View*)viewTmp.Get();
                    AutoPtr<IHardwareRenderer> renderer = viewTmp1->mAttachInfo->mHardwareRenderer;
                    if (renderer != NULL) {
                        renderer->DumpGfxInfo(pw, fd);
                    }
                }

                pw->Println(String("\nView hierarchy:\n"));

                Int32 viewsCount = 0;
                Int32 displayListsSize = 0;
                AutoPtr< ArrayOf<Int32> > info = ArrayOf<Int32>::Alloc(2);

                for (Int32 i = 0; i < count; ++i) {
                    AutoPtr<IInterface> interfaceTmp;
                    mRoots->Get(i, (IInterface**)&interfaceTmp);
                    IViewRootImpl* root = IViewRootImpl::Probe(interfaceTmp);
                    root->DumpGfxInfo(info);

                    String name = GetWindowName(root);
                    AutoPtr<ICharSequence> charSequenceTmp;
                    CString::New(name, (ICharSequence**)&charSequenceTmp);

                    AutoPtr<IInteger32> info0;
                    CInteger32::New((*info)[0], (IInteger32**)&info0);

                    AutoPtr<IInteger32> info1;
                    CInteger32::New((*info)[1], (IInteger32**)&info1);

                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
                    args->Set(0, TO_IINTERFACE(charSequenceTmp));
                    args->Set(1, TO_IINTERFACE(info0));
                    args->Set(2, TO_IINTERFACE(info1));

                    pw->Printf(String("  %s\n  %d views, %.2f kB of display lists"), args);
                    pw->Printf(String("\n\n"), NULL);

                    viewsCount += (*info)[0];
                    displayListsSize += (*info)[1];
                }

                AutoPtr<IInteger32> countTmp;
                CInteger32::New(count, (IInteger32**)&countTmp);
                AutoPtr< ArrayOf<IInterface*> > countArgs = ArrayOf<IInterface*>::Alloc(1);
                countArgs->Set(0, TO_IINTERFACE(countTmp));
                pw->Printf(String("\nTotal ViewRootImpl: %d\n"), countArgs);

                AutoPtr<IInteger32> viewsCountTmp;
                CInteger32::New(viewsCount, (IInteger32**)&viewsCountTmp);
                AutoPtr< ArrayOf<IInterface*> > viewsCountArgs = ArrayOf<IInterface*>::Alloc(1);
                viewsCountArgs->Set(0, TO_IINTERFACE(viewsCountTmp));
                pw->Printf(String("Total Views:        %d\n"), viewsCountArgs);

                AutoPtr<IFloat> displayListsSizeTmp;
                CFloat::New(displayListsSize / 1024.0f, (IFloat**)&displayListsSizeTmp);
                AutoPtr< ArrayOf<IInterface*> > displayListsSizeArgs = ArrayOf<IInterface*>::Alloc(1);
                displayListsSizeArgs->Set(0, TO_IINTERFACE(displayListsSizeTmp));
                pw->Printf(String("Total DisplayList:  %.2f kB\n\n"), displayListsSizeArgs);
            }
        }
    //} finally {
        IFlushable* flushTmp = IFlushable::Probe(pw);
        flushTmp->Flush();
    //}
    return NOERROR;
}

String CWindowManagerGlobal::GetWindowName(
    /* [in] */ IViewRootImpl* root)
{
    AutoPtr<ICharSequence> title;
    ((ViewRootImpl*)root)->mWindowAttributes->GetTitle((ICharSequence**)&title);
    String str;
    title->ToString(&str);
    Int32 code = Object::GetHashCode(root);
    return str + String("/ViewRootImpl@") + StringUtils::ToString(code);
}

ECode CWindowManagerGlobal::SetStoppedState(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean stopped)
{
    AutoLock lock(mLock);
    if (mViews != NULL) {
        Int32 count = 0;
        mViews->GetSize(&count);
        for (Int32 i=0; i < count; ++i) {
            AutoPtr<IInterface> temp;
            mParams->Get(i, (IInterface**)&temp);
            IWindowManagerLayoutParams* param = IWindowManagerLayoutParams::Probe(temp);
            AutoPtr<IBinder> paramToken;
            param->GetToken((IBinder**)&paramToken);

            if (NULL == token || TO_IINTERFACE(token) == TO_IINTERFACE(paramToken)) {
                temp = NULL;
                mRoots->Get(i, (IInterface**)&temp);
                IViewRootImpl::Probe(temp)->SetStopped(stopped);

            }
        }
    }
    return NOERROR;
}

ECode CWindowManagerGlobal::ReportNewConfiguration(
    /* [in] */ IConfiguration* config)
{
    AutoLock lock(mLock);
    if (mViews != NULL) {
        Int32 count = 0;
        mViews->GetSize(&count);
        AutoPtr<IConfiguration> newConfig;
        CConfiguration::New(config, (IConfiguration**)&newConfig);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> temp;
            mRoots->Get(i, (IInterface**)&temp);
            IViewRootImpl::Probe(temp)->RequestUpdateConfiguration(newConfig);
        }
    }
    return NOERROR;
}

/** @hide */
ECode CWindowManagerGlobal::ChangeCanvasOpacity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean opaque)
{
    if (NULL == token) {
        return NOERROR;
    }

    AutoLock lock(mLock);
    Int32 count = 0;
    mParams->GetSize(&count);
    for (Int32 i = count - 1; i >= 0; --i) {
        AutoPtr<IInterface> temp;
        mParams->Get(i, (IInterface**)&temp);
        IWindowManagerLayoutParams* param = IWindowManagerLayoutParams::Probe(temp);
        CWindowManagerLayoutParams* wm = (CWindowManagerLayoutParams*)param;
        if (TO_IINTERFACE(token) == TO_IINTERFACE(wm->mToken)) {
            temp = NULL;
            mRoots->Get(i, (IInterface**)&temp);
            IViewRootImpl* impl = IViewRootImpl::Probe(temp);
            impl->ChangeCanvasOpacity(opaque);
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
