
#include "elastos/droid/launcher2/WidgetPreviewLoader.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

SoftReferenceThreadLocal::SoftReferenceThreadLocal()
{
    mThreadLocal = new ThreadLocal<SoftReference<T>>();
}

void SoftReferenceThreadLocal::Set(
    /* [in] */ IInterface* t)
{
    mThreadLocal.set(new SoftReference<T>(t));
}

AutoPtr<IInterface> SoftReferenceThreadLocal::Get()
{
    SoftReference<T> reference = mThreadLocal.get();
    T obj;
    if (reference == null) {
        obj = initialValue();
        mThreadLocal.set(new SoftReference<T>(obj));
        return obj;
    } else {
        obj = reference.get();
        if (obj == null) {
            obj = initialValue();
            mThreadLocal.set(new SoftReference<T>(obj));
        }
        return obj;
    }
}

AutoPtr<IInterface> CanvasCache::InitialValue()
{

}

AutoPtr<IInterface> PaintCache::InitialValue()
{

}

AutoPtr<IInterface> BitmapCache::InitialValue()
{

}

AutoPtr<IInterface> RectCache::InitialValue()
{

}

AutoPtr<IInterface> BitmapFactoryOptionsCache::InitialValue()
{

}

static Int32 WidgetPreviewLoader::CacheDb::DB_VERSION = 2;
static String WidgetPreviewLoader::CacheDb::DB_NAME("widgetpreviews.db");
static String WidgetPreviewLoader::CacheDb::TABLE_NAME("shortcut_and_widget_previews");
static String WidgetPreviewLoader::CacheDb::COLUMN_NAME("name");
static String WidgetPreviewLoader::CacheDb::COLUMN_SIZE("size");
static String WidgetPreviewLoader::CacheDb::COLUMN_PREVIEW_BITMAP("preview_bitmap");

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
    /* [in] */ IBitmap** bitmap,
    /* [in] */ WidgetPreviewLoader* host)
    : mO(o)
    , mBitmap(bitmap)
    , mHost(host)
{
}

ECode WidgetPreviewLoader::MyAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    mHost->WriteToDb(o, generatedPreview);
    *result = NULL;
    return NOERROR;
}

WidgetPreviewLoader::MyAsyncTask2::MyAsyncTask2(
    /* [in] */ CacheDb* cacheDb,
    /* [in] */ const String& packageName,
    /* [in] */ WidgetPreviewLoader* host)
    : mCacheDb(cacheDb)
    , mPackageName(packageName)
    , mHost(host)
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
    sb += CacheDb::TABLE_NAME;
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

    ECode ec = db->Delete(args/*SELECT query*/, array/* args to SELECT query*/);
    if (ec == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        *result = NULL;
        return NOERROR;
    }
    synchronized(mHost->sInvalidPackagesLock) {
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(mPackageName);
        mHost->sInvalidPackages->Remove(TO_IINTERFACE(obj));
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

WidgetPreviewLoader::WidgetPreviewLoader(
    /* [in] */ ILauncher* launcher)
    : sWidgetPreviewIconPaddingPercentage(0.25f)
{
    mLauncher = launcher;
    mContext = IContext::Probe(mLauncher);
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::app_icon_size, &mAppIconSize);
    resources->GgetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::profile_badge_size, &mProfileBadgeSize);
    resources->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::dimen::profile_badge_margin, &mProfileBadgeMargin);

    AutoPtr<IContext> context;
    launcher->GetApplicationContext((IContext**)&context);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(context);
    mIconCache = app->GetIconCache();
    mDb = app->GetWidgetPreviewCacheDb();

    CHashMap::New((IHashMap**)&mLoadedPreviews);
    CArrayList::New((IArrayList**)&mUnusedBitmaps);

    String key;
    LauncherApplication::GetSharedPreferencesKey(&key);
    AutoPtr<ISharedPreferences> sp;
    launcher->GetSharedPreferences(key, IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    String lastVersionName;
    sp->GetString(ANDROID_INCREMENTAL_VERSION_NAME_KEY, NULL, &lastVersionName);
    String versionName = Build::VERSION::INCREMENTAL;
    if (!versionName.Equals(lastVersionName)) {
        // clear all the previews whenever the system version changes, to ensure that previews
        // are up-to-date for any apps that might have been updated with the system
        ClearDb();
        AutoPtr<ISharedPreferencesEditor> editor;
        sp->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutString(ANDROID_INCREMENTAL_VERSION_NAME_KEY, versionName);
        editor->Commit();
    }
}

ECode WidgetPreviewLoader::RecreateDb()
{
    AutoPtr<IApplication> application;
    mLauncher->GetApplication((IApplication**)&application);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(application);
    app->RecreateWidgetPreviewDb();
    mDb = app->GetWidgetPreviewCacheDb(mDb);
    return NOERROR;
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
            assert(0); //SoftReference
            unusedBitmap = mUnusedBitmaps->Remove(0).get();
        }


        if (unusedBitmap != NULL) {
            AutoPtr<IInterface> obj = mCachedAppWidgetPreviewCanvas->Get();
            AutoPtr<ICanvas> c = ICanvas::Probe(obj);
            c->SetBitmap(unusedBitmap);
            c->DrawColor(0, PorterDuff.Mode.CLEAR);
            c->SetBitmap(NULL);
        }
    }

    if (unusedBitmap == NULL) {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(mPreviewBitmapWidth, mPreviewBitmapHeight,
                Bitmap.Config.ARGB_8888, (IBitmap**)&unusedBitmap);
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
            mLoadedPreviews->Get(obj, (IInterface**)&outObj);
            AutoPtr<IWeakReference> wr = IWeakReference::Probe(outObj);
            AutoPtr<IInterface> mapObj;
            wr->Resolve(EIID_IBitmap, (IInterface**)&mapObj);
            AutoPtr<IBitmap> b = IBitmap::Probe(mapObj);

            if (b == bitmapToRecycle) {
                mLoadedPreviews->remove(TO_IINTERFACE(obj));
                Boolean res;
                if (bitmapToRecycle->IsMutable(&res), res) {
                    synchronized (mUnusedBitmapsLock) {
                        assert(0); //SoftReference
                        //mUnusedBitmaps.add(new SoftReference<Bitmap>(b));
                    }
                }
            }
            else {
                throw new RuntimeException("Bitmap passed in doesn't match up");
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
        handle->ToString(str);
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
    if (IAppWidgetProviderInfo::Probe(o) != NUll) {
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
    mDb->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    values->Put(CacheDb::COLUMN_NAME, name);
    AutoPtr<IByteArrayOutputStream> stream;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&stream);
    preview->Compress(Bitmap.CompressFormat.PNG, 100, stream);
    AutoPtr<ArrayOf<Byte> > array;
    stream->ToByteArray((ArrayOf<Byte>**)&array);
    values->Put(CacheDb::COLUMN_PREVIEW_BITMAP, array);
    values->Put(CacheDb::COLUMN_SIZE, mSize);

    //try {
    ECode ec = db->Insert(CacheDb::TABLE_NAME, NULL, values);
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
    mDb->GetWritableDatabase((ISQLiteDatabase**)&db);
    // Delete everything
    //try {
    ECode ec = db->Delete(CacheDb::TABLE_NAME, NULL, NULL);
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

    AutoPtr<MyAsyncTask2> task = new MyAsyncTask2(cacheDb, this);
    return task->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL;
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
    mDb->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> result;
    //try {
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = CacheDb::COLUMN_PREVIEW_BITMAP
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = name;
    (*args)[1] = mSize;
    ECode ec = db->Query(CacheDb::TABLE_NAME,
            array, // cols to return
            mCachedSelectQuery, // select query
            args, // args to select query
            NULL,
            NULL,
            NULL,
            NULL, (ICursor**)&result);
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
        result->MoveToFirst();
        AutoPtr<ArrayOf<Byte> > blob;
        result->GetBlob(0, (ArrayOf<Byte>**)&blob);
        result->Close();
        AutoPtr<IBitmapFactoryOptions> opts;
        mCachedBitmapFactoryOptions->Get((IBitmapFactoryOptions**)&opts);
        opts->SetInBitmap(b);
        opts->SetInSampleSize(1);
        AutoPtr<IBitmap> out;
        BitmapFactory::DecodeByteArray(blob, 0, blob->GetLength(), opts, (IBitmap**)&out);
        *bitmap = out;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }
    else {
        result->Close();
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
    Launcher::GetSpanForWidget(mLauncher, info, (ArrayOf<Int32>**)&cellSpans);
    Int32 maxWidth = MaxWidthForWidgetPreview((*cellSpans)[0]);
    Int32 maxHeight = MaxHeightForWidgetPreview((*cellSpans)[1]);
    return GenerateWidgetPreview(info, (*cellSpans)[0], (*cellSpans)[1], maxWidth,
            maxHeight, preview, NULL, bitmap);
}

Int32 WidgetPreviewLoader::MaxWidthForWidgetPreview(
    /* [in] */ Int32 spanX)
{
    Int32 width;
    mWidgetSpacingLayout->EstimateCellWidth(spanX, &width);
    return Math::Min(mPreviewBitmapWidth, width);
}

Int32 WidgetPreviewLoader::MaxHeightForWidgetPreview(
    /* [in] */ Int32 spanY)
{
    Int32 height;
    mWidgetSpacingLayout->EstimateCellHeight(spanY, &height);
    return Math::Min(mPreviewBitmapHeight, height);
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
        previewDrawable->GetIntrinsicWidth(&previewDrawableWidth);
        Int32 previewDrawableHeight;
        previewDrawable->GetIntrinsicHeight(&previewDrawableHeight);
        previewWidth = previewDrawableWidth * cellHSpan; // subtract 2 dips
        previewHeight = previewDrawableHeight * cellVSpan;

        Bitmap::CreateBitmap(previewWidth, previewHeight,
                Config.ARGB_8888, (IBitmap**)&defaultPreview);
        AutoPtr<IInterface> obj = mCachedAppWidgetPreviewCanvas->Get();
        AutoPtr<ICanvas> c = iCanvas::Probe(obj);
        c->SetBitmap(defaultPreview);
        previewDrawable->SetBounds(0, 0, previewWidth, previewHeight);
        previewDrawable->SetTileModeXY(Shader.TileMode.REPEAT,
                Shader.TileMode.REPEAT);
        previewDrawable->Draw(c);
        c->SetBitmap(NULL);

        // Draw the icon in the top left corner
        Int32 minOffset = (Int32) (mAppIconSize * sWidgetPreviewIconPaddingPercentage);
        Int32 smallestSide = Math::Min(previewWidth, previewHeight);
        Float iconScale = Math::Min((Float) smallestSide
                / (mAppIconSize + 2 * minOffset), 1f);

        //try {
        AutoPtr<IDrawable> icon;
        Int32 hoffset =
                (Int32)((previewDrawableWidth - mAppIconSize * iconScale) / 2);
        Int32 yoffset =
                (Int32)((previewDrawableHeight - mAppIconSize * iconScale) / 2);
        Int32 infoIcon;
        info->GetIcon(&infoIcon);
        if (infoIcon > 0)
            AutoPtr<IComponentName> name;
            info->GetProvider((IComponentName**)&name);
            String pname;
            name->GetPackageName(&pname);
            AutoPtr<IUserHandle> handle;
            info->GetProfile((IUserHandle**)&handle);
            mIconCache->GetFullResIcon(pname, infoIcon, handle, (IDrawable**)&icon);
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
    Float scale = 1f;
    if (preScaledWidthOut != NULL) {
        (*preScaledWidthOut)[0] = previewWidth;
    }
    if (previewWidth > maxPreviewWidth) {
        scale = maxPreviewWidth / (Float)previewWidth;
    }
    if (scale != 1f) {
        previewWidth = (Int32)(scale * previewWidth);
        previewHeight = (Int32)(scale * previewHeight);
    }

    // If a bitmap is passed in, we use it; otherwise, we create a bitmap of the right size
    if (preview == NULL) {
        Bitmap::CreateBitmap(previewWidth, previewHeight, Config.ARGB_8888, (IBitmap**)&preview);
    }

    // Draw the scaled preview into the final bitmap
    Int32 width;
    preview->GetWidth(&width)
    Int32 x = (width - previewWidth) / 2;
    if (widgetPreviewExists) {
        RenderDrawableToBitmap(drawable, preview, x, 0, previewWidth,
                previewHeight);
    }
    else {
        AutoPtr<IInterface> obj = mCachedAppWidgetPreviewCanvas->Get();
        AutoPtr<ICanvas> c = ICanvas::Probe(obj);

        AutoPtr<IInterface> obj2 = mCachedAppWidgetPreviewSrcRect->Get();
        AutoPtr<IRect> src = IRect::Probe(obj2);

        AutoPtr<IInterface> obj3 = mCachedAppWidgetPreviewDestRect->Get();
        AutoPtr<IRect> dest = IRect::Probe(obj3);

        c->SetBitmap(preview);
        Int32 width;
        defaultPreview->GetWidth(&width);
        Int32 height;
        defaultPreview->GetHeight(&height);
        src->Set(0, 0, width, height);
        dest->Set(x, 0, x + previewWidth, previewHeight);

        AutoPtr<IInterface> obj4 = mCachedAppWidgetPreviewPaint->Get();
        AutoPTR<IPaint> p = IPaint::Probe(obj4);
        if (p == NULL) {
            CPaint::New((IPaint**)&p);
            p->SetFilterBitmap(TRUE);
            mCachedAppWidgetPreviewPaint->Set(p);
        }
        c->DrawBitmap(defaultPreview, src, dest, p);
        c->SetBitmap(NULL);
    }

    // Finally, if the preview is for a managed profile, badge it.
    AutoPtr<IUserHandle> result;
    info->GetProfile((IUserHandle**)&result);
    AutoPtr<IUserHandle> result2;
    Process::GetMyUserHandle((IUserHandle**)&result2);
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
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<IBitmapDrawable> previewDrawable;
        CBitmapDrawable::New(resources, preview, (IBitmapDrawable**)&previewDrawable);

        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<IUserHandle> handle;
        info->GetProfile((IUserHandle**)&handle);
        AutoPtr<IDrawable> badgedPreviewDrawable;
        packageManager->GetUserBadgedDrawableForDensity(
                previewDrawable, handle, badgeLocation, 0, (IDrawable**)&badgedPreviewDrawable);

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

    AutoPtr<IInterface> obj = mCachedShortcutPreviewBitmap->Get();
    AutoPtr<IBitmap> tempBitmap = IBitmap::Probe(obj);

    AutoPtr<IInterface> obj2 = mCachedShortcutPreviewCanvas->Get();
    AutoPtr<ICanvas> c = ICanvas::Probe(obj2);

    Int32 width;
    tempBitmap->GetWidth(&width);
    Int32 height;
    tempBitmap->GetHeight(&height);
    if (tempBitmap == NULL ||
            width != maxWidth ||
            height != maxHeight) {
        Bitmap::CreateBitmap(maxWidth, maxHeight, Config.ARGB_8888, (IBitmap**)&tempBitmap);
        mCachedShortcutPreviewBitmap->Set(tempBitmap);
    }
    else {
        c->SetBitmap(tempBitmap);
        c->DrawColor(0, PorterDuff.Mode.CLEAR);
        c->SetBitmap(NULL);
    }
    // Render the icon
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    AutoPtr<IDrawable> icon;
    mIconCache->GetFullResIcon(info, userHandle, (IDrawable**)&icon);

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
        Bitmap::CreateBitmap(maxWidth, maxHeight, Config.ARGB_8888, (IBitmap**)&preview);
    }

    c->SetBitmap(preview);
    // Draw a desaturated/scaled version of the icon in the background as a watermark
    AutoPtr<IInterface> obj3 = mCachedShortcutPreviewPaint->Get();
    AutoPtr<IPaint> p = IPaint::Probe(obj3);
    if (p == NULL) {
        CPaint::New((IPaint**)&p);
        AutoPtr<IColorMatrix> colorMatrix;
        CColorMatrix::New((IColorMatrix**)&colorMatrix);
        colorMatrix->SetSaturation(0);
        AutoPtr<IColorMatrixColorFilter> filter;
        CColorMatrixColorFilter::New(colorMatrix, (IColorMatrixColorFilter**)&filter);
        p->SetColorFilter(filter);
        p->SetAlpha((Int32) (255 * 0.06f));
        mCachedShortcutPreviewPaint->Set(p);
    }
    c->DrawBitmap(tempBitmap, 0, 0, p);
    c->SetBitmap(NULL);

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
    RenderDrawableToBitmap(d, bitmap, x, y, w, h, 1f);
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
