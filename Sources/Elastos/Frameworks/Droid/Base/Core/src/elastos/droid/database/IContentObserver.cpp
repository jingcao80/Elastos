
#include "elastos/droid/database/IContentObserver.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

const Boolean IContentObserverStub::DEBUG = TRUE;
const String IContentObserverStub::TAG("IContentObserverStub");

IContentObserverStub::IContentObserverStub(
    /* [in] */ IIContentObserver* observer)
    : mObserver(observer)
{}

void IContentObserverStub::onLastStrongRef(
    /* [in] */ const void* id)
{
    if (DEBUG) {
        Logger::D(TAG, "call onLastStrongRef");
    }
}

android::status_t IContentObserverStub::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [out] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        default:
            assert(0);
    }
    return android::BBinder::onTransact(code, data, reply, flags);
}

} // namespace Database
} // namespace Droid
} // namespace Elastos
