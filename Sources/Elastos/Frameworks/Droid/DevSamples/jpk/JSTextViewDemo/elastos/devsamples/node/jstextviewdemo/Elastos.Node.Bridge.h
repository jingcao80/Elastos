#ifndef __ELASTOSNODEBRIDGE_H__
#define __ELASTOSNODEBRIDGE_H__

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

#endif // __H__