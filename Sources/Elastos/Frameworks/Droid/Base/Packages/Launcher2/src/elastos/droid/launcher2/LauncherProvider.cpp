
#include "elastos/droid/launcher2/LauncherProvider.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String LauncherProvider::DatabaseHelper::TAG_FAVORITES("favorites");
const String LauncherProvider::DatabaseHelper::TAG_FAVORITE("favorite");
const String LauncherProvider::DatabaseHelper::TAG_CLOCK("clock");
const String LauncherProvider::DatabaseHelper::TAG_SEARCH("search");
const String LauncherProvider::DatabaseHelper::TAG_APPWIDGET("appwidget");
const String LauncherProvider::DatabaseHelper::TAG_SHORTCUT("shortcut");
const String LauncherProvider::DatabaseHelper::TAG_FOLDER("folder");
const String LauncherProvider::DatabaseHelper::TAG_EXTRA("extra");

LauncherProvider::DatabaseHelper::DatabaseHelper(
    /* [in] */ IContext* context)
    : SQLiteOpenHelper(context, DATABASE_NAME, NULL, DATABASE_VERSION)
    , mContext(context)
    , mMaxId(-1)
{
    CAppWidgetHost::New(context, ILauncher::APPWIDGET_HOST_ID, (IAppWidgetHost**)&mAppWidgetHost);

    // In the case where neither onCreate nor onUpgrade gets called, we read the maxId from
    // the DB here
    if (mMaxId == -1) {
        AutoPtr<ISQLiteDatabase> db;
        GetWritableDatabase((ISQLiteDatabase**)&db);
        mMaxId = InitializeMaxId(db);
    }
}

ECode LauncherProvider::DatabaseHelper::SendAppWidgetResetNotify()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    return resolver->NotifyChange(CONTENT_APPWIDGET_RESET_URI, NULL);
}

ECode LauncherProvider::DatabaseHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    if (LOGD) Slogger::D(TAG, "creating new launcher database");

    mMaxId = 1;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    // Default profileId to the serial number of this user.
    AutoPtr<IUserHandle> user;
    Process::MyUserHandle((IUserHandle**)&user);
    Int64 userSerialNumber;
    um->GetSerialNumberForUser(user, &userSerialNumber);

    StringBuilder sb;
    sb += "CREATE TABLE favorites (";
    sb += "_id INTEGER PRIMARY KEY,";
    sb += "title TEXT,";
    sb += "intent TEXT,";
    sb += "container INTEGER,";
    sb += "screen INTEGER,";
    sb += "cellX INTEGER,";
    sb += "cellY INTEGER,";
    sb += "spanX INTEGER,";
    sb += "spanY INTEGER,";
    sb += "itemType INTEGER,";
    sb += "appWidgetId INTEGER NOT NULL DEFAULT -1,";
    sb += "isShortcut INTEGER,";
    sb += "iconType INTEGER,";
    sb += "iconPackage TEXT,";
    sb += "iconResource TEXT,";
    sb += "icon BLOB,";
    sb += "uri TEXT,";
    sb += "displayMode INTEGER,";
    sb += "profileId INTEGER DEFAULT ";
    sb += userSerialNumber;
    sb += ");";

    db->ExecSQL(sb.ToString());

    // Database was just created, so wipe any previous widgets
    if (mAppWidgetHost != NULL) {
        mAppWidgetHost->DeleteHost();
        SendAppWidgetResetNotify();
    }

    Boolean res;
    ConvertDatabase(db, &res);
    if (!res) {
        // Set a shared pref so that we know we need to load the default workspace later
        SetFlagToLoadDefaultWorkspaceLater();
    }
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::SetFlagToLoadDefaultWorkspaceLater()
{
    String spKey;
    LauncherApplication::GetSharedPreferencesKey(&spKey);
    AutoPtr<ISharedPreferences> sp;
    mContext->GetSharedPreferences(spKey, IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(DB_CREATED_BUT_DEFAULT_WORKSPACE_NOT_LOADED, TRUE);
    return editor->Commit();
}

ECode LauncherProvider::DatabaseHelper::ConvertDatabase(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (LOGD) Slogger::D(TAG, "converting database from an older format, but not onUpgrade");
    Boolean converted = FALSE;

    StringBuilder sb;
    sb += "content://";
    sb += ISettings::AUTHORITY;
    sb += "/old_favorites?notify=true";
    AutoPtr<IUri> uri = Uri::Parse(sb.ToString());
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;

    //try {
    cursor = resolver->Query(uri, NULL, NULL, NULL, NULL, (ICursor**)&cursor);
    //} catch (Exception e) {
        // Ignore
    //}

    // We already have a favorites database in the old provider
    if (cursor != NULL) {
        Boolean count;
        cursor->GetCount(&count);
        if (count > 0) {
            //try {
            Int32 num;
            CopyFromCursor(db, cursor, &num);
            converted = num > 0;
            //} finally {
            cursor->Close();
            //}
            if (converted) {
                resolver->Delete(uri, NULL, NULL);
            }
        }
    }

    if (converted) {
        // Convert widgets from this import into widgets
        if (LOGD) Slogger::D(TAG, "converted and now triggering widget upgrade");
        ConvertWidgets(db);
    }

    *result = converted;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::CopyFromCursor(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ICursor* c,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = 0;

    Int32 idIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::_ID);
    Int32 intentIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::INTENT);
    Int32 titleIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::TITLE);
    Int32 iconTypeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_TYPE);
    Int32 iconIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON);
    Int32 iconPackageIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_PACKAGE);
    Int32 iconResourceIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_RESOURCE);
    Int32 containerIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER);
    Int32 itemTypeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ITEM_TYPE);
    Int32 screenIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN);
    Int32 cellXIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX);
    Int32 cellYIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY);
    Int32 uriIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::URI);
    Int32 displayModeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::DISPLAY_MODE);

    Int32 count;
    c->GetCount(&count);
    AutoPtr<ArrayOf<IContentValues*> > rows = ArrayOf<IContentValues*>::Alloc(count);
    Int32 i = 0;
    Boolean res;
    while (c->MoveToNext(&res), res) {
        Int32 columnCount;
        c->GetColumnCount(&columnCount);
        AutoPtr<IContentValues> values;
        CContentValues::New(columnCount, (IContentValues**)&values);

        Int64 columnValue;
        c->GetInt64(idIndex, &columnValue);
        values->Put(LauncherSettings::Favorites::_ID, columnValue);

        String str1;
        c->GetString(intentIndex, &str1);
        values->Put(LauncherSettings::Favorites::INTENT, str1);

        String str2;
        c->GetString(titleIndex, &str2);
        values->Put(LauncherSettings::Favorites::TITLE, str2);

        Int32 value1;
        c->GetInt32(iconTypeIndex, &value1);
        values->Put(LauncherSettings::Favorites::ICON_TYPE, value1);

        AutoPtr<ArrayOf<Byte> > blob;
        c->GetBlob(iconIndex, (ArrayOf<Byte>**)&blob);
        values->Put(LauncherSettings::Favorites::ICON, blob);

        String str3;
        c->GetString(iconPackageIndex, &str3);
        values->Put(LauncherSettings::Favorites::ICON_PACKAGE, str3);

        String str4;
        c->GetString(iconResourceIndex, &str4);
        values->Put(LauncherSettings::Favorites::ICON_RESOURCE, str4);

        Int32 value2;
        c->GetInt32(containerIndex, &value2);
        values->Put(LauncherSettings::Favorites::CONTAINER, value2);

        Int32 value3;
        c->GetInt32(itemTypeIndex, &value3);
        values->Put(LauncherSettings::Favorites::ITEM_TYPE, value3);

        values->Put(LauncherSettings::Favorites::APPWIDGET_ID, -1);

        Int32 value4;
        c->GetInt32(screenIndex, &value4);
        values->Put(LauncherSettings::Favorites::SCREEN, value4);

        Int32 value5;
        c->GetInt32(cellXIndex, &value5);
        values->Put(LauncherSettings::Favorites::CELLX, value5);

        Int32 value6;
        c->GetInt32(cellYIndex, &value6);
        values->Put(LauncherSettings::Favorites::CELLY, value6);

        String str5;
        c->GetString(uriIndex, &str5);
        values->Put(LauncherSettings::Favorites::URI, str5);

        Int32 value7;
        c->GetInt32(displayModeIndex, &value7);
        values->Put(LauncherSettings::Favorites::DISPLAY_MODE, value7);
        rows->Set(i++, values);
    }

    db->BeginTransaction();
    Int32 total = 0;
    //try {
    Int32 numValues = rows->GetLength();
    for (i = 0; i < numValues; i++) {
        Int64 res;
        ECode ec = DbInsertAndCheck(this, db, TABLE_FAVORITES, NULL, (*rows)[i], &res);
        if (ec != NOERROR) {
            return db->EndTransaction();
        }
        if (res < 0) {
            *num = 0;
            return NOERROR;
        }
        else {
            total++;
        }
    }
    db->SetTransactionSuccessful();
    //} finally {
    db->EndTransaction();
    //}

    *num = total;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (LOGD) Slogger::D(TAG, "onUpgrade triggered");

    Int32 version = oldVersion;
    if (version < 3) {
        // upgrade 1,2 -> 3 added appWidgetId column
        db->BeginTransaction();
        //try {
            // Insert new column for holding appWidgetIds
        String str("ALTER TABLE favorites ADD COLUMN appWidgetId INTEGER NOT NULL DEFAULT -1;");
        ECode ec = db->ExecSQL(str);
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(TAG, ex.getMessage(), ex);
            return db->EndTransaction();
        }
        ec = db->SetTransactionSuccessful();
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(TAG, ex.getMessage(), ex);
            return db->EndTransaction();
        }
        version = 3;
        //} catch (SQLException ex) {
            // Old version remains, which means we wipe old data
            //Log.e(TAG, ex.getMessage(), ex);
        //} finally {
        db->EndTransaction();
        //}

        // Convert existing widgets only if table upgrade was successful
        if (version == 3) {
            ConvertWidgets(db);
        }
    }

    if (version < 4) {
        version = 4;
    }

    // Where's version 5?
    // - Donut and sholes on 2.0 shipped with version 4 of launcher1.
    // - Passion shipped on 2.1 with version 6 of launcher2
    // - Sholes shipped on 2.1r1 (aka Mr. 3) with version 5 of launcher 1
    //   but version 5 on there was the updateContactsShortcuts change
    //   which was version 6 in launcher 2 (first shipped on passion 2.1r1).
    // The updateContactsShortcuts change is idempotent, so running it twice
    // is okay so we'll do that when upgrading the devices that shipped with it.
    if (version < 6) {
        // We went from 3 to 5 screens. Move everything 1 to the right
        db->BeginTransaction();
        //try {
        ECode ec = db->ExecSQL(String("UPDATE favorites SET screen=(screen + 1);"));
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(TAG, ex.getMessage(), ex);
            return db->EndTransaction();
        }
        ec = db->SetTransactionSuccessful();
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(TAG, ex.getMessage(), ex);
            return db->EndTransaction();
        }
        //} catch (SQLException ex) {
            // Old version remains, which means we wipe old data
           //Log.e(TAG, ex.getMessage(), ex);
        //} finally {
        db->EndTransaction();
        //}

        // We added the fast track.
        Boolean res;
        UpdateContactsShortcuts(db, &res);
        if (ures) {
            version = 6;
        }
    }

    if (version < 7) {
        // Version 7 gets rid of the special search widget.
        ConvertWidgets(db);
        version = 7;
    }

    if (version < 8) {
        // Version 8 (froyo) has the icons all normalized.  This should
        // already be the case in practice, but we now rely on it and don't
        // resample the images each time.
        NormalizeIcons(db);
        version = 8;
    }

    if (version < 9) {
        // The max id is not yet set at this point (onUpgrade is triggered in the ctor
        // before it gets a change to get set, so we need to read it here when we use it)
        if (mMaxId == -1) {
            InitializeMaxId(db, &mMaxId);
        }

        // Add default hotseat icons
        LoadFavorites(db,
                Elastos::Droid::Launcher2::R::xml::update_workspace);
        version = 9;
    }

    // We bumped the version three time during JB, once to update the launch flags, once to
    // update the override for the default launch animation and once to set the mimetype
    // to improve startup performance
    if (version < 12) {
        // Contact shortcuts need a different set of flags to be launched now
        // The updateContactsShortcuts change is idempotent, so we can keep using it like
        // back in the Donut days
        UpdateContactsShortcuts(db);
        version = 12;
    }

    if (version < 13) {
        // Add userId column
        Boolean res;
        AddProfileColumn(db, &res);
        if (res) {
            version = 13;
        }
    }

    if (version != DATABASE_VERSION) {
        Slogger::W(TAG, "Destroying all old data.");
        StringBuilder sb;
        sb += "DROP TABLE IF EXISTS ";
        sb += TABLE_FAVORITES;
        db->ExecSQL(sb.ToString());
        OnCreate(db);
    }
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddProfileColumn(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    db->BeginTransaction();
    // try {
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    // Default to the serial number of this user, for older
    // shortcuts.
    AutoPtr<IUserHandle> user;
    Process::MyUserHandle((IUserHandle**)&user);
    Int64 userSerialNumber;
    um->GetSerialNumberForUser(user, &userSerialNumber);
    // Insert new column for holding user serial number
    StringBuilder sb;
    sb += "ALTER TABLE favorites ";
    sb += "ADD COLUMN profileId INTEGER DEFAULT ";
    sb += userSerialNumber;
    sb += ";"
    ECode ec = db->ExecSQL(sb.ToString());
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::E(TAG, ex.getMessage(), ex);
        *result = FALSE;
        return db->EndTransaction();
    }
    ec = db->SetTransactionSuccessful();
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::E(TAG, ex.getMessage(), ex);
        *result = FALSE;
        return db->EndTransaction();
    }
    *result = TRUE;
    // } catch (SQLException ex) {
    //     // Old version remains, which means we wipe old data
    //     Log.e(TAG, ex.getMessage(), ex);
    //     return false;
    // } finally {
    return db->EndTransaction();
    // }
}

ECode LauncherProvider::DatabaseHelper::UpdateContactsShortcuts(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = IFavorites::ITEM_TYPE_SHORTCUT;
    String selectWhere;
    BuildOrWhereString(IFavorites::ITEM_TYPE, array, &selectWhere);

    AutoPtr<ICursor> c;
    String actionQuickContact("com.android.contacts.action.QUICK_CONTACT");
    db->BeginTransaction();
    //try {
    // Select and iterate through each matching widget
    AutoPtr<ArrayOf<String> > stringArray = ArrayOf<String>::Alloc(2);
    (*stringArray)[0] = IFavorites::_ID;
    (*stringArray)[1] = IFavorites::INTENT;
    ECode ec = db->Query(TABLE_FAVORITES, stringArray,
            selectWhere, NULL, NULL, NULL, NULL, (ICursor**)&c);
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while upgrading contacts", ex);
        db->EndTransaction();
        if (c != NULL) {
            c->Close();
        }
        *result = FALSE;
        return NOERROR;
    }
    if (c == NULL) {
        *result = FALSE;
        db->EndTransaction();
        return NOERROR;
    }

    Int32 count;
    c->GetCount(&count);
    if (LOGD) Slogger::D(TAG, "found upgrade cursor count=%d", count);

    Int32 idIndex;
    c->GetColumnIndex(IFavorites::_ID, &idIndex);
    Int32 intentIndex;
    c->GetColumnIndex(IFavorites::INTENT);

    Boolean res;
    while (c->MoveToNext(&res), res) {
        Int64 favoriteId;
        c->GetInt64(idIndex, &favoriteId);
        String intentUri;
        c->GetString(intentIndex, &intentUri);
        if (!intentUri.IsNull()) {
            //try {
            AutoPtr<IIntent> intent;
            Intent::ParseUri(intentUri, 0, (IIntent**)&intent);
            Slogger::D("Home", intent.toString());
            AutoPtr<IUri> uri;
            intent->GetData((IUri**)&uri);
            if (uri != NULL) {
                String data;
                uri->ToString(&data);
                Boolean res1, res2, res3, res4;
                String action;
                intent->GetAction(&action);
                action.Equals(IIntent::ACTION_VIEW, &res1);
                actionQuickContact.Equals(action, &res2);
                data.StartsWith(String("content://contacts/people/"), &res3);
                data.StartsWith(String("content://com.android.contacts/contacts/lookup/"),
                        &res4);
                if ((res1 || res2) && (res3 || res4)) {
                    AutoPtr<IIntent> newIntent;
                    CIntent::New(actionQuickContact, (IIntent**)&newIntent);
                    // When starting from the launcher, start in a new, cleared task
                    // CLEAR_WHEN_TASK_RESET cannot reset the root of a task, so we
                    // clear the whole thing preemptively here since
                    // QuickContactActivity will finish itself when launching other
                    // detail activities.
                    newIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                            IIntent::FLAG_ACTIVITY_CLEAR_TASK);
                    newIntent->PutExtra(
                            ILauncher::INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION, TRUE);
                    newIntent->SetData(uri);
                    // Determine the type and also put that in the shortcut
                    // (that can speed up launch a bit)
                    String type;
                    newIntent->ResolveType(mContext, &type);
                    newIntent->SetDataAndType(uri, type);

                    AutoPtr<IContentValues> values;
                    CContentValues::New((IContentValues**)&values);
                    String str;
                    newIntent->ToURI(&str);
                    values->Put(LauncherSettings::Favorites::INTENT, str);

                    StringBuilder sb;
                    sb += IFavorites::_ID;
                    sb += "=";
                    sb += favoriteId
                    String updateWhere = sb.ToString();
                    ec = db->Update(TABLE_FAVORITES, values, updateWhere, NULL);
                    if (ec == (ECode)E_SQL_EXCEPTION) {
                        Slogger::W(TAG, "Problem while upgrading contacts", ex);
                        db->EndTransaction();
                        if (c != NULL) {
                            c->Close();
                        }
                        *result = FALSE;
                        return NOERROR;
                    }
                }
            }
            assert(0);
            //} catch (RuntimeException ex) {
                //Slogger::E(TAG, "Problem upgrading shortcut", ex);
            //} catch (URISyntaxException e) {
                //Slogger::E(TAG, "Problem upgrading shortcut", e);
            //}
        }
    }
    db->SetTransactionSuccessful();
    // } catch (SQLException ex) {
    //     Log.w(TAG, "Problem while upgrading contacts", ex);
    //     return false;
    // } finally {
    db->EndTransaction();
    if (c != NULL) {
        c->Close();
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::NormalizeIcons(
    /* [in] */ ISQLiteDatabase* db)
{
    Slogger::D(TAG, "normalizing icons");

    db->BeginTransaction();
    AutoPtr<ICursor> c;
    AutoPtr<ISQLiteStatement> update;
    //try {
    Boolean logged = FALSE;
    ECode ec = db->CompileStatement(String("UPDATE favorites SET icon=? WHERE _id=?"),
            (ISQLiteStatement**)&update);
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
        goto FINALLY;
    }

    StringBuilder sb;
    sb += "SELECT _id, icon FROM favorites WHERE iconType=";
    sb += IFavorites::ICON_TYPE_BITMAP;
    ec = db->RawQuery(sb.ToString(), NULL, (ICursor**)&c);
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
        goto FINALLY;
    }

    Int32 idIndex;
    c->GetColumnIndexOrThrow(IFavorites::_ID, &idIndex);
    Int32 iconIndex;
    c->GetColumnIndexOrThrow(IFavorites::ICON, &iconIndex);

    Boolean res;
    while (c->MoveToNext(&res), res) {
        Int64 id;
        c->GetInt64(idIndex, &id);
        AutoPtr<ArrayOf<Byte> > data;
        c->GetBlob(iconIndex, (ArrayOf<Byte>**)&data);
        //try {
        AutoPtr<IBitmap> _bitmap;
        FAIL_GOTO(BitmapFactory::DecodeByteArray(data, 0, data->GetLength(),
                (IBitmap**)&_bitmap), ERROR)
        AutoPtr<IBitmap> bitmap;
        FAIL_GOTO(Utilities::ResampleIconBitmap(_bitmap, mContext,
                (IBitmap**)&bitmap), ERROR)
        if (bitmap != NULL) {
            FAIL_GOTO(update->BindInt64(1, id), ERROR)
            FAIL_GOTO(ItemInfo::FlattenBitmap(bitmap, (ArrayOf<Byte>**)&data), ERROR)
            if (data != NULL) {
                FAIL_GOTO(update->BindBlob(2, data), ERROR)
                FAIL_GOTO(update->Execute(), ERROR)
            }
            FAIL_GOTO(bitmap->Recycle(), ERROR)
        }
        //} catch (Exception e) {
ERROR:
            if (!logged) {
                Slogger::E(TAG, "Failed normalizing icon " + id, e);
            } else {
                Slogger::E(TAG, "Also failed normalizing icon " + id);
            }
            logged = true;
            goto FINALLY;
        //}
    }
    ec = db->SetTransactionSuccessful();
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
        goto FINALLY;
    }
    //} catch (SQLException ex) {
        //Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
    //} finally {
FINALLY:
    db->EndTransaction();
    if (update != NULL) {
        update->Close();
    }
    if (c != NULL) {
        c->Close();
    }
    //}
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::GenerateNewId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    if (mMaxId < 0) {
        //throw new RuntimeException("Error: max id was not initialized");
        Slogger::E("LauncherProvider::DatabaseHelper",
                "Error: max id was not initialized");
        return E_RUNTIME_EXCEPTION;
    }
    mMaxId += 1;
    *id = mMaxId;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::InitializeMaxId(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ Int64* maxid)
{
    VALIDATE_NOT_NULL(maxid);
    *maxid = 0;

    AutoPtr<ICursor> c;
    db->RawQuery(String("SELECT MAX(_id) FROM favorites"), NULL, (ICursor**)&c);

    // get the result
    Int32 maxIdIndex = 0;
    Int64 id = -1;
    Boolean res;
    if (c != NULL && (c->MoveToNext(&res), res)) {
        c->GetInt64(maxIdIndex, &id);
    }
    if (c != NULL) {
        c->Close();
    }

    if (id == -1) {
        //throw new RuntimeException("Error: could not query max id");
        Slogger::E("LauncherProvider::DatabaseHelper",
                "Error: could not query max id");
        return E_RUNTIME_EXCEPTION;
    }

    *maxid = id;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::ConvertWidgets(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IAppWidgetManager> appWidgetManager;
    AppWidgetManager::GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);

    AutoPtr<ArrayOf<Int32> > bindSources = ArrayOf<Int32>::Alloc(3);
    (*bindSources)[0] = IFavorites::ITEM_TYPE_WIDGET_CLOCK;
    (*bindSources)[1] = IFavorites::ITEM_TYPE_WIDGET_PHOTO_FRAME;
    (*bindSources)[2] = IFavorites::ITEM_TYPE_WIDGET_SEARCH;

    String selectWhere;
    BuildOrWhereString(IFavorites::ITEM_TYPE, bindSources, &selectWhere);

    AutoPtr<ICursor> c;

    db->BeginTransaction();
    //try {
    // Select and iterate through each matching widget
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IFavorites::_ID;
    (*array)[1] = IFavorites::ITEM_TYPE;
    ECode ec = db->Query(TABLE_FAVORITES, array,
            selectWhere, NULL, NULL, NULL, NULL, (ICursor**)&c);
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets");
        goto FINALLY;
    }

    if (LOGD) Slogger::D(TAG, "found upgrade cursor count=" + c.getCount());

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Boolean res;
    while (c != null && (c->MoveToNext(&res), res)) {
        Int64 favoriteId;
        c->GetInt64(0, &favoriteId);
        Int32 favoriteType;
        c->GetInt32(1, &favoriteType);

        // Allocate and update database with new appWidgetId
        //try {
        Int32 appWidgetId;
        mAppWidgetHost->AllocateAppWidgetId(&appWidgetId);

        if (LOGD) {
            Slogger::D(TAG, "allocated appWidgetId=" + appWidgetId
                    + " for favoriteId=" + favoriteId);
        }
        values->Clear();
        values->Put(IFavorites::ITEM_TYPE, IFavorites::ITEM_TYPE_APPWIDGET);
        values->Put(IFavorites::APPWIDGET_ID, appWidgetId);

        // Original widgets might not have valid spans when upgrading
        if (favoriteType == IFavorites::ITEM_TYPE_WIDGET_SEARCH) {
            values->Put(LauncherSettings::Favorites::SPANX, 4);
            values->Put(LauncherSettings::Favorites::SPANY, 1);
        }
        else {
            values->Put(LauncherSettings::Favorites::SPANX, 2);
            values->Put(LauncherSettings::Favorites::SPANY, 2);
        }

        StringBuilder sb;
        sb += IFavorites::_ID;
        sb += "=";
        sb += favoriteId;
        String updateWhere;
        sb.ToString(&updateWhere);
        ec = db->Update(TABLE_FAVORITES, values, updateWhere, NULL);
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets");
            goto FINALLY;
        }

        if (favoriteType == IFavorites::ITEM_TYPE_WIDGET_CLOCK) {
            // TODO: check return value
            AutoPtr<IComponentName> name;
            CComponentName::New(String("com.android.alarmclock"),
                    String("com.android.alarmclock.AnalogAppWidgetProvider"),
                    (IComponentName**)&name);
            appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name);
        }
        else if (favoriteType == IFavorites::ITEM_TYPE_WIDGET_PHOTO_FRAME) {
            // TODO: check return value
            AutoPtr<IComponentName> name;
            CComponentName::New(String("com.android.camera"),
                    String("com.android.camera.PhotoAppWidgetProvider"),
                    (IComponentName**)&name);
            appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name);
        }
        else if (favoriteType == IFavorites::ITEM_TYPE_WIDGET_SEARCH) {
            // TODO: check return value
            AutoPtr<IComponentName> name;
            GetSearchWidgetProvider((IComponentName**)&name)
            appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name);
        }
        assert(0);
        //} catch (RuntimeException ex) {
            Slogger::E(TAG, "Problem allocating appWidgetId", ex);
        //}
    }

    ec = db->SetTransactionSuccessful();
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets");
        goto FINALLY;
    }
    //} catch (SQLException ex) {
    //    Log.w(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
    //} finally {
FINALLY:
    db->EndTransaction();
    if (c != NULL) {
        c->Close();
    }
    //}
}

ECode LauncherProvider::DatabaseHelper::BeginDocument(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& firstElementName)
{
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        //throw new XmlPullParserException("No start tag found");
        Slogger::E("LauncherProvider::DatabaseHelper", "No start tag found");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    String name;
    parser->GetName(&name);
    Boolean res;
    name.Equals(firstElementName, &res);
    if (!res) {
        // throw new XmlPullParserException("Unexpected start tag: found " + parser.getName() +
        //         ", expected " + firstElementName);
        Slogger::E("LauncherProvider::DatabaseHelper", "Unexpected start tag: found " + parser.getName() +
                ", expected " + firstElementName);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
}

ECode LauncherProvider::DatabaseHelper::LoadFavorites(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 workspaceResourceId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 allAppsButtonRank;
    resources->GetInteger(
            Elastos::Droid::Launcher2::R::integer::hotseat_all_apps_index,
            &allAppsButtonRank);
    Int32 i = 0;
    //try {
    AutoPtr<IXmlResourceParser> parser;
    ECode ec = NOERROR;
    FAIL_GOTO(ec = resources->GetXml(workspaceResourceId,
            (IXmlResourceParser**)&parser), ERROR);
    AutoPtr<IAttributeSet> attrs;
    FAIL_GOTO(ec = Xml::AsAttributeSet(parser, (IAttributeSet**)&attrs), ERROR);
    BeginDocument(parser, TAG_FAVORITES);

    Int32 depth;
    parser->GetDepth(&depth);

    Int32 type;
    Int32 _depth;
    while (((parser->Next(&type), type) != IXmlPullParser::END_TAG ||
            (parser->GetDepth(&_depth), _depth) > depth) &&
            type != IXmlPullParser::END_DOCUMENT) {

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        Boolean added = FALSE;
        String name;
        parser->GetName(&name);

        AutoPtr<ITypedArray> a;
        mContext->ObtainStyledAttributes(attrs,
                Elastos::Droid::Launcher2::R::styleable::Favorite,
                (ITypedArray**)&a);

        Int64 container = LauncherSettings::Favorites::CONTAINER_DESKTOP;
        Boolean res;
        a->HasValue(Elastos::Droid::Launcher2::R::styleable::Favorite_container,
                &res)
        if (res) {
            String str;
            a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_container,
                    &str);
            container = Long.valueOf(str);
        }

        String screen;
        a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_screen,
                &screen);
        String x;
        a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_x, &x);
        String y;
        a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_y, &y);

        // If we are adding to the hotseat, the screen is used as the position in the
        // hotseat. This screen can't be at position 0 because AllApps is in the
        // zeroth position.
        if (container == LauncherSettings::Favorites::CONTAINER_HOTSEAT
                && Integer.valueOf(screen) == allAppsButtonRank) {
            //throw new RuntimeException("Invalid screen position for hotseat item");
            Slogger::E(TAG, "Invalid screen position for hotseat item");
            return E_RUNTIME_EXCEPTION;
        }

        values->Clear();
        values->Put(LauncherSettings::Favorites::CONTAINER, container);
        values->Put(LauncherSettings::Favorites::SCREEN, screen);
        values->Put(LauncherSettings::Favorites::CELLX, x);
        values->Put(LauncherSettings::Favorites::CELLY, y);

        Boolean tmp;
        if (TAG_FAVORITE.Equals(name, &tmp), tmp) {
            Int64 id;
            AddAppShortcut(db, values, a, packageManager, intent, &id);
            added = id >= 0;
        }
        else if (TAG_SEARCH.Equals(name, &tmp), tmp) {
            AddSearchWidget(db, values, &added);
        }
        else if (TAG_CLOCK.Equals(name, &tmp), tmp) {
            AddClockWidget(db, values, &added);
        }
        else if (TAG_APPWIDGET.Equals(name, &tmp), tmp) {
            AddAppWidget(parser, attrs, type, db, values, a, packageManager, &added);
        }
        else if (TAG_SHORTCUT.Equals(name, &tmp), tmp) {
            Int64 id;
            AddUriShortcut(db, values, a, &id);
            added = id >= 0;
        }
        else if (TAG_FOLDER.Equals(name, &tmp), tmp) {
            String title;
            Int32 titleResId;
            a->GetResourceId(Elastos::Droid::Launcher2::R::styleable::Favorite_title, -1,
                    &titleResId);
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            if (titleResId != -1) {
                resources->GetString(titleResId, &title);
            } else {
                resources->GetString(
                        Elastos::Droid::Launcher2::R::string::folder_name,
                        &title);
            }
            values->Put(LauncherSettings::Favorites::TITLE, title);
            Int64 folderId;
            AddFolder(db, values, &folderId);
            added = folderId >= 0;

            AutoPtr<IArrayList> folderItems;
            CArrayList::New((IArrayList**)&folderItems);

            Int32 folderDepth;
            parser->GetDepth(&folderDepth);
            Int32 dep;
            while ((parser->Next(&type), type) != IXmlPullParser::END_TAG ||
                    (parser->GetDepth(&dep), dep) > folderDepth) {
                if (type != IXmlPullParser::START_TAG) {
                    continue;
                }
                String folder_item_name;
                parser->GetName(&folder_item_name);

                AutoPtr<ITypedArray> ar;
                mContext->ObtainStyledAttributes(attrs,
                        Elastos::Droid::Launcher2::R::styleable::Favorite,
                        (ITypedArray**)&ar);
                values->Clear();
                values->Put(LauncherSettings::Favorites::CONTAINER, folderId);

                Boolean res;
                if ((TAG_FAVORITE.Equals(folder_item_name, &res), res)
                        && folderId >= 0) {
                    Int64 id;
                    AddAppShortcut(db, values, ar, packageManager, intent, &id);
                    if (id >= 0) {
                        folderItems->Add(id);
                    }
                }
                else if ((TAG_SHORTCUT.Equals(folder_item_name, &res), res)
                        && folderId >= 0) {
                    Int64 id;
                    AddUriShortcut(db, values, ar, &id);
                    if (id >= 0) {
                        folderItems->Add(id);
                    }
                }
                else {
                    // throw new RuntimeException("Folders can " +
                    //         "contain only shortcuts");
                    Slogger::E(TAG, "Folders can contain only shortcuts");
                    return E_RUNTIME_EXCEPTION;
                }
                ar->Recycle();
            }
            // We can only have folders with >= 2 items, so we need to remove the
            // folder and clean up if less than 2 items were included, or some
            // failed to add, and less than 2 were actually added
            Int32 size;
            folderItems->GetSize(&size);
            if (size < 2 && folderId >= 0) {
                // We just delete the folder and any items that made it
                DeleteId(db, folderId);
                if (size > 0) {
                    DeleteId(db, folderItems.get(0));
                }
                added = FALSE;
            }
        }
        if (added) i++;
        a->Recycle();
    }
    *result = i;
    return NOERROR;

    assert(0);
ERROR:
    //} catch (XmlPullParserException e) {
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::W(TAG, "Got exception parsing favorites.", e);
    }
    //} catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Got exception parsing favorites.", e);
    }
    //} catch (RuntimeException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Slogger::W(TAG, "Got exception parsing favorites.", e);
    }
    //}
    *result = i;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddAppShortcut(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [in] */ ITypedArray* a,
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIntent* intent,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int64 id = -1;
    AutoPtr<IActivityInfo> info;
    String packageName;
    a->GetString(
        Elastos::Droid::Launcher2::::R::styleable::Favorite_packageName,
        &packageName);
    String className;
    a->GetString(
        Elastos::Droid::Launcher2::::R::styleable::Favorite_className,
        &className);
    //try {
        AutoPtr<IComponentName> cn;
        //try {
        CComponentName::New(packageName, className, (IComponentName**)&cn);
        ECode ec = packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info);
        //} catch (PackageManager.NameNotFoundException nnfe) {
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = packageName;
            AutoPtr<ArrayOf<String> > packages;
            packageManager->CurrentToCanonicalPackageNames(array,
                    (ArrayOf<String>**)&packages);
            CComponentName::New((*packages)[0], className, (IComponentName**)&cn;
            packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info);
        }
        //}
        GenerateNewId(&id);
        intent->SetComponent(cn);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        String str;
        intent->ToURI(0, &str);
        values->Put(IFavorites::INTENT, str);

        AutoPtr<ICharSequence> label;
        IPackageItemInfo::Probe(info)->LoadLabel(packageManager, (ICharSequence**)&label);
        String str2;
        label->ToString(&str2);
        values->Put(IFavorites::TITLE, str2);
        values->Put(IFavorites::ITEM_TYPE, IFavorites::ITEM_TYPE_APPLICATION);
        values->Put(IFavorites::SPANX, 1);
        values->Put(IFavorites::SPANY, 1);
        Int32 _id;
        GenerateNewId(&_id);
        values->Put(IFavorites::_ID, _id);
        Int64 num;
        DbInsertAndCheck(this, db, TABLE_FAVORITES, null, values, &num);
        if (num < 0) {
            *result = -1;
            return NOERROR;
        }
    //}
    assert(0);
    //catch (PackageManager.NameNotFoundException e) {
        Slogger:::W(TAG, "Unable to add favorite: " + packageName +
                "/" + className, e);
    //}
    *result = id;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddFolder(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    values->Put(IFavorites::ITEM_TYPE, IFavorites::ITEM_TYPE_FOLDER);
    values->Put(IFavorites::SPANX, 1);
    values->Put(IFavorites::SPANY, 1);
    Int64 id;
    GenerateNewId(&id);
    values->Put(IFavorites::_ID, id);
    Int64 num;
    DbInsertAndCheck(this, db, TABLE_FAVORITES, NULL, values, &num);
    if (num <= 0) {
        *result = -1;
        return NOERROR;
    } else {
        *result = id;
        return NOERROR;
    }
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::GetSearchWidgetProvider(
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(Context.SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
    AutoPtr<IComponentName> searchComponent;
    searchManager->GetGlobalSearchActivity((IComponentName**)&searchComponent);
    if (searchComponent == NULL) {
        *name = NULL;
        return NOERROR;
    }
    String packageName;
    searchComponent->GetPackageName(&packageName)
    return GetProviderInPackage(packageName, name);
}

ECode LauncherProvider::DatabaseHelper::GetProviderInPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IAppWidgetManager> appWidgetManager;
    AppWidgetManager::GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);
    AutoPtr<IList> providers;
    appWidgetManager->GetInstalledProviders((IList**)&providers);
    if (providers == NULL) {
        *name = NULL;
        return NOERROR;
    }
    Int32 providerCount;
    providers->GetSize(&providerCount);
    for (Int32 i = 0; i < providerCount; i++) {
        AutoPtr<IInterface> obj;
        providers->Get(i, (IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
        AutoPtr<IComponentName> provider;
        info.GetProvider((IComponentName**)&provider);
        if (provider != NULL) {
            String packageName;
            provider->GetPackageName(&packageName);
            Boolean res;
            packageName.Equals(packageName, &res);
            if (res) {
                *name = provider;
                return NOERROR;
            }
        }
    }
    *name = NULL;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddSearchWidget(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IComponentName> cn;
    GetSearchWidgetProvider((IComponentName**)&cn);
    return AddAppWidget(db, values, cn, 4, 1, NULL, result);
}

ECode LauncherProvider::DatabaseHelper::AddClockWidget(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IComponentName> cn;
    CComponentName::New(String("com.android.alarmclock"),
            String("com.android.alarmclock.AnalogAppWidgetProvider"),
            (IComponentName**)&cn);
    return AddAppWidget(db, values, cn, 2, 2, NULL, result);
}

ECode LauncherProvider::DatabaseHelper::AddAppWidget(
    /* [in] */ IXmlResourceParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 type,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [in] */ ITypedArray* a,
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    String packageName;
    a->GetString(
        Elastos::Droid::Launcher2::R::styleable::Favorite_packageName,
        &packageName);
    String className;
    a->GetString(
        Elastos::Droid::Launcher2::R::styleable::Favorite_className,
        &className);

    if (packageName.IsNull() || className.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean hasPackage = TRUE;
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, className, (IComponentName**)&cn);
    //try {
    ECode ec = packageManager->GetReceiverInfo(cn, 0);
    //} catch (Exception e) {
    if (ec != NOERROR) {
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
        (*array)[0] = packageName;
        AutoPtr<ArrayOf<String> > packages;
        packageManager->CurrentToCanonicalPackageNames(array,
                (ArrayOf<String>**)&packages);
        CComponentName::New((*packages)[0], className, (IComponentName**)&cn);

        //try {
        if(FAILED(packageManager->GetReceiverInfo(cn, 0))) {
            hasPackage = FALSE;
        }
        //} catch (Exception e1) {
            //hasPackage = false;
        //}
    }
    //}

    if (hasPackage) {
        Int32 spanX;
        a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::Favorite_spanX, 0, &spanX);
        Int32 spanY;
        a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::Favorite_spanY, 0, &spanY);

        // Read the extras
        AutoPtr<IBundle> extras;
        CBundle::New((IBundle**)&extras);
        Int32 widgetDepth;
        parser->GetDepth(&widgetDepth);
        Int32 dep;
        while ((parser-<Next(&type), type) != IXmlPullParser::END_TAG ||
                (parser->GetDepth(&dep), dep) > widgetDepth) {
            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            AutoPtr<ITypedArray> ar;
            mContext->ObtainStyledAttributes(attrs,
                    Elastos::Droid::Launcher2::R::styleable::Extra,
                    (ITypedArray**)&ar);
            String name;
            parser->GetName(&name);
            Boolean res;
            TAG_EXTRA.Equals(name, res);
            if (res) {
                String key;
                ar->GetString(Elastos::Droid::Launcher2::R::styleable::Extra_key,
                        &key);
                String value;
                ar->GetString(Elastos::Droid::Launcher2::R::styleable::Extra_value,
                        &value);
                if (!key.IsNull() && !value.IsNull()) {
                    extras->PutString(key, value);
                }
                else {
                    //throw new RuntimeException("Widget extras must have a key and value");
                    Slogger::E("LauncherProvider",
                        "Widget extras must have a key and value");
                    return E_RUNTIME_EXCEPTION;
                }
            }
            else {
                //throw new RuntimeException("Widgets can contain only extras");
                Slogger::E("LauncherProvider",
                    "Widgets can contain only extras");
                return E_RUNTIME_EXCEPTION;
            }
            ar->Recycle();
        }

        return AddAppWidget(db, values, cn, spanX, spanY, extras, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddAppWidget(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [in] */ IComponentName* cn,
    /* [in] */ Int32 spanX,
    /* [in] */ Int32 spanY,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean allocatedAppWidgets = FALSE;
    AutoPtr<IAppWidgetManager> appWidgetManager;
    AppWidgetManager->GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);

    //try {
    Int32 appWidgetId;
    ECode ec = mAppWidgetHost->AllocateAppWidgetId(&appWidgetId);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;

    values->Put(IFavorites::ITEM_TYPE, IFavorites::ITEM_TYPE_APPWIDGET);
    values->Put(IFavorites::SPANX, spanX);
    values->Put(IFavorites::SPANY, spanY);
    values->Put(IFavorites::APPWIDGET_ID, appWidgetId);
    Int32 _id;
    GenerateNewId(&_id);
    values->Put(IFavorites::_ID, _id);
    DbInsertAndCheck(this, db, TABLE_FAVORITES, NULL, values);

    allocatedAppWidgets = TRUE;

    // TODO: need to check return value
    ec = appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, cn);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;

    // Send a broadcast to configure the widget
    Boolean res;
    if (extras != NULL && (extras->IsEmpty(&res), !res)) {
        AutoPtr<IIntent> intent;
        CIntent::New(ACTION_APPWIDGET_DEFAULT_WORKSPACE_CONFIGURE,
                (IIntent**)&intent);
        ec = intent->SetComponent(cn);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;
        ec = intent->PutExtras(extras);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;
        ec = intent->PutExtra(IAppWidgetManager::EXTRA_APPWIDGET_ID, appWidgetId);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;
        ec = mContext->SendBroadcast(intent);
        if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;
    }
    *result = allocatedAppWidgets;
    return NOERROR;

ERROR:
    //} catch (RuntimeException ex) {
        Slogger::E(TAG, "Problem allocating appWidgetId", ex);
    //}

    *result = allocatedAppWidgets;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::AddUriShortcut(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ IContentValues* values,
    /* [in] */ ITypedArray* a,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);

    Int32 iconResId;
    a->GetResourceId(
        Elastos::Droid::Launcher2::R::styleable::Favorite_icon, 0,
        &iconResId);
    Int32 titleResId;
    a->GetResourceId(
        Elastos::Droid::Launcher2::R::styleable::Favorite_title, 0,
        &titleResId);

    AutoPtr<IIntent> intent;
    String uri;
    //try {
    a->GetString(
        Elastos::Droid::Launcher2::R::styleable::Favorite_uri,
        &uri);
    ECode ec = Intent::ParseUri(uri, 0, (IIntent**)&intent);
    //} catch (URISyntaxException e) {
    if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
        Slogger::W(TAG, "Shortcut has malformed uri: " + uri);
        *result = -1; // Oh well
        return NOERROR;
    }
    //}

    if (iconResId == 0 || titleResId == 0) {
        Slogger::W(TAG, "Shortcut is missing title or icon resource ID");
        *result = -1;
        return NOERROR;
    }

    Int64 id;
    GenerateNewId(&id);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    String str;
    intent->ToUri(0, &str);
    values->Put(IFavorites::INTENT, str);
    String str2;
    r->GetString(titleResId, &str2);
    values->Put(IFavorites::TITLE, str2);
    values->Put(IFavorites::ITEM_TYPE, IFavorites::ITEM_TYPE_SHORTCUT);
    values->Put(IFavorites::SPANX, 1);
    values->Put(IFavorites::SPANY, 1);
    values->Put(IFavorites::ICON_TYPE, IFavorites::ICON_TYPE_RESOURCE);
    String packageName;
    mContext->GetPackageName(&packageName);
    values->Put(IFavorites::ICON_PACKAGE, packageName);
    String resourceName;
    r->GetResourceName(iconResId, &resourceName);
    values->Put(IFavorites::ICON_RESOURCE, resourceName);
    values->Put(IFavorites::_ID, id);

    Int64 num;
    DbInsertAndCheck(this, db, TABLE_FAVORITES, NULL, values, &num);
    if (num < 0) {
        *result = -1;
        return NOERROR;
    }
    *result = id;
    return NOERROR;
}

LauncherProvider::SqlArguments::SqlArguments(
    /* [in] */ IUri* url,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* args)
{
    Authority<IList> pathSegments;
    url->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    pathSegments->GetSize(&size);
    if (size == 1) {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&mTable);
        mWhere = where;
        mArgs = args;
    }
    else if (size != 2) {
        // throw new IllegalArgumentException("Invalid URI: " + url);
        Slogger::E("LauncherProvider::SqlArguments", "Invalid URI: ");
    }
    else if (!TextUtils::IsEmpty(where)) {
        // throw new UnsupportedOperationException("WHERE clause not supported: " + url);
        Slogger::E("LauncherProvider::SqlArguments", "WHERE clause not supported: ");
    }
    else {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&mTable);
        Int64 id;
        ContentUris::ParseId(url, &id);
        StringBuilder sb;
        sb += "_id=";
        sb += id;
        mWhere = sb.ToString();
        mArgs = NULL;
    }
}

LauncherProvider::SqlArguments::SqlArguments(
    /* [in] */ IUri* url)
{
    Authority<IList> pathSegments;
    url->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    pathSegments->GetSize(&size);
    if (size == 1) {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&mTable);
        mWhere = NULL;
        mArgs = NULL;
    }
    else {
        //throw new IllegalArgumentException("Invalid URI: " + url);
        Slogger::E("LauncherProvider::SqlArguments", "Invalid URI: ");
    }
}

AutoPtr<IUri> LauncherProvider::CONTENT_APPWIDGET_RESET_URI =
        Uri::Parse(String("content://") + ILauncherProvider::AUTHORITY + String("/appWidgetReset"));

const String LauncherProvider::TAG("Launcher.LauncherProvider");
const Boolean LauncherProvider::LOGD = FASLE;

const String LauncherProvider::DATABASE_NAME("launcher.db");

const Int32 LauncherProvider::DATABASE_VERSION = 13;

const String LauncherProvider::ACTION_APPWIDGET_DEFAULT_WORKSPACE_CONFIGURE(
        "com.android.launcher.action.APPWIDGET_DEFAULT_WORKSPACE_CONFIGURE");

CAR_INTERFACE_IMPL(LauncherProvider, ContentProvider, ILauncherProvider);

ECode LauncherProvider::GetCONTENT_APPWIDGET_RESET_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    *uri = CONTENT_APPWIDGET_RESET_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode LauncherProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CDatabaseHelper::AcquireSingleton((IDatabaseHelper**)&mOpenHelper);
    ILauncherApplication::Probe(context)->SetLauncherProvider(this);
    *result = TRUE;
    return NOERROR;
}

ECode LauncherProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    AutoPtr<SqlArguments> args = new SqlArguments(uri, NULL, NULL);
    if (TextUtils::IsEmpty(args->mWhere)) {
        StringBuilder sb;
        sb += "vnd.android.cursor.dir/";
        sb += args->mTable;
        return sb.ToString(type);
    } else {
        StringBuilder sb;
        sb += "vnd.android.cursor.item/";
        sb += args->mTable;
        return sb.ToString(type);
    }
    return NOERROR;
}

ECode LauncherProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ARRAYoF<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ARRAYoF<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(type);

    AutoPtr<SqlArguments> args = new SqlArguments(uri, selection, selectionArgs);
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    qb->SetTables(args->mTable);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> result;
    qb->Query(db, projection, args->mWhere, args->mArgs, NULL, NULL, sortOrder, (ICursor**)&result);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    result->SetNotificationUri(resolver, uri);
    *cursor = result;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode LauncherProvider::DbInsertAndCheck(
    /* [in] */ IDatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Boolean res;
    values->ContainsKey(LauncherSettings::Favorites::_ID, &res);
    if (!res) {
        //throw new RuntimeException("Error: attempting to add item without specifying an id")
        Slogger::E("LauncherProvider", "Error: attempting to add item without specifying an id");
        return E_RUNTIME_EXCEPTION;
    }
    return db->Insert(table, nullColumnHack, values, result);
}

ECode LauncherProvider::DeleteId(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 id)
{
    AutoPtr<IUri> uri = LauncherSettings::Favorites::GetContentUri(id, FALSE);
    AutoPtr<SqlArguments> args = new SqlArguments(uri, NULL, NULL);
    return db->Delete(args->mTable, args->mWhere, args->mArgs);
}

ECode LauncherProvider::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* initialValues,
    /* [out] */ IUri** outuri)
{
    VALIDATE_NOT_NULL(outuri);

    AutoPtr<SqlArguments> args = new SqlArguments(uri);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int64 rowId;
    DbInsertAndCheck(mOpenHelper, db, args->mTable, NULL, initialValues, &rowId);
    if (rowId <= 0) {
        *outuri = NULL;
        return NOERROR;
    }

    uri = ContentUris::WithAppendedId(uri, rowId);
    SendNotify(uri);

    *outuri = uri;
    REFCOUNT_ADD(*outuri);
    return NOERROR;
}

ECode LauncherProvider::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues*>* values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<SqlArguments> args = new SqlArguments(uri);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    db->BeginTransaction();
    //try {
    Int32 numValues = values->GetLength();
    for (Int32 i = 0; i < numValues; i++) {
        Int64 rowId;
        FAIL_GOTO(DbInsertAndCheck(mOpenHelper, db, args->mTable, NULL,
                (*values)[i], &rowId), FINALLY)
        if (rowId < 0) {
            *result = 0;
            return NOERROR;
        }
    }
    db->SetTransactionSuccessful();
    //} finally {
FINALLY:
    db->EndTransaction();
    //}

    SendNotify(uri);
    *result = values->GetLength();
    return NOERROR;
}

ECode LauncherProvider::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<SqlArguments> args = new SqlArguments(uri, selection, selectionArgs);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count;
    db->Delete(args->mTable, args->mWhere, args->mArgs, &count);
    if (count > 0) SendNotify(uri);

    *result = count;
    return NOERROR;
}

ECode LauncherProvider::Update(
    /* [in] */ IUri uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<SqlArguments> args = new SqlArguments(uri, selection, selectionArgs);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int32 count;
    db->Update(args->mTable, values, args->mWhere, args->mArgs, &count);
    if (count > 0) SendNotify(uri);

    *result = count;
    return NOERROR;
}

void LauncherProvider::SendNotify(
    /* [in] */ IUri* uri)
{
    String notify;
    uri->GetQueryParameter(PARAMETER_NOTIFY, &notify);
    Boolean res;
    if (notify.IsNull() || (notify.Equals(String( "true"), &res), res)) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        resolver->NotifyChange(uri, NULL);
    }
}

ECode LauncherProvider::GenerateNewId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(result);

    return mOpenHelper->GenerateNewId(id);
}

ECode LauncherProvider::LoadDefaultFavoritesIfNecessary(
    /* [in] */ Int32 origWorkspaceResId,
    /* [in] */ Boolean overridePrevious)
{
    synchronized(this) {
        String spKey;
        LauncherApplication::GetSharedPreferencesKey(&spKey);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<ISharedPreferences> sp;
        context->GetSharedPreferences(spKey, IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
        Boolean dbCreatedNoWorkspace;
        sp->GetBoolean(DB_CREATED_BUT_DEFAULT_WORKSPACE_NOT_LOADED, FALSE, &dbCreatedNoWorkspace);
        if (dbCreatedNoWorkspace || overridePrevious) {
            Int32 workspaceResId = origWorkspaceResId;

            // Use default workspace resource if none provided
            if (workspaceResId == 0) {
                sp->GetInt32(DEFAULT_WORKSPACE_RESOURCE_ID,
                    Elastos::Droid::Launcher2::R::xml::default_workspace,
                    &workspaceResId);
            }

            // Populate favorites table with initial favorites
            AutoPtr<ISharedPreferencesEditor> editor;
            sp->Edit((ISharedPreferencesEditor**)&editor);
            editor->Remove(DB_CREATED_BUT_DEFAULT_WORKSPACE_NOT_LOADED);
            if (origWorkspaceResId != 0) {
                editor->PutInt32(DEFAULT_WORKSPACE_RESOURCE_ID, origWorkspaceResId);
            }
            if (!dbCreatedNoWorkspace && overridePrevious) {
                if (LOGD) Slogger::D(TAG, "Clearing old launcher database");
                // Workspace has already been loaded, clear the database.
                DeleteDatabase();
            }
            AutoPtr<ISQLiteDatabase> db;
            mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
            mOpenHelper->LoadFavorites(db, workspaceResId);
            editor->Commit();
        }
    }
    return NOERROR;
}

ECode LauncherProvider::DeleteDatabase()
{
    // Are you sure? (y/n)
    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    String path;
    db->GetPath(&path);
    AutoPtr<IFile> dbFile;
    CFile::New(path, (IFile**)&dbFile);
    mOpenHelper->Close();
    Boolean res;
    dbFile->Exists(&res);
    if (res) {
        SQLiteDatabase::DeleteDatabase(dbFile);
    }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mOpenHelper = new DatabaseHelper(context);
    return NOERROR;
}

ECode LauncherProvider::BuildOrWhereString(
    /* [in] */ const String& column,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder selectWhere;
    for (Int32 i = values->GetLength() - 1; i >= 0; i--) {
        selectWhere += column;
        selectWhere += "=";
        selectWhere += (*values)[i];
        if (i > 0) {
            selectWhere += " OR ";
        }
    }
    return selectWhere.ToString(&result);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos