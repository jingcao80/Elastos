
#include "org/alljoyn/bus/NativePropertiesChangedListener.h"
#include "org/alljoyn/bus/NativeProxyBusObject.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativePropertiesChangedListener");

NativePropertiesChangedListener::NativePropertiesChangedListener(
    /* [in] */ IWeakReference* listener,
    /* [in] */ IInterfaceInfo* changed,
    /* [in] */ IInterfaceInfo* invalidated)
    : mListener(listener)
    , mChangedType(changed)
    , mInvalidatedType(invalidated)
{}

NativePropertiesChangedListener::~NativePropertiesChangedListener()
{}

void NativePropertiesChangedListener::PropertiesChanged(
    /* [in] */ ajn::ProxyBusObject& obj,
    /* [in] */ const char* ifaceName,
    /* [in] */ const ajn::MsgArg& changed,
    /* [in] */ const ajn::MsgArg& invalidated,
    /* [in] */ void* context)
{
    Logger::I(TAG, "NativePropertiesChangedListener::PropertiesChanged()");

    /*
     * Translate the C++ formal parameters into their JNI counterparts.
     */
    String jifaceName(ifaceName);

    assert(0 && "TODO");
    AutoPtr<IMap> jchanged;
    // JLocalRef<jobjectArray> jchanged = (jobjectArray)MsgArg::Unmarshal(&changed, jchangedType);
    // if (env->ExceptionCheck()) {
    //     Logger::E(TAG, "NativePropertiesChangedListener::PropertiesChanged(): Exception extracting changed dictionary");
    //     return;
    // }

    AutoPtr<ArrayOf<String> > jinvalidated;
    // JLocalRef<jobjectArray> jinvalidated = (jobjectArray)Unmarshal(&invalidated, jinvalidatedType);
    // if (env->ExceptionCheck()) {
    //     Logger::E(TAG, "NativePropertiesChangedListener::PropertiesChanged(): Exception extracting invalidated list");
    //     return;
    // }

    /*
     * The weak global reference jbusListener cannot be directly used.  We have to get
     * a "hard" reference to it and then use that.  If you try to use a weak reference
     * directly you will crash and burn.
     */
    AutoPtr<IPropertiesChangedListener> jo;
    mListener->Resolve(EIID_IPropertiesChangedListener, (IInterface**)&jo);
    if (!jo) {
        Logger::E(TAG, "NativePropertiesChangedListener::PropertiesChanged():"
            " Can't get new local reference to ProxyBusObjectListener");
        return;
    }

    /*
     * This call out to the property changed handler implies that the Java method must be
     * MT-safe.  This is implied by the definition of the listener.
     */
    NativeProxyBusObject* npbo = (NativeProxyBusObject*)context;
    AutoPtr<IProxyBusObject> pbo;
    npbo->mPbo->Resolve(EIID_IProxyBusObject, (IInterface**)&pbo);
    if (pbo) {
        jo->PropertiesChanged(pbo, jifaceName, jchanged, jinvalidated);
    }

    Logger::I(TAG, "NativePropertiesChangedListener::PropertiesChanged(): Return");
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org