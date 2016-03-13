
#ifndef  __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__
#define  __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/droid/app/DialogFragment.h>
#include <elastos/droid/widget/BaseAdapter.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/graphics/drawable/Drawable.h>

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class WallpaperChooserDialogFragment
    : public DialogFragment
    , public IAdapterViewOnItemSelectedListener
    , public IAdapterViewOnItemClickListener
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnClickListener(
            /* [in] */ WallpaperChooserDialogFragment* host,
            /* [in] */ IGallery* gallery);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        WallpaperChooserDialogFragment* mHost;
        IGallery* mGallery;
    };

    class ImageAdapter
        : public BaseAdapter
        , public IListAdapter
        , public ISpinnerAdapter
        , public IAdapter
    {
    public:
        CAR_INTERFACE_DECL();

        ImageAdapter(
            /* [in] */ IActivity* activity);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        AutoPtr<ILayoutInflater> mLayoutInflater;
    };

    class WallpaperLoader
        : public AsyncTask
    {
    public:
        WallpaperLoader() {}

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

        CARAPI_(void) Cancel();
    };

    /**
     * Custom drawable that centers the bitmap fed to it.
     */
    class WallpaperDrawable
        : public Drawable
    {
    public:
        WallpaperDrawable();

        /* package */
        CARAPI_(void) SetBitmap(
            /* [in] */ IBitmap* bitmap);

        //@Override
        CARAPI Draw(
            /* [in] */ ICanvas* canvas);

        //@Override
        CARAPI GetOpacity(
            /* [out] */ Int32* opacity);

        //@Override
        CARAPI SetAlpha(
            /* [in] */ Int32 alpha);

        //@Override
        CARAPI SetColorFilter(
            /* [in] */ IColorFilter* cf);

    private:
        AutoPtr<IBitmap> mBitmap;
        Int32 mIntrinsicWidth;
        Int32 mIntrinsicHeight;
        AutoPtr<IMatrix> mMatrix;
    };

public:
    CAR_INTERFACE_DECL();

    WallpaperChooserDialogFragment();

    virtual ~WallpaperChooserDialogFragment() {}

    static CARAPI NewInstance(
        /* [out] */ IWallpaperChooserDialogFragment** fragment);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnDetach();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    /* This will only be called when in XLarge mode, since this Fragment is invoked like
     * a dialog in that mode
     */
    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // Click handler for the Dialog's GridView
    //@Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // Selection handler for the embedded Gallery view
    //@Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

private:
    CARAPI_(void) CancelLoader();

    CARAPI SelectWallpaper(
        /* [in] */ Int32 position);

    CARAPI_(void) FindWallpapers();

    CARAPI_(void) AddWallpapers(
        /* [in] */ IResources* resources,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 list);

private:
    static const String TAG;
    static const String EMBEDDED_KEY;

    Boolean mEmbedded;

    AutoPtr<IArrayList> mThumbs;
    AutoPtr<IArrayList> mImages;
    AutoPtr<WallpaperLoader> mLoader;
    AutoPtr<WallpaperDrawable> mWallpaperDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__
