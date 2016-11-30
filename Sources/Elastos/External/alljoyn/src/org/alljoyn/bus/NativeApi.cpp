
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/core/ClassLoader.h>
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/Init.h>
#include <qcc/Log.h>

using Elastos::Core::ClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::MakePair;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

// class StaticInitializer
// {
// public:
//     StaticInitializer()
//     {
//         if (AllJoynInit() != ER_OK) {
//             Logger::E("StaticInitializer", "StaticInitializer() call AllJoynInit failed.");
//             return;
//         }
//     #ifdef ROUTER
//         if (AllJoynRouterInit() != ER_OK) {
//             AllJoynShutdown();
//             Logger::E("StaticInitializer", "StaticInitializer() call AllJoynRouterInit failed.");
//             return;
//         }
//     #endif
//         QCC_UseOSLogging(true);
//     }

//     ~StaticInitializer()
//     {
//     #ifdef ROUTER
//         AllJoynRouterShutdown();
//     #endif
//         AllJoynShutdown();
//     }
// };

// static StaticInitializer sInit;
static Boolean sInitialized = FALSE;

void GlobalInitialize()
{
    if (!sInitialized) {
        if (AllJoynInit() != ER_OK) {
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynInit failed.");
            return;
        }
    #ifdef ROUTER
        if (AllJoynRouterInit() != ER_OK) {
            AllJoynShutdown();
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynRouterInit failed.");
            return;
        }
    #endif
        QCC_UseOSLogging(true);

        sInitialized = TRUE;
    }
}

/**
 * A map of Java Objects to NativeBusObjects.
 *
 * When we register a bus object, we are registering a plain old Java Object
 * that the client is claiming can act as a BusObject and has whatever it
 * takes to deal with the claimed interfaces.
 *
 * In order to make implementing bus objects easier (see the sidebar on Bus
 * Objects at the start of the file), what the client does is implement an
 * empty interface called BusObject (a Java marker interface).
 *
 * Since we have no super-powers to let us influence what goes into the
 * object that implements the empty interface, we have to provide some
 * scaffolding outside of the object to allow us to locate the C++ object
 * associated with the Java object and to reference count that C++ object.
 *
 * This is a different use case than a smart pointer, so once again, instead of
 * (mis) using the ManagedObj in another strange way, we just provide a
 * non-intrusive reference count here.
 */
Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> > gBusObjectMap;
Object gBusObjectMapLock;

/**
 * This function takes a Java Bus Object and a newly created C++ backing
 * object and creates an entry in a global structure to establish the
 * relationship between the two.
 *
 * Whevever a Java Bus Listener is registered with a Bus Attachment, a
 * corresponding C++ object must be created to form the plumbing between
 * the AllJoyn system and the Java client.  Since a Java Bus Object may be
 * registered multiple times with multiple bus attachments, the C++ object
 * must be reference counted.  This function sets that reference count to
 * one, indicating a newly established relationship.
 *
 * This function transfers ownership of a NativeBusObject* from the caller to the
 * underlying map.  The caller must not free a provided NativeBusObject* unless
 * responsibility is explicitly transferred back by a non-zero return from
 * the function DecRefBackingObject.
 *
 * Since a reference to a Java Object is stored in the underlying map, we
 * insist that the caller must have taken a strong global reference to that
 * Java object prior to calling this function.
 *
 * Whenever a registerBusObject call is made, we expect the caller to check
 * to see if a relationship between the provided Java Bus Object and a C++
 * backing object exists, and if not create a backing object and call this
 * function to establish the relationship.
 *
 * Note that the lock on the underlying map is not taken in this and other
 * associated functions.  This is because, in most cases, atomicity must be
 * ensured across several calls that access the underlying map.  Therefore it is
 * the responsibility of the calling code to acquire the lock (i.e. call
 * gBusObjectMapLock.Lock) before calling any of the functions which access the
 * gBusObjectMap.
 */
void NewRefBackingObject(
    /* [in] */ IBusObject* carObject,
    /* [in] */ NativeBusObject* cppObject)
{
    Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> >::Iterator i = gBusObjectMap.Find(carObject);
    if (i != gBusObjectMap.End()) {
        Logger::E("NativeApi", "NewRefBackingObject(): Mapping already established for Bus Object %p", carObject);
        return;
    }

    gBusObjectMap[carObject] = MakePair((uint32_t)1, (NativeBusObject*)cppObject);
}

/**
 * This function takes a Java Bus Object and increments the reference count to a
 * C++ backing object that must already exist.
 *
 * Whevever a Java Bus Listener is registered with a Bus Attachment, a
 * corresponding C++ object must be created to form the plumbing between the
 * AllJoyn system and the Java client.  Since a Java Bus Object may be
 * registered multiple times with multiple bus attachments, the C++ object must
 * be reference counted.  This function increments that reference count
 * indicating the given Java Object is referred to indirectly through an AllJoyn
 * Bus Attachment.
 *
 * Since a reference to a Java Object is stored in the underlying map, we insist
 * that the caller must have taken another strong global reference to the
 * provided Java object prior to calling this function.  That is, when
 * registering an Java Object with a Bus Attachment the caller is expected to
 * take a new reference to the Java object using a JNI call, and then take a new
 * reference to the C++ object by making this call.
 *
 * Whenever a registerBusObject call is made, we expect the caller to check
 * to see if a relationship between the provided Java Bus Object and a C++
 * backing object exists, and if so call this function to increment the reference
 * count on the C++ object.
 *
 * Note that the lock on the underlying map is not taken in this and other
 * associated functions.  This is because, in most cases, atomicity must be
 * ensured across several calls that access the underlying map.  Therefore it is
 * the responsibility of the calling code to acquire the lock (i.e. call
 * gBusObjectMapLock.Lock) before calling any of the functions which access the
 * gBusObjectMap.
 */
void IncRefBackingObject(
    /* [in] */ IBusObject* carObject)
{
    Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> >::Iterator i = gBusObjectMap.Begin();
    for (; i != gBusObjectMap.End(); ++i) {
        if (carObject == i->mFirst.Get()) {
            uint32_t refCount = i->mSecond.mFirst + 1;
            NativeBusObject* cppObject = i->mSecond.mSecond;
            gBusObjectMap[carObject] = MakePair((uint32_t)refCount, (NativeBusObject*) cppObject);
            return;
        }
    }
}

/**
 * This function takes a Java Bus Object and decrements the reference count to a
 * C++ backing object that must already exist.
 *
 * Whevever a Java Bus Listener is registered with a Bus Attachment, a
 * corresponding C++ object must be created to form the plumbing between the
 * AllJoyn system and the Java client.  Since a Java Bus Object may be
 * registered multiple times with multiple bus attachments, the C++ object must
 * be reference counted.  This function decrements that reference count
 * indicating the given Java Object is no longer referred to indirectly through
 * an AllJoyn Bus Attachment.
 *
 * This function transfers ownership of a NativeBusObject* to the caller if the
 * reference count is decremented to zero.  That is, if NULL is returned, there
 * is no change of responsibility, but if a non-zero pointer to a NativeBusObject*
 * is returned, the caller is expected to do whatever it takes to tear down the
 * object and free it.
 *
 * Since a reference to a Java Object is stored in the underlying map, we
 * previously insisted that the caller must have taken a strong global reference
 * to that Java object prior to calling this function.
 *
 * Whenever an unregisterBusObject call is made, we expect the caller to release
 * the Java global reference to the Java Bus Object and decrement the reference
 * count to the corresponding C++ object by calling this function.  If we return
 * a non-zero pointer, the caller must delete the NativeBusObject returned.
 *
 * Note that the lock on the underlying map is not taken in this and other
 * associated functions.  This is because, in most cases, atomicity must be
 * ensured across several calls that access the underlying map.  Therefore it is
 * the responsibility of the calling code to acquire the lock (i.e. call
 * gBusObjectMapLock.Lock) before calling any of the functions which access the
 * gBusObjectMap.
 */
NativeBusObject* DecRefBackingObject(
    /* [in] */ IBusObject* carObject)
{
    Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> >::Iterator i = gBusObjectMap.Begin();
    for (; i != gBusObjectMap.End(); ++i) {
        if (carObject == i->mFirst.Get()) {
            NativeBusObject* cppObject = i->mSecond.mSecond;
            uint32_t refCount = i->mSecond.mFirst - 1;
            if (refCount) {
                gBusObjectMap[carObject] = MakePair((uint32_t)refCount, (NativeBusObject*)cppObject);
                cppObject = NULL;
            }
            else {
                gBusObjectMap.Erase(i);
            }
            return cppObject;
        }
    }

    Logger::E("NativeApi", "DecRefBackingObject(): No mapping exists for Java Bus Object %p", carObject);
    return NULL;
}

NativeBusObject* GetBackingObject(
    /* [in] */ IBusObject* busObject)
{
    Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> >::Iterator i = gBusObjectMap.Begin();
    for (; i != gBusObjectMap.End(); ++i) {
        if (busObject == i->mFirst.Get()) {
            return i->mSecond.mSecond;
        }
    }

    return NULL;
}


/**
 * Given a Java object that someone is claiming has been registered as a bus
 * object with a bus attachment; return the corresponding strong reference to it
 * that we must have saved.
 */
AutoPtr<IBusObject> GetGlobalRefForObject(
    /* [in] */ IBusObject* busObject)
{
    AutoPtr<IBusObject> temp(busObject);
    Map<AutoPtr<IBusObject>, Pair<uint32_t, NativeBusObject*> >::Iterator i = gBusObjectMap.Find(temp);
    if (i != gBusObjectMap.End()) {
        return i->mFirst;
    }

    return NULL;
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
ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ PVoid obj,
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
ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ ArrayOf<Int64>* objs,
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
ECode Unmarshal(
    /* [in] */ const ajn::MsgArg* arg,
    /* [in] */ CarDataType type,
    /* [out] */ PVoid object)
{
    return MsgArg::Unmarshal((Int64)arg, type, object);
}

/**
 * Unmarshal MsgArgs into an Object[].
 *
 * @param[in] args the MsgArgs
 * @param[in] numArgs the number of MsgArgs
 * @param[in] jmethod the Method that will be invoked with the returned Object[]
 * @param[out] junmarshalled the unmarshalled Java Object[]
 */
QStatus Unmarshal(
    /* [in] */ const ajn::MsgArg* args,
    /* [in] */ size_t numArgs,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    ajn::MsgArg arg(ajn::ALLJOYN_STRUCT);
    arg.v_struct.members = (ajn::MsgArg*)args;
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
QStatus Unmarshal(
    /* [in] */ ajn::Message& msg,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    const ajn::MsgArg* args;
    size_t numArgs;
    msg->GetArgs(numArgs, args);
    return Unmarshal(args, numArgs, method, unmarshalled);
}

static const String PATH("/system/lib/Org.Alljoyn.Bus.eco");
static AutoPtr<IClassLoader> sClassLoader;

AutoPtr<IClassLoader> GetModuleClassLoader()
{
    if (sClassLoader == NULL) {
        CPathClassLoader::New(PATH, ClassLoader::GetSystemClassLoader(),
                (IClassLoader**)&sClassLoader);
    }

    return sClassLoader;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
