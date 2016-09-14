
#ifndef  __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__
#define  __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/droid/app/DialogFragment.h>
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/widget/BaseAdapter.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IGallery;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class WallpaperChooserDialogFragment
    : public DialogFragment
{
public:
    /**
     * Custom drawable that centers the bitmap fed to it.
     */
    class WallpaperDrawable
        : public Drawable
        , public IWallpaperDrawable
    {
    public:
        CAR_INTERFACE_DECL()

        WallpaperDrawable();

        CARAPI constructor();

        /* package */
        CARAPI SetBitmap(
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

private:

    class InnerListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerListener(
            /* [in] */ WallpaperChooserDialogFragment* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        WallpaperChooserDialogFragment* mHost;
    };

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
        AutoPtr<IGallery> mGallery;
    };

    class ImageAdapter
        : public BaseAdapter
    {
    public:
        ImageAdapter(
            /* [in] */ WallpaperChooserDialogFragment* host,
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
        WallpaperChooserDialogFragment* mHost;
        AutoPtr<ILayoutInflater> mLayoutInflater;
    };

    class WallpaperLoader
        : public AsyncTask
    {
    public:
        WallpaperLoader(
            /* [in] */ WallpaperChooserDialogFragment* host);

    //protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

        CARAPI_(void) Cancel();

    private:
        WallpaperChooserDialogFragment* mHost;
    };


public:

    WallpaperChooserDialogFragment();

    static CARAPI NewInstance(
        /* [out] */ IWallpaperChooserDialogFragment** fragment);

    CARAPI constructor();

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
    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    // Selection handler for the embedded Gallery view
    //@Override
    virtual CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    virtual CARAPI OnNothingSelected(
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
    AutoPtr<IWallpaperDrawable> mWallpaperDrawable;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_WALLPAPERCHOOSERDIALOGFRAGMENT_H__
