
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/storage/StorageEventListener.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CAR_INTERFACE_IMPL(StorageEventListener, Object, IStorageEventListener)

ECode StorageEventListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    return NOERROR;
}

ECode StorageEventListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

