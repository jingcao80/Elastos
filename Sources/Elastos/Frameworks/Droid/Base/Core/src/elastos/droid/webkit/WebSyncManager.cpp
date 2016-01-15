
#include "elastos/droid/webkit/WebSyncManager.h"
#include "elastos/droid/webkit/WebViewDatabase.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::CThread;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//                       WebSyncManager
//===============================================================

// log tag
const String WebSyncManager::LOGTAG("websync");

WebSyncManager::WebSyncManager()
{
}

WebSyncManager::WebSyncManager(
    /* [in] */ IContext* context,
    /* [in] */ const String& name)
{
}

ECode WebSyncManager::Run()
{
    return NOERROR;
}

/**
 * sync() forces sync manager to sync now
 */
ECode WebSyncManager::Sync()
{
    return NOERROR;
}

/**
 * resetSync() resets sync manager's timer
 */
ECode WebSyncManager::ResetSync()
{
    return NOERROR;
}

/**
 * startSync() requests sync manager to start sync
 */
ECode WebSyncManager::StartSync()
{
    return NOERROR;
}

/**
 * stopSync() requests sync manager to stop sync. remove any SYNC_MESSAGE in
 * the queue to break the sync loop
 */
ECode WebSyncManager::StopSync()
{
    return NOERROR;
}

AutoPtr<IInterface> WebSyncManager::Clone()
{
    assert(0);
    return NULL;
}

void WebSyncManager::OnSyncInit()
{
}

ECode WebSyncManager::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebSyncManager";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos