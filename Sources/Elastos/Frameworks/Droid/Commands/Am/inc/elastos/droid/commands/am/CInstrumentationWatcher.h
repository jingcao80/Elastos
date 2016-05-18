
#ifndef __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__

#include "_Elastos_Droid_Commands_Am_CInstrumentationWatcher.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CInstrumentationWatcher)
    , public Am::InstrumentationWatcher
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI InstrumentationStatus(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI InstrumentationFinished(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI SetRawOutput(
        /* [in] */ Boolean rawMode);

    CARAPI WaitForFinish(
        /* [out] */ Boolean* result);
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__
