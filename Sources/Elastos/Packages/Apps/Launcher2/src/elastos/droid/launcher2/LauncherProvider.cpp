
#include "elastos/droid/launcher2/LauncherProvider.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/Utilities.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::CAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Database::Sqlite::ISQLiteStatement;
using Elastos::Droid::Database::Sqlite::ISQLiteProgram;
using Elastos::Droid::Database::Sqlite::ISQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::CSQLiteQueryBuilder;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabaseHelper;
using Elastos::Droid::Database::Sqlite::CSQLiteDatabaseHelper;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Slogger;

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

LauncherProvider::DatabaseHelper::DatabaseHelper()
    : mMaxId(-1)
{
    Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::DatabaseHelper()");
}

ECode LauncherProvider::DatabaseHelper::constructor(
    /* [in] */ IContext* context)
{
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::constructor 1");
    SQLiteOpenHelper::constructor(context, DATABASE_NAME, NULL, DATABASE_VERSION);
    mContext = context;
    CAppWidgetHost::New(context, ILauncher::APPWIDGET_HOST_ID, (IAppWidgetHost**)&mAppWidgetHost);
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::constructor 2");
    // In the case where neither onCreate nor onUpgrade gets called, we read the maxId from
    // the DB here
    if (mMaxId == -1) {
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::constructor 3");
        AutoPtr<ISQLiteDatabase> db;
        GetWritableDatabase((ISQLiteDatabase**)&db);
        InitializeMaxId(db, &mMaxId);
    }
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::constructor return");
    return NOERROR;
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
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::OnCreate 1");
    if (LOGD) Slogger::D(LauncherProvider::TAG, "creating new launcher database");

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
Slogger::E("LauncherProvider::DatabaseHelper", "=================DatabaseHelper::OnCreate return");
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
    Boolean res;
    return editor->Commit(&res);
}

ECode LauncherProvider::DatabaseHelper::ConvertDatabase(
    /* [in] */ ISQLiteDatabase* db,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (LOGD) Slogger::D(LauncherProvider::TAG, "converting database"
            " from an older format, but not onUpgrade");
    Boolean converted = FALSE;

    StringBuilder sb;
    sb += "content://";
    sb += ISettings::AUTHORITY;
    sb += "/old_favorites?notify=true";

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(sb.ToString(), (IUri**)&uri);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;

    //try {
    resolver->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    //} catch (Exception e) {
        // Ignore
    //}

    // We already have a favorites database in the old provider
    if (cursor != NULL) {
        Int32 count;
        cursor->GetCount(&count);
        if (count > 0) {
            //try {
            Int32 num;
            CopyFromCursor(db, cursor, &num);
            converted = num > 0;
            //} finally {
            ICloseable::Probe(cursor)->Close();
            //}
            if (converted) {
                Int32 value;
                resolver->Delete(uri, String(NULL), NULL, &value);
            }
        }
    }

    if (converted) {
        // Convert widgets from this import into widgets
        if (LOGD) Slogger::D(LauncherProvider::TAG, "converted and now"
                " triggering widget upgrade");
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
    c->GetColumnIndexOrThrow(IBaseColumns::ID, &idIndex);
    Int32 intentIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::INTENT, &intentIndex);
    Int32 titleIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::TITLE, &titleIndex);
    Int32 iconTypeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_TYPE, &iconTypeIndex);
    Int32 iconIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON, &iconIndex);
    Int32 iconPackageIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_PACKAGE, &iconPackageIndex);
    Int32 iconResourceIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ICON_RESOURCE, &iconResourceIndex);
    Int32 containerIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CONTAINER, &containerIndex);
    Int32 itemTypeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::ITEM_TYPE, &itemTypeIndex);
    Int32 screenIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::SCREEN, &screenIndex);
    Int32 cellXIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLX, &cellXIndex);
    Int32 cellYIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::CELLY, &cellYIndex);
    Int32 uriIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::URI, &uriIndex);
    Int32 displayModeIndex;
    c->GetColumnIndexOrThrow(LauncherSettings::Favorites::DISPLAY_MODE, &displayModeIndex);

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
        values->Put(IBaseColumns::ID, columnValue);

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
        ECode ec = DbInsertAndCheck(this, db, TABLE_FAVORITES, String(NULL), (*rows)[i], &res);
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
    if (LOGD) Slogger::D(LauncherProvider::TAG, "onUpgrade triggered");

    Int32 version = oldVersion;
    if (version < 3) {
        // upgrade 1,2 -> 3 added appWidgetId column
        db->BeginTransaction();
        //try {
            // Insert new column for holding appWidgetIds
        String str("ALTER TABLE favorites ADD COLUMN appWidgetId INTEGER NOT NULL DEFAULT -1;");
        ECode ec = db->ExecSQL(str);
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
            return db->EndTransaction();
        }
        ec = db->SetTransactionSuccessful();
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
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
            Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
            return db->EndTransaction();
        }
        ec = db->SetTransactionSuccessful();
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
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
        if (res) {
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
        Int32 tmp;
        LoadFavorites(db,
                Elastos::Droid::Launcher2::R::xml::update_workspace, &tmp);
        version = 9;
    }

    // We bumped the version three time during JB, once to update the launch flags, once to
    // update the override for the default launch animation and once to set the mimetype
    // to improve startup performance
    if (version < 12) {
        // Contact shortcuts need a different set of flags to be launched now
        // The updateContactsShortcuts change is idempotent, so we can keep using it like
        // back in the Donut days
        Boolean res;
        UpdateContactsShortcuts(db, &res);
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
        Slogger::W(LauncherProvider::TAG, "Destroying all old data.");
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
    sb += ";";
    ECode ec = db->ExecSQL(sb.ToString());
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
        *result = FALSE;
        return db->EndTransaction();
    }
    ec = db->SetTransactionSuccessful();
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::E(LauncherProvider::TAG, "ex.getMessage(), ex");
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
    (*array)[0] = ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT;
    String selectWhere;
    BuildOrWhereString(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE, array, &selectWhere);

    AutoPtr<ICursor> c;
    String actionQuickContact("com.android.contacts.action.QUICK_CONTACT");
    db->BeginTransaction();
    //try
    ECode ec;
    {
        // Select and iterate through each matching widget
        AutoPtr<ArrayOf<String> > stringArray = ArrayOf<String>::Alloc(2);
        (*stringArray)[0] = IBaseColumns::ID;
        (*stringArray)[1] = ILauncherSettingsBaseLauncherColumns::INTENT;
        FAIL_GOTO(ec = db->Query(TABLE_FAVORITES, stringArray,
                selectWhere, NULL, String(NULL), String(NULL), String(NULL),
                (ICursor**)&c), ERROR2)

        if (c == NULL) {
            *result = FALSE;
            db->EndTransaction();
            return NOERROR;
        }

        Int32 count;
        FAIL_GOTO(ec = c->GetCount(&count), ERROR2)
        if (LOGD) Slogger::D(LauncherProvider::TAG, "found upgrade cursor count=%d", count);

        Int32 idIndex;
        FAIL_GOTO(ec = c->GetColumnIndex(IBaseColumns::ID, &idIndex), ERROR2)
        Int32 intentIndex;
        FAIL_GOTO(ec = c->GetColumnIndex(ILauncherSettingsBaseLauncherColumns::INTENT,
                &intentIndex), ERROR2)

        Boolean res;
        while (c->MoveToNext(&res), res) {
            Int64 favoriteId;
            FAIL_GOTO(ec = c->GetInt64(idIndex, &favoriteId), ERROR2)
            String intentUri;
            FAIL_GOTO(ec = c->GetString(intentIndex, &intentUri), ERROR2)
            if (!intentUri.IsNull()) {
                //try
                {
                    AutoPtr<IIntentHelper> helper;
                    CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
                    AutoPtr<IIntent> intent;
                    helper->ParseUri(intentUri, 0, (IIntent**)&intent);
                    String str;
                    intent->ToString(&str);
                    Slogger::D("Home", str);
                    AutoPtr<IUri> uri;
                    FAIL_GOTO(ec =intent->GetData((IUri**)&uri), ERROR1)
                    if (uri != NULL) {
                        String data;
                        IObject::Probe(uri)->ToString(&data);
                        String action;
                        FAIL_GOTO(ec =intent->GetAction(&action), ERROR1)
                        if ((action.Equals(IIntent::ACTION_VIEW) || actionQuickContact.Equals(action))
                                    && (data.StartWith(String("content://contacts/people/"))
                                    || data.StartWith(String("content://com.android.contacts/contacts/lookup/")))) {
                            AutoPtr<IIntent> newIntent;
                            CIntent::New(actionQuickContact, (IIntent**)&newIntent);
                            // When starting from the launcher, start in a new, cleared task
                            // CLEAR_WHEN_TASK_RESET cannot reset the root of a task, so we
                            // clear the whole thing preemptively here since
                            // QuickContactActivity will finish itself when launching other
                            // detail activities.
                            FAIL_GOTO(ec = newIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                                    IIntent::FLAG_ACTIVITY_CLEAR_TASK), ERROR1)
                            FAIL_GOTO(ec = newIntent->PutExtra(
                                    ILauncher::INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION, TRUE), ERROR1)
                            FAIL_GOTO(ec = newIntent->SetData(uri), ERROR1)
                            // Determine the type and also put that in the shortcut
                            // (that can speed up launch a bit)
                            String type;
                            FAIL_GOTO(ec = newIntent->ResolveType(mContext, &type), ERROR1)
                            FAIL_GOTO(ec = newIntent->SetDataAndType(uri, type), ERROR1)

                            AutoPtr<IContentValues> values;
                            CContentValues::New((IContentValues**)&values);
                            String str;
                            FAIL_GOTO(ec = newIntent->ToURI(&str), ERROR1)
                            FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::INTENT,
                                    str), ERROR1)

                            StringBuilder sb;
                            sb += IBaseColumns::ID;
                            sb += "=";
                            sb += favoriteId;
                            String updateWhere = sb.ToString();
                            Int32 tmp;
                            FAIL_GOTO(ec = db->Update(TABLE_FAVORITES, values, updateWhere,
                                    NULL, &tmp), ERROR1)
                        }
                    }
                }
            ERROR1:
                //} catch (RuntimeException ex) {
                if (ec == (ECode)E_RUNTIME_EXCEPTION) {
                    Slogger::E("LauncherProvider", "Problem upgrading shortcut %d", ec);
                }
                //} catch (URISyntaxException e) {
                if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
                    Slogger::E("LauncherProvider", "Problem upgrading shortcut %d", ec);
                }
                //}
            }
        }
        FAIL_GOTO(ec = db->SetTransactionSuccessful(), ERROR2)
    }

ERROR2:
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W("LauncherProvider", "Problem while upgrading contacts %d", ec);
        db->EndTransaction();
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
        *result = FALSE;
        return NOERROR;
    }
    // } catch (SQLException ex) {
    //     Log.w(TAG, "Problem while upgrading contacts", ex);
    //     return false;
    // } finally {
    db->EndTransaction();
    if (c != NULL) {
        ICloseable::Probe(c)->Close();
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::NormalizeIcons(
    /* [in] */ ISQLiteDatabase* db)
{
    Slogger::D(LauncherProvider::TAG, "normalizing icons");
    db->BeginTransaction();
    AutoPtr<ICursor> c;
    AutoPtr<ISQLiteStatement> update;
    //try
    ECode ec;
    {
        Boolean logged = FALSE;
        StringBuilder sb;
        Int32 idIndex;
        Int32 iconIndex;

        FAIL_GOTO(ec = db->CompileStatement(String("UPDATE favorites SET icon=? WHERE _id=?"),
                (ISQLiteStatement**)&update), ERROR2)
        sb += "SELECT _id, icon FROM favorites WHERE iconType=";
        sb += ILauncherSettingsBaseLauncherColumns::ICON_TYPE_BITMAP;
        FAIL_GOTO(ec = db->RawQuery(sb.ToString(), NULL, (ICursor**)&c), ERROR2)
        FAIL_GOTO(ec = c->GetColumnIndexOrThrow(IBaseColumns::ID, &idIndex), ERROR2)
        FAIL_GOTO(ec = c->GetColumnIndexOrThrow(ILauncherSettingsBaseLauncherColumns::ICON,
                &iconIndex), ERROR2)

        Boolean res;
        while (c->MoveToNext(&res), res) {
            Int64 id;
            FAIL_GOTO(ec = c->GetInt64(idIndex, &id), ERROR2)
            AutoPtr<ArrayOf<Byte> > data;
            FAIL_GOTO(ec = c->GetBlob(iconIndex, (ArrayOf<Byte>**)&data), ERROR2)
            //try
            {
                AutoPtr<IBitmapFactory> factory;
                CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
                AutoPtr<IBitmap> _bitmap;
                AutoPtr<IBitmap> bitmap;
                FAIL_GOTO(factory->DecodeByteArray(data, 0, data->GetLength(),
                        (IBitmap**)&_bitmap), ERROR1)

                bitmap = Utilities::ResampleIconBitmap(_bitmap, mContext);
                if (bitmap != NULL) {
                    FAIL_GOTO(ISQLiteProgram::Probe(update)->BindInt64(1, id), ERROR1)
                    data = ItemInfo::FlattenBitmap(bitmap);
                    if (data != NULL) {
                        FAIL_GOTO(ISQLiteProgram::Probe(update)->BindBlob(2, *data), ERROR1)
                        FAIL_GOTO(update->Execute(), ERROR1)
                    }
                    FAIL_GOTO(bitmap->Recycle(), ERROR1)
                }
            }
            //} catch (Exception e) {
        ERROR1:
            if (!logged) {
                Slogger::E(LauncherProvider::TAG, "Failed normalizing icon " /*+ id, e*/);
            }
            else {
                Slogger::E(LauncherProvider::TAG, "Also failed normalizing icon "/* + id*/);
            }
            logged = TRUE;
        //}
        }

        ec = db->SetTransactionSuccessful();

    ERROR2:
        if (ec == (ECode)E_SQL_EXCEPTION) {
            Slogger::W(LauncherProvider::TAG,
                    "Problem while allocating appWidgetIds for existing widgets %d", ec);
        }
    }
    //} catch (SQLException ex) {
        //Slogger::W(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
    //} finally {
    db->EndTransaction();
    if (update != NULL) {
        ICloseable::Probe(update)->Close();
    }
    if (c != NULL) {
        ICloseable::Probe(c)->Close();
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
        Slogger::E(LauncherProvider::TAG,
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
        ICloseable::Probe(c)->Close();
    }

    if (id == -1) {
        //throw new RuntimeException("Error: could not query max id");
        Slogger::E(LauncherProvider::TAG,
                "Error: could not query max id");
        return E_RUNTIME_EXCEPTION;
    }

    *maxid = id;
    return NOERROR;
}

ECode LauncherProvider::DatabaseHelper::ConvertWidgets(
    /* [in] */ ISQLiteDatabase* db)
{
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> appWidgetManager;
    helper->GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);

    AutoPtr<ArrayOf<Int32> > bindSources = ArrayOf<Int32>::Alloc(3);
    (*bindSources)[0] = LauncherSettings::Favorites::ITEM_TYPE_WIDGET_CLOCK;
    (*bindSources)[1] = LauncherSettings::Favorites::ITEM_TYPE_WIDGET_PHOTO_FRAME;
    (*bindSources)[2] = LauncherSettings::Favorites::ITEM_TYPE_WIDGET_SEARCH;

    String selectWhere;
    BuildOrWhereString(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE, bindSources, &selectWhere);

    AutoPtr<ICursor> c;

    db->BeginTransaction();
    //try
    ECode ec;
    {
        // Select and iterate through each matching widget
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
        (*array)[0] = IBaseColumns::ID;
        (*array)[1] = ILauncherSettingsBaseLauncherColumns::ITEM_TYPE;
        FAIL_GOTO(ec = db->Query(TABLE_FAVORITES, array,
                selectWhere, NULL, String(NULL), String(NULL), String(NULL),
                (ICursor**)&c), ERROR2)

        if (LOGD) Slogger::D("LauncherProvider", "found upgrade cursor count=" /*+ c.getCount()*/);

        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        Boolean res;
        while (c != NULL && (c->MoveToNext(&res), res)) {
            Int64 favoriteId;
            FAIL_GOTO(ec = c->GetInt64(0, &favoriteId), ERROR2)
            Int32 favoriteType;
            FAIL_GOTO(ec = c->GetInt32(1, &favoriteType), ERROR2)

            // Allocate and update database with new appWidgetId
            //try
            {
                Int32 appWidgetId;
                FAIL_GOTO(ec = mAppWidgetHost->AllocateAppWidgetId(&appWidgetId), ERROR1)

                if (LOGD) {
                    StringBuilder sb;
                    sb += "allocated appWidgetId=";
                    sb += appWidgetId;
                    sb += " for favoriteId=";
                    sb += favoriteId;
                    Slogger::D("LauncherProvider", sb.ToString());
                }
                FAIL_GOTO(ec = values->Clear(), ERROR1)
                FAIL_GOTO(ec = values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE,
                        LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET), ERROR1)
                FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::APPWIDGET_ID,
                        appWidgetId), ERROR1)

                // Original widgets might not have valid spans when upgrading
                if (favoriteType == LauncherSettings::Favorites::ITEM_TYPE_WIDGET_SEARCH) {
                    FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANX, 4), ERROR1)
                    FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANY, 1), ERROR1)
                }
                else {
                    FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANX, 2), ERROR1)
                    FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANY, 2), ERROR1)
                }

                StringBuilder sb;
                sb += IBaseColumns::ID;
                sb += "=";
                sb += favoriteId;
                String updateWhere;
                sb.ToString(&updateWhere);
                Int32 tmp;
                FAIL_GOTO(ec = db->Update(TABLE_FAVORITES, values, updateWhere, NULL, &tmp), ERROR1)

                if (favoriteType == LauncherSettings::Favorites::ITEM_TYPE_WIDGET_CLOCK) {
                    // TODO: check return value
                    AutoPtr<IComponentName> name;
                    CComponentName::New(String("com.android.alarmclock"),
                            String("com.android.alarmclock.AnalogAppWidgetProvider"),
                            (IComponentName**)&name);
                    Boolean res;
                    FAIL_GOTO(ec = appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name,
                            &res), ERROR1)
                }
                else if (favoriteType == LauncherSettings::Favorites::ITEM_TYPE_WIDGET_PHOTO_FRAME) {
                    // TODO: check return value
                    AutoPtr<IComponentName> name;
                    CComponentName::New(String("com.android.camera"),
                            String("com.android.camera.PhotoAppWidgetProvider"),
                            (IComponentName**)&name);
                    Boolean res;
                    FAIL_GOTO(ec = appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name,
                            &res), ERROR1)
                }
                else if (favoriteType == LauncherSettings::Favorites::ITEM_TYPE_WIDGET_SEARCH) {
                    // TODO: check return value
                    AutoPtr<IComponentName> name;
                    GetSearchWidgetProvider((IComponentName**)&name);
                    Boolean res;
                    FAIL_GOTO(ec = appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, name,
                            &res), ERROR1)
                }
            }
            //} catch (RuntimeException ex) {
        ERROR1:
                if (ec == (ECode)E_SQL_EXCEPTION) {
                    Slogger::W("LauncherProvider", "Problem while allocating appWidgetIds for existing widgets");
                }
            //}
        }

        FAIL_GOTO(ec = db->SetTransactionSuccessful(), ERROR2)
    }
    //} catch (SQLException ex) {
ERROR2:
    if (ec == (ECode)E_SQL_EXCEPTION) {
        Slogger::W("LauncherProvider", "Problem while allocating appWidgetIds for existing widgets");
    }
    //    Log.w(TAG, "Problem while allocating appWidgetIds for existing widgets", ex);
    //} finally {
    db->EndTransaction();
    if (c != NULL) {
        return ICloseable::Probe(c)->Close();
    }
    //}
    return NOERROR;
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
        Slogger::E(LauncherProvider::TAG, "No start tag found");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    String name;
    parser->GetName(&name);
    if (!name.Equals(firstElementName)) {
        // throw new XmlPullParserException("Unexpected start tag: found " + parser.getName() +
        //         ", expected " + firstElementName);
        String name;
        parser->GetName(&name);
        StringBuilder sb;
        sb += "Unexpected start tag: found ";
        sb += name;
        sb += ", expected ";
        sb += firstElementName;
        Slogger::E("LauncherProvider::DatabaseHelper", sb.ToString());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
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
    //try
    ECode ec;
    {
        AutoPtr<IXmlResourceParser> parser;
        FAIL_GOTO(ec = resources->GetXml(workspaceResourceId,
                (IXmlResourceParser**)&parser), ERROR1)
        AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));
        BeginDocument(IXmlPullParser::Probe(parser), TAG_FAVORITES);

        Int32 depth;
        IXmlPullParser::Probe(parser)->GetDepth(&depth);

        Int32 type;
        Int32 _depth;
        while (((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_TAG ||
                (IXmlPullParser::Probe(parser)->GetDepth(&_depth), _depth) > depth) &&
                type != IXmlPullParser::END_DOCUMENT) {

            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            Boolean added = FALSE;
            String name;
            IXmlPullParser::Probe(parser)->GetName(&name);

            AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::Favorite),
                ArraySize(Elastos::Droid::Launcher2::R::styleable::Favorite));
            AutoPtr<ITypedArray> a;
            FAIL_GOTO(ec = mContext->ObtainStyledAttributes(attrs, attrIds,
                    (ITypedArray**)&a), ERROR1)

            Int64 container = LauncherSettings::Favorites::CONTAINER_DESKTOP;
            Boolean res;
            FAIL_GOTO(ec = a->HasValue(Elastos::Droid::Launcher2::R::styleable::Favorite_container,
                    &res), ERROR1)
            if (res) {
                String str;
                FAIL_GOTO(ec = a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_container,
                        &str), ERROR1)
                container = StringUtils::ParseInt64(str);
            }

            String screen;
            FAIL_GOTO(ec = a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_screen,
                    &screen), ERROR1)
            String x;
            FAIL_GOTO(ec = a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_x,
                    &x), ERROR1)
            String y;
            FAIL_GOTO(ec = a->GetString(Elastos::Droid::Launcher2::R::styleable::Favorite_y,
                    &y), ERROR1)

            // If we are adding to the hotseat, the screen is used as the position in the
            // hotseat. This screen can't be at position 0 because AllApps is in the
            // zeroth position.
            if (container == LauncherSettings::Favorites::CONTAINER_HOTSEAT
                    && StringUtils::ParseInt32(screen) == allAppsButtonRank) {
                //throw new RuntimeException("Invalid screen position for hotseat item");
                Slogger::E(LauncherProvider::TAG, "Invalid screen position for hotseat item");
                return E_RUNTIME_EXCEPTION;
            }

            FAIL_GOTO(ec = values->Clear(), ERROR1)
            FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::CONTAINER, container), ERROR1)
            FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SCREEN, screen), ERROR1)
            FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::CELLX, x), ERROR1)
            FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::CELLY, y), ERROR1)

            if (TAG_FAVORITE.Equals(name)) {
                Int64 id;
                AddAppShortcut(db, values, a, packageManager, intent, &id);
                added = id >= 0;
            }
            else if (TAG_SEARCH.Equals(name)) {
                AddSearchWidget(db, values, &added);
            }
            else if (TAG_CLOCK.Equals(name)) {
                AddClockWidget(db, values, &added);
            }
            else if (TAG_APPWIDGET.Equals(name)) {
                AddAppWidget(parser, attrs, type, db, values, a, packageManager, &added);
            }
            else if (TAG_SHORTCUT.Equals(name)) {
                Int64 id;
                AddUriShortcut(db, values, a, &id);
                added = id >= 0;
            }
            else if (TAG_FOLDER.Equals(name)) {
                String title;
                Int32 titleResId;
                FAIL_GOTO(ec = a->GetResourceId(Elastos::Droid::Launcher2::R::styleable::Favorite_title, -1,
                        &titleResId), ERROR1)
                AutoPtr<IResources> resources;
                FAIL_GOTO(ec = mContext->GetResources((IResources**)&resources), ERROR1)
                if (titleResId != -1) {
                    FAIL_GOTO(ec = resources->GetString(titleResId, &title), ERROR1)
                }
                else {
                    FAIL_GOTO(ec = resources->GetString(
                            Elastos::Droid::Launcher2::R::string::folder_name,
                            &title), ERROR1)
                }
                FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::TITLE, title), ERROR1)
                Int64 folderId;
                AddFolder(db, values, &folderId);
                added = folderId >= 0;

                AutoPtr<IArrayList> folderItems;
                CArrayList::New((IArrayList**)&folderItems);

                Int32 folderDepth;
                IXmlPullParser::Probe(parser)->GetDepth(&folderDepth);
                Int32 dep;
                while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_TAG ||
                        (IXmlPullParser::Probe(parser)->GetDepth(&dep), dep) > folderDepth) {
                    if (type != IXmlPullParser::START_TAG) {
                        continue;
                    }
                    String folder_item_name;
                    IXmlPullParser::Probe(parser)->GetName(&folder_item_name);

                    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::Favorite),
                            ArraySize(Elastos::Droid::Launcher2::R::styleable::Favorite));
                    AutoPtr<ITypedArray> ar;
                    FAIL_GOTO(ec = mContext->ObtainStyledAttributes(attrs, attrIds,
                            (ITypedArray**)&ar), ERROR1)
                    FAIL_GOTO(ec = values->Clear(), ERROR1)
                    FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::CONTAINER, folderId), ERROR1)

                    if ((TAG_FAVORITE.Equals(folder_item_name))
                            && folderId >= 0) {
                        Int64 id;
                        AddAppShortcut(db, values, ar, packageManager, intent, &id);
                        if (id >= 0) {
                            AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
                            folderItems->Add(TO_IINTERFACE(obj));
                        }
                    }
                    else if ((TAG_SHORTCUT.Equals(folder_item_name))
                            && folderId >= 0) {
                        Int64 id;
                        AddUriShortcut(db, values, ar, &id);
                        if (id >= 0) {
                            AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
                            folderItems->Add(TO_IINTERFACE(obj));
                        }
                    }
                    else {
                        // throw new RuntimeException("Folders can " +
                        //         "contain only shortcuts");
                        Slogger::E(LauncherProvider::TAG, "Folders can contain only shortcuts");
                        return E_RUNTIME_EXCEPTION;
                    }
                    FAIL_GOTO(ec = ar->Recycle(), ERROR1)
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
                        AutoPtr<IInterface> obj;
                        folderItems->Get(0, (IInterface**)&obj);
                        AutoPtr<IInteger64> value = IInteger64::Probe(obj);
                        Int64 id;
                        value->GetValue(&id);
                        DeleteId(db, id);
                    }
                    added = FALSE;
                }
            }
            if (added) i++;
            FAIL_GOTO(ec = a->Recycle(), ERROR1)
        }
    }
ERROR1 :
    //} catch (XmlPullParserException e) {
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::W("LauncherProvider", "Got exception parsing favorites. %d", ec);
    }
    //} catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W("LauncherProvider", "Got exception parsing favorites. %d", ec);
    }
    //} catch (RuntimeException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Slogger::W("LauncherProvider", "Got exception parsing favorites. %d", ec);
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
        Elastos::Droid::Launcher2::R::styleable::Favorite_packageName,
        &packageName);
    String className;
    a->GetString(
        Elastos::Droid::Launcher2::R::styleable::Favorite_className,
        &className);
    //try
    ECode ec;
    {
        AutoPtr<IComponentName> cn;
        //try {
        CComponentName::New(packageName, className, (IComponentName**)&cn);
        ec = packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info);
        //} catch (PackageManager.NameNotFoundException nnfe) {
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = packageName;
            AutoPtr<ArrayOf<String> > packages;
            packageManager->CurrentToCanonicalPackageNames(*array, (ArrayOf<String>**)&packages);
            CComponentName::New((*packages)[0], className, (IComponentName**)&cn);
            packageManager->GetActivityInfo(cn, 0, (IActivityInfo**)&info);
        }
        //}
        GenerateNewId(&id);
        FAIL_GOTO(ec = intent->SetComponent(cn), ERROR)
        FAIL_GOTO(ec = intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED), ERROR)
        String str;
        FAIL_GOTO(ec = intent->ToUri(0, &str), ERROR)
        FAIL_GOTO(ec = values->Put(ILauncherSettingsBaseLauncherColumns::INTENT, str), ERROR)

        AutoPtr<ICharSequence> label;
        IPackageItemInfo::Probe(info)->LoadLabel(packageManager, (ICharSequence**)&label);
        String str2;
        label->ToString(&str2);
        FAIL_GOTO(ec = values->Put(ILauncherSettingsBaseLauncherColumns::TITLE, str2), ERROR)
        FAIL_GOTO(ec = values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE,
                ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_APPLICATION), ERROR)
        FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANX, 1), ERROR)
        FAIL_GOTO(ec = values->Put(LauncherSettings::Favorites::SPANY, 1), ERROR)
        Int64 _id;
        GenerateNewId(&_id);
        FAIL_GOTO(ec = values->Put(IBaseColumns::ID, _id), ERROR)
        Int64 num;
        DbInsertAndCheck(this, db, TABLE_FAVORITES, String(NULL), values, &num);
        if (num < 0) {
            *result = -1;
            return NOERROR;
        }
    }

ERROR:
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        StringBuilder sb;
        sb += "Unable to add favorite: ";
        sb += packageName;
        sb += "/";
        sb += className;
        sb += ec;
        Slogger::W("LauncherProvider", sb.ToString());
    }
    //catch (PackageManager.NameNotFoundException e) {
        // Slogger:::W(TAG, "Unable to add favorite: " + packageName +
        //         "/" + className, e);
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

    values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE,
            LauncherSettings::Favorites::ITEM_TYPE_FOLDER);
    values->Put(LauncherSettings::Favorites::SPANX, 1);
    values->Put(LauncherSettings::Favorites::SPANY, 1);
    Int64 id;
    GenerateNewId(&id);
    values->Put(IBaseColumns::ID, id);
    Int64 num;
    DbInsertAndCheck(this, db, TABLE_FAVORITES, String(NULL), values, &num);
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
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);
    AutoPtr<IComponentName> searchComponent;
    searchManager->GetGlobalSearchActivity((IComponentName**)&searchComponent);
    if (searchComponent == NULL) {
        *name = NULL;
        return NOERROR;
    }
    String packageName;
    searchComponent->GetPackageName(&packageName);
    return GetProviderInPackage(packageName, name);
}

ECode LauncherProvider::DatabaseHelper::GetProviderInPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> appWidgetManager;
    helper->GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);
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
        info->GetProvider((IComponentName**)&provider);
        if (provider != NULL) {
            String _packageName;
            provider->GetPackageName(&_packageName);
            if (_packageName.Equals(packageName)) {
                *name = provider;
                REFCOUNT_ADD(*name)
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
    AutoPtr<IActivityInfo> tmp;
    ECode ec = packageManager->GetReceiverInfo(cn, 0, (IActivityInfo**)&tmp);
    //} catch (Exception e) {
    if (ec != NOERROR) {
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
        (*array)[0] = packageName;
        AutoPtr<ArrayOf<String> > packages;
        packageManager->CurrentToCanonicalPackageNames(*array,
                (ArrayOf<String>**)&packages);
        CComponentName::New((*packages)[0], className, (IComponentName**)&cn);

        //try {
        AutoPtr<IActivityInfo> tmp2;
        if(FAILED(packageManager->GetReceiverInfo(cn, 0, (IActivityInfo**)&tmp2))) {
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
        IXmlPullParser::Probe(parser)->GetDepth(&widgetDepth);
        Int32 dep;
        while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_TAG ||
                (IXmlPullParser::Probe(parser)->GetDepth(&dep), dep) > widgetDepth) {
            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                    const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::Extra),
                    ArraySize(Elastos::Droid::Launcher2::R::styleable::Extra));
            AutoPtr<ITypedArray> ar;
            mContext->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&ar);
            String name;
            IXmlPullParser::Probe(parser)->GetName(&name);
            if (TAG_EXTRA.Equals(name)) {
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
                    Slogger::E(LauncherProvider::TAG, "Widget extras must have a key and value");
                    return E_RUNTIME_EXCEPTION;
                }
            }
            else {
                //throw new RuntimeException("Widgets can contain only extras");
                Slogger::E(LauncherProvider::TAG, "Widgets can contain only extras");
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

    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    AutoPtr<IAppWidgetManager> appWidgetManager;
    helper->GetInstance(mContext, (IAppWidgetManager**)&appWidgetManager);

    //try {
    Int32 appWidgetId;
    ECode ec = mAppWidgetHost->AllocateAppWidgetId(&appWidgetId);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;

    values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE,
            LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET);
    values->Put(LauncherSettings::Favorites::SPANX, spanX);
    values->Put(LauncherSettings::Favorites::SPANY, spanY);
    values->Put(LauncherSettings::Favorites::APPWIDGET_ID, appWidgetId);
    Int64 _id;
    GenerateNewId(&_id);
    values->Put(IBaseColumns::ID, _id);
    Int64 tmp;
    DbInsertAndCheck(this, db, TABLE_FAVORITES, String(NULL), values, &tmp);

    allocatedAppWidgets = TRUE;

    // TODO: need to check return value
    Boolean res;
    ec = appWidgetManager->BindAppWidgetIdIfAllowed(appWidgetId, cn, &res);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) goto ERROR;

    // Send a broadcast to configure the widget
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
        Slogger::E(LauncherProvider::TAG, "Problem allocating appWidgetId %d", ec);
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
    AutoPtr<IIntentHelper> helper;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
    ECode ec = helper->ParseUri(uri, 0, (IIntent**)&intent);
    //} catch (URISyntaxException e) {
    if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
        Slogger::W(LauncherProvider::TAG, "Shortcut has malformed uri: "/* + uri*/);
        *result = -1; // Oh well
        return NOERROR;
    }
    //}

    if (iconResId == 0 || titleResId == 0) {
        Slogger::W(LauncherProvider::TAG, "Shortcut is missing title or icon resource ID");
        *result = -1;
        return NOERROR;
    }

    Int64 id;
    GenerateNewId(&id);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    String str;
    intent->ToUri(0, &str);
    values->Put(ILauncherSettingsBaseLauncherColumns::INTENT, str);
    String str2;
    r->GetString(titleResId, &str2);
    values->Put(ILauncherSettingsBaseLauncherColumns::TITLE, str2);
    values->Put(ILauncherSettingsBaseLauncherColumns::ITEM_TYPE,
            ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT);
    values->Put(LauncherSettings::Favorites::SPANX, 1);
    values->Put(LauncherSettings::Favorites::SPANY, 1);
    values->Put(ILauncherSettingsBaseLauncherColumns::ICON_TYPE,
            ILauncherSettingsBaseLauncherColumns::ICON_TYPE_RESOURCE);
    String packageName;
    mContext->GetPackageName(&packageName);
    values->Put(ILauncherSettingsBaseLauncherColumns::ICON_PACKAGE, packageName);
    String resourceName;
    r->GetResourceName(iconResId, &resourceName);
    values->Put(ILauncherSettingsBaseLauncherColumns::ICON_RESOURCE, resourceName);
    values->Put(IBaseColumns::ID, id);

    Int64 num;
    DbInsertAndCheck(this, db, TABLE_FAVORITES, String(NULL), values, &num);
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
    AutoPtr<IList> pathSegments;
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
        Slogger::E(LauncherProvider::TAG, "Invalid URI: ");
    }
    else if (!TextUtils::IsEmpty(where)) {
        // throw new UnsupportedOperationException("WHERE clause not supported: " + url);
        Slogger::E(LauncherProvider::TAG, "WHERE clause not supported: ");
    }
    else {
        AutoPtr<IInterface> obj;
        pathSegments->Get(0, (IInterface**)&obj);
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
        cchar->ToString(&mTable);

        AutoPtr<IContentUris> helper;
        CContentUris::AcquireSingleton((IContentUris**)&helper);
        AutoPtr<IUri> uri;
        Int64 id;
        helper->ParseId(url, &id);

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
    AutoPtr<IList> pathSegments;
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
        Slogger::E(LauncherProvider::TAG, "Invalid URI: ");
    }
}

static AutoPtr<IUri> initCONTENT_APPWIDGET_RESET_URI()
{
    AutoPtr<IUri> uri;

    StringBuilder sb;
    sb += "content://";
    sb += ILauncherProvider::AUTHORITY;
    sb += "/appWidgetReset";

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    uriHelper->Parse(sb.ToString(), (IUri**)&uri);

    return uri;
}

const AutoPtr<IUri> LauncherProvider::CONTENT_APPWIDGET_RESET_URI = initCONTENT_APPWIDGET_RESET_URI();

const String LauncherProvider::TAG("Launcher.LauncherProvider");
const Boolean LauncherProvider::LOGD = FALSE;

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

LauncherProvider::LauncherProvider()
{
    Slogger::E("LauncherProvider", "============================LauncherProvider::LauncherProvider()");
}

ECode LauncherProvider::constructor()
{
Slogger::E("LauncherProvider", "============================LauncherProvider::constructor");
    return ContentProvider::constructor();
Slogger::E("LauncherProvider", "============================LauncherProvider::constructor return");
}

ECode LauncherProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
Slogger::E("LauncherProvider", "==================LauncherProvider::OnCreate 1");
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
Slogger::E("LauncherProvider", "==================LauncherProvider::OnCreate 2");
    mOpenHelper = new DatabaseHelper();
Slogger::E("LauncherProvider", "==================LauncherProvider::OnCreate 3");
    mOpenHelper->constructor(context);
Slogger::E("LauncherProvider", "==================LauncherProvider::OnCreate 4");
    ILauncherApplication::Probe(context)->SetLauncherProvider(this);
    *result = TRUE;
Slogger::E("LauncherProvider", "==================LauncherProvider::OnCreate 5");
    return NOERROR;
}

ECode LauncherProvider::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);

    AutoPtr<SqlArguments> args = new SqlArguments(uri, String(NULL), NULL);
    if (TextUtils::IsEmpty(args->mWhere)) {
        StringBuilder sb;
        sb += "vnd.android.cursor.dir/";
        sb += args->mTable;
        return sb.ToString(type);
    }
    else {
        StringBuilder sb;
        sb += "vnd.android.cursor.item/";
        sb += args->mTable;
        return sb.ToString(type);
    }
    return NOERROR;
}

ECode LauncherProvider::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);

    AutoPtr<SqlArguments> args = new SqlArguments(uri, selection, selectionArgs);
    AutoPtr<ISQLiteQueryBuilder> qb;
    CSQLiteQueryBuilder::New((ISQLiteQueryBuilder**)&qb);
    qb->SetTables(args->mTable);

    AutoPtr<ISQLiteDatabase> db;
    mOpenHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> result;
    qb->Query(db, projection, args->mWhere, args->mArgs, String(NULL), String(NULL),
            sortOrder, (ICursor**)&result);

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
    /* [in] */ DatabaseHelper* helper,
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& nullColumnHack,
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Boolean res;
    values->ContainsKey(IBaseColumns::ID, &res);
    if (!res) {
        //throw new RuntimeException("Error: attempting to add item without specifying an id")
        Slogger::E(TAG, "Error: attempting to add item without specifying an id");
        return E_RUNTIME_EXCEPTION;
    }
    return db->Insert(table, nullColumnHack, values, result);
}

ECode LauncherProvider::DeleteId(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int64 id)
{
    AutoPtr<IUri> uri;
    LauncherSettings::Favorites::GetContentUri(id, FALSE, (IUri**)&uri);
    AutoPtr<SqlArguments> args = new SqlArguments(uri, String(NULL), NULL);
    Int32 tmp;
    return db->Delete(args->mTable, args->mWhere, args->mArgs, &tmp);
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
    DbInsertAndCheck(mOpenHelper, db, args->mTable, String(NULL), initialValues, &rowId);
    if (rowId <= 0) {
        *outuri = NULL;
        return NOERROR;
    }

    AutoPtr<IContentUris> helper;
    CContentUris::AcquireSingleton((IContentUris**)&helper);
    AutoPtr<IUri> newUri;
    helper->WithAppendedId(uri, rowId, (IUri**)&newUri);
    SendNotify(newUri);

    *outuri = newUri;
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
        FAIL_GOTO(DbInsertAndCheck(mOpenHelper, db, args->mTable, String(NULL),
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
    /* [in] */ IUri* uri,
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
    if (notify.IsNull() || (notify.Equals(String( "true")))) {
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
    VALIDATE_NOT_NULL(id);

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
            Int32 tmp;
            mOpenHelper->LoadFavorites(db, workspaceResId, &tmp);
            Boolean res;
            editor->Commit(&res);
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
        AutoPtr<ISQLiteDatabaseHelper> helper;
        CSQLiteDatabaseHelper::AcquireSingleton((ISQLiteDatabaseHelper**)&helper);
        Boolean result;
        helper->DeleteDatabase(dbFile, &result);
    }
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    mOpenHelper = new DatabaseHelper();
    mOpenHelper->constructor(context);
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
    *result = selectWhere.ToString();
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos