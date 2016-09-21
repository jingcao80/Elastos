
#include "elastos_root.h"

using namespace v8;
using namespace node;

void (*cbEnqueueUIMessage)(void*, void*, void*);

NodeBridge gNodeBridge;
NodeBridge* pNodeBridge;
NodeBridge** ppNodeBridge;

static pthread_mutex_t uv_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t uv_cond = PTHREAD_COND_INITIALIZER;
static bool uv_waiting = FALSE;

void NodeBridge_Enqueue(NodeBridge* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload);
void _NodeBridge_Enqueue(void* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload)
{
    NodeBridge_Enqueue((NodeBridge*)_this, obj, send, proc, payload);
}
void NodeBridge_Invoke(NodeBridge* _this);
void _NodeBridge_Invoke(void* _this)
{
    NodeBridge_Invoke ((NodeBridge*)_this);
}
void NodeBridge_Tick(NodeBridge* _this);
void _NodeBridge_Tick(void* _this)
{
    NodeBridge_Tick((NodeBridge*)_this);
}
void NodeBridge_Init(NodeBridge* _this);
void _NodeBridge_Init(void* _this)
{
    NodeBridge_Init((NodeBridge*)_this);
}
NodeBridgeVT nodeBridgeVT = {
    _NodeBridge_Enqueue,
    _NodeBridge_Invoke,
    _NodeBridge_Tick,
    _NodeBridge_Init
};

NodeMessageQueue* _NodeBridge_GetThreadMessageQueue(NodeBridge* _this)
{
    NodeMessageQueue* mq;

    pthread_t tid = pthread_self();
    int idx = 0;

    int i = 0, im = _this->mTop;
    for (; i < im; i++) {
        mq = (NodeMessageQueue*)_this->mQueues[i];
        if (mq->mTid == tid) break;
    }

    idx = i;
    if (i == im) {
        mq = (NodeMessageQueue*)_this->mQueues[i];
        mq->mTid = tid;
        _this->mTop++;

        if (_this->mTop > _this->mMax) {
            ALOGD("_NodeBridge_GetThreadMessageQueue========mThreads buffer full========");
        }
    }

    return mq;
}

void NodeMessageQueue_UnLock(NodeMessageQueue* _this);

void NodeMessage_Finish(NodeMessage* _this) {
    NodeMessageQueue* mq;
    mq = _NodeBridge_GetThreadMessageQueue(pNodeBridge);
    NodeMessageQueue_UnLock(mq);
}

//Run in local thread
void NodeMessage_Invoke(NodeMessage* _this) {
    _this->mStatus = NodeMessage_Status_Running;
    _this->mProc(_this->mPayload);
    _this->mStatus = NodeMessage_Status_Finish;
    _this->mQueue->mTop--;
    return;
}

void NodeMessage_Send(NodeMessage* _this) {
    _this->mStatus = NodeMessage_Status_Running;
    if (_this->mSend != NULL) {
        _this->mSend(_this->mPayload);
    }
    else {
        ALOGD("NodeMessage_Send========mSend NULL========");
    }
    return;
}

void NodeMessageQueue_Invoke(NodeMessageQueue* _this) {
    if (_this->mTop == 0) return;
    NodeMessage* msg = _this->mMessages[0];
    if (msg->mStatus > NodeMessage_Status_Ready) return;
    NodeMessage_Invoke(msg);
    return;
}

void NodeBridge_Invoke(NodeBridge* _this) {
    NodeMessageQueue* mq;
    mq = _NodeBridge_GetThreadMessageQueue(_this);
    NodeMessageQueue_Invoke(mq);
    return;
}

void NodeBridge_Tick(NodeBridge* _this) {
    NodeMessageQueue* mq;
    mq = _NodeBridge_GetThreadMessageQueue(_this);
    if (mq->mTop > 0) {
       NodeMessage* msg = mq->mMessages[mq->mTop - 1];

        if (msg->mStatus == NodeMessage_Status_Ready) {
            NodeMessage_Invoke(msg);
        }
        else {
            sleep(1);
        }
    }
    else {
        sleep(1);
    }
    return;
}

bool NodeMessageQueue_IsLocking(NodeMessageQueue* _this);

void NodeMessageQueue_SendLock(NodeMessageQueue* _this)
{
    NodeMessage* msg = _this->mMessages[_this->mTop - 1];
    msg->mStatus = NodeMessage_Status_Ready;
    NodeMessageQueue* _that = (NodeMessageQueue*)_this->mBridge->mQueues[msg->mFromIdx];
    if (!NodeMessageQueue_IsLocking(_that)) {
        NodeMessage_Send(_this->mMessages[0]);
    }
}

void NodeMessageQueue_UnLock(NodeMessageQueue* _this)
{
    if (_this->mTop > 0) {
        _this->mTop--;
        NodeMessage* msg = _this->mMessages[_this->mTop];
        if (msg->mStatus == NodeMessage_Status_Ready) {
            msg->mStatus = NodeMessage_Status_Finish;

            NodeMessageQueue* mq = (NodeMessageQueue*)_this->mBridge->mQueues[msg->mFromIdx];
            NodeMessageQueue_UnLock(mq);
        }
    }
}

bool NodeMessage_IsLocking(NodeMessage* _this)
{
    return (_this->mStatus < NodeMessage_Status_Finish);

}
bool NodeMessageQueue_IsLocking(NodeMessageQueue* _this)
{
    bool result;
    if (_this->mTop == 0) {
        result = false;
    }
    else {
        NodeMessage* msg = _this->mMessages[_this->mTop - 1];
        result = NodeMessage_IsLocking(msg);
    }
    return result;
}

NodeMessage* NodeMessageQueue_Push(NodeMessageQueue* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload)
{
    pthread_mutex_lock(&_this->mMutex);
    NodeMessage* msg = _this->mMessages[_this->mTop];
    _this->mTop++;
    if (_this->mTop == _this->mMax) {
        //TODO:enlarge message stack
        ALOGD("NodeMessageQueue_Push ======= error: Queue overflow==========");
        //return NULL;
    }
    msg->mObj = obj;
    msg->mSend = send;
    msg->mProc = proc;
    msg->mPayload = payload;
    msg->mStatus = NodeMessage_Status_Ready;
    pthread_mutex_unlock(&_this->mMutex);
    return msg;
}

int _ttt = 0;

void NodeMessageQueue_Enqueue(NodeMessageQueue* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload)
{
    NodeMessage* msg = NodeMessageQueue_Push(_this, obj, send, proc, payload);
    NodeMessageQueue* mq = _NodeBridge_GetThreadMessageQueue(_this->mBridge);
    msg->mFromIdx = mq->mIndex;
    int top = mq->mTop;

    bool tag_signal_uv = true;
    bool tag_signal_queue = true;

    while ( NodeMessage_IsLocking(msg) ) {
        int _top = mq->mTop;
        if (_top > top) {
            NodeMessage* _msg = mq->mMessages[_top - 1];
            NodeMessage_Invoke(_msg);
        }
        else {
            bool sendSignal = TRUE;
            if ( uv_waiting && tag_signal_uv ) {
                tag_signal_uv = false;

                pthread_mutex_lock(&uv_mtx);
                pthread_cond_signal(&uv_cond);
                pthread_mutex_unlock(&uv_mtx);
            }
            else {
                if ( _this->queue_waiting && tag_signal_queue ){
                    tag_signal_queue = false;

                    pthread_mutex_lock(&_this->queue_mtx);
                    pthread_cond_signal(&_this->queue_cond);
                    pthread_mutex_unlock(&_this->queue_mtx);
                }
                else {
                    sendSignal = FALSE;
                }
            }
            if ( NodeMessage_IsLocking(msg) && sendSignal) {
                pthread_mutex_lock(&mq->queue_mtx);

                mq->queue_waiting = TRUE;
                pthread_cond_wait(&mq->queue_cond,&mq->queue_mtx);
                mq->queue_waiting = FALSE;

                tag_signal_uv = true;
                tag_signal_queue = true;

                pthread_mutex_unlock(&mq->queue_mtx);
            }
        }
    }

    return;
}

void NodeBridge_Enqueue(NodeBridge* _this, void* obj, void (*send)(void*), void (*proc)(void*), void* payload)   //to add parameter: RemoteInvoke
{
    NodeMessageQueue* mq;
    mq = _NodeBridge_GetThreadMessageQueue(_this);
    int toIdx = (mq->mIndex == _this->mNODE) ? _this->mEPK : _this->mNODE;    //to fix:shoud be an input parameter
    NodeMessageQueue* _mq;
    _mq = (NodeMessageQueue*)(_this->mQueues)[toIdx];
    NodeMessageQueue_Enqueue(_mq, obj, send, proc, payload);
    return;
}

void NodeMessage_Init(NodeMessage* _this)
{
    _this->mStatus = NodeMessage_Status_Null;
    return;
}
void NodeMessageQueue_Init(NodeMessageQueue* _this)
{
    _this->mMax = 5;
    _this->mTop = 1;    //_this[0] preserved for [epk looper/nodejs tick] thread
    pthread_mutex_init(&_this->mMutex, NULL);
    _this->mMessages = new NodeMessage*[_this->mMax];
    for (int i = 0; i < _this->mMax; i++) {
        NodeMessage* msg = new NodeMessage;
        _this->mMessages[i] = msg;
        msg->mQueue = _this;
        msg->mIndex = i;
        NodeMessage_Init(msg);

        pthread_mutex_init(&_this->queue_mtx, NULL);
        pthread_cond_init(&_this->queue_cond, NULL);
        _this->queue_waiting = FALSE;
    }
    return;
}

bool NodeBridge_UvReady(NodeBridge* _this) {
    NodeMessageQueue* mq = (NodeMessageQueue*)_this->mQueues[_this->mNODE];
    bool bUvReady = ( mq->mTop >1 && mq->mMessages[1]->mStatus < NodeMessage_Status_Running );
    return bUvReady;
}

//only run in node main thread
void NodeBridge_Init(NodeBridge* _this) {
    _this->mNODE = 1;
    _this->mEPK = 2;
    _this->mMax = 4;
    _this->mTop = 2;    //_this[0] preserved for management thread
    _this->mTag = 1234;

    pthread_mutex_init(&_this->mMutex, NULL);
    _this->mQueues = (void**)new NodeMessageQueue*[_this->mMax];
    for (int i = 0; i < _this->mMax; i++) {
        NodeMessageQueue* mq = new NodeMessageQueue;
        _this->mQueues[i] = mq;
        mq->mBridge = _this;
        mq->mIndex = i;
        if (i == _this->mNODE) {
            mq->mTid = pthread_self();
            ALOGD("NodeBridge_Init ====tid:%d============", int(mq->mTid));
            //TODO:set queue vt
        }
        NodeMessageQueue_Init(mq);
    }

    _this->vt = &nodeBridgeVT;
    _this->mInit = true;
    return;
}

void* _GetNodeBridge() {
    pNodeBridge = &gNodeBridge;
    ppNodeBridge = &pNodeBridge;
    if (pNodeBridge->mInit) {
        ALOGD("_GetNodeBridge========already init========");
    }
    else {
        NodeBridge_Init(pNodeBridge);
    }
    return (void*)pNodeBridge;
}

namespace JSC {
namespace Bindings {

ElCallbackMessage TestCallbackBuf::elCallbackQuene;
bool TestCallbackBuf::cbReady;

int TestCallbackBuf::__mMyLock;

//node.js message queue
TestCallbackBuf* TestCallbackBuf::mInstances[5];
int TestCallbackBuf::mUsed;

TestCallbackBuf::TestCallbackBuf(
    /* [in] */ void (*cbFunction)(void*),
    /* [in] */ void* cbParameters,
    /* [in] */ int cbLock)
    : mFunction(cbFunction)
    , mParameters(cbParameters)
    , mMyLock(cbLock)
{
    TestCallbackBuf::mInstances[0] = this;
}

TestCallbackBuf::~TestCallbackBuf()
{
    //
}

extern const char* ToCString(const v8::String::Utf8Value& value);
extern void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);

void Receive_(int input, v8::Local<v8::Function> callback);

// void Observe_bak(uv_work_t* r) {
//     async_req* req = reinterpret_cast<async_req*>(r->data);
//     NodeMessageQueue* mq = (NodeMessageQueue*)pNodeBridge->mQueues[pNodeBridge->mNODE];
//     while ( mq->mTop < 2 || mq->mMessages[1]->mStatus > NodeMessage_Status_Ready ) {
//         sleep(1);
//     }
// }
void Observe(uv_work_t* r) {
    ALOGD("Observe =======begin==========");

    async_req* req = reinterpret_cast<async_req*>(r->data);

    NodeMessageQueue* mq_node = (NodeMessageQueue*)pNodeBridge->mQueues[pNodeBridge->mNODE];
    NodeMessageQueue* mq_epk = (NodeMessageQueue*)pNodeBridge->mQueues[pNodeBridge->mEPK];

    pthread_mutex_lock(&uv_mtx);
    while( mq_node->mTop < 2 || mq_node->mMessages[1]->mStatus > NodeMessage_Status_Ready ) {
        if ( TRUE ){    //TODO
        //if ( mq_epk->queue_waiting ){
            pthread_mutex_lock(&mq_epk->queue_mtx);
            pthread_cond_signal(&mq_epk->queue_cond);
            pthread_mutex_unlock(&mq_epk->queue_mtx);
        }

        uv_waiting = TRUE;
        pthread_cond_wait(&uv_cond,&uv_mtx);
        uv_waiting = FALSE;
    }
    pthread_mutex_unlock(&uv_mtx);
}

void Back(uv_work_t* r) {
    NodeMessageQueue* mq = (NodeMessageQueue*)pNodeBridge->mQueues[pNodeBridge->mNODE];

    NodeMessage* msg = mq->mMessages[1];

    NodeMessage_Invoke(msg);

    //ALOGD("Back========js begin========0===");

    //call the back function, no use
    Isolate* isolate = Isolate::GetCurrent();
    //v8::Handle<v8::Context> context = isolate->GetCurrentContext();

    //ALOGD("Back========js begin========1.1===");
    HandleScope scope(isolate);
    //ALOGD("Back========js begin========1.2===");


            //v8::Isolate* isolate = mOwner->mIsolate;
            isolate->Enter();
    //ALOGD("Back========js begin========2===");

            v8::Isolate::Scope isolateScope(isolate);

    //ALOGD("Back========js begin========4===");
            v8::Handle<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();
    //ALOGD("Back========js begin========3===");

            v8::Context::Scope contextScope(context);
    //ALOGD("Back========js begin========5===");



    async_req* req = reinterpret_cast<async_req*>(r->data);
    //ALOGD("Back========js begin========6===");

    Handle<Value> argv[2] = {
        Null(isolate),
        Integer::New(isolate, req->output)
    };
    //ALOGD("Back========js begin========7===");

    v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, req->callback);
    //ALOGD("Back========js begin========8===");

    TryCatch try_catch;
    callback->Call(context->Global(), 2, argv);
    //ALOGD("Back========js begin========9===");
    if (try_catch.HasCaught()) {
        //FatalException(try_catch);
        ReportException(isolate, &try_catch);
    }
    //ALOGD("Back========js begin========10===");

            isolate->Exit();

    //ALOGD("Back========js end========");

    Receive_(req->input,callback);

    // cleanup
    req->callback.Reset();
    delete req;
}

void Receive_(int input,v8::Local<v8::Function> callback) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    async_req* req = new async_req;
    req->req.data = req;
    req->input = input;
    req->output = 0;
    req->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(),
        &req->req,
        Observe,
        (uv_after_work_cb)Back
    );
}

void Receive(const FunctionCallbackInfo<Value>& args) {
    int input = args[0]->IntegerValue();
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
    Receive_(input,callback);
}

void Require(const FunctionCallbackInfo<Value>& info) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    v8::Handle<v8::String> errorInfo;
    AutoPtr<IInterface> retObject;

    do {    //do begin
        if (info.Length() < 1) {
            errorInfo = v8::String::NewFromUtf8(isolate,"Wrong number of arguments");
            break;
        }

        if (!info[0]->IsString()) {
            errorInfo = v8::String::NewFromUtf8(isolate,"Wrong type of parameter 0: ModuleName");
            break;
        }

        v8::String::Utf8Value uft8ModuleName(info[0]);
        Elastos::String moduleName(*uft8ModuleName);

        AutoPtr<IModuleInfo> moduleInfo;
        ECode ec = _CReflector_AcquireModuleInfo(
                moduleName, (IModuleInfo**)&moduleInfo);
        if (FAILED(ec)) {
            errorInfo = v8::String::NewFromUtf8(isolate,"Acquire module info failed!");
            break;
        }

        if (info.Length() == 1) {
            //TODO:convert IModuleInfo to ITestModuleInfo
            retObject = (IInterface*)moduleInfo;
            break;
        }

        if (!info[1]->IsString()) {
            errorInfo = v8::String::NewFromUtf8(isolate,"Wrong type of parameter 1: ClassName");
            break;
        }

        v8::String::Utf8Value uft8ClassName(info[1]);
        Elastos::String className(*uft8ClassName);

        AutoPtr<IClassInfo> classInfo;
        ec = moduleInfo->GetClassInfo(
                className, (IClassInfo**)&classInfo);
        if (FAILED(ec)) {
            errorInfo = v8::String::NewFromUtf8(isolate,"Acquire class info failed!");
            break;
        }

        AutoPtr<IInterface> carObject;

        if (info.Length() == 2) {
            ec = classInfo->CreateObject((IInterface**)&carObject);
            if (FAILED(ec)) {
                errorInfo = v8::String::NewFromUtf8(isolate,"Create object failed!");
                break;
            }
            retObject = carObject;
            break;
        }
        else {
            ALOGD("Require========TODO: create object with parameters========");
        }

        //TODO: create object with parameters
        // Elastos::Int32 constructorCount;
        // ec = classInfo->GetConstructorCount(&constructorCount);
        // if (FAILED(ec)) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"GetConstructorCount failed!");
        //     break;
        // }
        // BufferOf<IConstructorInfo*>* constructorInfos;
        // constructorInfos = BufferOf<IConstructorInfo*>::Alloc(constructorCount);
        // ec = classInfo->GetAllConstructorInfos(constructorInfos);
        // if (FAILED(ec)) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"GetAllConstructorInfos failed!");
        //     break;
        // }
        // Elastos::Int32 paramCount;
        // IConstructorInfo* constructorInfo = NULL;
        // Elastos::Int32 argc = info.Length() - 2;
        // for (Elastos::Int32 i = 0; i < constructorCount; i ++) {
        //     IConstructorInfo *tmpConstructor = (*constructorInfos)[i];
        //     ec = tmpConstructor->GetParamCount(&paramCount);
        //     if (FAILED(ec)) {
        //         //
        //     }
        //     // TODO: Actually we should compare the parameters count, but also the
        //     // parameters data type. but now we just compare the parameter count.
        //     if (paramCount == argc) {
        //         constructorInfo = tmpConstructor;
        //         break;
        //     }
        // }
        // if (!constructorInfo) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"can not find class constructor!");
        //     break;
        // }

        // BufferOf<IParamInfo*>* paramInfos = NULL;
        // paramInfos = BufferOf<IParamInfo*>::Alloc(paramCount);
        // ec = constructorInfo->GetAllParamInfos(paramInfos);
        // if (FAILED(ec)) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"GetAllParamInfos failed!");
        //     break;
        // }
        // IArgumentList* argumentList = NULL;
        // ec = constructorInfo->CreateArgumentList(&argumentList);
        // if (FAILED(ec)) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"CreateArgumentList failed!");
        //     break;
        // }
        // //TODO:convert js parms to argumentlist
        // ec = constructorInfo->CreateObject(argumentList, (IInterface**)&carObject);
        // if (FAILED(ec)) {
        //     errorInfo = v8::String::NewFromUtf8(isolate,"ConstructorInfo CreateObject failed!");
        //     break;
        // }
        // retObject = carObject;

    } while (false);    //do end

    //return car object
    if (errorInfo.IsEmpty()) {
        REFCOUNT_ADD(retObject);

        NPVariant result;
        CarValue value;
        value.mType = CarDataType_Interface;
        value.value.mObjectValue = retObject;
        ALOGD("Require========return objet without dataTypeInfo");
        value.mObjectWrapper = new CobjectWrapper(retObject,NULL);
        convertCarValueToNPVariant(value, &result);

        v8::Local<v8::Object> v8Object = WebCore::createV8ObjectForNPObject(
            NPVARIANT_TO_OBJECT(result),
            /* NPVariant root */ NULL,
            isolate
        );

        info.GetReturnValue().Set(v8Object);
    }

    //Exception handler
    if (!errorInfo.IsEmpty()) {
        isolate->ThrowException(
            v8::Exception::TypeError(
                errorInfo
            )
        );
    }

    return;
}

void SetEnqueueUIMessagePtr(const FunctionCallbackInfo<Value>& info) {
    cbEnqueueUIMessage = (void(*)(void*, void*, void*))info[0]->Int32Value();
    return;
}

void GetNodeBridge(const FunctionCallbackInfo<Value>& info) {
    void* p = _GetNodeBridge();
    info.GetReturnValue().Set((int)p);
    return;
}

void GetVersion(const FunctionCallbackInfo<Value>& info) {
    void* p = _GetNodeBridge();
    info.GetReturnValue().Set(0.04);
    return;
}

}   //JSC
}   //Bindings

//--------WTF init function--------begin--------
static double CurrentTime()
{
    return 0.0;
}

static void AlwaysZeroNumberSource(unsigned char* buf, size_t len)
{
    memset(buf, '\0', len);
}
//--------WTF init function--------end--------

void init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module) {
    ALOGD("========elastos_root.cpp====init====begin====");

    ALOGD("========elastos_root.cpp====init WTF====begin====");

    WTF::setRandomSource(AlwaysZeroNumberSource);
    ALOGD("========elastos_root.cpp====init WTF====1====");
    WTF::initialize(CurrentTime, 0);
    ALOGD("========elastos_root.cpp====init WTF====2====");
    WTF::initializeMainThread(0);

    ALOGD("========elastos_root.cpp====init WTF====end====");

    ALOGD("========elastos_root.cpp====V8 initialize====begin====");
    Isolate* isolate = Isolate::GetCurrent();
    WebCore::V8Initializer::initializeMainThreadIfNeeded(isolate);
    ALOGD("========elastos_root.cpp====V8 initialize====end====");

    NODE_SET_METHOD(exports, "require", JSC::Bindings::Require);
    NODE_SET_METHOD(exports, "receive", JSC::Bindings::Receive);
    NODE_SET_METHOD(exports, "SetEnqueueUIMessagePtr", JSC::Bindings::SetEnqueueUIMessagePtr);
    NODE_SET_METHOD(exports, "GetNodeBridge", JSC::Bindings::GetNodeBridge);

    NODE_SET_METHOD(exports, "GetVersion", JSC::Bindings::GetVersion);

    ALOGD("========elastos_root.cpp====init====end====");
}

NODE_MODULE(binding, init);