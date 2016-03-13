#include "elastos/droid/app/Presentation.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/WindowManagerImpl.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::WindowManagerImpl;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;

namespace Elastos {
namespace Droid {
namespace App {

const String Presentation::TAG("Presentation");

//======================================================================================
// Presentation::PresentationDisplayListener
//======================================================================================
CAR_INTERFACE_IMPL(Presentation::PresentationDisplayListener, Object, IDisplayListener);

ECode Presentation::PresentationDisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

ECode Presentation::PresentationDisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    Int32 id;
    mHost->mDisplay->GetDisplayId(&id);
    if (displayId == id) {
        mHost->HandleDisplayRemoved();
    }
    return NOERROR;
}

ECode Presentation::PresentationDisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    Int32 id;
    mHost->mDisplay->GetDisplayId(&id);
    if (displayId == id) {
        mHost->HandleDisplayChanged();
    }
    return NOERROR;
}

//======================================================================================
// Presentation::PresentationHandler
//======================================================================================
Presentation::PresentationHandler::PresentationHandler(
    /* [in] */ Presentation* host)
    : Handler(FALSE)
    , mHost(host)
{
}

ECode Presentation::PresentationHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IPresentation::MSG_CANCEL:
            mHost->Cancel();
            break;
    }
    return NOERROR;
}

//======================================================================================
// Presentation::PresentationContextThemeWrapper
//======================================================================================

Presentation::PresentationContextThemeWrapper::PresentationContextThemeWrapper()
{}

ECode Presentation::PresentationContextThemeWrapper::constructor(
    /* [in] */ IContext* base,
    /* [in] */ Int32 theme,
    /* [in] */ IWindowManager* displayWindowManager)
{
    ContextThemeWrapper::constructor(base, theme);
    mDisplayWindowManager = displayWindowManager;
    return NOERROR;
}

ECode Presentation::PresentationContextThemeWrapper::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    if (name.Equals(IContext::WINDOW_SERVICE)) {
        *object = mDisplayWindowManager;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    return ContextThemeWrapper::GetSystemService(name, object);
}

//======================================================================================
// Presentation
//======================================================================================
Presentation::Presentation()
{
    mDisplayListener = new PresentationDisplayListener(this);
    mHandler = new PresentationHandler(this);
}

Presentation::~Presentation()
{
}

ECode Presentation::constructor(
    /* [in] */ IContext * outerContext,
    /* [in] */ IDisplay * display)
{
    return constructor(outerContext, display, 0);
}

ECode Presentation::constructor(
    /* [in] */ IContext * outerContext,
    /* [in] */ IDisplay * display,
    /* [in] */ Int32 theme)
{
    Dialog::constructor(CreatePresentationContext(outerContext, display, theme), theme, FALSE);

    mDisplay = display;

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IInterface> service;
    FAIL_RETURN(ctx->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&service))
    mDisplayManager = IDisplayManager::Probe(service);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->SetGravity(IGravity::FILL);
    SetCanceledOnTouchOutside(FALSE);
    return NOERROR;
}

ECode Presentation::GetDisplay(
    /* [out] */ IDisplay **display)
{
    *display = mDisplay;
    REFCOUNT_ADD(*display)
    return NOERROR;
}

ECode Presentation::GetResources(
    /* [out] */ IResources **resources)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    return ctx->GetResources(resources);
}

ECode Presentation::OnDisplayRemoved()
{
    return NOERROR;
}

ECode Presentation::OnDisplayChanged()
{
    return NOERROR;
}

/**
 * Inherited from {@link Dialog#show}. Will throw
 * {@link android.view.WindowManager.InvalidDisplayException} if the specified secondary
 * {@link Display} can't be found.
 */
//@Override
// ECode Presentation::Show()
// {
//     return Dialog::Show();
// }

ECode Presentation::OnStart()
{
    Dialog::OnStart();
    mDisplayManager->RegisterDisplayListener(mDisplayListener, mHandler);

    // Since we were not watching for display changes until just now, there is a
    // chance that the display metrics have changed.  If so, we will need to
    // dismiss the presentation immediately.  This case is expected
    // to be rare but surprising, so we'll write a log message about it.
    if (!IsConfigurationStillValid()) {
        Logger::I(TAG, "Presentation is being immediately dismissed because the display metrics have changed since it was created.");
        Boolean result;
        mHandler->SendEmptyMessage(IPresentation::MSG_CANCEL, &result);
    }
    return NOERROR;
}

ECode Presentation::OnStop()
{
    mDisplayManager->UnregisterDisplayListener(mDisplayListener);
    Dialog::OnStop();
    return NOERROR;
}

void Presentation::HandleDisplayRemoved()
{
    OnDisplayRemoved();
    Cancel();
}

void Presentation::HandleDisplayChanged()
{
    OnDisplayChanged();

    // We currently do not support configuration changes for presentations
    // (although we could add that feature with a bit more work).
    // If the display metrics have changed in any way then the current configuration
    // is invalid and the application must recreate the presentation to get
    // a new context.
    if (!IsConfigurationStillValid()) {
        Cancel();
    }
}

Boolean Presentation::IsConfigurationStillValid()
{
    AutoPtr<IDisplayMetrics> dm;
    CDisplayMetrics::New((IDisplayMetrics**)&dm);
    mDisplay->GetMetrics(dm);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm2;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm2);
    Boolean result;
    dm->EqualsPhysical(dm2, &result);
    return result;
}

AutoPtr<IContext> Presentation::CreatePresentationContext(
    /* [in] */ IContext* outerContext,
    /* [in] */ IDisplay* display,
    /* [in] */ Int32 theme)
{
    if (outerContext == NULL) {
        Logger::E(TAG, "outerContext must not be null");
        return NULL;    // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (display == NULL) {
        Logger::E(TAG, "display must not be null");
        return NULL;    // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IContext> displayContext;
    outerContext->CreateDisplayContext(display, (IContext**)&displayContext);
    if (theme == 0) {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);
        AutoPtr<IResourcesTheme> currentTheme;
        displayContext->GetTheme((IResourcesTheme**)&currentTheme);
        Boolean isFounded;
        currentTheme->ResolveAttribute(R::attr::presentationTheme,
                outValue, TRUE, &isFounded);
        outValue->GetResourceId(&theme);
    }

    // Derive the display's window manager from the outer window manager.
    // We do this because the outer window manager have some extra information
    // such as the parent window, which is important if the presentation uses
    // an application window type.
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(outerContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj));
    IWindowManager* wm = IWindowManager::Probe(obj);
    AutoPtr<IWindowManager> displayWindowManager;
    displayWindowManager = ((WindowManagerImpl*)wm)->CreatePresentationWindowManager(display);
    AutoPtr<PresentationContextThemeWrapper> pct = new PresentationContextThemeWrapper();
    pct->constructor(displayContext, theme, displayWindowManager);
    return IContext::Probe(pct);
}

}
}
}
