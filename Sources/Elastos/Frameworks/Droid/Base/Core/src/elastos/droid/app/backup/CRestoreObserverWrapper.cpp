
#include "elastos/droid/app/backup/CRestoreObserverWrapper.h"

using Elastos::Core::CString;

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const Int32 CRestoreObserverWrapper::MSG_RESTORE_STARTING = 1;
const Int32 CRestoreObserverWrapper::MSG_UPDATE = 2;
const Int32 CRestoreObserverWrapper::MSG_RESTORE_FINISHED = 3;
const Int32 CRestoreObserverWrapper::MSG_RESTORE_SETS_AVAILABLE = 4;

//============================================================================
// CRestoreObserverWrapper::WrapperHandler
//============================================================================
CRestoreObserverWrapper::WrapperHandler::WrapperHandler(
    /* [in] */ IRestoreObserver* appObserver)
{
    mObserver = appObserver;
}

CRestoreObserverWrapper::WrapperHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case MSG_RESTORE_STARTING: {
            mObserver->RestoreStarting(arg1);
            break;
        }
        case MSG_UPDATE: {
            AutoPtr<ICharSequence> seq = ICharSequence::Probe(obj);
            String str;
            seq->ToString(&str);
            mObserver->OnUpdate(arg1, str);
            break;
        }
        case MSG_RESTORE_FINISHED: {
            mObserver->RestoreFinished(arg1);
            break;
        }
        case MSG_RESTORE_SETS_AVAILABLE: {
            AutoPtr<ArrayOf<IRestoreSet*> > array;
            AutoPtr<IObjectContainer> container = IObjectContainer::Probe(obj);
            if (container) {
                Int32 cnt;
                container->GetObjectCount(&cnt);
                array = ArrayOf<IRestoreSet*>::Alloc(cnt);
                AutoPtr<IObjectEnumerator> emu;
                container->GetObjectEnumerator((IObjectEnumerator**)&emu);
                Boolean hasNext = FALSE;
                Int32 i = 0;
                while(emu->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> current;
                    emu->Current((IInterface**)&current);
                    array->Set(i++, IRestoreSet::Probe(current));
                }
            }
            else {
                array = ArrayOf<IRestoreSet*>::Alloc(0);
            }
            mObserver->RestoreSetsAvailable(array);
            break;
        }
    }
}

//============================================================================
// CRestoreObserverWrapper
//============================================================================
CRestoreObserverWrapper::CRestoreObserverWrapper()
{

}

CRestoreObserverWrapper::~CRestoreObserverWrapper()
{

}

ECode CRestoreObserverWrapper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRestoreObserver* appObserver)
{
    mAppObserver = appObserver;
    mHandler = new WrapperHandler(mAppObserver);

    return NOERROR;
}

ECode CRestoreObserverWrapper::RestoreSetsAvailable(
    /* [in] */ ArrayOf<IRestoreSet*>* result)
{
    AutoPtr<IObjectContainer> container;
    if (result != NULL) {
        CObjectContainer::New((IObjectContainer**)&container);
        for (Int32 i = 0; i < result->GetLength(); ++i) {
            container->Add((*result)[i]);
        }
    }
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_RESTORE_SETS_AVAILABLE, container, (IMessage**)&msg);
    Boolean r;
    return  mHandler->SendMessage(msg, &r);
}

ECode CRestoreObserverWrapper::RestoreStarting(
    /* [in] */ Int32 numPackages)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_RESTORE_STARTING, numPackages, 0, (IMessage**)&msg);
    Boolean r;
    return  mHandler->SendMessage(msg, &r);
}

ECode CRestoreObserverWrapper::OnUpdate(
    /* [in] */ Int32 nowBeingRestored,
    /* [in] */ const String& currentPackage)
{
    AutoPtr<ICharSequence> seq;
    CString::New(currentPackage, (ICharSequence**)&seq);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_UPDATE, nowBeingRestored, 0, seq, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode CRestoreObserverWrapper::RestoreFinished(
    /* [in] */ Int32 error)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_RESTORE_FINISHED, error, 0, (IMessage**)&msg);
    Boolean r;
    return  mHandler->SendMessage(msg, &r);
}

}
}
}
}
