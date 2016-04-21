
#include "CInstrumentationWatcher.h"

using Elastos::Droid::App::EIID_IInstrumentationWatcher;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CInstrumentationWatcher, Object, IInstrumentationWatcher, IBinder);

ECode CInstrumentationWatcher::constructor()
{
    return NOERROR;
}

ECode CInstrumentationWatcher::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Am::InstrumentationWatcher::InstrumentationStatus(
            name, resultCode, results);
}

ECode CInstrumentationWatcher::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Am::InstrumentationWatcher::InstrumentationFinished(
            name, resultCode, results);
}

ECode CInstrumentationWatcher::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CInstrumentationWatcher::SetRawOutput(
        /* [in] */ Boolean rawMode)
{
    return Am::InstrumentationWatcher::SetRawOutput(rawMode);
}

ECode CInstrumentationWatcher::WaitForFinish(
    /* [out] */ Boolean* result)
{
    return Am::InstrumentationWatcher::WaitForFinish(result);
}

}
}
}
}

