#include "org/alljoyn/bus/NativeBusObject.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

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
        busInterface->IsAnnounced(isAnnounced);

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
            if (MESSAGE_METHOD_CALL == membs[m]->memberType) {
                status = AddMethodHandler(membs[m], static_cast<ajn::MessageReceiver::MethodHandler>(&NativeBusObject::MethodHandler));
                if (ER_OK != status) {
                    break;
                }

                String name(membs[m]->name.c_str());
                if (name == NULL) {
                    status = ER_FAIL;
                    break;
                }

                AutoPtr<IMethodInfo> method = busInterface->>GetMember(name);
                if (!method) {
                    status = ER_BUS_INTERFACE_NO_SUCH_MEMBER;
                    break;
                }

                String key = String(intf->GetName().c_str()) + membs[m]->name.c_str();
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

            String key = String(intf->GetName().c_str()) + props[p]->name.c_str();
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

/**
 * Marshal an Object into a MsgArg.
 *
 * @param[in] signature the signature of the Object
 * @param[in] jarg the Object
 * @param[in] arg the MsgArg to marshal into
 * @return the marshalled MsgArg or NULL if the marshalling failed.  This will
 *         be the same as @param arg if marshalling succeeded.
 */
static ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ IInterface* obj,
    /* [in] */ ajn::MsgArg* arg)
{
    if (!signature) {
        return NULL;
    }

    if (FAILED(MsgArg::Marshal((Int64)arg, String(signature), obj))) {
        return NULL;
    }
    return arg;
}

/**
 * Marshal an Object[] into MsgArgs.  The arguments are marshalled into an
 * ALLJOYN_STRUCT with the members set to the marshalled Object[] elements.
 *
 * @param[in] signature the signature of the Object[]
 * @param[in] jargs the Object[]
 * @param[in] arg the MsgArg to marshal into
 * @return an ALLJOYN_STRUCT containing the marshalled MsgArgs or NULL if the
 *         marshalling failed.  This will be the same as @param arg if
 *         marshalling succeeded.
 */
static ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ ArrayOf<IInterface*>* objs,
    /* [in] */ ajn::MsgArg* arg)
{
    if (!signature) {
        return NULL;
    }
    if (FAILED(MsgArg::Marshal((Int64)arg, String(signature), objs))) {
        return NULL;
    }
    return arg;
}

/**
 * Unmarshal a single MsgArg into an Object.
 *
 * @param[in] arg the MsgArg
 * @param[in] jtype the Type of the Object to unmarshal into
 * @return the unmarshalled Java Object
 */
static AutoPtr<IInterface> Unmarshal(
    /* [in] */ const ajn::MsgArg* arg,
    /* [in] */ IType* type)
{
    AutoPtr<IInterface> obj
    if (FAILED(MsgArg::unmarshal((Int64)arg, type, (IInterface**)&obj)))
        return NULL
    return obj;
}

/**
 * Unmarshal MsgArgs into an Object[].
 *
 * @param[in] args the MsgArgs
 * @param[in] numArgs the number of MsgArgs
 * @param[in] jmethod the Method that will be invoked with the returned Object[]
 * @param[out] junmarshalled the unmarshalled Java Object[]
 */
static QStatus Unmarshal(
    /* [in] */ const ajn::MsgArg* args,
    /* [in] */ size_t numArgs,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    ajn::MsgArg arg(ALLJOYN_STRUCT);
    arg.v_struct.members = (MsgArg*)args;
    arg.v_struct.numMembers = numArgs;
    if (FAILED(MsgArg::Unmarshal(method, (Int64)&arg, unmarshalled))) {
        return ER_FAIL;
    }
    return ER_OK;
}

/**
 * Unmarshal an AllJoyn message into an Object[].
 *
 * @param[in] msg the AllJoyn message received
 * @param[in] jmethod the Method that will be invoked with the returned Object[]
 * @param[out] junmarshalled the unmarshalled Java Objects
 */
static QStatus Unmarshal(
    /* [in] */ ajn::Message& msg,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    const ajn::MsgArg* args;
    size_t numArgs;
    msg->GetArgs(numArgs, args);
    return Unmarshal(args, numArgs, method, unmarshalled);
}

void NativeBusObject::MethodHandler(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ ajn::Message& msg)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodHandler()");

    /*
     * JScopedEnv will automagically attach the JVM to the current native
     * thread.
     */

    ajn::MessageContext context(msg);
    /*
     * The Java method is called via invoke() on the
     * java.lang.reflect.Method object.  This allows us to package up
     * all the message args into an Object[], saving us from having to
     * figure out the signature of each method to lookup.
     */
    String key = String(member->iface->GetName().c_str()) + member->name.c_str();

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
        return;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    AutoPtr<IInterface> busObj;
    mBusObj->Resolve(EIID_IInterface, (IInterface**)busObj);
    if (!busObj) {
        mMapLock.Unlock();
        Logger::E(TAG, "NativeBusObject::MethodHandler(): Can't get new local reference to BusObject");
        return;
    }

    mMapLock.Unlock();

    Int32 count;
    args->GetParamCount(&count);
    AutoPtr<IInterface> reply;
    args->SetOutputArgumentOfObjectPtrPtr(count - 1, (IInterface**)&reply);

    if (FAILED(method->Invoke(busObj, args))) {
        MethodReply(member, msg, ER_FAIL);
        return;
    }

    MethodReply(member, msg, reply);
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const InterfaceDescription::Member* member,
    /* [in] */ Message& msg,
    /* [in] */ QStatus status)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        return ER_OK;
    } else {
        return BusObject::MethodReply(msg, status);
    }
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const InterfaceDescription::Member* member,
    /* [in] */ const Message& msg,
    /* [in] */ const char* error,
    /* [in] */ const char* errorMessage)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        return ER_OK;
    } else {
        return BusObject::MethodReply(msg, error, errorMessage);
    }
}

QStatus NativeBusObject::MethodReply(
    /* [in] */ const InterfaceDescription::Member* member,
    /* [in] */ Message& msg,
    /* [in] */ IInterface* reply)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::MethodReply()");

    qcc::String val;
    if (member->GetAnnotation(org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        if (!reply) {
            return ER_OK;
        } else {
            Logger::E(TAG, "Method %s is annotated as 'no reply' but value returned, replying anyway",
                          member->name.c_str());
        }
    }
    ajn::MsgArg replyArgs;
    QStatus status;
    uint8_t completeTypes = SignatureUtils::CountCompleteTypes(member->returnSignature.c_str());
    if (reply) {
        JLocalRef<jobjectArray> jreplyArgs;
        if (completeTypes > 1) {
            jmethodID mid = env->GetStaticMethodID(CLS_Signature, "structArgs",
                                                   "(Ljava/lang/Object;)[Ljava/lang/Object;");
            if (!mid) {
                return MethodReply(member, msg, ER_FAIL);
            }
            jreplyArgs = (jobjectArray)CallStaticObjectMethod(env, CLS_Signature, mid, (jobject)jreply);
            if (env->ExceptionCheck()) {
                return MethodReply(member, msg, ER_FAIL);
            }
        } else {
            /*
             * Create Object[] out of the invoke() return value to reuse
             * marshalling code in Marshal() for the reply message.
             */
            jreplyArgs = env->NewObjectArray(1, CLS_Object, NULL);
            if (!jreplyArgs) {
                return MethodReply(member, msg, ER_FAIL);
            }
            env->SetObjectArrayElement(jreplyArgs, 0, jreply);
            if (env->ExceptionCheck()) {
                return MethodReply(member, msg, ER_FAIL);
            }
        }
        if (!Marshal(member->returnSignature.c_str(), jreplyArgs, &replyArgs)) {
            return MethodReply(member, msg, ER_FAIL);
        }
        status = BusObject::MethodReply(msg, replyArgs.v_struct.members, replyArgs.v_struct.numMembers);
    } else if (completeTypes) {
        String errorMessage(member->iface->GetName());
        errorMessage += "." + member->name + " returned null";
        Logger::E(TAG, errorMessage.c_str());
        status = BusObject::MethodReply(msg, "org.alljoyn.bus.BusException", errorMessage.c_str());
    } else {
        status = BusObject::MethodReply(msg, (MsgArg*)NULL, 0);
    }
    if (ER_OK != status) {
        env->ThrowNew(CLS_BusException, QCC_StatusText(status));
    }
    return status;
}

QStatus NativeBusObject::Signal(
    /* [in] */ const char* destination,
    /* [in] */ SessionId sessionId,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const MsgArg* args,
    /* [in] */ size_t numArgs,
    /* [in] */ uint32_t timeToLive,
    /* [in] */ uint8_t flags,
    /* [in] */ Message& msg)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Signal()");

    const InterfaceDescription* intf = bus->GetInterface(ifaceName);
    if (!intf) {
        return ER_BUS_OBJECT_NO_SUCH_INTERFACE;
    }
    const InterfaceDescription::Member* signal = intf->GetMember(signalName);
    if (!signal) {
        return ER_BUS_OBJECT_NO_SUCH_MEMBER;
    }
    return BusObject::Signal(destination, sessionId, *signal, args, numArgs, timeToLive, flags, &msg);
}

QStatus NativeBusObject::Get(
    /* [in] */ const char* ifcName,
    /* [in] */ const char* propName,
    /* [in] */ MsgArg& val)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Get()");

    /*
     * JScopedEnv will automagically attach the JVM to the current native
     * thread.
     */
    JScopedEnv env;

    String key = String(ifcName) + propName;

    /*
     * We're going to wander into a list of properties and pick one.  Lock the
     * mutex that protects this list for the entire time we'll be using the list
     * and the found method.
     */
    mMapLock.Lock();

    JProperty::const_iterator property = properties.find(key);
    if (properties.end() == property) {
        mMapLock.Unlock();
        return ER_BUS_NO_SUCH_PROPERTY;
    }
    if (!property->second.jget) {
        mMapLock.Unlock();
        return ER_BUS_PROPERTY_ACCESS_DENIED;
    }

    JLocalRef<jclass> clazz = env->GetObjectClass(property->second.jget);
    jmethodID mid = env->GetMethodID(clazz, "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    if (!mid) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    jobject jo = env->NewLocalRef(jbusObj);
    if (!jo) {
        mMapLock.Unlock();
        Logger::E(TAG, "NativeBusObject::Get(): Can't get new local reference to BusObject");
        return ER_FAIL;
    }

    JLocalRef<jobject> jvalue = CallObjectMethod(env, property->second.jget, mid, jo, NULL);
    if (env->ExceptionCheck()) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    if (!Marshal(property->second.signature.c_str(), (jobject)jvalue, &val)) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    mMapLock.Unlock();
    return ER_OK;
}

QStatus NativeBusObject::Set(
    /* [in] */ const char* ifcName,
    /* [in] */ const char* propName,
    /* [in] */ MsgArg& val)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::Set()");

    /*
     * JScopedEnv will automagically attach the JVM to the current native
     * thread.
     */
    JScopedEnv env;

    String key = String(ifcName) + propName;

    /*
     * We're going to wander into a list of properties and pick one.  Lock the
     * mutex that protects this list for the entire time we'll be using the list
     * and the found method.
     */
    mMapLock.Lock();

    JProperty::const_iterator property = properties.find(key);
    if (properties.end() == property) {
        mMapLock.Unlock();
        return ER_BUS_NO_SUCH_PROPERTY;
    }
    if (!property->second.jset) {
        mMapLock.Unlock();
        return ER_BUS_PROPERTY_ACCESS_DENIED;
    }

    JLocalRef<jobjectArray> jvalue;
    QStatus status = Unmarshal(&val, 1, property->second.jset, jvalue);
    if (ER_OK != status) {
        mMapLock.Unlock();
        return status;
    }

    JLocalRef<jclass> clazz = env->GetObjectClass(property->second.jset);
    jmethodID mid = env->GetMethodID(clazz, "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    if (!mid) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    /*
     * The weak global reference jbusObj cannot be directly used.  We have to
     * get a "hard" reference to it and then use that.  If you try to use a weak
     * reference directly you will crash and burn.
     */
    jobject jo = env->NewLocalRef(jbusObj);
    if (!jo) {
        mMapLock.Unlock();
        Logger::E(TAG, "NativeBusObject::Set(): Can't get new local reference to BusObject");
        return ER_FAIL;
    }

    CallObjectMethod(env, property->second.jset, mid, jo, (jobjectArray)jvalue);
    if (env->ExceptionCheck()) {
        mMapLock.Unlock();
        return ER_FAIL;
    }

    mMapLock.Unlock();
    return ER_OK;
}

String NativeBusObject::GenerateIntrospection(
    /* [in] */ const char* languageTag,
    /* [in] */ bool deep,
    /* [in] */ size_t indent) const
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::GenerateIntrospection()");

    if (!languageTag) {
        return NativeBusObject::GenerateIntrospection(deep, indent);
    }

    if (MID_generateIntrospectionWithDesc) {
        /*
         * JScopedEnv will automagically attach the JVM to the current native
         * thread.
         */
        JScopedEnv env;

        /*
         * The weak global reference jbusObj cannot be directly used.  We have to
         * get a "hard" reference to it and then use that.  If you try to use a weak
         * reference directly you will crash and burn.
         */
        jobject jo = env->NewLocalRef(jbusObj);
        if (!jo) {
            Logger::E(TAG, "NativeBusObject::GenerateIntrospection(): Can't get new local reference to BusObject");
            return "";
        }

        JLocalRef<jstring> jlang = env->NewStringUTF(languageTag);
        JLocalRef<jstring> jintrospection = (jstring)CallObjectMethod(env,
                                                                      jo, MID_generateIntrospectionWithDesc, (jstring)jlang, deep, indent);
        if (env->ExceptionCheck()) {
            return BusObject::GenerateIntrospection(languageTag, deep, indent);
        }

        JString introspection(jintrospection);
        if (env->ExceptionCheck()) {
            return BusObject::GenerateIntrospection(languageTag, deep, indent);
        }

        return String(introspection.c_str());
    }

    return BusObject::GenerateIntrospection(languageTag, deep, indent);
}

String NativeBusObject::GenerateIntrospection(
    /* [in] */ bool deep,
    /* [in] */ size_t indent) const
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::GenerateIntrospection()");


    //Use either interface but prefer IntrospectionListener since it's more specific
    if (MID_generateIntrospectionWithDesc || MID_generateIntrospection) {
        /*
         * JScopedEnv will automagically attach the JVM to the current native
         * thread.
         */
        JScopedEnv env;

        /*
         * The weak global reference jbusObj cannot be directly used.  We have to
         * get a "hard" reference to it and then use that.  If you try to use a weak
         * reference directly you will crash and burn.
         */
        jobject jo = env->NewLocalRef(jbusObj);
        if (!jo) {
            Logger::E(TAG, "NativeBusObject::GenerateIntrospection(): Can't get new local reference to BusObject");
            return "";
        }

        JLocalRef<jstring> jintrospection;
        if (MID_generateIntrospection) {
            jintrospection = (jstring)CallObjectMethod(env, jo, MID_generateIntrospection, deep, indent);
        } else {
            jintrospection = (jstring)CallObjectMethod(env, jo, MID_generateIntrospectionWithDesc, deep, indent, NULL);
        }

        if (env->ExceptionCheck()) {
            return BusObject::GenerateIntrospection(deep, indent);
        }

        JString introspection(jintrospection);
        if (env->ExceptionCheck()) {
            return BusObject::GenerateIntrospection(deep, indent);
        }

        return String(introspection.c_str());
    }

    return BusObject::GenerateIntrospection(deep, indent);
}

void NativeBusObject::ObjectRegistered()
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::ObjectRegistered()");

    BusObject::ObjectRegistered();
    if (NULL != MID_registered) {
        /*
         * JScopedEnv will automagically attach the JVM to the current native
         * thread.
         */
        JScopedEnv env;

        /*
         * The weak global reference jbusObj cannot be directly used.  We have to
         * get a "hard" reference to it and then use that.  If you try to use a weak
         * reference directly you will crash and burn.
         */
        jobject jo = env->NewLocalRef(jbusObj);
        if (!jo) {
            Logger::E(TAG, "NativeBusObject::ObjectRegistered(): Can't get new local reference to BusObject");
            return;
        }

        env->CallVoidMethod(jo, MID_registered);
    }
}

void NativeBusObject::ObjectUnregistered()
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::ObjectUnregistered()");

    BusObject::ObjectUnregistered();
    if (NULL != MID_registered) {
        /*
         * JScopedEnv will automagically attach the JVM to the current native
         * thread.
         */
        JScopedEnv env;

        /*
         * The weak global reference jbusObj cannot be directly used.  We have to
         * get a "hard" reference to it and then use that.  If you try to use a weak
         * reference directly you will crash and burn.
         */
        jobject jo = env->NewLocalRef(jbusObj);
        if (!jo) {
            Logger::E(TAG, "NativeBusObject::ObjectUnregistered(): Can't get new local reference to BusObject");
            return;
        }

        env->CallVoidMethod(jo, MID_unregistered);
    }
}

void NativeBusObject::SetDescriptions(
    /* [in] */ jstring jlangTag,
    /* [in] */ jstring jdescription,
    /* [in] */ jobject jtranslator)
{
    if (DEBUG_LOG) Logger::D(TAG, "NativeBusObject::SetDescriptions()");
    JNIEnv* env = GetEnv();

    JString langTag(jlangTag);
    JString description(jdescription);

    if (langTag.c_str() && description.c_str()) {
        SetDescription(langTag.c_str(), description.c_str());
    }

    if (jtranslator) {
        jobject jglobalref = env->NewGlobalRef(jtranslator);
        if (!jglobalref) {
            return;
        }
        jtranslatorRef = jglobalref;
        JTranslator* translator = GetHandle<JTranslator*>(jtranslator);
        if (env->ExceptionCheck()) {
            Logger::E(TAG, "BusAttachment_setDescriptionTranslator(): Exception");
            return;
        }
        assert(translator);
        SetDescriptionTranslator(translator);
    }
}
