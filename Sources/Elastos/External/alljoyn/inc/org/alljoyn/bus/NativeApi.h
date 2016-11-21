
#ifndef __ORG_ALLJOYN_BUS_NATIVEAPI_H__
#define __ORG_ALLJOYN_BUS_NATIVEAPI_H__

#include <elastos/core/Object.h>
#include "org/alljoyn/bus/NativeBusObject.h"
#include <alljoyn/MsgArg.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

extern Object gBusObjectMapLock;

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
 * This function transfers ownership of a JBusObject* from the caller to the
 * underlying map.  The caller must not free a provided JBusObject* unless
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
    /* [in] */ IBusObject* busObject,
    /* [in] */ NativeBusObject* ntBusObject);

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
 * This function transfers ownership of a JBusObject* to the caller if the
 * reference count is decremented to zero.  That is, if NULL is returned, there
 * is no change of responsibility, but if a non-zero pointer to a JBusObject*
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
 * a non-zero pointer, the caller must delete the JBusObject returned.
 *
 * Note that the lock on the underlying map is not taken in this and other
 * associated functions.  This is because, in most cases, atomicity must be
 * ensured across several calls that access the underlying map.  Therefore it is
 * the responsibility of the calling code to acquire the lock (i.e. call
 * gBusObjectMapLock.Lock) before calling any of the functions which access the
 * gBusObjectMap.
 */
NativeBusObject* DecRefBackingObject(
    /* [in] */ IBusObject* busObject);

/**
 * Given a Java object that someone is claiming has been registered as a bus
 * object with a bus attachment; return the corresponding C++ object that
 * hooks it to the AllJoyn system.
 *
 * Note that the lock on the underlying map is not taken in this and other
 * associated functions.  This is because, in most cases, atomicity must be
 * ensured across several calls that access the underlying map.  Therefore it is
 * the responsibility of the calling code to acquire the lock (i.e. call
 * gBusObjectMapLock.Lock) before calling any of the functions which access the
 * gBusObjectMap.
 */
NativeBusObject* GetBackingObject(
    /* [in] */ IBusObject* busObject);

/**
 * Given a Java object that someone is claiming has been registered as a bus
 * object with a bus attachment; return the corresponding strong reference to it
 * that we must have saved.
 */
AutoPtr<IBusObject> GetGlobalRefForObject(
    /* [in] */ IBusObject* busObject);

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
    /* [in] */ IInterface* obj,
    /* [in] */ ajn::MsgArg* arg);

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEAPI_H__
