
#include "elastos/droid/server/am/ProviderMap.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Os::CTransferPipe;
using Elastos::Droid::Internal::Os::ITransferPipe;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const String ProviderMap::TAG("ProviderMap");
const Boolean ProviderMap::DBG = FALSE;

ProviderMap::ProviderMap(
    /* [in] */ CActivityManagerService* am)
{
    mAm = am;
}

ProviderMap::~ProviderMap()
{
}

AutoPtr<ContentProviderRecord> ProviderMap::GetProviderByName(
    /* [in] */ const String& name)
{
    return GetProviderByName(name, -1);
}

AutoPtr<ContentProviderRecord> ProviderMap::GetProviderByName(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    if (DBG) {
        AutoPtr<IBinderHelper> bHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
        Int32 callingUid;
        bHelper->GetCallingUid(&callingUid);
        Slogger::I(TAG, "getProviderByName: %s , callingUid = %d",
                name.string(), callingUid);
    }

    AutoPtr<ContentProviderRecord> result;

    // Try to find it in the global list
    NameRecordIterator it = mSingletonByName.Find(name);
    if (it != mSingletonByName.End()) {
        result = it->mSecond;
    }
    else {
        // Check the current user's list
        AutoPtr<NameRecordHashMap> map = GetProvidersByName(userId);
        if (map != NULL) {
            it = map->Find(name);
            if (it != map->End()) {
                result = it->mSecond;
            }
        }
    }

    return result;
}

AutoPtr<ContentProviderRecord> ProviderMap::GetProviderByClass(
    /* [in] */ IComponentName* name)
{
    return GetProviderByClass(name, -1);
}

AutoPtr<ContentProviderRecord> ProviderMap::GetProviderByClass(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userId)
{
    assert(name != NULL && "Error: name cannot be NULL in ProviderMap::GetProviderByClass(...)");

    if (DBG) {
        String str;
        name->ToString(&str);
        AutoPtr<IBinderHelper> bHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
        Int32 callingUid;
        bHelper->GetCallingUid(&callingUid);
        Slogger::I(TAG, "getProviderByClass: %s , callingUid = %d",
                str.string(), callingUid);
    }

    AutoPtr<ContentProviderRecord> result;

     // Try to find it in the global list
    ClassRecordIterator it = mSingletonByClass.Find(name);
    if (it != mSingletonByClass.End()) {
        result = it->mSecond;
    }
    else {
        // Check the current user's list
        AutoPtr<ClassRecordHashMap> map = GetProvidersByClass(userId);
        if (map != NULL) {
            it = map->Find(name);
            if (it != map->End()) {
                result = it->mSecond;
            }
        }
    }

    return result;
}

ECode ProviderMap::PutProviderByName(
    /* [in] */ const String& name,
    /* [in] */ ContentProviderRecord* aRecord)
{
    VALIDATE_NOT_NULL(aRecord);

    AutoPtr<ContentProviderRecord> record = aRecord;
    Int32 uid;
    record->mAppInfo->GetUid(&uid);

    if (DBG) {
        AutoPtr<IBinderHelper> bHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
        Int32 callingUid;
        bHelper->GetCallingUid(&callingUid);
        Slogger::I(TAG, "putProviderByName: %s, callingUid = %d, record uid = %d",
                name.string(), callingUid, uid);
    }

    if (record->mSingleton) {
        mSingletonByName.Insert(NameRecordValueType(name, record));
    }
    else {
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        Int32 userId;
        uhHelper->GetUserId(uid, &userId);
        AutoPtr<NameRecordHashMap> map = GetProvidersByName(userId);
        if (map != NULL) {
            map->Insert(NameRecordValueType(name, record));
        }
        else {
            map = new NameRecordHashMap();
            map->Insert(NameRecordValueType(name, record));
            mProvidersByNamePerUser.Insert(ProviderByNameValueType(userId, map));
        }
    }

    return NOERROR;
}

ECode ProviderMap::PutProviderByClass(
    /* [in] */ IComponentName* aName,
    /* [in] */ ContentProviderRecord* aRecord)
{
    VALIDATE_NOT_NULL(aName);
    VALIDATE_NOT_NULL(aRecord);

    AutoPtr<IComponentName> name = aName;
    AutoPtr<ContentProviderRecord> record = aRecord;

    Int32 uid;
    record->mAppInfo->GetUid(&uid);

    if (DBG) {
        String str;
        name->ToString(&str);
        AutoPtr<IBinderHelper> bHelper;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
        Int32 callingUid;
        bHelper->GetCallingUid(&callingUid);
        Slogger::I(TAG, "putProviderByClass: %s, callingUid = %d, record uid = %d",
                str.string(), callingUid, uid);
    }

    if (record->mSingleton) {
        mSingletonByClass.Insert(ClassRecordValueType(name, record));
    }
    else {
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        Int32 userId;
        uhHelper->GetUserId(uid, &userId);
        AutoPtr<ClassRecordHashMap> map = GetProvidersByClass(userId);
        if (map != NULL) {
            map->Insert(ClassRecordValueType(name, record));
        }
        else {
            map = new ClassRecordHashMap();
            map->Insert(ClassRecordValueType(name, record));
            mProvidersByClassPerUser.Insert(ProviderByClassValueType(userId, map));
        }
    }

    return NOERROR;
}

ECode ProviderMap::RemoveProviderByName(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    NameRecordIterator it = mSingletonByName.Find(name);
    if (it != mSingletonByName.End()) {
        if (DBG)
            Slogger::I(TAG, "Removing from globalByName name=%s", name.string());

        mSingletonByName.Erase(it);
    }
    else {
        if (userId < 0) {
            //throw new IllegalArgumentException("Bad user " + userId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (DBG) {
            Slogger::I(TAG, "Removing from providersByName name=%s user=%d",
                    name.string(), userId);
        }

        AutoPtr<NameRecordHashMap> map = GetProvidersByName(userId);
        if (map != NULL) {
            map->Erase(name);
            if (map->IsEmpty()) {
                mProvidersByNamePerUser.Erase(userId);
            }
        }
    }

    return NOERROR;
}

ECode ProviderMap::RemoveProviderByClass(
    /* [in] */ IComponentName* aName,
    /* [in] */ Int32 userId)
{
    VALIDATE_NOT_NULL(aName);
    AutoPtr<IComponentName> name = aName;

    ClassRecordIterator it = mSingletonByClass.Find(name);
    if (it != mSingletonByClass.End()) {
        if (DBG) {
            String str;
            name->ToString(&str);
            Slogger::I(TAG, "Removing from globalByClass name=%s", str.string());
        }

        mSingletonByClass.Erase(it);
    }
    else {
        if (userId < 0) {
            //throw new IllegalArgumentException("Bad user " + userId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (DBG) {
            String str;
            name->ToString(&str);
            Slogger::I(TAG, "Removing from providersByClass name=%s user=%d",
                    str.string(), userId);
        }

        AutoPtr<ClassRecordHashMap> map = GetProvidersByClass(userId);
        if (map != NULL) {
            map->Erase(name);
            if (map->IsEmpty()) {
                mProvidersByClassPerUser.Erase(userId);
            }
        }
    }

    return NOERROR;
}

AutoPtr<HashMap<String, AutoPtr<ContentProviderRecord> > > ProviderMap::GetProvidersByName(
    /* [in] */ Int32 userId)
{
    if (userId < 0) {
        // throw new IllegalArgumentException("Bad user " + userId);
        return NULL;
    }

    HashMap<Int32, AutoPtr<NameRecordHashMap> >::Iterator it = mProvidersByNamePerUser.Find(userId);
    if (it != mProvidersByNamePerUser.End()) {
        return it->mSecond;
    }
    else {
        AutoPtr<NameRecordHashMap> map = new NameRecordHashMap();
        mProvidersByNamePerUser.Insert(ProviderByNameValueType(userId, map));
        return map;
    }
}

AutoPtr<HashMap<AutoPtr<IComponentName>, AutoPtr<ContentProviderRecord> > > ProviderMap::GetProvidersByClass(
    /* [in] */ Int32 userId)
{
    if (userId < 0) {
        // throw new IllegalArgumentException("Bad user " + userId);
        return NULL;
    }

    HashMap<Int32, AutoPtr<ClassRecordHashMap> >::Iterator it = mProvidersByClassPerUser.Find(userId);
    if (it != mProvidersByClassPerUser.End()) {
        return it->mSecond;
    }
    else {
        AutoPtr<ClassRecordHashMap> map = new ClassRecordHashMap();
        mProvidersByClassPerUser.Insert(ProviderByClassValueType(userId, map));
        return map;
    }
}

Boolean ProviderMap::CollectForceStopProvidersLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 appId,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Int32 userId,
    /* [in] */ ProviderMap::ClassRecordHashMap* providers,
    /* [in] */ List<AutoPtr<ContentProviderRecord> >* result)
{
    if (providers == NULL || result == NULL)
        return FALSE;

    Boolean didSomething = FALSE;
    ClassRecordIterator it = providers->Begin();
    for (; it != providers->End(); ++it) {
        AutoPtr<ContentProviderRecord> provider = it->mSecond;
        if (provider != NULL) {
            String pkgName;
            IPackageItemInfo::Probe(provider->mInfo)->GetPackageName(&pkgName);
            if ((name.IsNull() || pkgName.Equals(name))
                    && (provider->mProc == NULL || evenPersistent || !provider->mProc->mPersistent)) {
                if (!doit) {
                    return TRUE;
                }
                didSomething = TRUE;
                result->PushBack(provider);
            }
        }
    }

    return didSomething;
}

Boolean ProviderMap::CollectForceStopProviders(
    /* [in] */ const String& name,
    /* [in] */ Int32 appId,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Int32 userId,
    /* [in] */ List<AutoPtr<ContentProviderRecord> >* result)
{
    if (result == NULL)
        return FALSE;

    Boolean didSomething = CollectForceStopProvidersLocked(
            name, appId, doit, evenPersistent, userId, &mSingletonByClass, result);
    if (!doit && didSomething) {
        return TRUE;
    }

    if (userId == IUserHandle::USER_ALL) {
        HashMap<Int32, AutoPtr<ClassRecordHashMap> >::Iterator it = mProvidersByClassPerUser.Begin();
        for (; it != mProvidersByClassPerUser.End(); ++it) {
            AutoPtr<ClassRecordHashMap> map = it->mSecond;
            if (CollectForceStopProvidersLocked(
                    name, appId, doit, evenPersistent, userId, map, result)) {
                if (!doit) {
                    return TRUE;
                }
                didSomething = TRUE;
            }
        }
    }
    else {
        AutoPtr<ClassRecordHashMap> items = GetProvidersByClass(userId);
        if (items != NULL) {
            didSomething |= CollectForceStopProvidersLocked(
                    name, appId, doit, evenPersistent, userId, items, result);
        }
    }
    return didSomething;
}

Boolean ProviderMap::DumpProvidersByClassLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage,
    /* [in] */ const String& header,
    /* [in] */ Boolean needSep,
    /* [in] */ ClassRecordHashMap* map)
{
    Boolean written = FALSE;
    ClassRecordIterator it = map->Begin();
    for (; it != map->End(); ++it) {
        AutoPtr<ContentProviderRecord> r = it->mSecond;
        String packageName;
        IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&packageName);
        if (dumpPackage != NULL && !dumpPackage.Equals(packageName)) {
            continue;
        }
        if (needSep) {
            pw->Println(String(""));
            needSep = FALSE;
        }
        if (header != NULL) {
            pw->Println(header);
        }
        written = TRUE;
        pw->Print(String("  * "));
        pw->Println(r->ToString());
        r->Dump(pw, String("    "), dumpAll);
    }
    return written;
}

Boolean ProviderMap::DumpProvidersByNameLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& dumpPackage,
    /* [in] */ const String& header,
    /* [in] */ Boolean needSep,
    /* [in] */ NameRecordHashMap* map)
{
    Boolean written = FALSE;
    NameRecordIterator it = map->Begin();
    for (; it != map->End(); ++it) {
        AutoPtr<ContentProviderRecord> r = it->mSecond;
        String packageName;
        IPackageItemInfo::Probe(r->mAppInfo)->GetPackageName(&packageName);
        if (dumpPackage != NULL && !dumpPackage.Equals(packageName)) {
            continue;
        }
        if (needSep) {
            pw->Println(String(""));
            needSep = FALSE;
        }
        if (header != NULL) {
            pw->Println(header);
        }
        written = TRUE;
        pw->Print(String("  "));
        pw->Print(it->mFirst);
        pw->Print(String(": "));
        pw->Println(r->ToShortString());
    }
    return written;
}

Boolean ProviderMap::DumpProvidersLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean dumpAll,
    /* [in] */ const String& dumpPackage)
{
    Boolean needSep = FALSE;

    if (mSingletonByClass.GetSize() > 0) {
        needSep |= DumpProvidersByClassLocked(pw, dumpAll, dumpPackage,
            String("  Published single-user content providers (by class):"), needSep,
            &mSingletonByClass);
    }

    HashMap<Int32, AutoPtr<ClassRecordHashMap> >::Iterator it = mProvidersByClassPerUser.Begin();
    for (; it != mProvidersByClassPerUser.End(); ++it) {
        AutoPtr<ClassRecordHashMap> map = it->mSecond;
        StringBuilder sb;
        sb += "  Published user ";
        sb += it->mFirst;
        sb += " content providers (by class):";
        needSep |= DumpProvidersByClassLocked(pw, dumpAll, dumpPackage, sb.ToString(), needSep, map);
    }

    if (dumpAll) {
        needSep |= DumpProvidersByNameLocked(pw, dumpPackage,
            String("  Single-user authority to provider mappings:"), needSep, &mSingletonByName);

        HashMap<Int32, AutoPtr<NameRecordHashMap> >::Iterator nit = mProvidersByNamePerUser.Begin();
        for (; nit != mProvidersByNamePerUser.End(); ++nit) {
            StringBuilder sb;
            sb += "  user ";
            sb += nit->mFirst;
            sb += " authority to provider mappings:";
            needSep |= DumpProvidersByNameLocked(pw, dumpPackage, sb.ToString(), needSep, nit->mSecond);
        }
    }
    return needSep;
}

Boolean ProviderMap::DumpProvider(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ String& name,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Int32 opti,
    /* [in] */ Boolean dumpAll)
{
    List<AutoPtr<ContentProviderRecord> > allProviders;
    List<AutoPtr<ContentProviderRecord> > providers;

    synchronized (mAm) {
        ClassRecordIterator crit = mSingletonByClass.Begin();
        for (; crit != mSingletonByClass.End(); ++crit) {
            allProviders.PushBack(crit->mSecond);
        }
        HashMap<Int32, AutoPtr<ClassRecordHashMap> >::Iterator it = mProvidersByClassPerUser.Begin();
        for (; it != mProvidersByClassPerUser.End(); ++it) {
            AutoPtr<ClassRecordHashMap> map = it->mSecond;
            crit = map->Begin();
            for (; crit != map->End(); ++it) {
                allProviders.PushBack(crit->mSecond);
            }
        }

        if (name.Equals("all")) {
            providers.Assign(allProviders.Begin(), allProviders.End());
        }
        else {
            AutoPtr<IComponentName> componentName;
            if (name != NULL) {
                AutoPtr<IComponentNameHelper> cnHelper;
                CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
                cnHelper->UnflattenFromString(name, (IComponentName**)&componentName);
            }
            Int32 objectId = 0;
            if (componentName == NULL) {
                // Not a '/' separated full component name; maybe an object ID?
                objectId = StringUtils::ParseInt32(name, 16);
                name = NULL;
                componentName = NULL;
            }

            List<AutoPtr<ContentProviderRecord> >::Iterator iter;
            for (iter = allProviders.Begin(); iter != allProviders.End(); ++iter) {
                AutoPtr<ContentProviderRecord> r1 = *iter;
                if (componentName != NULL) {
                    Boolean res;
                    IObject::Probe(r1->mName)->Equals(componentName, &res);
                    if (res) {
                        providers.PushBack(r1);
                    }
                }
                else if (name != NULL) {
                    String str;
                    r1->mName->FlattenToString(&str);
                    if (str.Contains(name)) {
                        providers.PushBack(r1);
                    }
                }
                else if ((Int32)r1.Get() == objectId) {
                    providers.PushBack(r1);
                }
            }
        }
    }

    if (providers.GetSize() <= 0) {
        return FALSE;
    }

    Boolean needSep = FALSE;
    List<AutoPtr<ContentProviderRecord> >::Iterator iter;
    for (iter = providers.Begin(); iter != providers.End(); ++iter) {
        if (needSep) {
            pw->Println();
        }
        needSep = TRUE;
        DumpProvider(String(""), fd, pw, *iter, args, dumpAll);
    }
    return TRUE;
}

/**
 * Invokes IApplicationThread.dumpProvider() on the thread of the specified provider if
 * there is a thread associated with the provider.
 */
void ProviderMap::DumpProvider(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ContentProviderRecord* r,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ Boolean dumpAll)
{
    String innerPrefix = prefix + "  ";
    synchronized (mAm) {
        pw->Print(prefix);
        pw->Print(String("PROVIDER "));
        pw->Print(r->ToString());
        pw->Print(String(" pid="));
        if (r->mProc != NULL)
            pw->Println(r->mProc->mPid);
        else
            pw->Println(String("(not running)"));
        if (dumpAll) {
            r->Dump(pw, innerPrefix, TRUE);
        }
    }
    if (r->mProc != NULL && r->mProc->mThread != NULL) {
        pw->Println(String("    Client:"));
        IFlushable::Probe(pw)->Flush();
        // try {
            AutoPtr<ITransferPipe> tp;
            CTransferPipe::New((ITransferPipe**)&tp);
            // try {
                AutoPtr<IParcelFileDescriptor> writeFd;
                tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
                AutoPtr<IFileDescriptor> fd2;
                writeFd->GetFileDescriptor((IFileDescriptor**)&fd2);
                assert(0);
                // r->mProc->mThread->DumpProvider(fd2, IBinder::Probe(r->mProvider), args);
                tp->SetBufferPrefix(String("      "));
                // Short timeout, since blocking here can
                // deadlock with the application.
                tp->Go(fd, 2000);
            // } finally {
                tp->Kill();
            // }
        // } catch (IOException ex) {
        //     pw->Println(String("      Failure while dumping the provider: " + ex));
        // } catch (RemoteException ex) {
        //     pw->Println(String("      Got a RemoteException while dumping the service"));
        // }
    }
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
