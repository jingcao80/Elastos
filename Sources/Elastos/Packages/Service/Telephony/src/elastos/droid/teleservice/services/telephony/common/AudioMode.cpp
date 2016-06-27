
#include "elastos/droid/teleservice/services/telephony/common/AudioMode.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Common {

Int32 AudioMode::EARPIECE      = 0x00000001;
Int32 AudioMode::BLUETOOTH     = 0x00000002;
Int32 AudioMode::WIRED_HEADSET = 0x00000004;
Int32 AudioMode::SPEAKER       = 0x00000008;

Int32 AudioMode::WIRED_OR_EARPIECE = EARPIECE | WIRED_HEADSET;
Int32 AudioMode::ALL_MODES = EARPIECE | BLUETOOTH | WIRED_HEADSET | SPEAKER;

String AudioMode::ToString(
    /* [in] */ Int32 mode)
{
    if ((mode & ~ALL_MODES) != 0x0) {
        return String("UNKNOWN");
    }

    StringBuffer buffer;
    if ((mode & EARPIECE) == EARPIECE) {
        ListAppend(&buffer, String("EARPIECE"));
    }
    if ((mode & BLUETOOTH) == BLUETOOTH) {
        ListAppend(&buffer, String("BLUETOOTH"));
    }
    if ((mode & WIRED_HEADSET) == WIRED_HEADSET) {
        ListAppend(&buffer, String("WIRED_HEADSET"));
    }
    if ((mode & SPEAKER) == SPEAKER) {
        ListAppend(&buffer, String("SPEAKER"));
    }

    return buffer.ToString();
}

void AudioMode::ListAppend(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ const String& str)
{
    if (buffer->GetLength() > 0) {
        *buffer += ", ";
    }
    *buffer += str;
}

} // namespace Common
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos