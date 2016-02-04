#ifndef __ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__
#define __ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurfaceSession;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Four black surfaces put together to make a black frame.
 */
class BlackFrame : public Object
{
public:
    class BlackSurface : public Object
    {
    public:
        BlackSurface(
            /* [in] */ ISurfaceSession* session,
            /* [in] */ Int32 layer,
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b,
            /* [in] */ Int32 layerStack,
            /* [in] */ BlackFrame* host);

        CARAPI_(void) SetAlpha(
            /* [in] */ Float alpha);

        CARAPI_(void) SetMatrix(
            /* [in] */ IMatrix* matrix);

        CARAPI_(void) ClearMatrix();

    public:
        Int32 mLeft;
        Int32 mTop;
        Int32 mLayer;
        AutoPtr<ISurfaceControl> mSurface;

    private:
        BlackFrame* mHost;
    };

public:
    BlackFrame(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ IRect* outer,
        /* [in] */ IRect* inner,
        /* [in] */ Int32 layer,
        /* [in] */ Int32 layerStack,
        /* [in] */ Boolean forceDefaultOrientation);

    // void printTo(String prefix, PrintWriter pw);

    CARAPI_(void) Kill();

    CARAPI_(void) Hide();

    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    CARAPI_(void) SetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI_(void) ClearMatrix();

public:
    AutoPtr<IRect> mOuterRect;
    AutoPtr<IRect> mInnerRect;
    AutoPtr<IMatrix> mTmpMatrix;
    AutoPtr< ArrayOf<Float> > mTmpFloats;
    AutoPtr< ArrayOf<BlackSurface*> > mBlackSurfaces;

    Boolean mForceDefaultOrientation;
};

} // Wm
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wm::BlackFrame::BlackSurface, IInterface)

#endif //__ELASTOS_DROID_SERVER_WM_BLACKFRAME_H__
