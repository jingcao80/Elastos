
#ifndef __ELASTOS_DROID_VIEW_CINPUTCHANNELHELPER_H__
#define __ELASTOS_DROID_VIEW_CINPUTCHANNELHELPER_H__

#include "_Elastos_Droid_View_CInputChannelHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CInputChannelHelper)
    , public Singleton
    , public IInputChannelHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a new input channel pair.  One channel should be provided to the input
     * dispatcher and the other to the application's input queue.
     * @param name The descriptive (non-unique) name of the channel pair.
     * @return A pair of input channels.  They are symmetric and indistinguishable.
     */
    CARAPI OpenInputChannelPair(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<IInputChannel*>** icp);
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CINPUTCHANNELHELPER_H__
