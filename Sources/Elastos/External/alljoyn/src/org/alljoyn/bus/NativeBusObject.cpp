#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/NativeBusObject.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/SignatureUtils.h>
#include <alljoyn/DBusStd.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const Boolean DEBUG_LOG = FALSE;
const String NativeBusObject::TAG("NativeBusObject");
/**
 * Create a new NativeBusObject to serve as the C++ half of a Java BusObject and C++
 * NativeBusObject pair.
 */
NativeBusObject::NativeBusObject(
    /* [in] */ NativeBusAttachment* bap,
    /* [in] */ const char* path,
    /* [in] */ IBusObject* obj)
    : BusObject(path)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::NativeBusObject()");

    /*
     * Note the sneaky case here where we get a JBusAttachmentPtr* and we give a
     * reference to the underlying BusAttachment to the constructing BusObject.
     * Since the uderlying BusObject takes a reference to the provided
     * NativeBusAttachment, we must take a reference to the bus attachment even
     * though we never actually use it.  Because we take a reference here, we
     * need to give one in the destructor, which means that we have to save a
     * a copy of the NativeBusAttachment* in the NativeBusObject and therefore a given
     * bus object cannot be shared among bus attachments.
     */
    mBusPtr = bap;
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::NativeBusObject(): Refcount on mBusPtr before is %d", mBusPtr->GetRef());
    mBusPtr->IncRef();
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::NativeBusObject(): Refcount on mBusPtr after is %d", mBusPtr->GetRef());

    /*
     * take a weak global reference back to the Java object.  We expect the bus
     * attachment to have a strong reference to keep it from being garbage
     * collected, but we need to get back to it without interfering with GC.
     */
    IWeakReferenceSource::Probe(obj)->GetWeakReference((IWeakReference**)&mBusObj);
    assert(mBusObj != NULL);
}

NativeBusObject::~NativeBusObject()
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::~NativeBusObject()");

    mBusPtr->DecRef();
    mBusPtr = NULL;
}

static void LogBusMethod(const ajn::InterfaceDescription::Member* member)
{
    Logger::I("NativeBusObject", " >> ===================== BusMethod Info ============================");
    Logger::I("NativeBusObject", " >> name: %s, signature: %s", member->name.c_str(), member->signature.c_str());
    Logger::I("NativeBusObject", " >> returnSignature: %s, argNames: %s", member->returnSignature.c_str(), member->argNames.c_str());
    Logger::I("NativeBusObject", " >> accessPerms: %s, description: %s", member->accessPerms.c_str(), member->description.c_str());
}

QStatus NativeBusObject::AddInterfaces(
    /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::AddInterfaces()");

    QStatus status;

    Int32 len = busInterfaces->GetLength();

    for (Int32 i = 0; i < len; ++i) {
        InterfaceDescription* busInterface = (InterfaceDescription*)(*busInterfaces)[i];
        assert(busInterface);

        const ajn::InterfaceDescription* intf =
            reinterpret_cast<const ajn::InterfaceDescription*>(busInterface->mHandle);
        assert(intf);

        Boolean isAnnounced = FALSE;
        busInterface->IsAnnounced(&isAnnounced);

        if (isAnnounced) {
            if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::AddInterfaces() isAnnounced returned true");
            status = AddInterface(*intf, ANNOUNCED);
        }
        else {
            if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::AddInterfaces() isAnnounced returned false");
            status = AddInterface(*intf);

        }
        if (ER_OK != status) {
            return status;
        }

        size_t numMembs = intf->GetMembers(NULL);
        const ajn::InterfaceDescription::Member** membs = new const ajn::InterfaceDescription::Member*[numMembs];
        if (!membs) {
            return ER_OUT_OF_MEMORY;
        }

        intf->GetMembers(membs, numMembs);
        for (size_t m = 0; m < numMembs; ++m) {
            if (ajn::MESSAGE_METHOD_CALL == membs[m]->memberType) {
                status = AddMethodHandler(membs[m], static_cast<ajn::MessageReceiver::MethodHandler>(&NativeBusObject::MethodHandler));
                if (ER_OK != status) {
                    break;
                }

                String name(membs[m]->name.c_str());
                if (name == NULL) {
                    status = ER_FAIL;
                    break;
                }

                AutoPtr<IMethodInfo> method = busInterface->GetMember(name);
                if (!method) {
                    status = ER_BUS_INTERFACE_NO_SUCH_MEMBER;
                    break;
                }

                String key = String(intf->GetName()) + membs[m]->name.c_str();
                mMethods[key] = method;
            }
        }

        delete [] membs;
        membs = NULL;
        if (ER_OK != status) {
            return status;
        }

        size_t numProps = intf->GetProperties(NULL);
        const ajn::InterfaceDescription::Property** props = new const ajn::InterfaceDescription::Property*[numProps];
        if (!props) {
            return ER_OUT_OF_MEMORY;
        }

        intf->GetProperties(props, numProps);

        for (size_t p = 0; p < numProps; ++p) {
            AutoPtr<Property> property = new Property();
            property->mSignature = props[p]->signature.c_str();

            String name(props[p]->name.c_str());
            if (name == NULL) {
                status = ER_FAIL;
                break;
            }

            AutoPtr<ArrayOf<IMethodInfo*> > methods = busInterface->GetProperty(name);
            if (!methods) {
                status = ER_BUS_NO_SUCH_PROPERTY;
                break;
            }

            property->mGet = (*methods)[0];
            if (!property->mGet) {
                status = ER_FAIL;
                break;
            }

            property->mSet = (*methods)[1];
            if (!property->mSet) {
                status = ER_FAIL;
                break;
            }

            String key = String(intf->GetName()) + props[p]->name.c_str();
            mProperties[key] = property;
        }
        delete [] props;
        props = NULL;
        if (ER_OK != status) {
            return status;
        }
    }

    return ER_OK;
}

void NativeBusObject::MethodHandler(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ ajn::Message& msg)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodHandler()");

    NativeMessageContext context(msg);
    /*
     * The Java method is called via invoke() on the
     * java.lang.reflect.Method object.  This allows us to package up
     * all the message args into an Object[], saving us from having to
     * figure out the signature of each method to lookup.
     */
    String key = String(member->iface->GetName()) + member->name.c_str();
    LogBusMethod(member);

    /*
     * We're going to wander into a list of methods and pick one.  Lock the
     * mutex that protects this list for the entire time we'll be using the
     * list and the found method.
     */
    mMapLock.Lock();

    MethodMap::Iterator find = mMethods.Find(key);
    if (mMethods.End() == find) {
        mMapLock.Unlock();
        MethodReply(member, msg, ER_BUS_OBJECT_NO_SUCH_MEMBER);
        return;
    }

    IMethodInfo* method = find->mSecond;
    AutoPtr<IArgumentList>  args;
    QStatus status = Unmarshal(msg, method, (IArgumentList**)&args);
    if (ER_OK != status) {
        mMapLock.Unlock();
        MethodReply(member, msg, status);
        Logger::E(TAG, "MethodHandler: Unmarshal error!");
        return;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    AutoPtr<IInterface> busObj;
    mBusObj->Resolve(EIID_IInterface, (IInterface**)&busObj);
    if (!busObj) {
        mMapLock.Unlock();
        MsgArg::ReleaseRecord(args);
        Logger::E(TAG, "NativeBusObject::MethodHandler(): Can't get new local reference to BusObject");
        return;
    }

    mMapLock.Unlock();

    if (FAILED(method->Invoke(busObj, args))) {
        MethodReply(member, msg, ER_FAIL);
        MsgArg::ReleaseRecord(args);
        return;
    }

    Boolean hasReply = FALSE;
    Int32 count = 0;
    method->GetParamCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Boolean isNull = FALSE;
        if (SUCCEEDED(args->IsOutputArgumentNullPtr(i, &isNull)) && !isNull) {
            hasReply = TRUE;
            break;
        }
    }
    if (hasReply) {
        MethodReply(member, msg, method, args);
    }
    else {
        MethodReply(member, msg, NULL, (IArgumentList*)NULL);
    }
    MsgArg::ReleaseRecord(args);
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ ajn::Message& msg,
    /* [in] */ QStatus status)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        return ER_OK;
    }
    else {
        return BusObject::MethodReply(msg, status);
    }
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ const ajn::Message& msg,
    /* [in] */ const char* error,
    /* [in] */ const char* errorMessage)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        return ER_OK;
    }
    else {
        return BusObject::MethodReply(msg, error, errorMessage);
    }
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ ajn::Message& msg,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* reply)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        if (!reply) {
            return ER_OK;
        }
        else {
            Logger::E(TAG, "Method %s is annotated as 'no reply' but value returned, replying anyway",
                          member->name.c_str());
        }
    }
    ajn::MsgArg replyArgs;
    QStatus status;
    uint8_t completeTypes = ajn::SignatureUtils::CountCompleteTypes(member->returnSignature.c_str());
    if (reply) {
        if (completeTypes > 1) {
            // jmethodID mid = env->GetStaticMethodID(CLS_Signature, "structArgs",
            //                                        "(Ljava/lang/Object;)[Ljava/lang/Object;");
            // if (!mid) {
            //     return MethodReply(member, msg, ER_FAIL);
            // }
            // jreplyArgs = (jobjectArray)CallStaticObjectMethod(env, CLS_Signature, mid, (jobject)jreply);
            // if (env->ExceptionCheck()) {
            //     return MethodReply(member, msg, ER_FAIL);
            // }
        }
        // else {
            /*
             * Create Object[] out of the invoke() return value to reuse
             * marshalling code in Marshal() for the reply message.
             */
        // }
        if (!Marshal(member->returnSignature.c_str(), method, reply, &replyArgs)) {
            return MethodReply(member, msg, ER_FAIL);
        }
        status = BusObject::MethodReply(msg, replyArgs.v_struct.members, replyArgs.v_struct.numMembers);
    }
    else if (completeTypes) {
        String errorMessage(member->iface->GetName());
        errorMessage = errorMessage + "." + member->name.c_str() + " returned null";
        Logger::E(TAG, errorMessage);
        status = BusObject::MethodReply(msg, "org.alljoyn.bus.BusException", errorMessage.string());
    }
    else {
        status = BusObject::MethodReply(msg, (ajn::MsgArg*)NULL, 0);
    }

    return status;
}

QStatus NativeBusObject::Signal(
    /* [in] */ const char* destination,
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const ajn::MsgArg* args,
    /* [in] */ size_t numArgs,
    /* [in] */ uint32_t timeToLive,
    /* [in] */ uint8_t flags,
    /* [in] */ ajn::Message& msg)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Signal()");

    const ajn::InterfaceDescription* intf = bus->GetInterface(ifaceName);
    if (!intf) {
        return ER_BUS_OBJECT_NO_SUCH_INTERFACE;
    }
    const ajn::InterfaceDescription::Member* signal = intf->GetMember(signalName);
    if (!signal) {
        return ER_BUS_OBJECT_NO_SUCH_MEMBER;
    }
    return BusObject::Signal(destination, sessionId, *signal, args, numArgs, timeToLive, flags, &msg);
}

QStatus NativeBusObject::Get(
    /* [in] */ const char* ifcName,
    /* [in] */ const char* propName,
    /* [in] */ ajn::MsgArg& val)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Get()");

    String key = String(ifcName) + propName;

    /*
     * We're going to wander into a list of properties and pick one.  Lock the
     * mutex that protects this list for the entire time we'll be using the list
     * and the found method.
     */
    mMapLock.Lock();

    PropertyMap::Iterator property = mProperties.Find(key);
    if (mProperties.End() == property) {
        mMapLock.Unlock();
        return ER_BUS_NO_SUCH_PROPERTY;
    }
    IMethodInfo* method = property->mSecond->mGet;
    if (!method) {
        mMapLock.Unlock();
        return ER_BUS_PROPERTY_ACCESS_DENIED;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    AutoPtr<IInterface> busObj;
    mBusObj->Resolve(EIID_IInterface, (IInterface**)&busObj);
    if (!busObj) {
        mMapLock.Unlock();
        Logger::E(TAG, "NativeBusObject::Get(): Can't get new local reference to BusObject");
        return ER_FAIL;
    }

    AutoPtr<IArgumentList> args;
    method->CreateArgumentList((IArgumentList**)&args);

    AutoPtr<IParamInfo> paramInfo;
    method->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
    AutoPtr<IDataTypeInfo> typeInfo;
    paramInfo->GetTypeInfo((IDataTypeInfo**)&typeInfo);
    CarDataType type;
    typeInfo->GetDataType(&type);
    AutoPtr<MsgArg::CarValue> value = new MsgArg::CarValue(type);
    value->mIOAttribute = ParamIOAttribute_CallerAllocOut;
    value->SetToArgumentList(args, 0);
    if (FAILED(method->Invoke(busObj, args))) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    PVoid arg = value->ToValuePtr();
    if (!Marshal(property->mSecond->mSignature.string(), arg, &val)) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    mMapLock.Unlock();
    return ER_OK;
}

QStatus NativeBusObject::Set(
    /* [in] */ const char* ifcName,
    /* [in] */ const char* propName,
    /* [in] */ ajn::MsgArg& val)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Set()");

    String key = String(ifcName) + propName;

    /*
     * We're going to wander into a list of properties and pick one.  Lock the
     * mutex that protects this list for the entire time we'll be using the list
     * and the found method.
     */
    mMapLock.Lock();

    PropertyMap::Iterator property = mProperties.Find(key);
    if (mProperties.End() == property) {
        mMapLock.Unlock();
        return ER_BUS_NO_SUCH_PROPERTY;
    }
    IMethodInfo* method = property->mSecond->mSet;
    if (!method) {
        mMapLock.Unlock();
        return ER_BUS_PROPERTY_ACCESS_DENIED;
    }

    AutoPtr<IArgumentList> value;
    QStatus status = Unmarshal(&val, 1, method, (IArgumentList**)&value);
    if (ER_OK != status) {
        mMapLock.Unlock();
        MsgArg::ReleaseRecord(value);
        return status;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    AutoPtr<IInterface> busObj;
    mBusObj->Resolve(EIID_IInterface, (IInterface**)&busObj);
    if (!busObj) {
        mMapLock.Unlock();
        Logger::E(TAG, "NativeBusObject::Set(): Can't get new local reference to BusObject");
        MsgArg::ReleaseRecord(value);
        return ER_FAIL;
    }

    if (FAILED(method->Invoke(busObj, value))) {
        mMapLock.Unlock();
        MsgArg::ReleaseRecord(value);
        return ER_FAIL;
    }

    mMapLock.Unlock();
    MsgArg::ReleaseRecord(value);
    return ER_OK;
}

qcc::String NativeBusObject::GenerateIntrospection(
    /* [in] */ const char* languageTag,
    /* [in] */ bool deep,
    /* [in] */ size_t indent) const
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::GenerateIntrospection()");

    if (!languageTag) {
        return NativeBusObject::GenerateIntrospection(deep, indent);
    }

    AutoPtr<IIntrospectionWithDescriptionListener> busObj;
    mBusObj->Resolve(EIID_IIntrospectionWithDescriptionListener, (IInterface**)&busObj);
    if (busObj != NULL) {
        String introspection;
        if (FAILED(busObj->GenerateIntrospection(String(languageTag), deep, indent, &introspection))) {
            return BusObject::GenerateIntrospection(languageTag, deep, indent);
        }

        return qcc::String(introspection.string());
    }

    return BusObject::GenerateIntrospection(languageTag, deep, indent);
}

qcc::String NativeBusObject::GenerateIntrospection(
    /* [in] */ bool deep,
    /* [in] */ size_t indent) const
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::GenerateIntrospection()");

    AutoPtr<IIntrospectionWithDescriptionListener> busObj1;
    mBusObj->Resolve(EIID_IIntrospectionWithDescriptionListener, (IInterface**)&busObj1);
    AutoPtr<IIntrospectionListener> busObj2;
    mBusObj->Resolve(EIID_IIntrospectionListener, (IInterface**)&busObj2);
    //Use either interface but prefer IntrospectionListener since it's more specific
    if (busObj1 != NULL || busObj2 != NULL) {
        String introspection;
        ECode ec;
        if (busObj2 != NULL) {
            ec = busObj2->GenerateIntrospection(deep, indent, &introspection);
        }
        else {
            ec = busObj1->GenerateIntrospection(String(NULL), deep, indent, &introspection);
        }

        if (FAILED(ec)) {
            return BusObject::GenerateIntrospection(deep, indent);
        }

        return qcc::String(introspection.string());
    }

    return BusObject::GenerateIntrospection(deep, indent);
}

void NativeBusObject::ObjectRegistered()
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::ObjectRegistered()");

    BusObject::ObjectRegistered();
    AutoPtr<IBusObjectListener> busObj;
    mBusObj->Resolve(EIID_IBusObjectListener, (IInterface**)&busObj);
    if (busObj != NULL) {
        busObj->Registered();
    }
}

void NativeBusObject::ObjectUnregistered()
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::ObjectUnregistered()");

    BusObject::ObjectUnregistered();
    AutoPtr<IBusObjectListener> busObj;
    mBusObj->Resolve(EIID_IBusObjectListener, (IInterface**)&busObj);
    if (busObj != NULL) {
        busObj->Unregistered();
    }
}

void NativeBusObject::SetDescriptions(
    /* [in] */ const String& langTag,
    /* [in] */ const String& description,
    /* [in] */ ITranslator* translator)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::SetDescriptions()");

    if (langTag.string() && description.string()) {
        SetDescription(langTag.string(), description.string());
    }

    if (translator) {
        mTranslatorRef = translator;
        assert(0);
        // NativeTranslator* nativeTranslator = reinterpret_cast<NativeTranslator*>(((Translator*)translator)->mHandle);
        // assert(nativeTranslator);
        // SetDescriptionTranslator(nativeTranslator);
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
