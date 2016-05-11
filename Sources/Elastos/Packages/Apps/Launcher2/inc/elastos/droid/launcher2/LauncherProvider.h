#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERPROVIDER_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERPROVIDER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/database/sqlite/SQLiteOpenHelper.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.External.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::Object;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherProvider
    : public ContentProvider
    , public ILauncherProvider
{
public:
    class SqlArguments
        : public Object
    {
    public:
        SqlArguments(
            /* [in] */ IUri* url,
            /* [in] */ const String& where,
            /* [in] */ ArrayOf<String>* args);

        SqlArguments(
            /* [in] */ IUri* url);

    public:
        String mTable;
        String mWhere;
        AutoPtr<ArrayOf<String> > mArgs;
    };

private:
    class DatabaseHelper
        : public SQLiteOpenHelper
    {
    public:
        DatabaseHelper();

        CARAPI constructor(
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        //@Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

        // Generates a new ID to use for an object in your database. This method should be only
        // called from the main UI thread. As an exception, we do call it when we call the
        // constructor from the worker thread; however, this doesn't extend until after the
        // constructor is called, and we only pass a reference to LauncherProvider to LauncherApp
        // after that point
        CARAPI GenerateNewId(
            /* [out] */ Int64* id);

    private:
        friend class LauncherProvider;
        /**
         * Send notification that we've deleted the {@link AppWidgetHost},
         * probably as part of the initial database creation. The receiver may
         * want to re-call {@link AppWidgetHost#startListening()} to ensure
         * callbacks are correctly set.
         */
        CARAPI SendAppWidgetResetNotify();

        CARAPI SetFlagToLoadDefaultWorkspaceLater();

        CARAPI ConvertDatabase(
            /* [in] */ ISQLiteDatabase* db,
            /* [out] */ Boolean* result);

        CARAPI CopyFromCursor(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ ICursor* c,
            /* [out] */ Int32* num);

        CARAPI AddProfileColumn(
            /* [in] */ ISQLiteDatabase* db,
            /* [out] */ Boolean* result);

        CARAPI UpdateContactsShortcuts(
            /* [in] */ ISQLiteDatabase* db,
            /* [out] */ Boolean* result);

        CARAPI NormalizeIcons(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI InitializeMaxId(
            /* [in] */ ISQLiteDatabase* db,
            /* [out] */ Int64* id);

        /**
         * Upgrade existing clock and photo frame widgets into their new widget
         * equivalents.
         */
        CARAPI ConvertWidgets(
            /* [in] */ ISQLiteDatabase* db);

        CARAPI static BeginDocument(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& firstElementName);

        /**
         * Loads the default set of favorite packages from an xml file.
         *
         * @param db The database to write the values into
         * @param filterContainerId The specific container id of items to load
         */
        CARAPI LoadFavorites(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 workspaceResourceId,
            /* [out] */ Int32* result);

        CARAPI AddAppShortcut(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [in] */ ITypedArray* a,
            /* [in] */ IPackageManager* packageManager,
            /* [in] */ IIntent* intent,
            /* [out] */ Int64* result);

        CARAPI AddFolder(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [out] */ Int64* result);

        CARAPI GetSearchWidgetProvider(
            /* [out] */ IComponentName** name);

        /**
         * Gets an appwidget provider from the given package. If the package contains more than
         * one appwidget provider, an arbitrary one is returned.
         */
        CARAPI GetProviderInPackage(
            /* [in] */ const String& packageName,
            /* [out] */ IComponentName** name);

        CARAPI AddSearchWidget(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [out] */ Boolean* result);

        CARAPI AddClockWidget(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [out] */ Boolean* result);

        CARAPI AddAppWidget(
            /* [in] */ IXmlResourceParser* parser,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 type,
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [in] */ ITypedArray* a,
            /* [in] */ IPackageManager* packageManager,
            /* [out] */ Boolean* result);

        CARAPI AddAppWidget(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [in] */ IComponentName* cn,
            /* [in] */ Int32 spanX,
            /* [in] */ Int32 spanY,
            /* [in] */ IBundle* extras,
            /* [out] */ Boolean* result);

        CARAPI AddUriShortcut(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ IContentValues* values,
            /* [in] */ ITypedArray* a,
            /* [out] */ Int64* result);

    private:
        static const String TAG_FAVORITES;
        static const String TAG_FAVORITE;
        static const String TAG_CLOCK;
        static const String TAG_SEARCH;
        static const String TAG_APPWIDGET;
        static const String TAG_SHORTCUT;
        static const String TAG_FOLDER;
        static const String TAG_EXTRA;

        AutoPtr<IContext> mContext;
        AutoPtr<IAppWidgetHost> mAppWidgetHost;
        Int64 mMaxId;
    };

public:
    CAR_INTERFACE_DECL();

    LauncherProvider();

    CARAPI constructor();

    static CARAPI GetCONTENT_APPWIDGET_RESET_URI(
        /* [out] */ IUri** uri);

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    //@Override
    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** outuri);

    //@Override
    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* values,
        /* [out] */ Int32* result);

    //@Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    //@Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    CARAPI GenerateNewId(
        /* [out] */ Int64* id);

    /**
     * @param workspaceResId that can be 0 to use default or non-zero for specific resource
     */
    CARAPI LoadDefaultFavoritesIfNecessary(
        /* [in] */ Int32 origWorkspaceResId,
        /* [in] */ Boolean overridePrevious);

    CARAPI DeleteDatabase();

    /**
     * Build a query string that will match any row where the column matches
     * anything in the values list.
     */
    static CARAPI BuildOrWhereString(
        /* [in] */ const String& column,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ String* result);

private:
    static CARAPI DbInsertAndCheck(
        /* [in] */ DatabaseHelper* helper,
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ const String& nullColumnHack,
        /* [in] */ IContentValues* values,
        /* [out] */ Int64* result);

    static CARAPI DeleteId(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int64 id);

    CARAPI_(void) SendNotify(
        /* [in] */ IUri* uri);

public:
    /**
     * {@link Uri} triggered at any registered {@link android.database.ContentObserver} when
     * {@link AppWidgetHost#deleteHost()} is called during database creation.
     * Use this to recall {@link AppWidgetHost#startListening()} if needed.
     */
    static const AutoPtr<IUri> CONTENT_APPWIDGET_RESET_URI;

private:
    static const String TAG;
    static const Boolean LOGD;

    static const String DATABASE_NAME;

    static const Int32 DATABASE_VERSION;

    static const String ACTION_APPWIDGET_DEFAULT_WORKSPACE_CONFIGURE;

    AutoPtr<DatabaseHelper> mOpenHelper;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERPROVIDER_H__