
#include "elastos/droid/launcher2/PackageChangedReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

InstallShortcutReceiver::PendingInstallShortcutInfo::PendingInstallShortcutInfo(
    /* [in] */ IIntent* rawData,
    /* [in] */ const String& shortcutName,
    /* [in] */ IIntent* shortcutIntent)
    : mData(rawData)
    , mName(shortcutName)
    , mLaunchIntent(shortcutIntent)
{

}

InstallShortcutReceiver::MyThread::MyThread(
    /* [in] */ const String& name,
    /* [in] */ InstallShortcutReceiver* host,
    /* [in] */ ISharedPreferences* sharedPrefs,
    /* [in] */ Int32 screen,
    /* [in] */ IIntent* intent)
    : Thread(name)
    , mHost(host)
    , mSharedPrefs(sharedPrefs)
    , mScreen(screen)
    , mIntent(intent)
{
}

ECode InstallShortcutReceiver::MyThread::Run()
{
    synchronized(mHost->sLock) {
        // If the new app is going to fall into the same page as before,
        // then just continue adding to the current page
        Int32 newAppsScreen;
        sharedPrefs->GetInt32(InstallShortcutReceiver::NEW_APPS_PAGE_KEY,
                mScreen, &newAppsScreen);
        AutoPtr<ISharedPreferencesEditor> editor;
        sharedPrefs->Edit((ISharedPreferencesEditor**)&editor);
        if (newAppsScreen == -1 || newAppsScreen == mScreen) {
            String str;
            mIntent->ToUri(0, &str);
            mHost->AddToStringSet(sharedPrefs,
                editor, InstallShortcutReceiver::NEW_APPS_LIST_KEY,
                str);
        }
        editor->PutInt32(InstallShortcutReceiver::NEW_APPS_PAGE_KEY, mScreen);
        editor->Commit();
    }
    return NOERROR;
}

const Int32 InstallShortcutReceiver::INSTALL_SHORTCUT_SUCCESSFUL = 0;
const Int32 InstallShortcutReceiver::INSTALL_SHORTCUT_IS_DUPLICATE = -1;
const Int32 InstallShortcutReceiver::INSTALL_SHORTCUT_NO_SPACE = -2;

Object InstallShortcutReceiver::sLock;

// Determines whether to defer installing shortcuts immediately until
// processAllPendingInstalls() is called.
Boolean InstallShortcutReceiver::mUseInstallQueue = FALSE;

CAR_INTERFACE_IMPL(InstallShortcutReceiver, BroadcastReceiver, IInstallShortcutReceiver);

ECode InstallShortcutReceiver::AddToStringSet(
    /* [in] */ ISharedPreferences* sharedPrefs,
    /* [in] */ ISharedPreferencesEditor* editor,
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<ISet> strings;
    sharedPrefs->GetStringSet(key, NULL, (ISet**)&strings);
    if (strings == NULL) {
        CHashSet::New(0, (ISet**)&strings);
    }
    else {
        CHashSet::New(ICollection::Probe(strings), (ISet**)&strings);
    }
    AutoPtr<ICharSequence> chars = CoreUtils::Convert(value);
    strings->Add(TO_IINTERFACE(chars));
    return editor->PutStringSet(key, strings);
}

ECode InstallShortcutReceiver::AddToInstallQueue(
    /* [in] */ ISharedPreferences* sharedPrefs,
    /* [in] */ PendingInstallShortcutInfo* info)
{
    synchronized(sLock){
        //try {
        AutoPtr<IJSONStringer> json;
        CJSONStringer::New((IJSONStringer**)&json);
        ECode ec = json->Object();
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        ec = json->Key(DATA_INTENT_KEY);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        String str;
        ec = json->Value(info->mData->ToUri(0, &str), str);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        ec = json->Key(LAUNCH_INTENT_KEY);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        ec = json->Value(info->mLaunchIntent->ToUri(0, &str), str);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        ec = json->Key(NAME_KEY);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        ec = json->Value(info->mName);
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        if (info->mIcon != NULL) {
            AutoPtr<ArrayOf<Byte> > iconByteArray;
            ItemInfo::FlattenBitmap(info->mIcon, (ArrayOf<Byte>**)&iconByteArray);
            ec = json->Key(ICON_KEY);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
            AutoPtr<IBase64> base64;
            CBase64::AcquireSingleton((IBase64**)&base64);
            String str;
            base64->EncodeToString(iconByteArray, 0, iconByteArray->GetLength(),
                    IBase64::DEFAULT, &str);
            ec = json->Value(str);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        }
        if (info->mIconResource != NULL) {
            ec = json->Key(ICON_RESOURCE_NAME_KEY);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
            String resourceName;
            info->mIconResource->GetResourceName(&resourceName);
            ec = json->Value(resourceName);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
            ec = json->Key(ICON_RESOURCE_PACKAGE_NAME_KEY);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
            String packageName;
            info->mIconResource->GetPackageName(&packageName);
            ec = json->Value(packageName);
            if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        }
        ec = json->EndObject();
        if (ec == (ECode)E_JSON_EXCEPTION) goto EXIT;
        AutoPtr<ISharedPreferencesEditor> editor;
        sharedPrefs->Edit((ISharedPreferencesEditor**)&editor);
        String res;
        IObject::Probe(json)->ToString(&res);
        AddToStringSet(sharedPrefs, editor, APPS_PENDING_INSTALL, res);
        editor->Commit();
        //} catch (org.json.JSONException e) {
EXIT:
        Slogger::D("InstallShortcutReceiver", "Exception when adding shortcut: %d", ec);
        //}
    }
    return NOERROR;
}

AutoPtr<IArrayList> InstallShortcutReceiver::GetAndClearInstallQueue(
    /* [in] */ ISharedPreferences* sharedPrefs)
{
    synchronized(sLock) {
        AutoPtr<ISet> strings;
        sharedPrefs->GetStringSet(APPS_PENDING_INSTALL, NULL, (ISet**)&strings);
        if (strings == NULL) {
            AutoPtr<ArrayList> list;
            CArrayList::New((ArrayList**)&list);
            return list;
        }
        AutoPtr<ArrayList> infos;
        CArrayList::New((ArrayList**)&infos);

        AutoPtr<ArrayOf<IInterface*> > array;
        strings->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<ICharSequence> cobj = ICharSequence::Probe((*array)[i]);
            String json;
            cobj->ToString(json);

            //try {
            AutoPtr<IJSONTokener> tokener;
            CJSONTokener::New(json, (IJSONTokener**)&tokener);
            AutoPtr<IInterface> obj;
            tokener->NextValue((IInterface**)&obj);
            AutoPtr<IJSONObject> object = IJSONObject::Probe(obj);

            String key;
            object->GetString(DATA_INTENT_KEY, &key);
            AutoPtr<IIntent> data;
            Intent::ParseUri(key, 0, (IIntent**)&data);

            String key2;
            object->GetString(LAUNCH_INTENT_KEY, &key2);
            AutoPtr<IIntent> launchIntent;
            Intent::ParseUri(key2, 0, (IIntent**)&launchIntent);

            String name;
            object->GetString(NAME_KEYm, &name);
            String iconBase64;
            object->OptString(ICON_KEY, &iconBase64);
            String iconResourceName;
            object->OptString(ICON_RESOURCE_NAME_KEY, &iconResourceName);
            String iconResourcePackageName;
            object->OptString(ICON_RESOURCE_PACKAGE_NAME_KEY, &iconResourcePackageName);

            if (iconBase64 != NULL && !iconBase64.isEmpty()) {
                AutoPtr<IBase64> base64;
                CBase64::AcquireSingleton((IBase64**)&base64);
                AutoPtr<ArrayOf<Byte> > iconArray;
                base64->Decode(iconBase64, IBase64::DEFAULT, (ArrayOf<Byte>**)&iconArray);

                AutoPtr<IBitmap> b;
                BitmapFactory::DecodeByteArray(iconArray, 0,
                        iconArray->GetLength(), (IBitmap**)&b);
                data->PutExtra(IIntent::EXTRA_SHORTCUT_ICON, b);
            }
            else if (iconResourceName != NULL && !iconResourceName.isEmpty()) {
                AutoPtr<IIntentShortcutIconResource> iconResource;
                CIntentShortcutIconResource::New((IIntentShortcutIconResource**)&iconResource);
                iconResource->SetResourceName(iconResourceName);
                iconResource->SetPackageName(iconResourcePackageName);
                data->PutExtra(IIntent::EXTRA_SHORTCUT_ICON_RESOURCE, iconResource);
            }
            data->PutExtra(IIntent::EXTRA_SHORTCUT_INTENT, launchIntent);
            AutoPtr<PendingInstallShortcutInfo> info =
                    new PendingInstallShortcutInfo(data, name, launchIntent);
            infos->Add(info);
            // } catch (org.json.JSONException e) {
            //     Log.d("InstallShortcutReceiver", "Exception reading shortcut to add: " + e);
            // } catch (java.net.URISyntaxException e) {
            //     Log.d("InstallShortcutReceiver", "Exception reading shortcut to add: " + e);
            // }
        }

        AutoPtr<ISharedPreferencesEditor> edit;
        sharedPrefs->Edit((ISharedPreferencesEditor**)&edit);
        AutoPtr<ISet> set;
        HashSet::New((ISet**)&set);
        edit->PutStringSet(APPS_PENDING_INSTALL, set);
        Boolean res;
        edit->Commit(&res);
        return infos;
    }
}

ECode InstallShortcutReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    mData->GetAction(&action);
    Boolean res;
    ACTION_INSTALL_SHORTCUT.Equals(action, &res);
    if (!res) {
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_INTENT, (IIntent**)&intent);
    if (intent == NULL) {
        return NOERROR;
    }
    // This name is only used for comparisons and notifications, so fall back to activity name
    // if not supplied
    String name;
    data->GetStringExtra(IIntent::EXTRA_SHORTCUT_NAME, &name);
    if (name.IsNull()) {
        //try {
        AutoPtr<IPackageManager> pm;
        ECode ec = context->GetPackageManager((IPackageManager**)&pm);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) return;
        AutoPtr<IComponentName> componentName;
        ec = intent->GetComponent((IComponentName**)&componentName);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) return;
        AutoPtr<IActivityInfo> info;
        ec = pm->GetActivityInfo(componentName, 0, (IActivityInfo**)&info);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) return;
        AutoPtr<ICharSequence> label;
        ec = info->LoadLabel(pm, (ICharSequence**)&label);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) return;
        label->ToString(&name);
        //} catch (PackageManager.NameNotFoundException nnfe) {
        //    return;
        //}
    }
    AutoPtr<IBitmap> icon;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_ICON, (IBitmap**)&icon);
    AutoPtr<IIntentShortcutIconResource> iconResource;
    data->GetParcelableExtra(IIntent::EXTRA_SHORTCUT_ICON_RESOURCE,
            (IIntentShortcutIconResource*8)&iconResource);

    // Queue the item up for adding if launcher has not loaded properly yet
    Int32 x;
    LauncherModel::GetCellCountX(&x);
    Int32 y;
    LauncherModel::GetCellCountY(&y);
    Boolean launcherNotLoaded = x <= 0 || y <= 0;

    AutoPtr<PendingInstallShortcutInfo> info =
            new PendingInstallShortcutInfo(data, name, intent);
    info->mIcon = icon;
    info->mIconResource = iconResource;
    if (mUseInstallQueue || launcherNotLoaded) {
        String spKey;
        LauncherApplication::GetSharedPreferencesKey(&spKey);
        AutoPtr<ISharedPreferences> sp;
        context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE,
                (ISharedPreferences**)&sp);
        AddToInstallQueue(sp, info);
    }
    else {
        ProcessInstallShortcut(context, info);
    }
    return NOERROR;
}

ECode InstallShortcutReceiver::EnableInstallQueue()
{
    mUseInstallQueue = TRUE;
    return NOERROR;
}

ECode InstallShortcutReceiver::DisableAndFlushInstallQueue(
    /* [in] */ IContext* context)
{
    mUseInstallQueue = FALSE;
    return FlushInstallQueue(context);
}

ECode InstallShortcutReceiver::FlushInstallQueue(
    /* [in] */ IContext* context)
{
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE,
            (ISharedPreferences**)&sp);
    AutoPtr<IArrayList> installQueue = GetAndClearInstallQueue(sp);

    AutoPtr<IIterator> iter;
    installQueue->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(res), res) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        PendingInstallShortcutInfo* info = (PendingInstallShortcutInfo*)IObject::Probe(obj);
        ProcessInstallShortcut(context, info);
    }
    return NOERROR;
}

void InstallShortcutReceiver::ProcessInstallShortcut(
    /* [in] */ IContext* context,
    /* [in] */ PendingInstallShortcutInfo* pendingInfo)
{
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE,
            (ISharedPreferences**)&sp);

    AutoPtr<IIntent> data = pendingInfo->mData;
    AutoPtr<IIntent> intent = pendingInfo->mLaunchIntent;
    String name = pendingInfo->mName;

    // Lock on the app so that we don't try and get the items while apps are being added
    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(1);
    (*result)[0] = INSTALL_SHORTCUT_SUCCESSFUL;
    Boolean found = FALSE;
    synchronized(app) {
        // Flush the LauncherModel worker thread, so that if we just did another
        // processInstallShortcut, we give it time for its shortcut to get added to the
        // database (getItemsInLocalCoordinates reads the database)
        AutoPtr<LauncherModel> modle;
        app->GetModel((*LauncherModel*)&modle);
        modle->FlushWorkerThread();
        AutoPtr<IArrayList> items;
        LauncherModel::GetItemsInLocalCoordinates(context, (IArrayList**)&items);
        Boolean exists;
        LauncherModel::ShortcutExists(context, name, intent, &exists);

        // Try adding to the workspace screens incrementally, starting at the default or center
        // screen and alternating between +1, -1, +2, -2, etc. (using ~ ceil(i/2f)*(-1)^(i-1))
        const Int32 screen = ILauncher::DEFAULT_SCREEN;
        for (Int32 i = 0; i < (2 * ILauncher::SCREEN_COUNT) + 1 && !found; ++i) {
            Int32 si = screen + (Int32)((i / 2f) + 0.5f) * ((i % 2 == 1) ? 1 : -1);
            if (0 <= si && si < ILauncher::SCREEN_COUNT) {
                found = InstallShortcut(context, data, items, name, intent, si, exists, sp,
                        result);
            }
        }
    }

    // We only report error messages (duplicate shortcut or out of space) as the add-animation
    // will provide feedback otherwise
    if (!found) {
        if ((*result)[0] == INSTALL_SHORTCUT_NO_SPACE) {
            String str;
            context->GetString(Elastos::Droid::Launcher2::R::
                    string::completely_out_of_space, &str);

            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(context, str,
                    IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
        }
        else if ((*result)[0] == INSTALL_SHORTCUT_IS_DUPLICATE) {
            String str;
            context->GetString(Elastos::Droid::Launcher2::R::
                    string::shortcut_duplicate, name, &str);
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(context, str,
                    IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
        }
    }
}

Boolean InstallShortcutReceiver::InstallShortcut(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* data,
    /* [in] */ IArrayList* items,
    /* [in] */ const String& name,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 screen,
    /* [in] */ Boolean shortcutExists,
    /* [in] */ ISharedPreferences* sharedPrefs,
    /* [in] */ ArrayOf<Int32>* result)
{
    AutoPtr<ArrayOf<Int32> > tmpCoordinates = ArrayOf<Int32>::Alloc(2);
    if (FindEmptyCell(context, items, tmpCoordinates, screen)) {
        if (intent != NULL) {
            String action;
            intent->GetAction(&action);
            if (action.IsNull()) {
                intent->SetAction(IIntent::ACTION_VIEW);
            }
            else {
                Boolean res1;
                String action;
                intent->GetAction(&action);
                action.Equals(IIntent::ACTION_MAIN, res1);
                if (res1) {
                    AutoPtr<ArrayOf<String> > categories;
                    intent->GetCategories((ArrayOf<String>**)&categories);
                    if (categories != NULL) {
                        Boolean res2;
                        categories->Contains(IIntent::CATEGORY_LAUNCHER, &res2);
                        if (res2) {
                            intent->AddFlags(
                                    IIntent::FLAG_ACTIVITY_NEW_TASK |
                                    IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
                        }
                    }
                }
            }

            // By default, we allow for duplicate entries (located in
            // different places)
            Boolean duplicate;
            data->GetBooleanExtra(ILauncher::EXTRA_SHORTCUT_DUPLICATE, TRUE, &duplicate);
            if (duplicate || !shortcutExists) {
                AutoPtr<MyThread> t = new MyThread(String("setNewAppsThread"),
                    this, sharedPrefs, screen, intent);
                t->Start();

                // Update the Launcher db
                AutoPtr<IContext> ctx;
                context->GetApplicationContext((IContext**)&ctx);
                AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
                AutoPtr<LauncherModel> model;
                app->GetModel((LauncherModel**)&model);
                AutoPtr<IShortcutInfo> info;
                model->AddShortcut(context, data,
                        ILauncherSettings::Favorites::CONTAINER_DESKTOP, screen,
                        (*tmpCoordinates)[0], (*tmpCoordinates)[1], TRUE, (IShortcutInfo**)&info);
                if (info == NULL) {
                    return FALSE;
                }
            }
            else {
                (*result)[0] = INSTALL_SHORTCUT_IS_DUPLICATE;
            }

            return TRUE;
        }
    }
    else {
        (*result)[0] = INSTALL_SHORTCUT_NO_SPACE;
    }

    return FALSE;
}

Boolean InstallShortcutReceiver::FindEmptyCell(
    /* [in] */ IContext* context,
    /* [in] */ IArrayList* items,
    /* [in] */ ArrayOf<Int32>* xy,
    /* [in] */ Int32 screen)
{
    Int32 xCount;
    LauncherModel::GetCellCountX(&xCount);
    Int32 yCount;
    LauncherModel::GetCellCountY(&yCount);
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Boolean> > > > occupied = ArrayOf<AutoPtr<ArrayOf<Boolean> > >::Alloc(xCount);
    for (Int32 i = 0; i < xCount; i++) {
        (*occupied)[i] = ArrayOf<Boolean>::Alloc(yCount);
    }

    AutoPtr<IItemInfo> item;
    Int32 cellX, cellY, spanX, spanY;
    Int32 size;
    items->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        item = IItemInfo::Probe(obj);
        ItemInfo* _info = (ItemInfo*)item
        if (_info->mContainer == ILauncherSettings::Favorites::CONTAINER_DESKTOP) {
            if (_info->mScreen == screen) {
                cellX = _info->mCellX;
                cellY = _info->mCellY;
                spanX = _info->mSpanX;
                spanY = _info->mSpanY;
                for (Int32 x = cellX; 0 <= x && x < cellX + spanX && x < xCount; x++) {
                    for (Int32 y = cellY; 0 <= y && y < cellY + spanY && y < yCount; y++) {
                        (*occupied)[x][y] = TRUE;
                    }
                }
            }
        }
    }

    return CellLayout::FindVacantCell(xy, 1, 1, xCount, yCount, occupied);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos