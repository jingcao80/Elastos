
#include "CIntentReceiver.h"

using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CIntentReceiver, Object, IIntentReceiver, IBinder);

CAR_OBJECT_IMPL(CIntentReceiver)

ECode CIntentReceiver::constructor()
{
    return NOERROR;
}

ECode CIntentReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return Am::IntentReceiver::PerformReceive(
            intent, resultCode,  data,  extras, ordered, sticky, sendingUser);
}

ECode CIntentReceiver::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CIntentReceiver::WaitForFinish()
{
    return Am::IntentReceiver::WaitForFinish();
}

}
}
}
}

