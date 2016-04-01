
#include "elastos/droid/view/CSurfaceSession.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CSurfaceSession)

CAR_INTERFACE_IMPL(CSurfaceSession, Object, ISurfaceSession)

ECode CSurfaceSession::constructor()
{
    mNativeClient = new android::SurfaceComposerClient();
    return NOERROR;
}

CSurfaceSession::CSurfaceSession()
{
}

CSurfaceSession::~CSurfaceSession()
{
    if (mNativeClient != NULL) {
        mNativeClient = NULL;
    }
}

ECode CSurfaceSession::Kill()
{
    mNativeClient->dispose();

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
