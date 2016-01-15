
#include "CActivityOne.h"
#include "R.h"
//#include "Uri.h"
#include <elautoptr.h>
#include <stdio.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_ISpinnerAdapter;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::IGalleryLayoutParams;
using Elastos::Droid::Widget::CGalleryLayoutParams;
using Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER;
using Elastos::Droid::Widget::IGallery;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::IListAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ElGallery {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IServiceManager> sm;
    AutoPtr<IIActivityManager> am;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(String("MyImageDemo"));
    intent->SetAction(String("android.intent.action.MAIN"));
    Int32 status;
    if (FAILED(am->StartActivity(NULL, intent, String(NULL),
            NULL, String(NULL), 0, 0, String(NULL), NULL, NULL, &status))) {
    }

    return NOERROR;
}

Int32 CActivityOne::ImageAdapter::mImage[] = {
    R::drawable::sample_0,
    R::drawable::sample_1,
    R::drawable::sample_2,
    R::drawable::sample_3,
    R::drawable::sample_4,
    R::drawable::sample_5,
};

Int32 CActivityOne::ImageAdapter::mImageLength = 6;

CActivityOne::ImageAdapter::ImageAdapter(
    /* [in] */ IContext* c)
{
    mContext = c;
}

PInterface CActivityOne::ImageAdapter::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBaseAdapter*)this;
    }
    else if (riid == EIID_ISpinnerAdapter) {
        return (PInterface)(ISpinnerAdapter*)this;
    }
    else if (riid == EIID_IBaseAdapter) {
        return (PInterface)(IBaseAdapter*)this;
    }
    else if (riid == EIID_IListAdapter) {
        return (PInterface)(IListAdapter*)this;
    }
    else if (riid == EIID_IAdapter) {
        return (PInterface)(IBaseAdapter*)this;
    }

    return NULL;
}

UInt32 CActivityOne::ImageAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::ImageAdapter::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::ImageAdapter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    assert(0);
    return NOERROR;
}

ECode CActivityOne::ImageAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CActivityOne::ImageAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return E_NOT_IMPLEMENTED;
}

ECode CActivityOne::ImageAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mImageLength;
    return NOERROR;
}

ECode CActivityOne::ImageAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    return E_NOT_IMPLEMENTED;
}

ECode CActivityOne::ImageAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

ECode CActivityOne::ImageAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = FALSE;

    return NOERROR;
}

ECode CActivityOne::ImageAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* pConvertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    printf("CActivityOne::ImageAdapter::GetView, Pos: %d\n", position);
    VALIDATE_NOT_NULL(view);
    AutoPtr<IImageView> i;
    CImageView::New(mContext, (IImageView**)&i);
    i->SetImageResource(mImage[position]);
    i->SetScaleType(ImageViewScaleType_FIT_CENTER);
    AutoPtr<IGalleryLayoutParams> lp;
    CGalleryLayoutParams::New(
        IViewGroupLayoutParams::FILL_PARENT, IViewGroupLayoutParams::FILL_PARENT,
        (IGalleryLayoutParams**)&lp);
    i->SetLayoutParams(lp.Get());

    *view = i;
    REFCOUNT_ADD(*view);

    return NOERROR;
}

ECode CActivityOne::ImageAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32 * viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = 0;
    return NOERROR;
}

ECode CActivityOne::ImageAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;

    return NOERROR;
}

ECode CActivityOne::ImageAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = mImageLength == 0;
    return NOERROR;
}

ECode CActivityOne::ImageAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;

    return NOERROR;
}

ECode CActivityOne::ImageAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;

    return NOERROR;
}

ECode CActivityOne::ImageAdapter::NotifyDataSetChanged()
{
    return E_NOT_IMPLEMENTED;
}

ECode CActivityOne::ImageAdapter::NotifyDataSetInvalidated()
{
    return E_NOT_IMPLEMENTED;
}

CARAPI CActivityOne::ImageAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return E_NOT_IMPLEMENTED;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::gallery);
    assert(view != NULL);

    AutoPtr<IGallery> gallery = IGallery::Probe(view);
    AutoPtr<IBaseAdapter> adapter = new ImageAdapter(
        (IContext*)this->Probe(EIID_IContext));

    return gallery->SetAdapter(adapter);
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace ElGallery
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
