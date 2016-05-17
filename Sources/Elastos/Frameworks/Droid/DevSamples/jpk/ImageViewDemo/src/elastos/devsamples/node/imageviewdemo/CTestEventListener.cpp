
#include "CTestEventListener.h"

#include <elastos/utility/logging/Logger.h>

#include "node.h"
#include "uv.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace ImageViewDemo {

static const String DBG_TAG("CTestEventListener");

IHandler* myHandler;

NodeBridge* g_pNodeBridge;
NodeBridge** g_ppNodeBridge;

pthread_mutex_t CTestEventListener::mMutex;

AutoPtr<INodeBridgeListener> CTestEventListener::mNodeBridgeListener = NULL;

bool CTestEventListener::mNodeInit = false;

// CallbackRunnable* CallbackRunnable::mInstances[5];  //CallbackRunnable to be renameed: EpkCallbackRunnable

// CallbackRunnable::CallbackRunnable(
//     /* [in] */ IInterface* object,
//     /* [in] */ IMethodInfo* method,
//     /* [in] */ IArgumentList* argumentList,
//     /* [in] */ pthread_mutex_t* mutex)
//     : mObject(object)
//     , mMethod(method)
//     , mArgumentList(argumentList)
// {
//     CallbackRunnable::mInstances[0] = this;
// }

// //CAR_INTERFACE_IMPL(CallbackRunnable, IRunnable);
// CAR_INTERFACE_IMPL(CallbackRunnable, ElRefBase, IRunnable);

// ECode CallbackRunnable::Run()
// {
//     g_pNodeBridge->vt->Invoke(g_pNodeBridge);

//     mMyLock = 0;

//     return NOERROR;
// }

// CallbackRunnable::~CallbackRunnable()
// {
//     //TODO
// }

// ECode CTestEventListener::_Thread::Run()
// {
//     int argc = 3;
//     char** argv = new char*[4];

//     argv[0] = new char[1024];
//     memset(argv[0],0,1024);

//     char processName[] = "/system/bin/ElApp_process";
//     Int32 length_0 = sizeof(processName);
//     snprintf(argv[0], length_0 + 1, "%s", processName);

//     char jsFileName[] = "/data/temp/node/elastos_node.js";
//     Int32 length_1 = sizeof(jsFileName);
//     argv[1] = argv[0] + length_0;
//     snprintf(argv[1], length_1 + 1, "%s", jsFileName);

//     const char* packageName = mPackageName.string();
//     Int32 length_2 = strlen(packageName) + 1;
//     argv[2] = argv[1] + length_1;
//     snprintf(argv[2], length_2 + 1, "%s", packageName);

//     argv[3] = NULL;

//     node::Start(argc,argv);

//     //delete argv

//     return NOERROR;
// }

//CAR_OBJECT_IMPL(CTestEventListener)

CAR_INTERFACE_IMPL(CTestEventListener, Object, ITestEventListener)

CAR_OBJECT_IMPL(CTestEventListener)

ECode CTestEventListener::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Object::constructor();
    //return NOERROR;
}

ECode CTestEventListener::OnNodeEvent(
    /* [in] */ Int32 eventId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestEventListener::GetEnqueueUIMessagePtr(
    /* [out] */ Int32 * pOutPtr)
{
    // *pOutPtr = (Int32)CallbackRunnable::EnqueueUIMessage;
    // return NOERROR;
}

ECode CTestEventListener::Sleep(
    /* [in] */ Int32 interval)
{
    sleep(interval);
    return NOERROR;
}

ECode CTestEventListener::Lock()
{
    pthread_mutex_t* pMutex = &CTestEventListener::mMutex;
    pthread_mutex_lock(pMutex);
    return NOERROR;
}

ECode CTestEventListener::Unlock()
{
    pthread_mutex_t* pMutex = &CTestEventListener::mMutex;
    pthread_mutex_unlock(pMutex);
   return NOERROR;
}

ECode CTestEventListener::SetActivityListener(
    /* [in] */ Int32 ppActivityListener,    //IActivityListener**
    /* [in] */ IActivityListener* pJsActivityListener)
{
    *(IActivityListener**)ppActivityListener = pJsActivityListener;
    return NOERROR;
}

ECode CTestEventListener::SetNodeBridgeListener(
    /* [in] */ INodeBridgeListener* pNodeBridgeListener)
{
    CTestEventListener::mNodeBridgeListener = pNodeBridgeListener;
    return NOERROR;
}

ECode CTestEventListener::SetNodeBridge(
    /* [in] */ Int32 from,
    /* [in] */ Int32 threadIndex)
{
    g_ppNodeBridge = &g_pNodeBridge;
    g_pNodeBridge = (NodeBridge*)from;
    return NOERROR;
}

}   //namespace ImageViewDemo
}   //namespace Node
}   //namespace DevSamples
}   //namespace Elastos
