
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/display/CVirtualDisplay.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CVirtualDisplay, Object, IVirtualDisplay)

CAR_OBJECT_IMPL(CVirtualDisplay)

CVirtualDisplay::CVirtualDisplay()
{}

CVirtualDisplay::~CVirtualDisplay()
{
    if (mToken != NULL) {
        mGlobal->ReleaseVirtualDisplay(mToken);
        mToken = NULL;
    }
}

ECode CVirtualDisplay::constructor(
    /* [in] */ IDisplayManagerGlobal* global,
    /* [in] */ IDisplay* display,
    /* [in] */ IIVirtualDisplayCallback* token,
    /* [in] */ ISurface* surface)
{
    mGlobal = global;
    mDisplay = display;
    mToken = token;
    mSurface = surface;
    return NOERROR;
}

ECode CVirtualDisplay::GetDisplay(
    /* [out] */ IDisplay** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDisplay;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CVirtualDisplay::GetSurface(
    /* [out] */ ISurface** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSurface;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CVirtualDisplay::SetSurface(
    /* [in] */ ISurface* surface)
{
    if (mSurface.Get() != surface) {
        FAIL_RETURN(mGlobal->SetVirtualDisplaySurface(mToken, surface))
        mSurface = surface;
    }
    return NOERROR;
}

ECode CVirtualDisplay::Resize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    return mGlobal->ResizeVirtualDisplay(mToken, width, height, densityDpi);
}

ECode CVirtualDisplay::ReleaseResources()
{
    if (mToken != NULL) {
        mGlobal->ReleaseVirtualDisplay(mToken);
        mToken = NULL;
    }
    return NOERROR;
}

ECode CVirtualDisplay::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb ;
    sb += "VirtualDisplay{display=";
    String tmp;
    IObject::Probe(mDisplay)->ToString(&tmp);
    sb += tmp;
    sb += ", token=";
    IObject::Probe(mToken)->ToString(&tmp);
    sb += tmp;
    sb += ", surface=";
    IObject::Probe(mSurface)->ToString(&tmp);
    sb += tmp;
    sb += "}";

    return sb.ToString(result);
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos