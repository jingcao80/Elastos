#include "updates/CCertPinInstallReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Updates {

CCertPinInstallReceiver::CCertPinInstallReceiver()
: CConfigUpdateInstallReceiver("/data/misc/keychain/", "pins", "metadata/", "version")
{

}

//namespace Updates
//namespace Server
//namespace Droid
//namespace Elastos