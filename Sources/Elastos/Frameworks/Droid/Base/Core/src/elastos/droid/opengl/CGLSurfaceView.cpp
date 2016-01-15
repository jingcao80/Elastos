#include "opengl/CGLSurfaceView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace Opengl{

IVIEW_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
IDRAWABLECALLBACK_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
IKEYEVENTCALLBACK_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
ISURFACEVIEW_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
ISURFACEHOLDERCALLBACK_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)
IGLSURFACEVIEW_METHODS_IMPL(CGLSurfaceView, GLSurfaceView)

PInterface CGLSurfaceView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_GLSurfaceView) {
        return reinterpret_cast<PInterface>((GLSurfaceView*)this);
    }
    return _CGLSurfaceView::Probe(riid);
}

ECode CGLSurfaceView::constructor(
    /* [in] */ IContext* context)
{
    return GLSurfaceView::Init(context);
}

ECode CGLSurfaceView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return GLSurfaceView::Init(context, attrs);
}

ECode CGLSurfaceView::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos