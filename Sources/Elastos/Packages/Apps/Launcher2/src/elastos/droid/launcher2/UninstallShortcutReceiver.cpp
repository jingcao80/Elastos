
#include "elastos/droid/launcher2/UninstallShortcutReceiver.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Slogger;

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
        editor->PutStringSet(IInstallShortcutReceiver::NEW_APPS_LIST_KEY,
                mSavedNewApps);
        Boolean res;
        mSavedNewApps->IsEmpty(&res);
        if (res) {
            // Reset the page index if there are no more items
            editor->PutInt32(IInstallShortcutReceiver::NEW_APPS_PAGE_KEY, -1);
        }
        Boolean succeded;
        return editor->Commit(&succeded);
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

UninstallShortcutReceiver::UninstallShortcutReceiver()
{
    Slogger::E("UninstallShortcutReceiver", "=========UninstallShortcutReceiver::UninstallShortcutReceiver()");
}

ECode UninstallShortcutReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode UninstallShortcutReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data)
{
    String action;
    data->GetAction(&action);
    if (!ACTION_UNINSTALL_SHORTCUT.Equals(action)) {
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
    context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE, (ISharedPreferences**)&sharedPrefs);

    AutoPtr<IIntent> data = pendingInfo->mData;

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
    AutoPtr<IParcelable> parcelable;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_INTENT, (IParcelable**)&parcelable);
    AutoPtr<IIntent> intent = IIntent::Probe(parcelable);
    String name;
    data->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &name);
    Boolean duplicate;
    data->GetBooleanExtra(ILauncher::EXTRA_SHORTCUT_DUPLICATE, TRUE, &duplicate);

    if (intent != NULL && name != NULL) {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        (*array)[0] = IBaseColumns::ID;
        (*array)[1] = LauncherSettings::Favorites::INTENT;
        StringBuilder sb;
        sb += LauncherSettings::Favorites::TITLE;
        sb += "=?";
        AutoPtr<ArrayOf<String> > names = ArrayOf<String>::Alloc(1);
        (*names)[0] = name;
        AutoPtr<ICursor> c;
        cr->Query(LauncherSettings::Favorites::CONTENT_URI,
            array, sb.ToString(), names, String(NULL), (ICursor**)&c);

        Int32 intentIndex;
        c->GetColumnIndexOrThrow(LauncherSettings::Favorites::INTENT, &intentIndex);
        Int32 idIndex;
        c->GetColumnIndexOrThrow(IBaseColumns::ID, &idIndex);

        Boolean changed = FALSE;

        //try {
        Boolean res;
        while (c->MoveToNext(&res), res) {
            //try {
            String index;
            c->GetString(intentIndex, &index);
            AutoPtr<IIntentHelper> helper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
            AutoPtr<IIntent> tmp;
            helper->ParseUri(index, 0, (IIntent**)&tmp);

            Boolean res;
            intent->FilterEquals(tmp, &res);
            if (res) {
                Int64 id;
                c->GetInt64(idIndex, &id);
                AutoPtr<IUri> uri;
                LauncherSettings::Favorites::GetContentUri(id, FALSE, (IUri**)&uri);
                Int32 num;
                cr->Delete(uri, String(NULL), NULL, &num);
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
            ICloseable::Probe(c)->Close();
        //}

        if (changed) {
            cr->NotifyChange(LauncherSettings::Favorites::CONTENT_URI, NULL);

            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(name);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
            array->Set(0, TO_IINTERFACE(cchar));
            String str;
            context->GetString(
                    Elastos::Droid::Launcher2::R::string::shortcut_uninstalled, array, &str);

            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            AutoPtr<ICharSequence> cs = CoreUtils::Convert(str);
            helper->MakeText(context, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
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
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
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