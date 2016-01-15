
#include "ContentType.h"
namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

const Byte ContentType::CHANGE_CIPHER_SPEC = 20;

/**
 * Identifies alert message
 */
const Byte ContentType::ALERT = 21;

/*
 * Identifies handshake message
*/
const Byte ContentType::HANDSHAKE = 22;

/**
 * Identifies application data message
 */
const Byte ContentType::APPLICATION_DATA = 23;

} // namespace Jsse
} // namespace Provider
} // namespace Xnet
} // namespace Harmony
} // namespace Apache
} // namespace Org