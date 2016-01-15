#include "updates/CSmsShortCodesInstallReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Updates {

CSmsShortCodesInstallReceiver::CSmsShortCodesInstallReceiver()
: CConfigUpdateInstallReceiver("/data/misc/sms/", "codes", "metadata/", "version")
{
    
}

//namespace Updates
//namespace Server
//namespace Droid
//namespace Elastos