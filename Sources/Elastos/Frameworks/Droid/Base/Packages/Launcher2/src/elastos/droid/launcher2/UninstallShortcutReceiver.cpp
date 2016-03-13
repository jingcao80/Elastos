
#include "elastos/droid/launcher2/UninstallShortcutReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

UninstallShortcutReceiver::PendingUninstallShortcutInfo::PendingUninstallShortcutInfo(
    /* [in] */ IIntent* rawData)
    : mData(rawData)
{
}

UninstallShortcutReceiver::MyThread::MyThread(
    /* [in] */ ISet* savedNewApps,
    /* [in] */ ISharedPreferences* sharedPrefs)
    : mSavedNewApps(savedNewApps)
    , mSharedPrefs(sharedPrefs)
{
}

ECode UninstallShortcutReceiver::MyThread::Run()
{
    synchronized(mSavedNewApps) {
        AutoPtr<ISharedPreferencesEditor> editor;
        mSharedPrefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutStringSet(InstallShortcutReceiver::NEW_APPS_LIST_KEY,
                mSavedNewApps);
        Boolean res;
        mSavedNewApps->IsEmpty(&res);
        if (res) {
            // Reset the page index if there are no more items
            editor->PutInt32(InstallShortcutReceiver::NEW_APPS_PAGE_KEY, -1);
        }
        return editor->Commit();
    }
    return NOERROR;
}

static AutoPtr<IArrayList> InitUninstallQueue()
{
    AutoPtr<IArrayList> tmp;
    CArrayList::New((IArrayList**)&tmp);
    return tmp;
}

const String UninstallShortcutReceiver::ACTION_UNINSTALL_SHORTCUT("com.android.launcher.action.UNINSTALL_SHORTCUT");
AutoPtr<IArrayList> UninstallShortcutReceiver::mUninstallQueue = InitUninstallQueue();
Boolean UninstallShortcutReceiver::mUseUninstallQueue = FALSE;

CAR_INTERFACE_IMPL(UninstallShortcutReceiver, BroadcastReceiver, IUninstallShortcutReceiver);

ECode UninstallShortcutReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data)
{
    String action;
    data->GetAction(&action);
    Boolean res;
    ACTION_UNINSTALL_SHORTCUT.Equals(action, &res);
    if (!res) {
        return NOERROR;
    }

    AutoPtr<PendingUninstallShortcutInfo> info = new PendingUninstallShortcutInfo(data);
    if (mUseUninstallQueue) {
        mUninstallQueue->Add(TO_IINTERFACE(info));
    }
    else {
        ProcessUninstallShortcut(context, info);
    }
    return NOERROR;
}

ECode UninstallShortcutReceiver::EnableUninstallQueue()
{
    mUseUninstallQueue = TRUE;
    return NOERROR;
}

ECode UninstallShortcutReceiver::DisableAndFlushUninstallQueue(
    /* [in] */ IContext* context)
{
    mUseUninstallQueue = FALSE;
    AutoPtr<IIterator> iter;
    mUninstallQueue->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        PendingUninstallShortcutInfo* info = (PendingUninstallShortcutInfo*)IObject::Probe(obj);
        ProcessUninstallShortcut(context, info);
        iter->Remove();
    }
    return NOERROR;
}

void UninstallShortcutReceiver::ProcessUninstallShortcut(
    /* [in] */ IContext* context,
    /* [in] */ PendingUninstallShortcutInfo* pendingInfo)
{
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    AutoPtr<ISharedPreferences> sharedPrefs;
    context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE);

    AutoPtr<IIntent> data = pendingInfo->mDdata;

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    synchronized(app) {
        RemoveShortcut(context, data, sharedPrefs);
    }
}

void UninstallShortcutReceiver::RemoveShortcut(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data,
    /* [in] */ ISharedPreferences* sharedPrefs)
{
    AutoPtr<IIntent> intent;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_INTENT, (IIntent**)&intent);
    String name;
    data->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &name);
    Boolean duplicate;
    data->GetBooleanExtra(iLauncher::EXTRA_SHORTCUT_DUPLICATE, TRUE, &duplicate);

    if (intent != NULL && name != NULL) {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        (*array)[0] = ILauncherSettings::Favorites::_ID;
        (*array)[1] = ILauncherSettings::Favorites::INTENT
        StringBuilder sb;
        sb += ILauncherSettings::Favorites::TITLE;
        sb += "=?";
        AutoPtr<ArrayOf<String> > names = ArrayOf<String>::Alloc(1);
        (*names)[0] = name;
        AutoPtr<ICursor> c;
        cr->Query(ILauncherSettings::Favorites::CONTENT_URI,
            array, sb.ToString(), names, NULL);

        Int32 intentIndex;
        c->GetColumnIndexOrThrow(ILauncherSettings::Favorites::INTENT, &intentIndex);
        Int32 idIndex;
        c->GetColumnIndexOrThrow(ILauncherSettings::Favorites::_ID, &idIndex);

        Boolean changed = FALSE;

        //try {
        Boolean res;
        while (c->MoveToNext(&res), res) {
            //try {
            String index;
            c->GetString(intentIndex, &index);
            AutoPtr<IIntent> tmp;
            Intent::ParseUri(index, 0, (IIntent**)&tmp);
            Boolean res;
            intent->FilterEquals(tmp, &res);
            if (res) {
                Int64 id;
                c->GetInt64(idIndex, &id);
                AutoPtr<IUri> uri;
                LauncherSettings::Favorites::GetContentUri(id, FALSE, (IUri**)&uri);
                cr->Delete(uri, NULL, NULL);
                changed = TRUE;
                if (!duplicate) {
                    break;
                }
            }
            //} catch (URISyntaxException e) {
            // if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
            //     // Ignore
            // }
            //}
        }
        //} finally {
            c->Close();
        //}

        if (changed) {
            cr->NotifyChange(ILauncherSettings::Favorites::CONTENT_URI, NULL);
            String str;
            context->GetString(R.string.shortcut_uninstalled, name, &str);
            AutoPtr<IToast> toast;
            Toast::MakeText(context, str, IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
        }

        // Remove any items due to be animated
        Boolean appRemoved;
        AutoPtr<ISet> newApps;
        CHashSet::New((ISet**)&newApps);
        sharedPrefs->GetStringSet(IInstallShortcutReceiver::NEW_APPS_LIST_KEY, newApps, (ISet**)&newApps);
        synchronized(newApps) {
            do {
                String str;
                intent->ToUri(0, &str);
                AutoPtr<ICharSequence> cchar = CoreUtil::Convert(str);
                newApps->Remove(TO_IINTERFACE(cchar), &appRemoved);
            } while (appRemoved);
        }
        if (appRemoved) {
            AutoPtr<ISet> savedNewApps = newApps;
            AutoPtr<MyThread> thread = new MyThread(savedNewApps, sharedPrefs);
            thread->Start();
        }
    }
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos