#ifndef __ELASTOS_ROOT_H__
#define __ELASTOS_ROOT_H__

#ifndef PLATFORM
#include "ElNode.h"
#include "config.h"
#endif  //PLATFORM

#include "wtf/CryptographicallyRandomNumber.h"
#include "wtf/MainThread.h"
#include "wtf/WTF.h"

#include "npruntime.h"

#include "V8Initializer.h"

#include <unistd.h>
#include "V8NPObject.h"
#include <node.h>
#include <uv.h>

#include "elastos.h"
#include "CarValueV8.h"
#include "CarUtilityPrivate.h"

#include <utils/Log.h>

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

    int mStatus;    //0:null/1:ready/2:running/3:finished

    NodeMessageQueue* mQueue;
    int mIndex;

    int mFromIdx;   //mSource
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

namespace JSC {
namespace Bindings {

struct async_req {
    uv_work_t req;
    int input;
    int output;
    v8::Persistent<v8::Function> callback;
};

struct ElCallbackMessage
{
    void (*cbFunction)(void*);
    void* cbParameters;
};

class TestCallbackBuf {
public:
    TestCallbackBuf(
        /* [in] */ void (*cbFunction)(void*),
        /* [in] */ void* cbParameters,
        /* [in] */ Int32 cbLock);

    ~TestCallbackBuf();

    static void SetReady(bool ready) {
        TestCallbackBuf* testCallbackBuf = TestCallbackBuf::mInstances[0];
        testCallbackBuf->mReady = ready;
    }

    static bool GetReady() {
        TestCallbackBuf* testCallbackBuf = TestCallbackBuf::mInstances[0];
        return testCallbackBuf->mReady;
    }

    //Run in elastos ui thread
    static void EnqueueFunctionPtr(void (*proc)(void*), void* payload) {
        pthread_t mThread = pthread_self();
        elCallbackQuene.cbFunction = proc;
        elCallbackQuene.cbParameters = payload;
        TestCallbackBuf* callback = new TestCallbackBuf(
            (void(*)(void*))proc, (void*)payload, 1);
        SetReady(true);
        Int32 cnt = 0;
        while (callback->mMyLock) {
            //to be finished: check elastos ui message queue (looper)
            cnt++;
            if (cnt == 3) callback->mMyLock = 0;
            sleep(1);
        }
    }

    //Run in node.js main thread
    static void Invoke() {
        TestCallbackBuf* testCallbackBuf = TestCallbackBuf::mInstances[0];
        testCallbackBuf->mFunction(testCallbackBuf->mParameters);
        testCallbackBuf->mMyLock = 0;
    }

    //to be update to an event quene
    static ElCallbackMessage elCallbackQuene;
    //event quene flag
    static bool  cbReady;

    bool mReady;

    //node.js message queue
    static TestCallbackBuf* mInstances[];
    static int mUsed;

    static int __mMyLock;

    void (*mFunction)(void*);
    void* mParameters;
    int mMyLock;
};

}   //JSC
}   //Bindings

#endif //__ELASTOS_ROOT_H__