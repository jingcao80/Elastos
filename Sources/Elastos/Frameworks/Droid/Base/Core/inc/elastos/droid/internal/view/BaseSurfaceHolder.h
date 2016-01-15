
#ifndef __ELASTOS_DROID_VIEW_BASESURFACEHOLDER_H__
#define __ELASTOS_DROID_VIEW_BASESURFACEHOLDER_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class ECO_PUBLIC BaseSurfaceHolder
    : public Object
    , public ISurfaceHolder
{
public:
    BaseSurfaceHolder();

    CAR_INTERFACE_DECL();

    virtual CARAPI OnUpdateSurface() = 0;

    virtual CARAPI OnRelayoutContainer() = 0;

    virtual CARAPI_(Boolean) OnAllowLockCanvas() = 0;

    CARAPI_(Int32) GetRequestedWidth();

    CARAPI_(Int32) GetRequestedHeight();

    CARAPI_(Int32) GetRequestedFormat();

    CARAPI_(Int32) GetRequestedType();

    CARAPI AddCallback(
        /* [in] */ ISurfaceHolderCallback* cback);

    CARAPI RemoveCallback(
        /* [in] */ ISurfaceHolderCallback* cback);

    CARAPI_(AutoPtr<ArrayOf<ISurfaceHolderCallback*> >) GetCallbacks();

    CARAPI_(void) UngetCallbacks();

    CARAPI SetFixedSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SetSizeFromLayout();

    CARAPI SetFormat(
        /* [in] */ Int32 format);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI LockCanvas(
        /* [out] */ ICanvas** canvas);

    CARAPI LockCanvas(
        /* [in] */ IRect* dirty,
        /* [out] */ ICanvas** canvas);

    CARAPI UnlockCanvasAndPost(
        /* [in] */ ICanvas* canvas);

    CARAPI GetSurface(
        /* [out] */ ISurface** surface);

    CARAPI GetSurfaceFrame(
        /* [out] */ IRect** rect);

    virtual CARAPI SetSurfaceFrameSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    CARAPI InternalLockCanvas(
        /* [in] */ IRect* dirty,
        /* [out] */ ICanvas** canvas);

public:
    AutoPtr<IArrayList> mCallbacks;
    // In order to use convenient
    AutoPtr<ILock> mSurfaceLock;
    // AutoPtr<IReentrantLock> mSurfaceLock;
    AutoPtr<ISurface> mSurface;

protected:
    ECO_LOCAL static const Boolean DEBUG;
    AutoPtr<ArrayOf<ISurfaceHolderCallback*> > mGottenCallbacks;
    Boolean mHaveGottenCallbacks;
    Int32 mRequestedWidth;
    Int32 mRequestedHeight;
    /** @hide */
    Int32 mRequestedFormat;
    Int32 mRequestedType;
    Int64 mLastLockTime;
    Int32 mType;
    AutoPtr<IRect> mSurfaceFrame;
    AutoPtr<IRect> mTmpDirty;

private:
    ECO_LOCAL static const String TAG;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_BASESURFACEHOLDER_H__
