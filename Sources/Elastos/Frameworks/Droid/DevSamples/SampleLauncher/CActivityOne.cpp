
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

// #include "../../Recorder/Recorder.h"
// #include <elastos/System.h>
// #include <elastos/core/StringUtils.h>
// using Elastos::Core::System;
// using Elastos::Core::StringUtils;

using Elastos::Core::CStringWrapper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SampleLauncher {

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
        return (PInterface)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return this;
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

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IAdapterViewOnItemClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<ITextView> item = ITextView::Probe(view);
    assert(item != NULL);
    AutoPtr<ICharSequence> text;
    item->GetText((ICharSequence**) &text);
    String pkg;
    text->ToString(&pkg);
    Slogger::D("SampleLauncher", "OnItemClick=======================pkg=[%s]", pkg.string());
    if (pkg.Equals("reboot")) {
        return mHost->Reboot();
    }

    List<AutoPtr<IResolveInfo> >::Iterator ator = mHost->mApps.Begin();
    for (; ator != mHost->mApps.End(); ++ator) {
        AutoPtr<IResolveInfo> info = *ator;
        assert(info != NULL);

        AutoPtr<IActivityInfo> aInfo;
        info->GetActivityInfo((IActivityInfo**)&aInfo);
        assert(aInfo != NULL);
        String appPkg;
        aInfo->GetPackageName(&appPkg);
        if (pkg.Equals(appPkg)) {
            //Start app.
            String appName;
            aInfo->GetName(&appName);
            AutoPtr<IComponentName> cn;
            CComponentName::New(appPkg, appName, (IComponentName**)&cn);

            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetComponent(cn);
            mHost->StartActivity(intent);
        }
    }

    return NOERROR;
}

ECode CActivityOne::Reboot()
{
    AutoPtr<IInterface> tmpObj;
    GetSystemService(IContext::POWER_SERVICE, (IInterface**)&tmpObj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(tmpObj.Get());

    // AutoPtr<ISystem> system;
    // Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // Int64 end;
    // system->GetCurrentTimeMillis(&end);
//     Int64 start = Recorder::GetStart();
//     String recorder = String("end time:[") + StringUtils::Int64ToString(end) + String("]\n\n");
//     Recorder::WriteRecord(recorder.string(), 0);

// Slogger::D("SampleLauncher", "Reboot=======================recorder=[%s], start=[%s]",
// recorder.string(),StringUtils::Int64ToString(start).string());

    return pm->Reboot(FALSE, String("reboot"), TRUE);
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::myListView);
    assert(view != NULL);
    mListView = IListView::Probe(view);
    assert(mListView != NULL);

    //1. Get App info.
    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(GetPackageManager((IPackageManager**)&pm));

    AutoPtr<IIntent> mainIntent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**) &mainIntent);
    mainIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<IObjectContainer> resolves;
    pm->QueryIntentActivities(mainIntent, 0, (IObjectContainer**) &resolves);

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    AutoPtr<IObjectEnumerator> enumerator;
    resolves->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> info;
        enumerator->Current((IInterface**)&info);
        AutoPtr<IResolveInfo> resInfo = IResolveInfo::Probe(info);

        //Generate list item string.
        AutoPtr<IActivityInfo> aInfo;
        resInfo->GetActivityInfo((IActivityInfo**) &aInfo);
        assert(aInfo != NULL);
        String pkg;
        aInfo->GetPackageName(&pkg);
        if (!pkg.Equals("SampleLauncher")) {
Slogger::D("SampleLauncher", "OnCreate=======================pkg=[%s]", pkg.string());
            AutoPtr<ICharSequence> seq;
            CStringWrapper::New(pkg, (ICharSequence**) &seq);
            dataList->Add(seq->Probe(EIID_IInterface));
            mApps.PushBack(resInfo);
        }
    }

    //For test: reboot
    AutoPtr<ICharSequence> rb;
    CStringWrapper::New(String("reboot"), (ICharSequence**) &rb);
    dataList->Add(rb->Probe(EIID_IInterface));

    //2. Make adapter
    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    mListView->SetAdapter(adapter.Get());

    AutoPtr<MyListener> l = new MyListener(this);
    IAdapterViewOnItemClickListener* itemClickListener = (IAdapterViewOnItemClickListener*)(l->Probe(EIID_IAdapterViewOnItemClickListener));

    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mListView->SetDivider(drawable);
    mListView->SetDividerHeight(1);
    mListView->SetOnItemClickListener(itemClickListener);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // return Reboot();
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

} // namespace SampleLauncher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
