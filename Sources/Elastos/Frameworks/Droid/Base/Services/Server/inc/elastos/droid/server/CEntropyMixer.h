#ifndef __ELASTOS_DROID_SERVER_CENTROPY_MIXER_H__
#define __ELASTOS_DROID_SERVER_CENTROPY_MIXER_H__

#include "_Elastos_Droid_Server_CEntropyMixer.h"
#include "elastos/droid/server/EntropyMixer.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CEntropyMixer)
    , public EntropyMixer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CENTROPY_MIXER_H__
