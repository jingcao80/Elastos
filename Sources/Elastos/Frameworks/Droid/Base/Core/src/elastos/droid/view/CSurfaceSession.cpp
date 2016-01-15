
#include "elastos/droid/view/CSurfaceSession.h"

namespace Elastos {
namespace Droid {
namespace View {
CAR_OBJECT_IMPL(CSurfaceSession)

CAR_INTERFACE_IMPL(CSurfaceSession, Object, ISurfaceSession)

ECode CSurfaceSession::constructor()
{
    return NOERROR;
}

CSurfaceSession::CSurfaceSession()
{
    mNativeClient = new android::SurfaceComposerClient();
}

CSurfaceSession::~CSurfaceSession()
{
    if (mNativeClient != NULL) {
        delete mNativeClient.get();
        mNativeClient = NULL;
    }
}

ECode CSurfaceSession::Kill()
{
    mNativeClient->dispose();;

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
