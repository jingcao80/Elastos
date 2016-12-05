
#ifndef __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeBusObject : public ajn::BusObject
{
public:
    NativeBusObject(
        /* [in] */ NativeBusAttachment* bap,
        /* [in] */ const char* path,
        /* [in] */ IBusObject* obj);

    ~NativeBusObject();

    QStatus AddInterfaces(
        /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces);

    void MethodHandler(
        /* [in] */ const ajn::InterfaceDescription::Member* member,
        /* [in] */ ajn::Message& msg);

    QStatus MethodReply(
        /* [in] */ const ajn::InterfaceDescription::Member* member,
        /* [in] */ ajn::Message& msg,
        /* [in] */ QStatus status);

    QStatus MethodReply(
        /* [in] */ const ajn::InterfaceDescription::Member* member,
        /* [in] */ const ajn::Message& msg,
        /* [in] */ const char* error,
        /* [in] */ const char* errorMessage = NULL);

    QStatus MethodReply(
        /* [in] */ const ajn::InterfaceDescription::Member* member,
        /* [in] */ ajn::Message& msg,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* reply);

    QStatus Signal(
        /* [in] */ const char* destination,
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* signalName,
        /* [in] */ const ajn::MsgArg* args,
        /* [in] */ size_t numArgs,
        /* [in] */ uint32_t timeToLive,
        /* [in] */ uint8_t flags,
        /* [in] */ ajn::Message& msg);

    QStatus Get(
        /* [in] */ const char* ifcName,
        /* [in] */ const char* propName,
        /* [in] */ ajn::MsgArg& val);

    QStatus Set(
        /* [in] */ const char* ifcName,
        /* [in] */ const char* propName,
        /* [in] */ ajn::MsgArg& val);

    qcc::String GenerateIntrospection(
        /* [in] */ const char* languageTag,
        /* [in] */ bool deep = false,
        /* [in] */ size_t indent = 0) const;

    qcc::String GenerateIntrospection(
        /* [in] */ bool deep = false,
        /* [in] */ size_t indent = 0) const;

    void ObjectRegistered();

    void ObjectUnregistered();

    void SetDescriptions(
        /* [in] */ const String& langTag,
        /* [in] */ const String& description,
        /* [in] */ ITranslator* translator);

private:
    NativeBusObject(const NativeBusObject& other);
    NativeBusObject& operator =(const NativeBusObject& other);

    class Property : public ElLightRefBase
    {
    public:
        String mSignature;
        AutoPtr<IMethodInfo> mGet;
        AutoPtr<IMethodInfo> mSet;
    };
    typedef HashMap<String, AutoPtr<IMethodInfo> > MethodMap;
    typedef HashMap<String, AutoPtr<Property> > PropertyMap;
    AutoPtr<IWeakReference> mBusObj;

    MethodMap mMethods;
    PropertyMap mProperties;
    Object mMapLock;

    NativeBusAttachment* mBusPtr;

    AutoPtr<ITranslator> mTranslatorRef;

    static const String TAG;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__
