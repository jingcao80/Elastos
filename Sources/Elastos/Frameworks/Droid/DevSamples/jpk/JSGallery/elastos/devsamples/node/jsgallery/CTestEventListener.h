#ifndef __CTESTEVENTLISTENER_H__
#define __CTESTEVENTLISTENER_H__

#define JSPkgName JSGallery

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"

#include "_Elastos_DevSamples_Node_JSGallery_CTestEventListener.h"

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

    // interface ICallbackRunnable {
    //     GetInstance(
    //         [out] IInterface** instance);
    // }


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

    //CAR_OBJECT_DECL()

    // CARAPI constructor(
    //     /* [in] */ IInterface* object,
    //     /* [in] */ IMethodInfo* method,
    //     /* [in] */ IArgumentList* argumentList,
    //     /* [in] */ pthread_mutex_t* mutex);

    CARAPI Run();

    CARAPI GetInstance(IInterface** ppInstance);

    static void NodeMessage_FireCallback(void* payload) {
        ALOGD("NodeMessage_FireCallback================begin================");

        ECode ec = NOERROR;

        AutoPtr<IMessage> msg = (IMessage*)payload;
        AutoPtr<IRunnable> runnable;
        msg->GetCallback((IRunnable**)&runnable);

        ALOGD("NodeMessage_FireCallback================1================");

        IInterface* _interface = runnable->Probe(EIID_IInterface);
        //IInterface* _object = runnable->Probe(EIID_IObject);
        //CallbackRunnable* callback = (CallbackRunnable*)_interface;
        CallbackRunnable* callback = *(CallbackRunnable**)&_interface;
        //CallbackRunnable* callback = *(CallbackRunnable**)&runnable;


        ICallbackRunnable* _callbackRunnable = (ICallbackRunnable*)runnable->Probe(EIID_ICallbackRunnable);

        CallbackRunnable* callback_1;
        _callbackRunnable->GetInstance((IInterface**)&callback_1);

        ALOGD("NodeMessage_FireCallback================2================");
        ALOGD("NodeMessage_FireCallback================2.0================mTag:%d",callback_1->mTag);
        ALOGD("NodeMessage_FireCallback================2.1================mTag:%d",callback->mTag);

        ALOGD("NodeMessage_FireCallback================2.2================check args");
        IMethodInfo* _methodInfo = callback_1->mMethod.Get();
        ALOGD("NodeMessage_FireCallback================2.3================mMethod:%p",_methodInfo);
        IInterface* _object = callback_1->mObject.Get();
        ALOGD("NodeMessage_FireCallback================2.4================mObject:%p",_object);
        IArgumentList* _argumentList = callback_1->mArgumentList.Get();
        ALOGD("NodeMessage_FireCallback================2.5================mArgumentList:%p",_argumentList);

        //ec = callback->mMethod->Invoke(callback->mObject, callback->mArgumentList);
        ec = callback_1->mMethod->Invoke(callback_1->mObject, callback_1->mArgumentList);

        ALOGD("NodeMessage_FireCallback================3================");

        if (FAILED(ec)) {
            ALOGD("NodeMessage_FireCallback================Invoke failed================");
        }

        ALOGD("NodeMessage_FireCallback================end================");
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

CarClass(CTestEventListener)
    , public Object
    , public ITestEventListener
{

public:
    class _Thread : public Thread
    {
    public:
        _Thread(
            /* [in] */ const String& packageName)
            : mPackageName(packageName)
        {
            Thread::constructor(String("CTestEventListener::_Thread"));
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
        ALOGD("CTestEventListener::RegisterActivity================begin================");

        if (!CTestEventListener::mNodeInit) {
            CTestEventListener::InitBridge(packageName);
            CTestEventListener::mNodeInit = true;
        }

        Boolean result = false;
        if(CTestEventListener::mNodeBridgeListener) {
            ALOGD("CTestEventListener::RegisterActivity================mNodeBridgeListener OnRegistActivity.begin================");

            CTestEventListener::mNodeBridgeListener->OnRegistActivity(
                packageName, activityName, activityInstance, (Int32)activityListener, activityHandler, &result);

            ALOGD("CTestEventListener::RegisterActivity================mNodeBridgeListener OnRegistActivity.end================");
        }
        else {
            ALOGD("CTestEventListener::RegisterActivity================mNodeBridgeListener is null================");
        }

        ALOGD("CTestEventListener::RegisterActivity================end================");

        //g_pNodeBridge->vt->RegisterActivity(packageName.string(), activityName.string());
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

    //String mPackageName;
    //String mActivityName;

    static AutoPtr<INodeBridgeListener> mNodeBridgeListener;

    static bool mNodeInit;
};

}   //namespace JSPkgName
}   //namespace Node
}   //namespace DevSamples
}   //namespace Elastos

#endif // __CTESTEVENTLISTENER_H__
