
#ifndef __ELASTOS_DROID_VIEW_CWINDOWIDFOCUSOBSERVER_H__
#define __ELASTOS_DROID_VIEW_CWINDOWIDFOCUSOBSERVER_H__

#include "_Elastos_Droid_View_CWindowIdFocusObserver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowIdFocusObserver)
    , public Object
    , public IIWindowFocusObserver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IWindowIdFocusObserver* observer);

    // @Override
    CARAPI FocusGained(
        /* [in] */ IBinder* inputToken);

    // @Override
    CARAPI FocusLost(
        /* [in] */ IBinder* inputToken);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<IWindowIdFocusObserver> mObserver;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWIDFOCUSOBSERVER_H__
