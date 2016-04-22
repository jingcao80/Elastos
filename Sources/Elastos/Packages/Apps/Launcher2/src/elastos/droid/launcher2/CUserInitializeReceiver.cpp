
#include "elastos/droid/launcher2/CUserInitializeReceiver.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CARAPI CUserInitializeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    // Context.getPackageName() may return the "original" package name,
    // com.android.launcher2; Resources needs the real package name,
    // com.android.launcher. So we ask Resources for what it thinks the
    // package name should be.
    String packageName;
    resources->GetResourcePackageName(Elastos::Droid::Launcher2::R::array::wallpapers, &packageName);
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AddWallpapers(resources, packageName, Elastos::Droid::Launcher2::R::array::wallpapers, list);
    AddWallpapers(resources, packageName, Elastos::Droid::Launcher2::R::array::extra_wallpapers, list);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WALLPAPER_SERVICE, (IInterface**)&obj);
    AutoPtr<IWallpaperManager> wpm = IWallpaperManager::Probe(obj);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 1; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 resid;
        intObj->GetValue(&resid);
        Boolean res;
        wpm->HasResourceWallpaper(resid, &res);
        if (!res) {
            //try {
            ECode ec = wpm->SetResource(resid);
            //} catch (IOException e) {
            if (ec == (ECode)E_IO_EXCEPTION) {
                ;
            }
            //}
            return NOERROR;
        }
    }
    return NOERROR;
}

void CUserInitializeReceiver::AddWallpapers(
    /* [in] */ IResources* resources,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resid,
    /* [in] */ IArrayList* outList)
{
    AutoPtr<ArrayOf<String> > extras;
    resources->GetStringArray(resid, (ArrayOf<String>**)&extras);

    for (Int32 i = 0; i < extras->GetLength(); i++) {
        String extra = (*extras)[i];

        Int32 res;
        resources->GetIdentifier(extra, String("drawable"), packageName, &res);
        if (res != 0) {
            AutoPtr<IInteger32> obj = CoreUtils::Convert(res);
            outList->Add(TO_IINTERFACE(obj));
        }
    }
    return;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos