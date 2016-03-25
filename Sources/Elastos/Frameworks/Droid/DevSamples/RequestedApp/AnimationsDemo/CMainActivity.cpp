
#include "CMainActivity.h"
#include <R.h>
#include "R.h"
#include "CSample.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

AutoPtr<IObjectContainer> CMainActivity::mSamples;

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&mSamples);

    String pkgName;
    GetPackageName(&pkgName);

    // Instantiate the list of samples.
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    String path = String("/data/elastos/") + pkgName + String(".eco");
    CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    assert(moduleInfo != NULL);

    moduleInfo->GetClassInfo(String("CCrossfadeActivity"), (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    AutoPtr<ICharsequence> data;
    CSample::New(this, R::string::title_crossfade, classInfo, (ICharsequence**)&data);
    mSamples->Add(data);
    classInfo->Release();
    data->Release();
    // moduleInfo->GetClassInfo(String("CZoomActivity"), (IClassInfo**)&classInfo);
    // assert(classInfo != NULL);
    // data = new Sample(this, R::string::title_zoom, classInfo);
    // mSamples->Add(data);
    // classInfo->Release();
    // data->Release();

    moduleInfo->GetClassInfo(String("CLayoutChangesActivity"), (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    CSample::New(this, R::string::title_layout_changes, classInfo, (ICharsequence**)&data);
    mSamples->Add(data->Probe(EIID_IInterface));

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this,
                Elastos::Droid::R::layout::simple_list_item_1,
                Elastos::Droid::R::id::text1,
                mSamples, (IArrayAdapter**)&adapter);

    return SetListAdapter(adapter);
}

ECode CMainActivity::OnListItemClick(
    /* [in] */ IListView* listView,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
Slogger::D("CMainActivity", "OnListItemClick, position=[%d]=========1", position);
    assert(mSamples != NULL);
    AutoPtr<IObjectEnumerator> ator;
    mSamples->GetObjectEnumerator((IObjectEnumerator**)&ator);
    Boolean hasNext = FALSE;
    Int32 pos = 0;
    AutoPtr<IIntent> intent;
    while (ator->MoveNext(&hasNext), hasNext) {
Slogger::D("CMainActivity", "OnListItemClick, pos=[%d]=========2", pos);
        if (pos == position) {
            AutoPtr<IInterface> obj;
            ator->Current((IInterface**)&obj);
            AutoPtr<ICharsequence> data = ICharsequence::Probe(obj);
            assert(data != NULL);

            ClassID id;
            id.pUunm = (char*)malloc(80);
            ((CSample*)data.Get())->mActivityClass->GetId(&id);
            CIntent::New(this, id, (IIntent**)&intent);
            free(id.pUunm);

            break;
        }
        pos++;
    }
Slogger::D("CMainActivity", "OnListItemClick, position=[%d]=========3", position);
    assert(intent != NULL);
    // Launch the sample associated with this list position.
    return StartActivity(intent);
}

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
