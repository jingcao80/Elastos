
#include "elastos/droid/launcher2/WidgetPreviewLoader.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/Launcher.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::Graphics::EIID_IBitmap;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::ShaderTileMode_REPEAT;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Process;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

// SoftReferenceThreadLocal::SoftReferenceThreadLocal()
// {
//     mThreadLocal = new ThreadLocal<SoftReference<T>>();
// }

// void SoftReferenceThreadLocal::Set(
//     /* [in] */ IInterface* t)
// {
//     mThreadLocal.set(new SoftReference<T>(t));
// }

// AutoPtr<IInterface> SoftReferenceThreadLocal::Get()
// {
//     SoftReference<T> reference = mThreadLocal.get();
//     T obj;
//     if (reference == null) {
//         obj = initialValue();
//         mThreadLocal.set(new SoftReference<T>(obj));
//         return obj;
//     } else {
//         obj = reference.get();
//         if (obj == null) {
//             obj = initialValue();
//             mThreadLocal.set(new SoftReference<T>(obj));
//         }
//         return obj;
//     }
// }

// AutoPtr<IInterface> CanvasCache::InitialValue()
// {

// }

// AutoPtr<IInterface> PaintCache::InitialValue()
// {

// }

// AutoPtr<IInterface> BitmapCache::InitialValue()
// {

// }

// AutoPtr<IInterface> RectCache::InitialValue()
// {

// }

// AutoPtr<IInterface> BitmapFactoryOptionsCache::InitialValue()
// {

// }

pthread_key_t WidgetPreviewLoader::mCachedShortcutPreviewBitmap;
pthread_key_t WidgetPreviewLoader::mCachedShortcutPreviewPaint;
pthread_key_t WidgetPreviewLoader::mCachedShortcutPreviewCanvas;

pthread_key_t WidgetPreviewLoader::mCachedAppWidgetPreviewCanvas;
pthread_key_t WidgetPreviewLoader::mCachedAppWidgetPreviewSrcRect;
pthread_key_t WidgetPreviewLoader::mCachedAppWidgetPreviewDestRect;
pthread_key_t WidgetPreviewLoader::mCachedAppWidgetPreviewPaint;

pthread_key_t WidgetPreviewLoader::mCachedBitmapFactoryOptions;
pthread_once_t WidgetPreviewLoader::sKeyOnce;

const Int32 WidgetPreviewLoader::CacheDb::DB_VERSION = 2;
const String WidgetPreviewLoader::CacheDb::DB_NAME("widgetpreviews.db");
const String WidgetPreviewLoader::CacheDb::TABLE_NAME("shortcut_and_widget_previews");
const String WidgetPreviewLoader::CacheDb::COLUMN_NAME("name");
const String WidgetPreviewLoader::CacheDb::COLUMN_SIZE("size");
const String WidgetPreviewLoader::CacheDb::COLUMN_PREVIEW_BITMAP("preview_bitmap");

CAR_INTERFACE_IMPL(WidgetPreviewLoader::CacheDb, SQLiteOpenHelper, IWidgetPreviewLoaderCacheDb);

WidgetPreviewLoader::CacheDb::CacheDb(
    /* [in] */ IContext* context)
{
    AutoPtr<IFile> cacheDir;
    context->GetCacheDir((IFile**)&cacheDir);
    AutoPtr<IFile> file;
    CFile::New(cacheDir, DB_NAME, (IFile**)&file);
    String path;
    file->GetPath(&path);
    SQLiteOpenHelper::constructor(context, path, NULL, DB_VERSION);
    // Store the context for later use
    mContext = context;
}

ECode WidgetPreviewLoader::CacheDb::OnCreate(
    /* [in] */ ISQLiteDatabase* database)
{
Slogger::E("WidgetPreviewLoader::CacheDb", "============================CacheDb::OnCreate 1");
    StringBuilder sb;
    sb += "CREATE TABLE IF NOT EXISTS ";
    sb += TABLE_NAME;
    sb += " (";
    sb += COLUMN_NAME;
    sb += " TEXT NOT NULL, ";
    sb += COLUMN_SIZE;
    sb += " TEXT NOT NULL, ";
    sb += COLUMN_PREVIEW_BITMAP;
    sb += " BLOB NOT NULL, ";
    sb += "PRIMARY KEY (";
    sb += COLUMN_NAME;
    sb += ", ";
    sb += COLUMN_SIZE;
    sb += ") ";
    sb += ");";
Slogger::E("WidgetPreviewLoader::CacheDb", "============================CacheDb::OnCreate return");
    return database->ExecSQL(sb.ToString());
}

ECode WidgetPreviewLoader::CacheDb::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (oldVersion != newVersion) {
        // Delete all the records; they'll be repopulated as this is a cache
        StringBuilder sb;
        sb += "DELETE FROM ";
        sb += TABLE_NAME;
        return db->ExecSQL(sb.ToString());
    }
    return NOERROR;
}

WidgetPreviewLoader::MyAsyncTask::MyAsyncTask(
    /* [in] */ IInterface* o,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ WidgetPreviewLoader* host)
    : mO(o)
    , mGeneratedPreview(bitmap)
    , mHost(host)
{
}

ECode WidgetPreviewLoader::MyAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    mHost->WriteToDb(mO, mGeneratedPreview);
    *result = NULL;
    return NOERROR;
}

WidgetPreviewLoader::MyAsyncTask2::MyAsyncTask2(
    /* [in] */ CacheDb* cacheDb,
    /* [in] */ const String& packageName)
    : mCacheDb(cacheDb)
    , mPackageName(packageName)
{
}

ECode WidgetPreviewLoader::MyAsyncTask2::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ISQLiteDatabase> db;
    mCacheDb->GetWritableDatabase((ISQLiteDatabase**)&db);
    //try {
    StringBuilder sb;
    sb += CacheDb::COLUMN_NAME;
    sb += " LIKE ? OR ";
    sb += CacheDb::COLUMN_NAME;
    sb += " LIKE ?";
    String args = sb.ToString();

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    StringBuilder sb2;
    sb2 += WidgetPreviewLoader::WIDGET_PREFIX;
    sb2 += mPackageName;
    sb2 += "/%";
    String arg1 = sb2.ToString();
    (*array)[0] = arg1;

    StringBuilder sb3;
    sb3 += WidgetPreviewLoader::SHORTCUT_PREFIX;
    sb3 += mPackageName;
    sb3 += "/%";
    String arg2 = sb3.ToString();
    (*array)[1] = arg2;

    Int32 res;
    ECode ec = db->Delete(CacheDb::TABLE_NAME, args/*SELECT query*/,
        array/* args to SELECT query*/, &res);
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        *result = NULL;
        return NOERROR;
    }
    synchronized(sInvalidPackagesLock) {
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(mPackageName);
        sInvalidPackages->Remove(TO_IINTERFACE(obj));
    }
    //} catch (SQLiteDiskIOException e) {
    // if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
    //     ;
    // }
    //}
    *result = NULL;
    return NOERROR;
}

static AutoPtr<IHashSet> initInvalidPackages()
{
    AutoPtr<IHashSet> tmp;
    CHashSet::New((IHashSet**)&tmp);
    return tmp;
}

const String WidgetPreviewLoader::TAG("WidgetPreviewLoader");
const String WidgetPreviewLoader::ANDROID_INCREMENTAL_VERSION_NAME_KEY("android.incremental.version");
AutoPtr<IHashSet> WidgetPreviewLoader::sInvalidPackages = initInvalidPackages();

Object WidgetPreviewLoader::sInvalidPackagesLock;

const String WidgetPreviewLoader::WIDGET_PREFIX("Widget:");
const String WidgetPreviewLoader::SHORTCUT_PREFIX("Shortcut:");

CAR_INTERFACE_IMPL(WidgetPreviewLoader, Object, IWidgetPreviewLoader);


static void BitmapDestructor(void* st)
{
    IBitmap* obj = static_cast<IBitmap*>(st);
    if (obj) {
        obj->Release();
    }
}

static void PaintDestructor(void* st)
{
    IPaint* obj = static_cast<IPaint*>(st);
    if (obj) {
        obj->Release();
    }
}

static void CanvasDestructor(void* st)
{
    ICanvas* obj = static_cast<ICanvas*>(st);
    if (obj) {
        obj->Release();
    }
}

static void RectDestructor(void* st)
{
    IRect* obj = static_cast<IRect*>(st);
    if (obj) {
        obj->Release();
    }
}

static void BitmapFactoryOptionsDestructor(void* st)
{
    IBitmapFactoryOptions* obj = static_cast<IBitmapFactoryOptions*>(st);
    if (obj) {
        obj->Release();
    }
}

static void MakeKey()
{
    Int32 UNUSED(result) = pthread_key_create(&WidgetPreviewLoader::mCachedShortcutPreviewBitmap, BitmapDestructor);
    assert(result == 0);

    result = pthread_key_create(&WidgetPreviewLoader::mCachedShortcutPreviewPaint, PaintDestructor);
    assert(result == 0);

    result = pthread_key_create(&WidgetPreviewLoader::mCachedShortcutPreviewCanvas, CanvasDestructor);
    assert(result == 0);
    AutoPtr<ICanvas> canvas1;
    CCanvas::New((ICanvas**)&canvas1);
    pthread_setspecific(WidgetPreviewLoader::mCachedShortcutPreviewCanvas, canvas1.Get());

    result = pthread_key_create(&WidgetPreviewLoader::mCachedAppWidgetPreviewCanvas,
            CanvasDestructor);
    assert(result == 0);
    AutoPtr<ICanvas> canvas2;
    CCanvas::New((ICanvas**)&canvas2);
    pthread_setspecific(WidgetPreviewLoader::mCachedAppWidgetPreviewCanvas, canvas2.Get());

    result = pthread_key_create(&WidgetPreviewLoader::mCachedAppWidgetPreviewSrcRect,
            RectDestructor);
    assert(result == 0);
    AutoPtr<IRect> rect1;
    CRect::New((IRect**)&rect1);
    pthread_setspecific(WidgetPreviewLoader::mCachedAppWidgetPreviewSrcRect, rect1.Get());

    result = pthread_key_create(&WidgetPreviewLoader::mCachedAppWidgetPreviewDestRect,
            RectDestructor);
    assert(result == 0);
    AutoPtr<IRect> rect2;
    CRect::New((IRect**)&rect2);
    pthread_setspecific(WidgetPreviewLoader::mCachedAppWidgetPreviewDestRect, rect2.Get());

    result = pthread_key_create(&WidgetPreviewLoader::mCachedAppWidgetPreviewPaint,
            PaintDestructor);
    assert(result == 0);

    result = pthread_key_create(&WidgetPreviewLoader::mCachedBitmapFactoryOptions,
            BitmapFactoryOptionsDestructor);
    assert(result == 0);
    AutoPtr<IBitmapFactoryOptions> op;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&op);
    pthread_setspecific(WidgetPreviewLoader::mCachedBitmapFactoryOptions, op.Get());
}

WidgetPreviewLoader::WidgetPreviewLoader()
    : mPreviewBitmapWidth(0)
    , mPreviewBitmapHeight(0)
    , mSize(NULL)
    , mCachedSelectQuery(NULL)
    , mAppIconSize(0)
    , mProfileBadgeSize(0)
    , mProfileBadgeMargin(0)
    , sWidgetPreviewIconPaddingPercentage(0.25f)
{
    pthread_once(&sKeyOnce, MakeKey);
}

ECode WidgetPreviewLoader::constructor(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    mContext = IContext::Probe(mLauncher);
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::app_icon_size, &mAppIconSize);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::profile_badge_size, &mProfileBadgeSize);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::profile_badge_margin, &mProfileBadgeMargin);

    AutoPtr<IContext> context;
    IContext::Probe(launcher)->GetApplicationContext((IContext**)&context);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(context);
    app->GetIconCache((IIconCache**)&mIconCache);
    app->GetWidgetPreviewCacheDb((IWidgetPreviewLoaderCacheDb**)&mDb);

    CHashMap::New((IHashMap**)&mLoadedPreviews);
    CArrayList::New((IArrayList**)&mUnusedBitmaps);

    String key;
    LauncherApplication::GetSharedPreferencesKey(&key);
    AutoPtr<ISharedPreferences> sp;
    IContext::Probe(launcher)->GetSharedPreferences(key, IContext::MODE_PRIVATE,
            (ISharedPreferences**)&sp);
    String lastVersionName;
    sp->GetString(ANDROID_INCREMENTAL_VERSION_NAME_KEY, String(NULL), &lastVersionName);
    String versionName = Build::VERSION::INCREMENTAL;
    if (!versionName.Equals(lastVersionName)) {
        // clear all the previews whenever the system version changes, to ensure that previews
        // are up-to-date for any apps that might have been updated with the system
        ClearDb();
        AutoPtr<ISharedPreferencesEditor> editor;
        sp->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutString(ANDROID_INCREMENTAL_VERSION_NAME_KEY, versionName);
        Boolean res;
        editor->Commit(&res);
    }
    return NOERROR;
}

ECode WidgetPreviewLoader::RecreateDb()
{
    AutoPtr<IApplication> application;
    IActivity::Probe(mLauncher)->GetApplication((IApplication**)&application);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(application);
    app->RecreateWidgetPreviewDb();
    return app->GetWidgetPreviewCacheDb((IWidgetPreviewLoaderCacheDb**)&mDb);
}

ECode WidgetPreviewLoader::SetPreviewSize(
    /* [in] */ Int32 previewWidth,
    /* [in] */ Int32 previewHeight,
    /* [in] */ IPagedViewCellLayout* widgetSpacingLayout)
{
    mPreviewBitmapWidth = previewWidth;
    mPreviewBitmapHeight = previewHeight;
    StringBuilder sb;
    sb += previewWidth;
    sb += "x";
    sb += previewHeight;
    mSize = sb.ToString();
    mWidgetSpacingLayout = widgetSpacingLayout;
    return NOERROR;
}

ECode WidgetPreviewLoader::GetPreview(
    /* [in] */ IInterface* o,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    String name = GetObjectName(o);
    // check if the package is valid
    Boolean packageValid = TRUE;
    synchronized(sInvalidPackagesLock) {
        String pname = GetObjectPackage(o);
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(pname);
        sInvalidPackages->Contains(TO_IINTERFACE(obj), &packageValid);
        packageValid = !packageValid;
    }
    if (!packageValid) {
        *bitmap = NULL;
        return NOERROR;
    }
    if (packageValid) {
        synchronized(mLoadedPreviewsLock) {
            // check if it exists in our existing cache
            Boolean hasKey;
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(name);
            mLoadedPreviews->ContainsKey(TO_IINTERFACE(obj), &hasKey);
            if (hasKey) {
                AutoPtr<IInterface> outObj;
                mLoadedPreviews->Get(obj, (IInterface**)&outObj);
                if (outObj != NULL) {
                    AutoPtr<IWeakReference> wr = IWeakReference::Probe(outObj);
                    AutoPtr<IInterface> mapObj;
                    wr->Resolve(EIID_IBitmap, (IInterface**)&mapObj);
                    AutoPtr<IBitmap> _bitmap = IBitmap::Probe(mapObj);
                    *bitmap = _bitmap;
                    REFCOUNT_ADD(*bitmap);
                    return NOERROR;
                }
            }
        }
    }

    AutoPtr<IBitmap> unusedBitmap;
    synchronized(mUnusedBitmapsLock) {
        // not in cache; we need to load it from the db
        Boolean isMutable;
        Int32 width;
        Int32 height;
        Int32 size;
        while ((unusedBitmap == NULL || !(unusedBitmap->IsMutable(&isMutable), isMutable) ||
                (unusedBitmap->GetWidth(&width), width) != mPreviewBitmapWidth ||
                (unusedBitmap->GetHeight(&height), height) != mPreviewBitmapHeight)
                && (mUnusedBitmaps->GetSize(&size), size) > 0) {
            AutoPtr<IInterface> object;
            mUnusedBitmaps->Remove(0, (IInterface**)&object);
            unusedBitmap = IBitmap::Probe(object);
        }

        if (unusedBitmap != NULL) {
            AutoPtr<ICanvas> c = (ICanvas*)pthread_getspecific(mCachedAppWidgetPreviewCanvas);
            c->SetBitmap(unusedBitmap);
            c->DrawColor(0, PorterDuffMode_CLEAR);
            c->SetBitmap(NULL);
        }
    }

    if (unusedBitmap == NULL) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(mPreviewBitmapWidth, mPreviewBitmapHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&unusedBitmap);
    }

    AutoPtr<IBitmap> preview;

    if (packageValid) {
        ReadFromDb(name, unusedBitmap, (IBitmap**)&preview);
    }

    if (preview != NULL) {
        synchronized(mLoadedPreviewsLock) {
            AutoPtr<IWeakReference> wr;
            IWeakReferenceSource::Probe(preview)->GetWeakReference((IWeakReference**)&wr);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(name);
            mLoadedPreviews->Put(obj, TO_IINTERFACE(wr));
        }
        *bitmap = preview;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }
    else {
        // it's not in the db... we need to generate it
        AutoPtr<IBitmap> generatedPreview;
        GeneratePreview(o, unusedBitmap, (IBitmap**)&generatedPreview);
        preview = generatedPreview;
        if (preview != unusedBitmap) {
            //throw new RuntimeException("generatePreview is not recycling the bitmap " + o);
            Slogger::E("WidgetPreviewLoader", "generatePreview is not recycling the bitmap ");
            *bitmap = NULL;
            return E_RUNTIME_EXCEPTION;
        }

        synchronized(mLoadedPreviewsLock) {
            AutoPtr<IWeakReference> wr;
            IWeakReferenceSource::Probe(preview)->GetWeakReference((IWeakReference**)&wr);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(name);
            mLoadedPreviews->Put(obj, TO_IINTERFACE(wr));
        }

        // write to db on a thread pool... this can be done lazily and improves the performance
        // of the first time widget previews are loaded
        AutoPtr<MyAsyncTask> task = new MyAsyncTask(o, generatedPreview, this);
        task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);

        *bitmap = preview;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }
}

ECode WidgetPreviewLoader::RecycleBitmap(
    /* [in] */ IInterface* o,
    /* [in] */ IBitmap* bitmapToRecycle)
{
    String name = GetObjectName(o);
    synchronized (mLoadedPreviewsLock) {
        Boolean hasKey;
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(name);
        mLoadedPreviews->ContainsKey(TO_IINTERFACE(obj), &hasKey);
        if (hasKey) {
            AutoPtr<IInterface> outObj;
            mLoadedPreviews->Get(obj, (IInterface**)&outObj);
            AutoPtr<IWeakReference> wr = IWeakReference::Probe(outObj);
            AutoPtr<IInterface> mapObj;
            wr->Resolve(EIID_IBitmap, (IInterface**)&mapObj);
            AutoPtr<IBitmap> b = IBitmap::Probe(mapObj);

            if (TO_IINTERFACE(b) == TO_IINTERFACE(bitmapToRecycle)) {
                mLoadedPreviews->Remove(TO_IINTERFACE(obj));
                Boolean res;
                if (bitmapToRecycle->IsMutable(&res), res) {
                    synchronized (mUnusedBitmapsLock) {
                        mUnusedBitmaps->Add(TO_IINTERFACE(b));
                    }
                }
            }
            else {
                //throw new RuntimeException("Bitmap passed in doesn't match up");
                Slogger::E("WidgetPreviewLoader", "Bitmap passed in doesn't match up");
                return E_RUNTIME_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

String WidgetPreviewLoader::GetObjectName(
    /* [in] */ IInterface* o)
{
    // should cache the string builder
    StringBuilder sb;
    String output;
    if (IAppWidgetProviderInfo::Probe(o) != NULL) {
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(o);
        sb += WIDGET_PREFIX;
        AutoPtr<IUserHandle> handle;
        info->GetProfile((IUserHandle**)&handle);
        String str;
        handle->ToString(&str);
        sb += str;
        sb += '/';
        AutoPtr<IComponentName> name;
        info->GetProvider((IComponentName**)&name);
        String flatten;
        name->FlattenToString(&flatten);
        sb += flatten;
        output = sb.ToString();
        sb.SetLength(0);
    }
    else {
        sb += SHORTCUT_PREFIX;

        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(o);
        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        String pname;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
        String name;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);

        AutoPtr<IComponentName> cname;
        CComponentName::New(pname, name, (IComponentName**)&cname);
        String str;
        cname->FlattenToString(&str);
        sb += str;
        output = sb.ToString();
        sb.SetLength(0);
    }
    return output;
}

String WidgetPreviewLoader::GetObjectPackage(
        /* [in] */ IInterface* o)
{
    if (IAppWidgetProviderInfo::Probe(o) != NULL) {
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(o);
        AutoPtr<IComponentName> name;
        info->GetProvider((IComponentName**)&name);
        String pname;
        IPackageItemInfo::Probe(name)->GetPackageName(&pname);
        return pname;
    }
    else {
        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(o);
        AutoPtr<IActivityInfo> activityInfo;
        info->GetActivityInfo((IActivityInfo**)&activityInfo);
        String pname;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
        return pname;
    }
}

ECode WidgetPreviewLoader::WriteToDb(
    /* [in] */ IInterface* o,
    /* [in] */ IBitmap* preview)
{
    String name = GetObjectName(o);
    AutoPtr<ISQLiteDatabase> db;
    ISQLiteOpenHelper::Probe(mDb)->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    values->Put(CacheDb::COLUMN_NAME, name);
    AutoPtr<IByteArrayOutputStream> stream;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&stream);
    Boolean res;
    preview->Compress(BitmapCompressFormat_PNG, 100, IOutputStream::Probe(stream), &res);
    AutoPtr<ArrayOf<Byte> > array;
    stream->ToByteArray((ArrayOf<Byte>**)&array);
    values->Put(CacheDb::COLUMN_PREVIEW_BITMAP, array);
    values->Put(CacheDb::COLUMN_SIZE, mSize);

    //try {
    Int64 num;
    ECode ec = db->Insert(CacheDb::TABLE_NAME, String(NULL), values, &num);
    //} catch (SQLiteDiskIOException e) {
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        RecreateDb();
    }
    //}
    return NOERROR;
}

ECode WidgetPreviewLoader::ClearDb()
{
    AutoPtr<ISQLiteDatabase> db;
    ISQLiteOpenHelper::Probe(mDb)->GetWritableDatabase((ISQLiteDatabase**)&db);
    // Delete everything
    //try {
    Int32 res;
    ECode ec = db->Delete(CacheDb::TABLE_NAME, String(NULL), NULL, &res);
    //} catch (SQLiteDiskIOException e) {
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        ;
    }
    //}
    return NOERROR;
}

ECode WidgetPreviewLoader::RemoveFromDb(
    /* [in] */ CacheDb* cacheDb,
    /* [in] */ const String& packageName)
{
    synchronized(sInvalidPackagesLock) {
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(packageName);
        sInvalidPackages->Add(TO_IINTERFACE(obj));
    }

    AutoPtr<MyAsyncTask2> task = new MyAsyncTask2(cacheDb, packageName);
    return task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
}

ECode WidgetPreviewLoader::ReadFromDb(
    /* [in] */ const String& name,
    /* [in] */ IBitmap* b,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    if (mCachedSelectQuery.IsNull()) {
        StringBuilder sb;
        sb += CacheDb::COLUMN_NAME;
        sb += " = ? AND ";
        sb += CacheDb::COLUMN_SIZE;
        sb += " = ?";
        mCachedSelectQuery = sb.ToString();
    }
    AutoPtr<ISQLiteDatabase> db;
    ISQLiteOpenHelper::Probe(mDb)->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> result;
    //try {
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = CacheDb::COLUMN_PREVIEW_BITMAP;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = name;
    (*args)[1] = mSize;
    ECode ec = db->Query(CacheDb::TABLE_NAME,
            array, // cols to return
            mCachedSelectQuery, // select query
            args, // args to select query
            String(NULL),
            String(NULL),
            String(NULL),
            String(NULL), (ICursor**)&result);
    //} catch (SQLiteDiskIOException e) {
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        RecreateDb();
        *bitmap = NULL;
        return NOERROR;
    }
    //}

    Int32 conut;
    result->GetCount(&conut);
    if (conut > 0) {
        Boolean res;
        result->MoveToFirst(&res);
        AutoPtr<ArrayOf<Byte> > blob;
        result->GetBlob(0, (ArrayOf<Byte>**)&blob);
        ICloseable::Probe(result)->Close();
        AutoPtr<IBitmapFactoryOptions> opts =
                (IBitmapFactoryOptions*)pthread_getspecific(mCachedBitmapFactoryOptions);
        opts->SetInBitmap(b);
        opts->SetInSampleSize(1);
        AutoPtr<IBitmapFactory> helper;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
        AutoPtr<IBitmap> out;
        helper->DecodeByteArray(blob, 0, blob->GetLength(), opts, (IBitmap**)&out);
        *bitmap = out;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }
    else {
        ICloseable::Probe(result)->Close();
        *bitmap = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode WidgetPreviewLoader::GeneratePreview(
    /* [in] */ IInterface* info,
    /* [in] */ IBitmap* preview,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    if (preview != NULL) {
        Int32 width;
        preview->GetWidth(&width);
        Int32 height;
        preview->GetHeight(&height);
        if (width != mPreviewBitmapWidth || height != mPreviewBitmapHeight) {
            //throw new RuntimeException("Improperly sized bitmap passed as argument");
            Slogger::E("WidgetPreviewLoader", "Improperly sized bitmap passed as argument");
            *bitmap = NULL;
            return E_RUNTIME_EXCEPTION;
        }
    }
    if (IAppWidgetProviderInfo::Probe(info) != NULL) {
        return GenerateWidgetPreview(IAppWidgetProviderInfo::Probe(info), preview, bitmap);
    }
    else {
        return GenerateShortcutPreview(
                IResolveInfo::Probe(info), mPreviewBitmapWidth, mPreviewBitmapHeight, preview, bitmap);
    }
    return NOERROR;
}

ECode WidgetPreviewLoader::GenerateWidgetPreview(
    /* [in] */ IAppWidgetProviderInfo* info,
    /* [in] */ IBitmap* preview,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<ArrayOf<Int32> > cellSpans;
    Launcher::GetSpanForWidget(IContext::Probe(mLauncher), info, (ArrayOf<Int32>**)&cellSpans);
    Int32 maxWidth;
    MaxWidthForWidgetPreview((*cellSpans)[0], &maxWidth);
    Int32 maxHeight;
    MaxHeightForWidgetPreview((*cellSpans)[1], &maxHeight);
    return GenerateWidgetPreview(info, (*cellSpans)[0], (*cellSpans)[1], maxWidth,
            maxHeight, preview, NULL, bitmap);
}

ECode WidgetPreviewLoader::MaxWidthForWidgetPreview(
    /* [in] */ Int32 spanX,
    /* [out] */ Int32* outwidth)
{
    VALIDATE_NOT_NULL(outwidth);

    Int32 width;
    mWidgetSpacingLayout->EstimateCellWidth(spanX, &width);
    *outwidth = Elastos::Core::Math::Min(mPreviewBitmapWidth, width);
    return NOERROR;
}

ECode WidgetPreviewLoader::MaxHeightForWidgetPreview(
    /* [in] */ Int32 spanY,
    /* [out] */ Int32* outheight)
{
    VALIDATE_NOT_NULL(outheight);

    Int32 height;
    mWidgetSpacingLayout->EstimateCellHeight(spanY, &height);
    *outheight = Elastos::Core::Math::Min(mPreviewBitmapHeight, height);
    return NOERROR;
}

ECode WidgetPreviewLoader::GenerateWidgetPreview(
    /* [in] */ IAppWidgetProviderInfo* info,
    /* [in] */ Int32 cellHSpan,
    /* [in] */ Int32 cellVSpan,
    /* [in] */ Int32 maxPreviewWidth,
    /* [in] */ Int32 maxPreviewHeight,
    /* [in] */ IBitmap* preview,
    /* [in] */ ArrayOf<Int32>* preScaledWidthOut,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    // Load the preview image if possible
    if (maxPreviewWidth < 0) maxPreviewWidth = Elastos::Core::Math::INT32_MAX_VALUE;
    if (maxPreviewHeight < 0) maxPreviewHeight = Elastos::Core::Math::INT32_MAX_VALUE;

    AutoPtr<IDrawable> drawable;
    info->LoadPreviewImage(mContext, 0, (IDrawable**)&drawable);

    Int32 previewWidth;
    Int32 previewHeight;
    AutoPtr<IBitmap> defaultPreview;
    Boolean widgetPreviewExists = (drawable != NULL);
    if (widgetPreviewExists) {
        drawable->GetIntrinsicWidth(&previewWidth);
        drawable->GetIntrinsicHeight(&previewHeight);
    }
    else {
        // Generate a preview image if we couldn't load one
        if (cellHSpan < 1) cellHSpan = 1;
        if (cellVSpan < 1) cellVSpan = 1;

        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<IDrawable> _drawable;
        resources->GetDrawable(Elastos::Droid::Launcher2
                ::R::drawable::widget_preview_tile, (IDrawable**)&_drawable);
        AutoPtr<IBitmapDrawable> previewDrawable = IBitmapDrawable::Probe(_drawable);

        Int32 previewDrawableWidth;
        IDrawable::Probe(previewDrawable)->GetIntrinsicWidth(&previewDrawableWidth);
        Int32 previewDrawableHeight;
        IDrawable::Probe(previewDrawable)->GetIntrinsicHeight(&previewDrawableHeight);
        previewWidth = previewDrawableWidth * cellHSpan; // subtract 2 dips
        previewHeight = previewDrawableHeight * cellVSpan;

        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(previewWidth, previewHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&defaultPreview);

        AutoPtr<ICanvas> c = (ICanvas*)pthread_getspecific(mCachedAppWidgetPreviewCanvas);
        c->SetBitmap(defaultPreview);
        IDrawable::Probe(previewDrawable)->SetBounds(0, 0, previewWidth, previewHeight);
        previewDrawable->SetTileModeXY(ShaderTileMode_REPEAT,
                ShaderTileMode_REPEAT);
        IDrawable::Probe(previewDrawable)->Draw(c);
        c->SetBitmap(NULL);

        // Draw the icon in the top left corner
        Int32 minOffset = (Int32) (mAppIconSize * sWidgetPreviewIconPaddingPercentage);
        Int32 smallestSide = Elastos::Core::Math::Min(previewWidth, previewHeight);
        Float iconScale = Elastos::Core::Math::Min((Float) smallestSide
                / (mAppIconSize + 2 * minOffset), 1.0f);

        //try {
        AutoPtr<IDrawable> icon;
        Int32 hoffset =
                (Int32)((previewDrawableWidth - mAppIconSize * iconScale) / 2);
        Int32 yoffset =
                (Int32)((previewDrawableHeight - mAppIconSize * iconScale) / 2);
        Int32 infoIcon;
        info->GetIcon(&infoIcon);
        if (infoIcon > 0) {
            AutoPtr<IComponentName> name;
            info->GetProvider((IComponentName**)&name);
            String pname;
            name->GetPackageName(&pname);
            AutoPtr<IUserHandle> handle;
            info->GetProfile((IUserHandle**)&handle);
            icon = ((IconCache*)mIconCache.Get())->GetFullResIcon(pname, infoIcon, handle);
        }
        if (icon != NULL) {
            RenderDrawableToBitmap(icon, defaultPreview, hoffset,
                    yoffset, (Int32)(mAppIconSize * iconScale),
                    (Int32)(mAppIconSize * iconScale));
        }
        //} catch (Resources.NotFoundException e) {
        //}
    }

    // Scale to fit width only - let the widget preview be clipped in the
    // vertical dimension
    Float scale = 1.0f;
    if (preScaledWidthOut != NULL) {
        (*preScaledWidthOut)[0] = previewWidth;
    }
    if (previewWidth > maxPreviewWidth) {
        scale = maxPreviewWidth / (Float)previewWidth;
    }
    if (scale != 1.0f) {
        previewWidth = (Int32)(scale * previewWidth);
        previewHeight = (Int32)(scale * previewHeight);
    }

    // If a bitmap is passed in, we use it; otherwise, we create a bitmap of the right size
    if (preview == NULL) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(previewWidth, previewHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&preview);
    }

    // Draw the scaled preview into the final bitmap
    Int32 width;
    preview->GetWidth(&width);
    Int32 x = (width - previewWidth) / 2;
    if (widgetPreviewExists) {
        RenderDrawableToBitmap(drawable, preview, x, 0, previewWidth,
                previewHeight);
    }
    else {
        AutoPtr<ICanvas> c = (ICanvas*)pthread_getspecific(mCachedAppWidgetPreviewCanvas);
        AutoPtr<IRect> src = (IRect*)pthread_getspecific(mCachedAppWidgetPreviewSrcRect);
        AutoPtr<IRect> dest = (IRect*)pthread_getspecific(mCachedAppWidgetPreviewDestRect);

        c->SetBitmap(preview);
        Int32 width;
        defaultPreview->GetWidth(&width);
        Int32 height;
        defaultPreview->GetHeight(&height);
        src->Set(0, 0, width, height);
        dest->Set(x, 0, x + previewWidth, previewHeight);

        AutoPtr<IPaint> p = (IPaint*)pthread_getspecific(mCachedAppWidgetPreviewPaint);
        if (p == NULL) {
            CPaint::New((IPaint**)&p);
            p->SetFilterBitmap(TRUE);
            pthread_setspecific(mCachedAppWidgetPreviewPaint, p.Get());
        }
        c->DrawBitmap(defaultPreview, src, dest, p);
        c->SetBitmap(NULL);
    }

    // Finally, if the preview is for a managed profile, badge it.
    AutoPtr<IUserHandle> result;
    info->GetProfile((IUserHandle**)&result);
    AutoPtr<IUserHandle> result2;
    Process::MyUserHandle((IUserHandle**)&result2);
    Boolean equals;
    result->Equals(result2, &equals);
    if (!equals) {
        Int32 previewBitmapWidth;
        preview->GetWidth(&previewBitmapWidth);
        Int32 previewBitmapHeight;
        preview->GetHeight(&previewBitmapHeight);

        // Figure out the badge location.
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> configuration;
        resources->GetConfiguration((IConfiguration**)&configuration);
        AutoPtr<IRect> badgeLocation;
        Int32 direction;
        configuration->GetLayoutDirection(&direction);
        if (direction == IView::LAYOUT_DIRECTION_LTR) {
            Int32 badgeLeft = previewBitmapWidth - mProfileBadgeSize - mProfileBadgeMargin;
            Int32 badgeTop = previewBitmapHeight - mProfileBadgeSize - mProfileBadgeMargin;
            Int32 badgeRight = badgeLeft + mProfileBadgeSize;
            Int32 badgeBottom = badgeTop + mProfileBadgeSize;
            CRect::New(badgeLeft, badgeTop, badgeRight, badgeBottom, (IRect**)&badgeLocation);
        }
        else {
            Int32 badgeLeft = mProfileBadgeMargin;
            Int32 badgeTop = previewBitmapHeight - mProfileBadgeSize - mProfileBadgeMargin;
            Int32 badgeRight = badgeLeft + mProfileBadgeSize;
            Int32 badgeBottom = badgeTop + mProfileBadgeSize;
            CRect::New(badgeLeft, badgeTop, badgeRight, badgeBottom, (IRect**)&badgeLocation);
        }

        // Badge the preview.
        AutoPtr<IResources> resources2;
        mContext->GetResources((IResources**)&resources2);
        AutoPtr<IBitmapDrawable> previewDrawable;
        CBitmapDrawable::New(resources2, preview, (IBitmapDrawable**)&previewDrawable);

        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<IUserHandle> handle;
        info->GetProfile((IUserHandle**)&handle);
        AutoPtr<IDrawable> badgedPreviewDrawable;
        packageManager->GetUserBadgedDrawableForDensity(
                IDrawable::Probe(previewDrawable), handle, badgeLocation, 0,
                (IDrawable**)&badgedPreviewDrawable);

        // Reture the nadged bitmap.
        if (IBitmapDrawable::Probe(badgedPreviewDrawable) != NULL) {
            AutoPtr<IBitmapDrawable> bitmapDrawable = IBitmapDrawable::Probe(badgedPreviewDrawable);
            return bitmapDrawable->GetBitmap(bitmap);
        }
    }

    *bitmap = preview;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode WidgetPreviewLoader::GenerateShortcutPreview(
    /* [in] */ IResolveInfo* info,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight,
    /* [in] */ IBitmap* preview,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<IBitmap> tempBitmap = (IBitmap*)pthread_getspecific(mCachedShortcutPreviewBitmap);
    AutoPtr<ICanvas> c = (ICanvas*)pthread_getspecific(mCachedShortcutPreviewCanvas);

    Int32 width;
    tempBitmap->GetWidth(&width);
    Int32 height;
    tempBitmap->GetHeight(&height);
    if (tempBitmap == NULL || width != maxWidth || height != maxHeight) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(maxWidth, maxHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&tempBitmap);
        pthread_setspecific(mCachedShortcutPreviewBitmap, tempBitmap.Get());
    }
    else {
        c->SetBitmap(tempBitmap);
        c->DrawColor(0, PorterDuffMode_CLEAR);
        c->SetBitmap(NULL);
    }
    // Render the icon
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    AutoPtr<IDrawable> icon = ((IconCache*)mIconCache.Get())->GetFullResIcon(
            info, userHandle);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Int32 paddingTop;
    resources->GetDimensionPixelOffset(Elastos::Droid::Launcher2
            ::R::dimen::shortcut_preview_padding_top, &paddingTop);
    Int32 paddingLeft;
    resources->GetDimensionPixelOffset(Elastos::Droid::Launcher2
            ::R::dimen::shortcut_preview_padding_left, &paddingLeft);
    Int32 paddingRight;
    resources->GetDimensionPixelOffset(Elastos::Droid::Launcher2
            ::R::dimen::shortcut_preview_padding_right, &paddingRight);

    Int32 scaledIconWidth = (maxWidth - paddingLeft - paddingRight);

    RenderDrawableToBitmap(
            icon, tempBitmap, paddingLeft, paddingTop, scaledIconWidth, scaledIconWidth);

    if (preview != NULL) {
        Int32 width;
        preview->GetWidth(&width);
        Int32 height;
        preview->GetHeight(&height);
        if (width != maxWidth || height != maxHeight) {
            //throw new RuntimeException("Improperly sized bitmap passed as argument");
            Slogger::E("WidgetPreviewLoader", "Improperly sized bitmap passed as argument");
            *bitmap = NULL;
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (preview == NULL) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(maxWidth, maxHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&preview);
    }

    c->SetBitmap(preview);
    // Draw a desaturated/scaled version of the icon in the background as a watermark
    AutoPtr<IPaint> p = (IPaint*)pthread_getspecific(mCachedShortcutPreviewPaint);
    if (p == NULL) {
        CPaint::New((IPaint**)&p);
        AutoPtr<IColorMatrix> colorMatrix;
        CColorMatrix::New((IColorMatrix**)&colorMatrix);
        colorMatrix->SetSaturation(0);
        AutoPtr<IColorMatrixColorFilter> filter;
        CColorMatrixColorFilter::New(colorMatrix, (IColorMatrixColorFilter**)&filter);
        p->SetColorFilter(IColorFilter::Probe(filter));
        p->SetAlpha((Int32)(255 * 0.06f));
        pthread_setspecific(mCachedShortcutPreviewPaint, p.Get());
    }
    c->DrawBitmap(tempBitmap, 0.0f, 0.0f, p);
    c->SetBitmap(NULL);
Slogger::E("WidgetPreviewLoader", "==========GenerateShortcutPreview (icon hight&width) mAppIconSize=%",mAppIconSize);
    RenderDrawableToBitmap(icon, preview, 0, 0, mAppIconSize, mAppIconSize);

    *bitmap = preview;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

void WidgetPreviewLoader::RenderDrawableToBitmap(
    /* [in] */ IDrawable* d,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    RenderDrawableToBitmap(d, bitmap, x, y, w, h, 1.0f);
    return;
}

void WidgetPreviewLoader::RenderDrawableToBitmap(
    /* [in] */ IDrawable* d,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Float scale)
{
    if (bitmap != NULL) {
        AutoPtr<ICanvas> c;
        CCanvas::New(bitmap, (ICanvas**)&c);
        c->Scale(scale, scale);
        AutoPtr<IRect> oldBounds;
        d->CopyBounds((IRect**)&oldBounds);
        d->SetBounds(x, y, x + w, y + h);
        d->Draw(c);
        d->SetBounds(oldBounds); // Restore the bounds
        c->SetBitmap(NULL);
    }
    return;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
