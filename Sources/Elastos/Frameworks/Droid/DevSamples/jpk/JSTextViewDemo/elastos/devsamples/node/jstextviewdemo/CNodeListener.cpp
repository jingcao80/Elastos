#include "CNodeListener.h"

#include <elastos/utility/logging/Logger.h>

// #include "node.h"
// #include "uv.h"

#include <poll.h>
#include <dlfcn.h>
#include <fcntl.h>

using Elastos::Utility::Logging::Logger;

# define NODE_EXTERN /* nothing */

//NODE_EXTERN int Start(int argc, char *argv[]);
namespace node {
    // node::Start(argc,argv);

    NODE_EXTERN int Start(int argc, char *argv[]);

    int Start_bak(int argc, char *argv[]) {
        ALOGD("====test====node::Start====begin====");

        char ecoName[] = "/system/lib/libnode.so";
        char funcName[] = "Start";

        void* handle = NULL;
        handle = dlopen(ecoName, RTLD_LAZY);
        if (handle == NULL) {
            ALOGD("====test====node::Start====open libnode.so fail====");
            return 0;
        }
        else {
            ALOGD("====test====node::Start====open libnode.so success====");

        }
        ALOGD("====test====node::Start====handl ptr====%p", handle);

        void* funcPtr = NULL;
        funcPtr = dlsym(handle, funcName);
        ALOGD("====test====node::Start====func ptr====%p", funcPtr);

        ALOGD("====test====node::Start====end====");

//--------begin--------
    // void* handle = dlopen(pathStr, RTLD_LAZY);
    // if (handle == NULL) {
    //     if (android::NativeBridgeIsSupported(pathStr)) {
    //         handle = android::NativeBridgeLoadLibrary(pathStr, RTLD_LAZY);
    //         needNativeBridge = true;
    //     }
    // }

    // if (handle != NULL) {
    //     void* funcPtr = NULL;
    //     const char* funcStr = funcName.string();
    //     if (needNativeBridge) {
    //         funcPtr = android::NativeBridgeGetTrampoline(handle, funcStr, NULL, 0);
    //     } else {
    //         funcPtr = dlsym(handle, funcStr);
    //     }


//--------end--------


        return 0;
    }
}


namespace Elastos {
namespace DevSamples {
namespace Node {
namespace JSPkgName {

static const String DBG_TAG(JSEvtNameStr);

IHandler* myHandler;

NodeBridge* g_pNodeBridge;
NodeBridge** g_ppNodeBridge;

pthread_mutex_t JSEvtName::mMutex;

AutoPtr<INodeBridgeListener> JSEvtName::mNodeBridgeListener = NULL;

bool JSEvtName::mNodeInit = false;

CallbackRunnable* CallbackRunnable::mInstances[5];  //CallbackRunnable to be renameed: EpkCallbackRunnable

CallbackRunnable::CallbackRunnable(
    /* [in] */ IInterface* object,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* argumentList,
    /* [in] */ pthread_mutex_t* mutex,
    /* [in] */ Int32 tag)
    : mObject(object)
    , mMethod(method)
    , mArgumentList(argumentList)
    , mTag(tag)
{
    ALOGD("CallbackRunnable::CallbackRunnable================oooooooooooo================");
    CallbackRunnable::mInstances[0] = this;
}

//CAR_INTERFACE_IMPL(CallbackRunnable, Object, IRunnable);
CAR_INTERFACE_IMPL_2(CallbackRunnable, Object, IRunnable, ICallbackRunnable);

//CAR_OBJECT_IMPL(CallbackRunnable)

ECode CallbackRunnable::Run()
{
    g_pNodeBridge->vt->Invoke(g_pNodeBridge);

    mMyLock = 0;

    return NOERROR;
}

ECode CallbackRunnable::GetInstance(IInterface** ppInstance)
{
    Int32 ttt = (Int32)this;
    *ppInstance = (IInterface*)ttt;

    return NOERROR;
}

CallbackRunnable::~CallbackRunnable()
{
    //TODO
}

ECode JSEvtName::_Thread::Run()
{
    int argc = 3;
    char** argv = new char*[4];

    argv[0] = new char[1024];
    memset(argv[0],0,1024);

    char processName[] = "/system/bin/ElApp_process";
    Int32 length_0 = sizeof(processName);
    snprintf(argv[0], length_0 + 1, "%s", processName);

    //char jsFileName[] = "/data/temp/node/elastos_node.js";
    char jsFileName[] = "/data/temp/node/Common/js/elastos_node.js";
    Int32 length_1 = sizeof(jsFileName);
    argv[1] = argv[0] + length_0;
    snprintf(argv[1], length_1 + 1, "%s", jsFileName);

    const char* packageName = mPackageName.string();
    Int32 length_2 = strlen(packageName) + 1;
    argv[2] = argv[1] + length_1;
    snprintf(argv[2], length_2 + 1, "%s", packageName);

    argv[3] = NULL;

    node::Start(argc,argv);

    //delete argv

    return NOERROR;
}

CAR_INTERFACE_IMPL(JSEvtName, Object, INodeListener)

JS_CAR_OBJECT_IMPL(JSEvtName)

ECode JSEvtName::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Object::constructor();
    //return NOERROR;
}

ECode JSEvtName::OnNodeEvent(
    /* [in] */ Int32 eventId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode JSEvtName::GetEnqueueUIMessagePtr(
    /* [out] */ Int32 * pOutPtr)
{
    *pOutPtr = (Int32)CallbackRunnable::EnqueueUIMessage;
    return NOERROR;
}

ECode JSEvtName::Sleep(
    /* [in] */ Int32 interval)
{
    sleep(interval);
    return NOERROR;
}

ECode JSEvtName::Lock()
{
    pthread_mutex_t* pMutex = &JSEvtName::mMutex;
    pthread_mutex_lock(pMutex);
    return NOERROR;
}

ECode JSEvtName::Unlock()
{
    pthread_mutex_t* pMutex = &JSEvtName::mMutex;
    pthread_mutex_unlock(pMutex);
   return NOERROR;
}

ECode JSEvtName::SetActivityListener(
    /* [in] */ Int32 ppActivityListener,    //IActivityListener**
    /* [in] */ IActivityListener* pJsActivityListener)
{
    *(IActivityListener**)ppActivityListener = pJsActivityListener;
    return NOERROR;
}

ECode JSEvtName::SetNodeBridgeListener(
    /* [in] */ INodeBridgeListener* pNodeBridgeListener)
{
    JSEvtName::mNodeBridgeListener = pNodeBridgeListener;
    return NOERROR;
}

ECode JSEvtName::SetNodeBridge(
    /* [in] */ Int32 from,
    /* [in] */ Int32 threadIndex)
{
    g_ppNodeBridge = &g_pNodeBridge;
    g_pNodeBridge = (NodeBridge*)from;
    return NOERROR;
}

}   //namespace JSPkgName
}   //namespace Node
}   //namespace DevSamples
}   //namespace Elastos
