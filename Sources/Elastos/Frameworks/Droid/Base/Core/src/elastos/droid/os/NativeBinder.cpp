//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/NativeBinder.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/BinderProxy.h"
#include <binder/Binder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#define DEBUG_DEATH 0
#if DEBUG_DEATH
#define LOGDEATH ALOGD
#else
#define LOGDEATH ALOGV
#endif

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

static String TAG("NativeBinder");

static struct bindernative_offsets_t
{
    // Object state.
    Int32 mObject;

} gBinderOffsets;

static struct binderproxy_offsets_t
{
    // Object state.
    Int32 mObject;
    Int32 mSelf;
    Int32 mOrgue;

} gBinderProxyOffsets;

class DroidBBinder : public android::BBinder
{
public:
    DroidBBinder(
        /* [in] */ IInterface* object)
        : mObject(object)
    {}

    bool checkSubclass(const void* subclassID) const
    {
        return subclassID == &gBinderOffsets;
    }

    AutoPtr<IInterface> object() const
    {
        return mObject;
    }

private:
    AutoPtr<IInterface> mObject;
};

class DroidBBinderHolder : public android::RefBase
{
public:
    android::sp<DroidBBinder> get(IInterface* obj)
    {
        AutoLock _l(mLock);

        android::sp<DroidBBinder> b = mBinder.promote();
        if (b == NULL) {
            b = new DroidBBinder(obj);
            mBinder = b;
            // ALOGV("Creating JavaBinder %p (refs %p) for Object %p, weakCount=%d\n",
            //      b.get(), b->getWeakRefs(), obj, b->getWeakRefs()->getWeakCount());
        }

        return b;
    }

    android::sp<DroidBBinder> getExisting()
    {
        AutoLock _l(mLock);

        return mBinder.promote();
    }

private:
    Object mLock;
    android::wp<DroidBBinder> mBinder;
};

static Object sProxyLock;

AutoPtr<IBinder> DroidObjectForIBinder(const android::sp<android::IBinder>& val)
{
    if (val == NULL) return NULL;

    if (val->checkSubclass(&gBinderOffsets)) {
        // One of our own!
        AutoPtr<IInterface> object = static_cast<DroidBBinder*>(val.get())->object();
        // LOGDEATH("objectForBinder %p: it's our own %p!\n", val.get(), object.Get());
        return IBinder::Probe(object);
    }

    // For the rest of the function we will hold this lock, to serialize
    // looking/creation of Java proxies for native Binder proxies.
    AutoLock _l(sProxyLock);

    // Someone else's...  do we know about it?
    IWeakReference* ref = (IWeakReference*)val->findObject(&gBinderProxyOffsets);
    AutoPtr<IBinder> object;
    if (ref != NULL) {
        ref->Resolve(EIID_IBinder, (IInterface**)&object);
    }
    if (object != NULL) {
        // jobject res = env->CallObjectMethod(object, gWeakReferenceOffsets.mGet);
        // if (res != NULL) {
        //     ALOGV("objectForBinder %p: found existing %p!\n", val.get(), res);
        //     return res;
        // }
        // LOGDEATH("Proxy object %p of IBinder %p no longer in working set!!!", object.Get(), val.get());
        // android_atomic_dec(&gNumProxyRefs);
        // val->detachObject(&gBinderProxyOffsets);
        // env->DeleteGlobalRef(object);
        return object;
    }

    AutoPtr<BinderProxy> proxy = new BinderProxy();
    // LOGDEATH("objectForBinder %p: created new proxy %p !\n", val.get(), object.Get());
    // The proxy holds a reference to the native object.
    proxy->mObject = (Int32)val.get();
    val->incStrong(proxy.Get());

    // The native object needs to hold a weak reference back to the
    // proxy, so we can retrieve the same proxy if it is still active.
    AutoPtr<IWeakReference> wr;
    proxy->GetWeakReference((IWeakReference**)&wr);
    val->attachObject(&gBinderProxyOffsets, wr.Get(),
            NULL/*jnienv_to_javavm(env)*/, NULL/*proxy_cleanup*/);
    wr->AddRef();

    // Also remember the death recipients registered on this proxy
    // sp<DeathRecipientList> drl = new DeathRecipientList;
    // drl->incStrong((void*)javaObjectForIBinder);
    // env->SetIntField(object, gBinderProxyOffsets.mOrgue, reinterpret_cast<jint>(drl.get()));

    // Note that a new object reference has been created.
    // android_atomic_inc(&gNumProxyRefs);
    // incRefsCreated(env);

    return proxy.Get();
}

android::sp<android::IBinder> IBinderForDroidObject(IBinder* obj)
{
    android::sp<android::IBinder> jbinder = NULL;

    if (obj == NULL) {
        jbinder = NULL;
    }
    else if (obj->Probe(EIID_IBinderProxy) != NULL) {
        jbinder = (android::IBinder*)((BinderProxy*)obj)->mObject;
    }
    else if (obj->Probe(EIID_IBinder) != NULL) {
        DroidBBinderHolder* dbh = (DroidBBinderHolder*)((CBinder*)obj)->mObject;
        jbinder = dbh != NULL ? dbh->get(obj) : NULL;
    }
    else {
        Logger::W(TAG, "IBinderForDroidObject: %p is not a Binder object", obj);
    }

    return jbinder;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
