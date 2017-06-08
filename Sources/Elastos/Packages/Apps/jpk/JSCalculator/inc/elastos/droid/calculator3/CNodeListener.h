#ifndef __CTESTEVENTLISTENER_H__
#define __CTESTEVENTLISTENER_H__

//#define JSPkgName JSTextViewDemo
#define JSPkgName Calculator3
#define JSEvtName CNodeListener

#ifndef JSCarClassHead

#define AA(y) #y
#define BB(x,y) AA(x##y.h)
#define CC(x,y,z) BB(x##z,y)
#define DD(x,y,z) CC(z##x,y,_)
//#define EE(x,y) DD(x,y,_Elastos_DevSamples_Node_)
#define EE(x,y) DD(x,y,_Elastos_Droid_)
#define FF(x) AA(x)

#define JSEvtCarClassHead EE(JSPkgName,JSEvtName)

#define JSPkgNameStr FF(JSPkgName)
#define JSEvtNameStr FF(JSEvtName)

#define JSCarClass(x) CarClass(x)

#define JS_TO_STRING_IMPL_0(x) TO_STRING_IMPL(#x)
#define JS_TO_STRING_IMPL(x,y) JS_TO_STRING_IMPL_0(x::y)

#define JS_CAR_INTERFACE_IMPL(x,y,z) CAR_INTERFACE_IMPL(x,y,z)
#define JS_CAR_OBJECT_IMPL(x) CAR_OBJECT_IMPL(x)

#endif

#include JSEvtCarClassHead

#include "Elastos.Node.Bridge.h"

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

#include <cutils/log.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::Thread;
using Elastos::Utility::ILinkedList;

using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CMessage;

//namespace Elastos {
//namespace DevSamples {
//namespace Node {
//namespace JSPkgName {

namespace Elastos {
namespace Droid {
namespace Calculator3 {

EXTERN IHandler* myHandler;

EXTERN NodeBridge* g_pNodeBridge;
EXTERN NodeBridge** g_ppNodeBridge;

class CallbackRunnable
    : public Object
    , public IRunnable
    , public ICallbackRunnable
{
public:

    CallbackRunnable(
        /* [in] */ IInterface* object,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* argumentList,
        /* [in] */ pthread_mutex_t* mutex,
        /* [in] */ Int32 tag);

    ~CallbackRunnable();

    CAR_INTERFACE_DECL()

    CARAPI Run();

    CARAPI GetInstance(IInterface** ppInstance);

    static void NodeMessage_FireCallback(void* payload) {
        ECode ec = NOERROR;

        AutoPtr<IMessage> msg = (IMessage*)payload;
        AutoPtr<IRunnable> runnable;
        msg->GetCallback((IRunnable**)&runnable);

        IInterface* _interface = runnable->Probe(EIID_IInterface);
        //IInterface* _object = runnable->Probe(EIID_IObject);
        //CallbackRunnable* callback = (CallbackRunnable*)_interface;
        CallbackRunnable* callback = *(CallbackRunnable**)&_interface;
        //CallbackRunnable* callback = *(CallbackRunnable**)&runnable;


        ICallbackRunnable* _callbackRunnable = (ICallbackRunnable*)runnable->Probe(EIID_ICallbackRunnable);

        CallbackRunnable* callback_1;
        _callbackRunnable->GetInstance((IInterface**)&callback_1);

        IMethodInfo* _methodInfo = callback_1->mMethod.Get();
        IInterface* _object = callback_1->mObject.Get();
        IArgumentList* _argumentList = callback_1->mArgumentList.Get();

        //ec = callback->mMethod->Invoke(callback->mObject, callback->mArgumentList);
        ec = callback_1->mMethod->Invoke(callback_1->mObject, callback_1->mArgumentList);

        if (FAILED(ec)) {
            ALOGD("NodeMessage_FireCallback================Invoke failed================");
        }
    }

    static void NodeMessage_Send(void* payload) {
        AutoPtr<IHandler> target = myHandler;
        AutoPtr<IMessage> msg = (IMessage*)payload;

        Boolean result;
        target->SendMessage(msg, &result);
    }

    static void EnqueueUIMessage(void* obj, void* method, void* params)
    {
        pthread_t mThread = pthread_self();

        AutoPtr<IMessage> msg;
        CMessage::New((IMessage**)&msg);

        AutoPtr<IHandler> target = myHandler;
        msg->SetTarget(target);

        Int32 MSG_RUNONUITHREAD = 0x0;
        msg->SetWhat(MSG_RUNONUITHREAD);

        pthread_mutex_t* mutex;

        CallbackRunnable* callback = new CallbackRunnable(
            (IInterface*)obj, (IMethodInfo*)method, (IArgumentList*)params, (pthread_mutex_t*)mutex, 258);
        IRunnable* runnable = IRunnable::Probe(callback);
        msg->SetCallback(runnable);

        ALOGD("EnqueueUIMessage======================mTag:%d",callback->mTag);

        g_pNodeBridge->vt->Enqueue(g_pNodeBridge, obj, NodeMessage_Send, NodeMessage_FireCallback, (void*)msg);

        return;
    };

private:
    AutoPtr<IInterface> mObject;
    AutoPtr<IMethodInfo> mMethod;
    AutoPtr<IArgumentList> mArgumentList;

    static CallbackRunnable* mInstances[];

    Int32 mMyLock;

    Int32 mTag;
};

JSCarClass(JSEvtName)
    , public Object
    , public INodeListener
{

public:
    class _Thread : public Thread
    {
    public:
        _Thread(
            /* [in] */ const String& packageName)
            : mPackageName(packageName)
        {
            pthread_t ctid = pthread_self();
            ALOGD("CNodeListener::_Thread============THREAD_MAIN:%d", ctid);

            Thread::constructor(String("CNodeListener::_Thread"));
        }

        CARAPI Run();

    private:
        String mPackageName;
    };

    static void InitBridge(const String& packageName) {
        AutoPtr<_Thread> t = new _Thread(packageName);

        pthread_mutex_t* pMutex = &mMutex;

        pthread_mutex_init(pMutex, NULL);
        pthread_mutex_lock(pMutex);

        t->Start();

        //todo: use wait()
        pthread_mutex_lock(pMutex);
        pthread_mutex_unlock(pMutex);

        return;
    };

    static void RegisterActivity(const String& packageName, const String& activityName, IInterface* activityInstance, IActivityListener** activityListener, IHandler* activityHandler) {
        if (!JSEvtName::mNodeInit) {
            JSEvtName::InitBridge(packageName);
            JSEvtName::mNodeInit = true;
        }

        Boolean result = false;
        if(JSEvtName::mNodeBridgeListener) {
            JSEvtName::mNodeBridgeListener->OnRegisterActivity(
                packageName, activityName, activityInstance, (Int32)activityListener, activityHandler, &result);
        }
        else {
            ALOGD("RegisterActivity================mNodeBridgeListener is null================");
        }
    }

    static ECode Require(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface** object)
    {
        assert(object != NULL);

        ECode ec = NOERROR;

        AutoPtr<IModuleInfo> moduleInfo;
        ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
        if (FAILED(ec)) {
            ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
            return ec;
        }

        AutoPtr<IClassInfo> classInfo;
        ec = moduleInfo->GetClassInfo(className, (IClassInfo**)&classInfo);
        if (FAILED(ec)) {
            ALOGD("Acquire \"%s\" class info failed!\n", className.string());
            return ec;
        }

        AutoPtr<IInterface> testObject;
        ec = classInfo->CreateObject((IInterface**)&testObject);
        if (FAILED(ec)) {
            ALOGD("Create object failed!\n");
            return ec;
        }

        *object = testObject;
        REFCOUNT_ADD(*object);

        return ec;
    }

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnNodeEvent(
        /* [in] */ Int32 eventId);

    CARAPI GetEnqueueUIMessagePtr(
        /* [out] */ Int32 * pOutPtr);

    CARAPI Sleep(
        /* [in] */ Int32 interval);

    CARAPI Lock();

    CARAPI Unlock();

    CARAPI SetActivityListener(
        ///* [in] */ IActivityListener** ppActivityListener,
        /* [in] */ Int32 ppActivityListener,
        /* [in] */ IActivityListener* pJsActivityListener);

    CARAPI SetNodeBridgeListener(
        /* [in] */ INodeBridgeListener* pNodeBridgeListener);

    CARAPI SetNodeBridge(
        /* [in] */ Int32 from,
        /* [in] */ Int32 threadIndex);

private:
    void (*cbFunction)(void*);
public:
    static pthread_mutex_t mMutex;

    static AutoPtr<INodeBridgeListener> mNodeBridgeListener;

    static bool mNodeInit;
};

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

//}   //namespace JSPkgName
//}   //namespace Node
//}   //namespace DevSamples
//}   //namespace Elastos

#endif // __H__