#ifndef __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGER_H__
#define __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGER_H__

#include "_Elastos_Droid_Server_CNetworkManagementSocketTagger.h"
#include "elastos/droid/server/NetworkManagementSocketTagger.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CNetworkManagementSocketTagger)
    , public NetworkManagementSocketTagger
{
public:
    CAR_OBJECT_DECL()

};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSOCKETTAGGER_H__
