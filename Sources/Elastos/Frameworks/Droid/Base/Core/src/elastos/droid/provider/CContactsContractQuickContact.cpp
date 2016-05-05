
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/provider/CContactsContractQuickContact.h"
#include "elastos/droid/widget/Toast.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractQuickContact)

CAR_INTERFACE_IMPL(CContactsContractQuickContact, Singleton, IContactsContractQuickContact)

ECode CContactsContractQuickContact::ComposeQuickContactsIntent(
    /* [in] */ IContext* context,
    /* [in] */ IView* target,
    /* [in] */ IUri* lookupUri,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* excludeMimes,
    /* [out] */ IIntent** intent)
{
    // Find location and bounds of target view, adjusting based on the
    // assumed local density.
    Float appScale;
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    AutoPtr<ICompatibilityInfo> ccInfo;
    FAIL_RETURN(res->GetCompatibilityInfo((ICompatibilityInfo**)&ccInfo))
    FAIL_RETURN(ccInfo->GetApplicationScale(&appScale))
    AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
    FAIL_RETURN(target->GetLocationOnScreen(pos.Get()))

    AutoPtr<IRect> rect;
    FAIL_RETURN(CRect::New((IRect**)&rect))
    FAIL_RETURN(rect->SetLeft((Int32)((*pos)[0] * appScale + 0.5f)))
    FAIL_RETURN(rect->SetTop((Int32) ((*pos)[1] * appScale + 0.5f)))
    Int32 width;
    FAIL_RETURN(target->GetWidth(&width))
    FAIL_RETURN(rect->SetRight((Int32) (((*pos)[0] + width) * appScale + 0.5f)))
    Int32 height;
    FAIL_RETURN(target->GetWidth(&height))
    FAIL_RETURN(rect->SetBottom((Int32) (((*pos)[1] + height) * appScale + 0.5f)))

    return ComposeQuickContactsIntent(context, rect, lookupUri, mode, excludeMimes, intent);
}

ECode CContactsContractQuickContact::ComposeQuickContactsIntent(
    /* [in] */ IContext* context,
    /* [in] */ IRect* target,
    /* [in] */ IUri* lookupUri,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* excludeMimes,
    /* [out] */ IIntent** intent)
{
    // When launching from an Activiy, we don't want to start a new task, but otherwise
    // we *must* start a new task.  (Otherwise startActivity() would crash.)
    AutoPtr<IContext> actualContext = context;
    AutoPtr<IContextWrapper> cw = IContextWrapper::Probe(actualContext);
    AutoPtr<IActivity> act = IActivity::Probe(actualContext);
    while (cw != NULL && act == NULL) {
        actualContext = NULL;
        cw->GetBaseContext((IContext**)&actualContext);
        cw = IContextWrapper::Probe(actualContext);
        act = IActivity::Probe(actualContext);
    }
    const Int32 intentFlags = (act != NULL)
            ? IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET
            : IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK;

    // Launch pivot dialog through intent for now
    AutoPtr<IIntent> _intent;
    FAIL_RETURN(CIntent::New(IContactsContractQuickContact::ACTION_QUICK_CONTACT, (IIntent**)&_intent))
    FAIL_RETURN(_intent->AddFlags(intentFlags))

    FAIL_RETURN(_intent->SetData(lookupUri))
    FAIL_RETURN(_intent->SetSourceBounds(target))
    FAIL_RETURN(_intent->PutExtra(IContactsContractQuickContact::EXTRA_MODE, mode))
    FAIL_RETURN(_intent->PutExtra(IContactsContractQuickContact::EXTRA_EXCLUDE_MIMES, excludeMimes))
    *intent =  _intent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CContactsContractQuickContact::ShowQuickContact(
    /* [in] */ IContext* context,
    /* [in] */ IView* target,
    /* [in] */ IUri* lookupUri,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* excludeMimes)
{
    // Trigger with obtained rectangle
    AutoPtr<IIntent> intent;
    FAIL_RETURN(ComposeQuickContactsIntent(context, target, lookupUri, mode, excludeMimes, (IIntent**)&intent))
    StartActivityWithErrorToast(context, intent);
    return NOERROR;
}

ECode CContactsContractQuickContact::ShowQuickContact(
    /* [in] */ IContext* context,
    /* [in] */ IRect* target,
    /* [in] */ IUri* lookupUri,
    /* [in] */ Int32 mode,
    /* [in] */ ArrayOf<String>* excludeMimes)
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(ComposeQuickContactsIntent(context, target, lookupUri, mode, excludeMimes, (IIntent**)&intent))
    StartActivityWithErrorToast(context, intent);
    return NOERROR;
}

void CContactsContractQuickContact::StartActivityWithErrorToast(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // try {
      ECode ec = context->StartActivity(intent);
    // } catch (ActivityNotFoundException e) {
      if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        AutoPtr<IToast> t;
        Toast::MakeText(context, 0/*com.android.internal.R.string.quick_contacts_not_available*/,
                        IToast::LENGTH_SHORT, (IToast**)&t);
        t->Show();
      }
    // }
}

} //Provider
} //Droid
} //Elastos