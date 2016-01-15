
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/LauncherActivity.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/CLauncherActivityListItem.h"
#include "elastos/droid/app/CLauncherActivityIconResizer.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/pm/CResolveInfoDisplayNameComparator.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
//#include "elastos/droid/widget/CFilterResults.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::CResolveInfoDisplayNameComparator;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IDrawFilter;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IPaintDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
//using Elastos::Droid::Widget::CFilterResults;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::Filter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::EIID_IFilterable;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::IComparator;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace App {

//==========================================================================
// LauncherActivity::ListItem
//==========================================================================
CAR_INTERFACE_IMPL(LauncherActivity::ListItem, Object, ILauncherActivityListItem)

LauncherActivity::ListItem::ListItem()
{}

LauncherActivity::ListItem::~ListItem()
{}

ECode LauncherActivity::ListItem::constructor()
{
    return NOERROR;
}

ECode LauncherActivity::ListItem::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IResolveInfo* resolveInfo,
    /* [in] */ ILauncherActivityIconResizer* resizer)
{
    mResolveInfo = resolveInfo;
    resolveInfo->LoadLabel(pm, (ICharSequence**)&mLabel);
    AutoPtr<IActivityInfo> ai;
    resolveInfo->GetActivityInfo((IActivityInfo**)&ai);
    AutoPtr<IComponentInfo> ci = IComponentInfo::Probe(ai);
    if (ci == NULL) {
        AutoPtr<IServiceInfo> si;
        resolveInfo->GetServiceInfo((IServiceInfo**)&si);
        ci = IComponentInfo::Probe(si);
    }

    IPackageItemInfo* pi = IPackageItemInfo::Probe(ci);
    String name;
    if (mLabel == NULL && ci != NULL) {
        pi->GetName(&name);
        mLabel = CoreUtils::Convert(name);
    }

    if (resizer != NULL) {
        AutoPtr<IDrawable> icon;
        resolveInfo->LoadIcon(pm, (IDrawable**)&icon);
        resizer->CreateIconThumbnail(icon, (IDrawable**)&mIcon);
    }
    AutoPtr<IApplicationInfo> appInfo;
    ci->GetApplicationInfo((IApplicationInfo**)&appInfo);
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&mPackageName);
    pi->GetName(&mClassName);
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetResolveInfo(
    /* [in] */ IResolveInfo* resolveInfo)
{
    mResolveInfo = resolveInfo;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetResolveInfo(
    /* [out] */ IResolveInfo** resolveInfo)
{
    VALIDATE_NOT_NULL(resolveInfo)
    *resolveInfo = mResolveInfo;
    REFCOUNT_ADD(*resolveInfo)
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    *label = mLabel;
    REFCOUNT_ADD(*label)
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetClassName(
    /* [in] */ const String& className)
{
    mClassName = className;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className)
    *className = mClassName;
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackageName;
    return NOERROR;
}

ECode LauncherActivity::ListItem::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode LauncherActivity::ListItem::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

//==========================================================================
// LauncherActivity::IconResizer
//==========================================================================
CAR_INTERFACE_IMPL(LauncherActivity::IconResizer, Object, ILauncherActivityIconResizer)

LauncherActivity::IconResizer::IconResizer()
    : mIconWidth(-1)
    , mIconHeight(-1)
{
    CRect::New((IRect**)&mOldBounds);
    CCanvas::New((ICanvas**)&mCanvas);
}

LauncherActivity::IconResizer::~IconResizer()
{}

ECode LauncherActivity::IconResizer::constructor(
    /* [in] */ ILauncherActivity* host)
{
    mHost = (LauncherActivity*)host;
    AutoPtr<IDrawFilter> filter;
    CPaintFlagsDrawFilter::New(
        IPaint::DITHER_FLAG, IPaint::FILTER_BITMAP_FLAG, (IDrawFilter**)&filter);

    mCanvas->SetDrawFilter(filter);

    AutoPtr<IResources> resources;
    mHost->GetResources((IResources**)&resources);
    Float ival;
    resources->GetDimension(R::dimen::app_icon_size, &ival);
    mIconWidth = mIconHeight = (Int32) ival;
    return NOERROR;
}

ECode LauncherActivity::IconResizer::CreateIconThumbnail(
    /* [in] */ IDrawable* inIcon,
    /* [out] */ IDrawable** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail)
    *thumbnail = NULL;

    Int32 width = mIconWidth;
    Int32 height = mIconHeight;

    AutoPtr<IDrawable> icon = inIcon;
    Int32 iconWidth, iconHeight;
    icon->GetIntrinsicWidth(&iconWidth);
    icon->GetIntrinsicHeight(&iconHeight);

    IPaintDrawable* painter = IPaintDrawable::Probe(icon);
    if (painter) {
        IShapeDrawable::Probe(painter)->SetIntrinsicWidth(width);
        IShapeDrawable::Probe(painter)->SetIntrinsicHeight(height);
    }

    if (width > 0 && height > 0) {
        AutoPtr<IResources> resources;
        mHost->GetResources((IResources**)&resources);

        if (width < iconWidth || height < iconHeight) {
            Float ratio = (Float) iconWidth / iconHeight;

            if (iconWidth > iconHeight) {
                height = (Int32) (width / ratio);
            }
            else if (iconHeight > iconWidth) {
                width = (Int32) (height * ratio);
            }

            Int32 opacity;
            icon->GetOpacity(&opacity);
            BitmapConfig c = opacity != IPixelFormat::OPAQUE ?
                BitmapConfig_ARGB_8888 : BitmapConfig_RGB_565;
            AutoPtr<IBitmap> thumb;
            CBitmap::CreateBitmap(mIconWidth, mIconHeight, c, (IBitmap**)&thumb);
            ICanvas* canvas = mCanvas;
            canvas->SetBitmap(thumb);
            // Copy the old bounds to restore them later
            // If we were to do oldBounds = icon->GetBounds(),
            // the call to SetBounds() that follows would
            // change the same instance and we would lose the
            // old bounds
            AutoPtr<IRect> rect;
            icon->GetBounds((IRect**)&rect);
            mOldBounds->Set(rect);
            Int32 x = (mIconWidth - width) / 2;
            Int32 y = (mIconHeight - height) / 2;
            icon->SetBounds(x, y, x + width, y + height);
            icon->Draw(canvas);
            icon->SetBounds(mOldBounds);

            icon = NULL;
            CBitmapDrawable::New(resources, thumb, (IDrawable**)&icon);
            canvas->SetBitmap(NULL);
        }
        else if (iconWidth < width && iconHeight < height) {
            BitmapConfig c = BitmapConfig_ARGB_8888;
            AutoPtr<IBitmap> thumb;
            CBitmap::CreateBitmap(mIconWidth, mIconHeight, c, (IBitmap**)&thumb);
            ICanvas* canvas = mCanvas;
            canvas->SetBitmap(thumb);
            AutoPtr<IRect> rect;
            icon->GetBounds((IRect**)&rect);
            mOldBounds->Set(rect);
            Int32 x = (width - iconWidth) / 2;
            Int32 y = (height - iconHeight) / 2;
            icon->SetBounds(x, y, x + iconWidth, y + iconHeight);
            icon->Draw(canvas);
            icon->SetBounds(mOldBounds);

            icon = NULL;
            CBitmapDrawable::New(resources, thumb, (IDrawable**)&icon);
            canvas->SetBitmap(NULL);
        }
    }

    *thumbnail = icon;
    REFCOUNT_ADD(*thumbnail)
    return NOERROR;
}

//==========================================================================
// LauncherActivity::CancelButtonOnClickListener
//==========================================================================
CAR_INTERFACE_IMPL(LauncherActivity::CancelButtonOnClickListener, Object, IViewOnClickListener)

LauncherActivity::CancelButtonOnClickListener::CancelButtonOnClickListener(
    /* [in] */ LauncherActivity* host)
    : mHost(host)
{}

ECode LauncherActivity::CancelButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->Finish();
    return NOERROR;
}


//==========================================================================
// LauncherActivity::ActivityAdapter
//==========================================================================
CAR_INTERFACE_IMPL(LauncherActivity::ActivityAdapter, BaseAdapter, IFilterable)

LauncherActivity::ActivityAdapter::ActivityAdapter(
    /* [in] */ ILauncherActivityIconResizer* resizer,
    /* [in] */ LauncherActivity* host)
    : mShowIcons(FALSE)
    , mHost(host)
{
    mIconResizer = resizer;
    AutoPtr<IInterface> obj;
    mHost->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    mShowIcons = mHost->OnEvaluateShowIcons();
    mActivitiesList = mHost->MakeListItems();
}

ECode LauncherActivity::ActivityAdapter::IntentForPosition(
    /* [in] */ Int32 position,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mActivitiesList == NULL) {
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(mHost->mIntent, (IIntent**)&intent);
    AutoPtr<IInterface> obj;
    mActivitiesList->Get(position, (IInterface**)&obj);
    ILauncherActivityListItem* item = ILauncherActivityListItem::Probe(obj);
    String packageName, className;
    item->GetPackageName(&packageName);
    item->GetClassName(&className);
    intent->SetClassName(packageName, className);
    AutoPtr<IBundle> extras;
    item->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        intent->PutExtras(extras);
    }

    *result = intent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LauncherActivity::ActivityAdapter::ItemForPosition(
    /* [in] */ Int32 position,
    /* [out] */ ILauncherActivityListItem** item)
{
    VALIDATE_NOT_NULL(item)
    *item = NULL;
    if (mActivitiesList.Get() == NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    mActivitiesList->Get(position, (IInterface**)&obj);
    *item = ILauncherActivityListItem::Probe(obj);
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode LauncherActivity::ActivityAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    if (mActivitiesList != NULL) {
        mActivitiesList->GetSize(count);
    }
    return NOERROR;
}

ECode LauncherActivity::ActivityAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out]　*/ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> obj = CoreUtils::Convert(position);
    *result = obj.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LauncherActivity::ActivityAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = position;
    return position;
}

ECode LauncherActivity::ActivityAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    if (convertView == NULL) {
        mInflater->Inflate(
            R::layout::activity_list_item_2, parent, FALSE, (IView**)&view);
    }
    else {
        view = convertView;
    }

    AutoPtr<ILauncherActivityListItem> item;
    ItemForPosition(position, (ILauncherActivityListItem**)&item);
    BindView(view, item);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LauncherActivity::ActivityAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    if (mFilter == NULL) {
        mFilter = new ArrayFilter(this);
    }
    *filter = mFilter;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

void LauncherActivity::ActivityAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ ILauncherActivityListItem* item)
{
    AutoPtr<ITextView> text = ITextView::Probe(view);
    AutoPtr<ICharSequence> label;
    item->GetLabel((ICharSequence**)&label);
    text->SetText(label);
    if (mShowIcons) {
        AutoPtr<IDrawable> icon;
        item->GetIcon((IDrawable**)&icon);
        if (icon == NULL) {
            AutoPtr<IPackageManager> pm;
            mHost->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IResolveInfo> ri;
            item->GetResolveInfo((IResolveInfo**)&ri);
            AutoPtr<IDrawable> tmp;
            ri->LoadIcon(pm, (IDrawable**)&tmp);
            mIconResizer->CreateIconThumbnail(tmp, (IDrawable**)&icon);
        }
        text->SetCompoundDrawablesWithIntrinsicBounds(icon, NULL, NULL, NULL);
    }
}

//==========================================================================
// LauncherActivity::ArrayFilter
//==========================================================================

LauncherActivity::ArrayFilter::ArrayFilter(
    /* [in] */ LauncherActivity::ActivityAdapter* host)
    : mHost(host)
{}

ECode LauncherActivity::ArrayFilter::PerformFiltering(
    /* [in] */ ICharSequence* prefix,
    /* [out] */ IFilterResults** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IFilterResults> results;
    assert(0 && "TODO");
    // CFilterResults::New((IFilterResults**)&results);

    if (mHost->mOriginalValues == NULL) {
        Object& obj = mHost->mLock;
        synchronized(obj) {
            CArrayList::New((IArrayList**)&mHost->mOriginalValues);
        }
    }

    Int32 length = 0;
    if (prefix != NULL) {
        prefix->GetLength(&length);
    }

    if (length == 0) {
        Object& obj = mHost->mLock;
        synchronized(obj) {
            AutoPtr<IArrayList> list;
            CArrayList::New(ICollection::Probe(mHost->mOriginalValues), (IArrayList**)&list);

            results->SetValues(TO_IINTERFACE(list));
            Int32 size;
            list->GetSize(&size);
            results->SetCount(size);
        }
    }
    else {

        String prefixString = Object::ToString(prefix).ToLowerCase();

        AutoPtr<IArrayList> values = mHost->mOriginalValues;
        Int32 count;
        values->GetSize(&count);

        AutoPtr<IArrayList> newValues;
        CArrayList::New(count, (IArrayList**)&newValues);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            values->Get(i, (IInterface**)&obj);
            ILauncherActivityListItem* item = ILauncherActivityListItem::Probe(obj);
            AutoPtr<ICharSequence> label;
            item->GetLabel((ICharSequence**)&label);
            String labelStr = Object::ToString(label).ToLowerCase();
            AutoPtr<ArrayOf<String> > words;
            StringUtils::Split(labelStr, String(" "), (ArrayOf<String>**)&words);
            Int32 wordCount = words->GetLength();

            for (Int32 k = 0; k < wordCount; k++) {
                String word = (*words)[k];

                if (word.StartWith(prefixString)) {
                    newValues->Add(item);
                    break;
                }
            }
        }

        results->SetValues(TO_IINTERFACE(newValues));
        Int32 size;
        newValues->GetSize(&size);
        results->SetCount(size);
    }

    *result = results;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LauncherActivity::ArrayFilter::PublishResults(
    /* [in] */ ICharSequence* constraint,
    /* [in] */ IFilterResults* results)
{
    //noinspection unchecked
    AutoPtr<IInterface> obj;
    results->GetValues((IInterface**)&obj);
    mHost->mActivitiesList = IList::Probe(obj);
    Int32 count;
    results->GetCount(&count);
    if (count > 0) {
        mHost->NotifyDataSetChanged();
    }
    else {
        mHost->NotifyDataSetInvalidated();
    }
    return NOERROR;
}

//==========================================================================
// LauncherActivity
//==========================================================================

CAR_INTERFACE_IMPL(LauncherActivity, ListActivity, ILauncherActivity)

LauncherActivity::LauncherActivity()
{}

LauncherActivity::~LauncherActivity()
{}

ECode LauncherActivity::constructor()
{
    return NOERROR;
}

ECode LauncherActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(ListActivity::OnCreate(icicle))

    GetPackageManager((IPackageManager**)&mPackageManager);

    Boolean bval;
    mPackageManager->HasSystemFeature(IPackageManager::FEATURE_WATCH, &bval);
    if (!bval) {
        RequestWindowFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS, &bval);
        SetProgressBarIndeterminateVisibility(TRUE);
    }
    OnSetContentView();

    CLauncherActivityIconResizer::New(this, (ILauncherActivityIconResizer**)&mIconResizer);

    AutoPtr<IIntent> tgtIntent = GetTargetIntent();
    CIntent::New(tgtIntent, (IIntent**)&mIntent);
    mIntent->SetComponent(NULL);
    mAdapter = new ActivityAdapter(mIconResizer, this);

    SetListAdapter(mAdapter);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAbsListView::Probe(listView)->SetTextFilterEnabled(TRUE);

    UpdateAlertTitle();
    UpdateButtonText();

    mPackageManager->HasSystemFeature(IPackageManager::FEATURE_WATCH, &bval);
    if (!bval) {
        SetProgressBarIndeterminateVisibility(FALSE);
    }
    return NOERROR;
}

void LauncherActivity::UpdateAlertTitle()
{
    AutoPtr<IView> view;
    FindViewById(R::id::alertTitle, (IView**)&view);
    ITextView* alertTitle = ITextView::Probe(view);
    if (alertTitle != NULL) {
        AutoPtr<ICharSequence> title;
        GetTitle((ICharSequence**)&title);
        alertTitle->SetText(title);
    }
}

void LauncherActivity::UpdateButtonText()
{
    AutoPtr<IView> view;
    FindViewById(R::id::button1, (IView**)&view);
    IButton* cancelButton = IButton::Probe(view);
    if (cancelButton != NULL) {
        AutoPtr<IViewOnClickListener> listener = new CancelButtonOnClickListener(this);
        view->SetOnClickListener(listener);
    }
}

ECode LauncherActivity::SetTitle(
    /* [in] */ ICharSequence* title)
{
    ListActivity::SetTitle(title);
    UpdateAlertTitle();
    return NOERROR;
}

ECode LauncherActivity::SetTitle(
    /* [in] */ Int32 titleId)
{
    ListActivity::SetTitle(titleId);
    UpdateAlertTitle();
    return NOERROR;
}

ECode LauncherActivity::OnSetContentView()
{
    SetContentView(R::layout::activity_list);
    return NOERROR;
}

ECode LauncherActivity::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ long id)
{
    AutoPtr<IIntent> intent = IntentForPosition(position);
    StartActivity(intent);
    return NOERROR;
}

AutoPtr<IIntent> LauncherActivity::IntentForPosition(
    /* [in] */ Int32 position)
{
    ActivityAdapter* adapter = (ActivityAdapter*) mAdapter.Get();
    AutoPtr<IIntent> intent;
    adapter->IntentForPosition(position, (IIntent**)&intent);
    return intent;
}

AutoPtr<ILauncherActivityListItem> LauncherActivity::ItemForPosition(
    /* [in] */ Int32 position)
{
    ActivityAdapter* adapter = (ActivityAdapter*) mAdapter.Get();
    AutoPtr<ILauncherActivityListItem> item;
    adapter->ItemForPosition(position, (ILauncherActivityListItem**)&item);
    return item;
}

AutoPtr<IIntent> LauncherActivity::GetTargetIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    return NOERROR;
}

AutoPtr<IList> LauncherActivity::OnQueryPackageManager(
    /* [in] */ IIntent* queryIntent)
{
    AutoPtr<IList> list;
    mPackageManager->QueryIntentActivities(queryIntent, /* no flags */ 0, (IList**)&list);
    return list;
}

ECode LauncherActivity::OnSortResultList(
    /* [in] */ IList* results)
{
    AutoPtr<IComparator> comparator;
    CResolveInfoDisplayNameComparator::New(mPackageManager, (IComparator**)&comparator);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(results, comparator);
    return NOERROR;
}

AutoPtr<IList> LauncherActivity::MakeListItems()
{
    // Load all matching activities and sort correctly
    AutoPtr<IList> list = OnQueryPackageManager(mIntent);
    OnSortResultList(list);

    Int32 listSize;
    list->GetSize(&listSize);
    AutoPtr<IArrayList> result;
    CArrayList::New(listSize, (IArrayList**)&result);

    for (Int32 i = 0; i < listSize; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<ILauncherActivityListItem> listItem;
        CLauncherActivityListItem::New(mPackageManager, resolveInfo, NULL,
            (ILauncherActivityListItem**)&listItem);
        result->Add(listItem.Get());
    }

    return IList::Probe(result);
}

Boolean LauncherActivity::OnEvaluateShowIcons()
{
    return TRUE;
}



} // namespace App
} // namespace Droid
} // namespace Elastos
