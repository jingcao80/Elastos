#ifndef __CTESTEVENTLISTENER_H__
#define __CTESTEVENTLISTENER_H__

#define JSPkgName JSHTMLViewer

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"

#include "_Elastos_DevSamples_Node_JSHTMLViewer_CTestEventListener.h"

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

//----------------NodeBridge Definition Start---------------

enum NodeMessage_Status {
        NodeMessage_Status_Null = 0,
        NodeMessage_Status_Ready = 1,
        NodeMessage_Status_Running = 2,
        NodeMessage_Status_Finish = 3,
};

struct NodeMessage;
struct NodeMessageQueue;
struct NodeBridge;

struct NodeMessage
{
    void* mObj;
    void (*mSend)(void*);
    void (*mProc)(void*);
    void* mPayload;
    int mSync;

    int mStatus;

    NodeMessageQueue* mQueue;
    int mIndex;

    int mFromIdx;
    NodeMessage* mSource;
};

struct NodeMessageQueue
{
    NodeBridge* mBridge;
    int mIndex;
    NodeMessage** mMessages;
    pthread_mutex_t mMutex;
    int mPid;   //pid_t
    int mTid;   //pthread_t
    int mMax;
    int mTop;
    char* mActivityName;
    void* mActivity;    //activity/service/provider/receiver
    void* mActivityListener;
    void* mHandler;
    void* mHandlerListener;

    pthread_mutex_t queue_mtx;
    pthread_cond_t queue_cond;
    bool queue_waiting;
};

struct NodeBridgeVT
{
    void (*Enqueue) (void* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload);
    void (*Invoke)  (void* _this);
    void (*Tick)    (void* _this);
    void (*Init)    (void* _this);
};

struct NodeBridge
{
    NodeBridgeVT* vt;

    void** mQueues;
    pthread_mutex_t mMutex;

    int mMax;
    int mTop;

    int mTag;
    bool mInit;

    int mNODE;
    int mEPK;

    char* mPackageName;
};

//----------------NodeBridge Definition End---------------

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

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
        /* [in] */ Int32 tag /*for debug only*/);

    ~CallbackRunnable();

    CAR_INTERFACE_DECL()

    CARAPI Run();

    CARAPI GetInstance(IInterface** ppInstance);

    static void NodeMessage_FireCallback(void* payload)
    {
        ECode ec = NOERROR;

        AutoPtr<IMessage> msg = (IMessage*)payload;
        AutoPtr<IRunnable> runnable;
        msg->GetCallback((IRunnable**)&runnable);

        ICallbackRunnable* _callbackRunnable = (ICallbackRunnable*)runnable->Probe(EIID_ICallbackRunnable);
        CallbackRunnable* callbackInstance;
        _callbackRunnable->GetInstance((IInterface**)&callbackInstance);

        ec = callbackInstance->mMethod->Invoke(callbackInstance->mObject, callbackInstance->mArgumentList);
        if (FAILED(ec)) {
            ALOGD("NodeMessage_FireCallback================Invoke failed================");
        }
    }

    static void NodeMessage_Send(void* payload)
    {
        AutoPtr<IHandler> target = myHandler;
        AutoPtr<IMessage> msg = (IMessage*)payload;

        Boolean result;
        target->SendMessage(msg, &result);
    }

    static void EnqueueUIMessage(void* obj, void* method, void* params)
    {
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

CarClass(CTestEventListener)
    , public Object
    , public ITestEventListener
{

public:
    class _Thread : public Thread
    {
    public:
        _Thread(
            const String& packageName)
            : mPackageName(packageName)
        {
            Thread::constructor(String("CTestEventListener::_Thread"));
        }

        CARAPI Run();

    private:
        String mPackageName;
    };

    static void InitBridge(
        /* [in] */ const String& packageName)
    {
        AutoPtr<_Thread> t = new _Thread(packageName);

        pthread_mutex_t* pMutex = &mMutex;

        pthread_mutex_init(pMutex, NULL);
        pthread_mutex_lock(pMutex);

        t->Start();

        //TODO: use condition()
        pthread_mutex_lock(pMutex);
        pthread_mutex_unlock(pMutex);

        return;
    };

    static void RegisterActivity(
        /* [in] */ const String& packageName,
        /* [in] */ const String& activityName,
        /* [in] */ IInterface* activityInstance,
        /* [out] */ IActivityListener** activityListener,
        /* [in] */ IHandler* activityHandler)
    {
        if (!CTestEventListener::mNodeInit) {
            CTestEventListener::InitBridge(packageName);
            CTestEventListener::mNodeInit = true;
        }

        Boolean result = false;
        if(CTestEventListener::mNodeBridgeListener) {
            ECode ec = CTestEventListener::mNodeBridgeListener->OnRegistActivity(
                packageName, activityName, activityInstance, (Int32)activityListener, activityHandler, &result);
            if (FAILED(ec)) {
                ALOGD("NodeMessage_FireCallback================OnRegistActivity failed================");
            }
        }

        //TODO: deal with result
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
        /* [in] */ Int32 ppActivityListener,    //IActivityListener**
        /* [in] */ IActivityListener* pJsActivityListener);

    CARAPI SetNodeBridgeListener(
        /* [in] */ INodeBridgeListener* pNodeBridgeListener);

private:
    void (*cbFunction)(void*);
public:
    static pthread_mutex_t mMutex;

    static AutoPtr<INodeBridgeListener> mNodeBridgeListener;

    static bool mNodeInit;

    CARAPI SetNodeBridge(
        /* [in] */ Int32 from,
        /* [in] */ Int32 threadIndex);
};

}   //namespace JSPkgName
}   //namespace Node
}   //namespace DevSamples
}   //namespace Elastos

#endif // __CTESTEVENTLISTENER_H__