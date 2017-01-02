//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/AppImportanceMonitor.h"
#include "elastos/droid/app/CActivityManagerRunningAppProcessInfo.h"
#include "elastos/droid/app/CAppImportanceMonitorProcessObserver.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 AppImportanceMonitor::MSG_UPDATE = 1;

//=================================================================================
// AppImportanceMonitor::AppEntry
//=================================================================================

AppImportanceMonitor::AppEntry::AppEntry(
    /* [in] */ Int32 uid)
    : mUid(uid)
    , mImportance(IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE)
{
}

//=================================================================================
// AppImportanceMonitor::ProcessObserver
//=================================================================================
CAR_INTERFACE_IMPL_2(AppImportanceMonitor::ProcessObserver, Object, IIProcessObserver, IBinder)

AppImportanceMonitor::ProcessObserver::ProcessObserver()
{}

ECode AppImportanceMonitor::ProcessObserver::constructor(
    /* [in] */ IAppImportanceMonitor* host)
{
    mHost = (AppImportanceMonitor*)host;
    return NOERROR;
}

AppImportanceMonitor::ProcessObserver::~ProcessObserver()
{}

ECode AppImportanceMonitor::ProcessObserver::OnForegroundActivitiesChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean foregroundActivities)
{
    return NOERROR;
}

ECode AppImportanceMonitor::ProcessObserver::OnProcessStateChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 procState)
{
    Object& lock = mHost->mAppsLock;
    {    AutoLock syncLock(lock);
        mHost->UpdateImportanceLocked(pid, uid,
            CActivityManagerRunningAppProcessInfo::ProcStateToImportance(procState), TRUE);
    }
    return NOERROR;
}

ECode AppImportanceMonitor::ProcessObserver::OnProcessDied(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    Object& lock = mHost->mAppsLock;
    {    AutoLock syncLock(lock);
        mHost->UpdateImportanceLocked(pid, uid,
            IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE, TRUE);
    }
    return NOERROR;
}

ECode AppImportanceMonitor::ProcessObserver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "AppImportanceMonitor::ProcessObserver";
    return NOERROR;
}

//=================================================================================
// AppImportanceMonitor::MyHandler
//=================================================================================
AppImportanceMonitor::MyHandler::MyHandler(
    /* [in] */ AppImportanceMonitor* host)
    : mHost(host)
{}

ECode AppImportanceMonitor::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch (what) {
        case AppImportanceMonitor::MSG_UPDATE:
            mHost->OnImportanceChanged(arg1, arg2 & 0xffff, arg2 >> 16);
            break;
        default:
            return Handler::HandleMessage(msg);
    }
    return NOERROR;
}

//=================================================================================
// AppImportanceMonitor
//=================================================================================

CAR_INTERFACE_IMPL(AppImportanceMonitor, Object, IAppImportanceMonitor)

AppImportanceMonitor::AppImportanceMonitor()
{}

AppImportanceMonitor::~AppImportanceMonitor()
{}

ECode AppImportanceMonitor::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper)
{
    mContext = context;
    mHandler = new MyHandler(this);
    mHandler->constructor(looper);

    CAppImportanceMonitorProcessObserver::New(this, (IIProcessObserver**)&mProcessObserver);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(obj);

    // try {
    ActivityManagerNative::GetDefault()->RegisterProcessObserver(mProcessObserver);
    // } catch (RemoteException e) {
    // }
    AutoPtr<IList> apps;
    am->GetRunningAppProcesses((IList**)&apps);

    if (apps != NULL) {
        Int32 size;
        apps->GetSize(&size);
        Int32 uid, pid, importance;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            apps->Get(i, (IInterface**)&obj);
            IActivityManagerRunningAppProcessInfo* app = IActivityManagerRunningAppProcessInfo::Probe(obj);
            app->GetUid(&uid);
            app->GetPid(&pid);
            app->GetImportance(&importance);
            UpdateImportanceLocked(uid, pid, importance, FALSE);
        }
    }
    return NOERROR;
}

ECode AppImportanceMonitor::GetImportance(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    HashMap<Int32, AutoPtr<AppEntry> >::Iterator it = mApps.Find(uid);
    if (it == mApps.End()) {
        *result = IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE;
        return NOERROR;
    }
    *result = it->mSecond->mImportance;
    return NOERROR;
}

ECode AppImportanceMonitor::OnImportanceChanged(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 importance,
    /* [in] */ Int32 oldImportance)
{
    return NOERROR;
}

void AppImportanceMonitor::UpdateImportanceLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 importance,
    /* [in] */ Boolean repChange)
{
    AutoPtr<AppEntry> ent;

    HashMap<Int32, AutoPtr<AppEntry> >::Iterator it = mApps.Find(uid);
    if (it == mApps.End()) {
        ent = new AppEntry(uid);
        mApps[uid] = ent;
    }
    if (importance >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE) {
        HashMap<Int32, Int32>::Iterator it = ent->mProcs.Find(pid);
        if (it != ent->mProcs.End()) {
            ent->mProcs.Erase(it);
        }
    }
    else {
        ent->mProcs[pid] = importance;
    }
    UpdateImportanceLocked(ent, repChange);
}

void AppImportanceMonitor::UpdateImportanceLocked(
    /* [in] */ AppEntry* ent,
    /* [in] */ Boolean repChange)
{
    Int32 appImp = IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE;
    HashMap<Int32, Int32>::Iterator it = ent->mProcs.Begin();
    for (; it != ent->mProcs.End(); ++it) {
        Int32 procImp = it->mSecond;
        if (procImp < appImp) {
            appImp = procImp;
        }
    }

    if (appImp != ent->mImportance) {
        Int32 impCode = appImp | (ent->mImportance << 16);
        ent->mImportance = appImp;
        if (appImp >= IActivityManagerRunningAppProcessInfo::IMPORTANCE_GONE) {
            HashMap<Int32, AutoPtr<AppEntry> >::Iterator it = mApps.Find(ent->mUid);
            if (it != mApps.End()) {
                mApps.Erase(it);
            }
        }

        if (repChange) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MSG_UPDATE, ent->mUid, impCode, (IMessage**)&msg);
            msg->SendToTarget();
        }
    }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
