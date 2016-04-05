
#ifndef  __ELASTOS_DROID_LAUNCHER2_WIDGETPREVIEWLOADER_H__
#define  __ELASTOS_DROID_LAUNCHER2_WIDGETPREVIEWLOADER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class SoftReferenceThreadLocal
    : public Object
{
public:
    SoftReferenceThreadLocal();

    CARAPI_(void) Set(
        /* [in] */ IInterface* t);

    CARAPI_(AutoPtr<IInterface>) Get();

protected:
    virtual CARAPI_(AutoPtr<IInterface>) InitialValue() = 0;

private:
    AutoPtr<IThreadLocal> mThreadLocal;
};

class CanvasCache
    : public SoftReferenceThreadLocal
{
protected:
    //@Override
    CARAPI_(AutoPtr<IInterface>) InitialValue();
};

class PaintCache
    : public SoftReferenceThreadLocal
{
protected:
    //@Override
    CARAPI_(AutoPtr<IInterface>) InitialValue();
};

class BitmapCache
    : public SoftReferenceThreadLocal
{
protected:
    //@Override
    CARAPI_(AutoPtr<IInterface>) InitialValue();
};

class RectCache
    : public SoftReferenceThreadLocal
{
protected:
    //@Override
    CARAPI_(AutoPtr<IInterface>) InitialValue();
};

class BitmapFactoryOptionsCache
    : public SoftReferenceThreadLocal
{
protected::
    //@Override
    CARAPI_(AutoPtr<IInterface>) InitialValue();
};

class WidgetPreviewLoader
    : public Object
{
private:
    class CacheDb
        : public SQLiteOpenHelper
    {
    public:
        CacheDb(
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnCreate(
            /* [in] */ ISQLiteDatabase* db);

        //@Override
        CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    private:
        const static Int32 DB_VERSION;
        const static String DB_NAME;
        const static String TABLE_NAME;
        const static String COLUMN_NAME;
        const static String COLUMN_SIZE;
        const static String COLUMN_PREVIEW_BITMAP;
        AutoPtr<IContext> mContext;
    };

    class MyAsyncTask
        : public AsyncTask
    {
    public:
        MyAsyncTask(
            /* [in] */ IInterface* o,
            /* [in] */ IBitmap** bitmap,
            /* [in] */ WidgetPreviewLoader* host);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        IInterface* mO;
        IBitmap* mBitmap;
        WidgetPreviewLoader* mHost;
    };

    class MyAsyncTask2
        : public AsyncTask
    {
    public:
        MyAsyncTask2(
            /* [in] */ CacheDb* cacheDb,
            /* [in] */ const String& packageName,
            /* [in] */ WidgetPreviewLoader* host);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        CacheDb* mCacheDb;
        String mPackageName;
        WidgetPreviewLoader* mHost;
    };

public:
    WidgetPreviewLoader(
        /* [in] */ ILauncher* launcher);

    CARAPI_(void RecreateDb();

    CARAPI SetPreviewSize(
        /* [in] */ Int32 previewWidth,
        /* [in] */ Int32 previewHeight,
        /* [in] */ IPagedViewCellLayout* widgetSpacingLayout);

    CARAPI GetPreview(
        /* [in] */ IInterface* o,
        /* [out] */ IBitmap** bitmap);

    CARAPI RecycleBitmap(
        /* [in] */ IInterface* o,
        /* [in] */ IBitmap* bitmapToRecycle);

    static CARAPI RemoveFromDb(
        /* [in] */ CacheDb* cacheDb,
        /* [in] */ const String& packageName);

    CARAPI GeneratePreview(
        /* [in] */ IInterface* info,
        /* [in] */ IBitmap* preview,
        /* [out] */ IBitmap** bitmap);

    CARAPI GenerateWidgetPreview(
        /* [in] */ IAppWidgetProviderInfo* info,
        /* [in] */ IBitmap* preview,
        /* [out] */ IBitmap** bitmap);

    CARAPI_(Int32) MaxWidthForWidgetPreview(
        /* [in] */ Int32 spanX);

    CARAPI_(Int32) MaxHeightForWidgetPreview(
        /* [in] */ Int32 spanY);

    public GenerateWidgetPreview(
        /* [in] */ IAppWidgetProviderInfo* info,
        /* [in] */ Int32 cellHSpan,
        /* [in] */ Int32 cellVSpan,
        /* [in] */ Int32 maxPreviewWidth,
        /* [in] */ Int32 maxPreviewHeight,
        /* [in] */ IBitmap* preview,
        /* [in] */ ArrayOf<Int32>* preScaledWidthOut,
        /* [out] */ IBitmap** bitmap);

    static CARAPI_(void) RenderDrawableToBitmap(
        /* [in] */ IDrawable* d,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

private:
    static CARAPI_(String) GetObjectName(
        /* [in] */ IInterface* o);

    CARAPI_(String) GetObjectPackage(
        /* [in] */ IInterface* o);

    CARAPI WriteToDb(
        /* [in] */ IInterface* o,
        /* [in] */ IBitmap* preview);

    CARAPI ClearDb();

    CARAPI ReadFromDb(
        /* [in] */ const String& name,
        /* [in] */ IBitmap* b,
        /* [out] */ IBitmap** bitmap);

    CARAPI GenerateShortcutPreview(
        /* [in] */ IResolveInfo* info,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight,
        /* [in] */ IBitmap* preview,
        /* [out] */ IBitmap** bitmap);

    static CARAPI_(void) RenderDrawableToBitmap(
        /* [in] */ IDrawable* d,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Float scale);

private:
    static const String TAG;
    static const String ANDROID_INCREMENTAL_VERSION_NAME_KEY;

    Int32 mPreviewBitmapWidth;
    Int32 mPreviewBitmapHeight;
    String mSize;
    AutoPtr<IContext> mContext;
    AutoPtr<ILauncher> mLauncher;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IPagedViewCellLayout> mWidgetSpacingLayout;

    // Used for drawing shortcut previews
    AutoPtr<BitmapCache> mCachedShortcutPreviewBitmap = new BitmapCache();
    AutoPtr<PaintCache> mCachedShortcutPreviewPaint = new PaintCache();
    AutoPtr<CanvasCache> mCachedShortcutPreviewCanvas = new CanvasCache();

    // Used for drawing widget previews
    AutoPtr<CanvasCache> mCachedAppWidgetPreviewCanvas = new CanvasCache();
    AutoPtr<RectCache> mCachedAppWidgetPreviewSrcRect = new RectCache();
    AutoPtr<RectCache> mCachedAppWidgetPreviewDestRect = new RectCache();
    AutoPtr<PaintCache> mCachedAppWidgetPreviewPaint = new PaintCache();
    String mCachedSelectQuery;
    AutoPtr<BitmapFactoryOptionsCache> mCachedBitmapFactoryOptions = new BitmapFactoryOptionsCache();

    Int32 mAppIconSize;
    Int32 mProfileBadgeSize;
    Int32 mProfileBadgeMargin;

    AutoPtr<IconCache> mIconCache;

    const Float sWidgetPreviewIconPaddingPercentage;

    AutoPtr<CacheDb> mDb;

    AutoPtr<IHashMap> mLoadedPreviews;
    Object mLoadedPreviewsLock;
    AutoPtr<IArrayList> mUnusedBitmaps;
    Object mUnusedBitmapsLock;
    static AutoPtr<IHashSet> sInvalidPackages;
    static Object sInvalidPackagesLock;

    static const String WIDGET_PREFIX;
    static const String SHORTCUT_PREFIX;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_WIDGETPREVIEWLOADER_H__
