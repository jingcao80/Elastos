#include "SelectableChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

SelectableChannel::SelectableChannel()
{
}

SelectableChannel::~SelectableChannel()
{
}

CAR_INTERFACE_IMPL(SelectableChannel, Object, ISelectableChannel)

ECode SelectableChannel::Register(
    /* [in] */ ISelector* selector,
    /* [in] */ Int32 operations,
    /* [out] */ ISelectionKey** key)
{
    return Register(selector, operations, NULL, key);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
