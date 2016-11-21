
#ifndef __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>

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
        /* [in] */ ArrayOf<InterfaceID>* busInterfaces);

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
        /* [in] */ IInterface* reply);

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

    // struct Property {
    //     String signature;
    //     jobject jget;
    //     jobject jset;
    // };
    // typedef map<String, jobject> JMethod;
    // typedef map<String, Property> JProperty;
    AutoPtr<IBusObject> mBusObj;

    // JMethod methods;
    // JProperty properties;
    Object mMapLock;

    NativeBusAttachment* mBusPtr;

    AutoPtr<ITranslator> mTranslatorRef;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEBUSOBJECT_H__
