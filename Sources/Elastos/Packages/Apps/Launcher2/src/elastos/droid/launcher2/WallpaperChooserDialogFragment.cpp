
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"
#include "elastos/droid/launcher2/CWallpaperChooserDialogFragment.h"
#include "elastos/droid/launcher2/CWallpaperDrawable.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(WallpaperChooserDialogFragment::MyOnClickListener, Object,
        IViewOnClickListener);

WallpaperChooserDialogFragment::MyOnClickListener::MyOnClickListener(
    /* [in] */ WallpaperChooserDialogFragment* host,
    /* [in] */ IGallery* gallery)
    : mHost(host)
    , mGallery(gallery)
{
}

ECode WallpaperChooserDialogFragment::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 position;
    IAdapterView::Probe(mGallery)->GetSelectedItemPosition(&position);
    return mHost->SelectWallpaper(position);
}

WallpaperChooserDialogFragment::ImageAdapter::ImageAdapter(
    /* [in] */ WallpaperChooserDialogFragment* host,
    /* [in] */ IActivity* activity)
    : mHost(host)
{
    activity->GetLayoutInflater((ILayoutInflater**)&mLayoutInflater);
}

ECode WallpaperChooserDialogFragment::ImageAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return mHost->mThumbs->GetSize(count);
}

ECode WallpaperChooserDialogFragment::ImageAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);

    AutoPtr<IInteger32> obj = CoreUtils::Convert(position);
    *item = TO_IINTERFACE(obj);
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::ImageAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);

    *itemId = position;
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::ImageAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** outView)
{
    VALIDATE_NOT_NULL(outView);
    *outView = NULL;

    AutoPtr<IView> view;

    if (convertView == NULL) {
        mLayoutInflater->Inflate(Elastos::Droid::Launcher2::R::layout::wallpaper_item, parent,
                FALSE, (IView**)&view);
    }
    else {
        view = convertView;
    }

    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::Launcher2::R::id::wallpaper_image, (IView**)&tmp);
    AutoPtr<IImageView> image = IImageView::Probe(tmp);

    AutoPtr<IInterface> obj;
    mHost->mThumbs->Get(position, (IInterface**)&obj);
    AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
    Int32 thumbRes;
    intObj->GetValue(&thumbRes);

    image->SetImageResource(thumbRes);
    AutoPtr<IDrawable> thumbDrawable;
    image->GetDrawable((IDrawable**)&thumbDrawable);
    if (thumbDrawable != NULL) {
        thumbDrawable->SetDither(TRUE);
    }
    else {
        Slogger::E(TAG, "Error decoding thumbnail resId=%d for wallpaper #%d", thumbRes
                , position);
    }

    *outView = view;
    REFCOUNT_ADD(*outView);
    return NOERROR;
}

WallpaperChooserDialogFragment::WallpaperLoader::WallpaperLoader(
    /* [in] */ WallpaperChooserDialogFragment* host)
    : mHost(host)
{
}

ECode WallpaperChooserDialogFragment::WallpaperLoader::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (IsCancelled()) {
        *result = NULL;
        return NOERROR;
    }

    //try {
    AutoPtr<IInteger32> intObj = IInteger32::Probe((*params)[0]);
    Int32 value;
    intObj->GetValue(&value);

    AutoPtr<IResources> resources;
    ECode ec = mHost->GetResources((IResources**)&resources);
    if (ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
        Slogger::W("WallpaperChooserDialogFragment", "Out of memory trying to load wallpaper res=%08x", value);
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IDrawable> d;
    ec = resources->GetDrawable(value, (IDrawable**)&d);
    if (ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
        Slogger::W("WallpaperChooserDialogFragment", "Out of memory trying to load wallpaper res=%08x", value);
        *result = NULL;
        return NOERROR;
    }

    if (IBitmapDrawable::Probe(d) != NULL) {
        AutoPtr<IBitmap> bitmap;
        ec = IBitmapDrawable::Probe(d)->GetBitmap((IBitmap**)&bitmap);
        if (ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
            Slogger::W("WallpaperChooserDialogFragment", "Out of memory trying to load wallpaper res=%08x", value);
            *result = NULL;
            return NOERROR;
        }
        *result = TO_IINTERFACE(bitmap);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
    //} catch (OutOfMemoryError e) {
    // if (ec == (ECode)E_OUT_OF_MEMORY_ERROR) {
    //     Slogger::W("WallpaperChooserDialogFragment", "Out of memory trying to load wallpaper res=%08x", value);
    //     *result = NULL;
    //     return NOERROR;
    // }
    //}
}

ECode WallpaperChooserDialogFragment::WallpaperLoader::OnPostExecute(
    /* [in] */ IInterface* result)
{
    if (result == NULL) {
        return NOERROR;
    }

    AutoPtr<IBitmap> b = IBitmap::Probe(result);

    if (!IsCancelled()) {
        AutoPtr<IView> v;
        mHost->GetView((IView**)&v);
        if (v != NULL) {
            mHost->mWallpaperDrawable->SetBitmap(b);
            v->PostInvalidate();
        }
        else {
            mHost->mWallpaperDrawable->SetBitmap(NULL);
        }
        mHost->mLoader = NULL;
    }
    else {
       b->Recycle();
    }
    return NOERROR;
}

void WallpaperChooserDialogFragment::WallpaperLoader::Cancel()
{
    AsyncTask::Cancel(TRUE);
}

CAR_INTERFACE_IMPL(WallpaperChooserDialogFragment::WallpaperDrawable, Drawable, IWallpaperDrawable)

WallpaperChooserDialogFragment::WallpaperDrawable::WallpaperDrawable()
    : mIntrinsicWidth(0)
    , mIntrinsicHeight(0)
{
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::constructor()
{
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    mBitmap = bitmap;
    if (mBitmap == NULL)
        return NOERROR;

    mBitmap->GetWidth(&mIntrinsicWidth);
    mBitmap->GetHeight(&mIntrinsicHeight);
    mMatrix = NULL;
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    if (mBitmap == NULL) {
        return NOERROR;
    }

    if (mMatrix == NULL) {
        Int32 vwidth;
        canvas->GetWidth(&vwidth);
        Int32 vheight;
        canvas->GetHeight(&vheight);
        Int32 dwidth = mIntrinsicWidth;
        Int32 dheight = mIntrinsicHeight;

        Float scale = 1.0f;

        if (dwidth < vwidth || dheight < vheight) {
            scale = Elastos::Core::Math::Max((Float) vwidth / (Float) dwidth,
                    (Float) vheight / (Float) dheight);
        }

        Float dx = (vwidth - dwidth * scale) * 0.5f + 0.5f;
        Float dy = (vheight - dheight * scale) * 0.5f + 0.5f;

        CMatrix::New((IMatrix**)&mMatrix);
        mMatrix->SetScale(scale, scale);
        Boolean res;
        mMatrix->PostTranslate((Int32)dx, (Int32)dy, &res);
    }

    return canvas->DrawBitmap(mBitmap, mMatrix, NULL);
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);

    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // Ignore
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::WallpaperDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // Ignore
    return NOERROR;
}


CAR_INTERFACE_IMPL_2(WallpaperChooserDialogFragment::InnerListener, Object,
    IAdapterViewOnItemSelectedListener, IAdapterViewOnItemClickListener);

WallpaperChooserDialogFragment::InnerListener::InnerListener(
    /* [in] */ WallpaperChooserDialogFragment* host)
    : mHost(host)
{}

ECode WallpaperChooserDialogFragment::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ECode WallpaperChooserDialogFragment::InnerListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemSelected(parent, view, position, id);
}

ECode WallpaperChooserDialogFragment::InnerListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return mHost->OnNothingSelected(parent);
}


const String WallpaperChooserDialogFragment::TAG("Launcher.WallpaperChooserDialogFragment");
const String WallpaperChooserDialogFragment::EMBEDDED_KEY("Elastos.Droid.Launcher2.WallpaperChooserDialogFragment.EMBEDDED_KEY");


WallpaperChooserDialogFragment::WallpaperChooserDialogFragment()
    : mEmbedded(FALSE)
{
}

ECode WallpaperChooserDialogFragment::constructor()
{
    CWallpaperDrawable::New((IWallpaperDrawable**)&mWallpaperDrawable);
    return DialogFragment::constructor();
}

ECode WallpaperChooserDialogFragment::NewInstance(
    /* [out] */ IWallpaperChooserDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IWallpaperChooserDialogFragment> _fragment;
    CWallpaperChooserDialogFragment::New((IWallpaperChooserDialogFragment**)&_fragment);
    IDialogFragment::Probe(_fragment)->SetCancelable(TRUE);
    *fragment = _fragment;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnCreate(
     /* [in] */ IBundle* savedInstanceState)
{
    DialogFragment::OnCreate(savedInstanceState);
    if (savedInstanceState != NULL) {
        Boolean res;
        savedInstanceState->ContainsKey(EMBEDDED_KEY, &res);
        if (res) {
            return savedInstanceState->GetBoolean(EMBEDDED_KEY, &mEmbedded);
        }
    }
    else {
        return IsInLayout(&mEmbedded);
    }

    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    return outState->PutBoolean(EMBEDDED_KEY, mEmbedded);
}

void WallpaperChooserDialogFragment::CancelLoader()
{
    if (mLoader != NULL) {
        AsyncTask::Status status = mLoader->GetStatus();
        if (status != AsyncTask::FINISHED) {
            mLoader->Cancel();
            mLoader = NULL;
        }
    }
    return;
}

ECode WallpaperChooserDialogFragment::OnDetach()
{
    DialogFragment::OnDetach();

    CancelLoader();
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnDestroy()
{
    DialogFragment::OnDestroy();

    CancelLoader();
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    DialogFragment::OnDismiss(dialog);
    /* On orientation changes, the dialog is effectively "dismissed" so this is called
     * when the activity is no longer associated with this dying dialog fragment. We
     * should just safely ignore this case by checking if getActivity() returns null
     */
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity != NULL) {
        return activity->Finish();
    }
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);

    FindWallpapers();

    *dialog = NULL;
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** outView)
{
    VALIDATE_NOT_NULL(outView);
    *outView = NULL;

    FindWallpapers();

    /* If this fragment is embedded in the layout of this activity, then we should
     * generate a view to display. Otherwise, a dialog will be created in
     * onCreateDialog()
     */
    if (mEmbedded) {
        AutoPtr<IView> view;
        inflater->Inflate(Elastos::Droid::Launcher2::R::layout::wallpaper_chooser, container, FALSE, (IView**)&view);
        view->SetBackground(IDrawable::Probe(mWallpaperDrawable));

        AutoPtr<IView> tmp;
        view->FindViewById(Elastos::Droid::Launcher2::R::id::gallery, (IView**)&tmp);
        AutoPtr<IGallery> gallery = IGallery::Probe(tmp);
        gallery->SetCallbackDuringFling(FALSE);
        AutoPtr<InnerListener> listener = new InnerListener(this);
        IAdapterView::Probe(gallery)->SetOnItemSelectedListener(listener);

        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<ImageAdapter> adapter = new ImageAdapter(this, activity);
        IAdapterView::Probe(gallery)->SetAdapter(adapter);

        AutoPtr<IView> setButton;
        view->FindViewById(Elastos::Droid::Launcher2::R::id::set, (IView**)&setButton);
        AutoPtr<MyOnClickListener> clickListener = new MyOnClickListener(this, gallery);
        setButton->SetOnClickListener(clickListener);
        *outView = view;
        REFCOUNT_ADD(*outView);
    }
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::SelectWallpaper(
    /* [in] */ Int32 position)
{
    Slogger::I(TAG, " >> SelectWallpaper: %d", position);
    //try {
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::WALLPAPER_SERVICE, (IInterface**)&obj);
    AutoPtr<IWallpaperManager> wpm = IWallpaperManager::Probe(obj);

    AutoPtr<IInterface> tmp;
    mImages->Get(position, (IInterface**)&tmp);
    AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
    Int32 value;
    intObj->GetValue(&value);
    ECode ec = wpm->SetResource(value);

    activity->SetResult(IActivity::RESULT_OK);
    ec = activity->Finish();
    //} catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed to set wallpaper: %d", ec);
    }
    //}
    return NOERROR;
}

ECode WallpaperChooserDialogFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return SelectWallpaper(position);
}

ECode WallpaperChooserDialogFragment::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (mLoader != NULL) {
        AsyncTask::Status status = mLoader->GetStatus();
        if (status != AsyncTask::FINISHED) {
            mLoader->Cancel();
        }
    }
    mLoader = new WallpaperLoader(this);
    AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<IInteger32> obj = CoreUtils::Convert(position);
    params->Set(0, TO_IINTERFACE(obj));
    return mLoader->Execute(params);
}

ECode WallpaperChooserDialogFragment::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return NOERROR;
}

void WallpaperChooserDialogFragment::FindWallpapers()
{
    mThumbs = NULL;
    mImages = NULL;
    CArrayList::New(24, (IArrayList**)&mThumbs);
    CArrayList::New(24, (IArrayList**)&mImages);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    // Context.getPackageName() may return the "original" package name,
    // com.android.launcher2; Resources needs the real package name,
    // com.android.launcher. So we ask Resources for what it thinks the
    // package name should be.
    String packageName;
    resources->GetResourcePackageName(Elastos::Droid::Launcher2::R::array::wallpapers, &packageName);

    AddWallpapers(resources, packageName, Elastos::Droid::Launcher2::R::array::wallpapers);
    AddWallpapers(resources, packageName, Elastos::Droid::Launcher2::R::array::extra_wallpapers);
    return;
}

void WallpaperChooserDialogFragment::AddWallpapers(
    /* [in] */ IResources* resources,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 list)
{
    AutoPtr<ArrayOf<String> > extras;
    resources->GetStringArray(list, (ArrayOf<String>**)&extras);

    if (extras != NULL) {
        for (Int32 i = 0; i < extras->GetLength(); i++) {
            String extra = (*extras)[i];
            Int32 res;
            resources->GetIdentifier(extra, String("drawable"), packageName, &res);
            if (res != 0) {
                Int32 thumbRes;
                resources->GetIdentifier(extra + String("_small"), String("drawable"), packageName, &thumbRes);

                if (thumbRes != 0) {
                    AutoPtr<IInteger32> obj1 = CoreUtils::Convert(thumbRes);
                    mThumbs->Add(TO_IINTERFACE(obj1));
                    AutoPtr<IInteger32> obj2 = CoreUtils::Convert(res);
                    mImages->Add(TO_IINTERFACE(obj2));
                    // Log.d(TAG, "add: [" + packageName + "]: " + extra + " (" + res + ")");
                }
            }
        }
    }

    return;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos
