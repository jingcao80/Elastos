
#include "elastos/droid/systemui/statusbar/phone/CMultiUserSwitch.h"
#include "elastos/droid/systemui/statusbar/policy/UserSwitcherController.h"
#include "../R.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Provider::CContactsContractQuickContact;
using Elastos::Droid::Provider::CContactsContractProfile;
using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserSwitcherController;
using Elastos::Droid::SystemUI::StatusBar::Policy::UserSwitcherController;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_INTERFACE_IMPL(CMultiUserSwitch::ViewOnClickListener, Object, IViewOnClickListener)
CMultiUserSwitch::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ CMultiUserSwitch* host)
    : mHost(host)
{}

ECode CMultiUserSwitch::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

CAR_OBJECT_IMPL(CMultiUserSwitch)

CAR_INTERFACE_IMPL(CMultiUserSwitch, FrameLayout, IMultiUserSwitch);

CMultiUserSwitch::CMultiUserSwitch()
    : mKeyguardMode(FALSE)
{}

ECode CMultiUserSwitch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);

    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    helper->Get(ctx, (IUserManager**)&mUserManager);
    return NOERROR;
}

ECode CMultiUserSwitch::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<ViewOnClickListener> listener = new ViewOnClickListener(this);
    SetOnClickListener(listener);
    return NOERROR;
}

ECode CMultiUserSwitch::SetQsPanel(
    /* [in] */ IQSPanel* qsPanel)
{
    mQsPanel = qsPanel;
    return NOERROR;
}

ECode CMultiUserSwitch::SetKeyguardUserSwitcher(
    /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher)
{
    mKeyguardUserSwitcher = keyguardUserSwitcher;
    return NOERROR;
}

ECode CMultiUserSwitch::SetKeyguardMode(
    /* [in] */ Boolean keyguardShowing)
{
    mKeyguardMode = keyguardShowing;
    return NOERROR;
}

ECode CMultiUserSwitch::OnClick(
    /* [in] */ IView* v)
{
    if (OpensUserSwitcherWhenClicked()) {
        if (mKeyguardMode) {
            if (mKeyguardUserSwitcher != NULL) {
                mKeyguardUserSwitcher->Show(TRUE /* animate */);
            }
        }
        else {
            if (mQsPanel != NULL) {
                AutoPtr<IPhoneQSTileHost> pt;
                mQsPanel->GetHost((IPhoneQSTileHost**)&pt);
                AutoPtr<IUserSwitcherController> usc;
                pt->GetUserSwitcherController((IUserSwitcherController**)&usc);
                mQsPanel->ShowDetailAdapter(TRUE, ((UserSwitcherController*)usc.Get())->mUserDetailAdapter);
            }
        }
    }
    else {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IContactsContractQuickContact> ccqc;
        CContactsContractQuickContact::AcquireSingleton((IContactsContractQuickContact**)&ccqc);
        AutoPtr<IContactsContractProfile> ccp;
        CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&ccp);
        AutoPtr<IUri> uri;
        ccp->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<IIntent> intent;
        ccqc->ComposeQuickContactsIntent(ctx, v, uri, IContactsContractQuickContact::MODE_LARGE
                , NULL, (IIntent**)&intent);

        AutoPtr<IUserHandle> uh;
        CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&uh);
        ctx->StartActivityAsUser(intent, uh);
    }
    return NOERROR;
}

ECode CMultiUserSwitch::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnPopulateAccessibilityEvent(event);

    Boolean tmp = FALSE;
    if (IsClickable(&tmp), tmp) {
        String text;
        if (OpensUserSwitcherWhenClicked()) {
            String currentUser;
            if (mQsPanel != NULL) {
                AutoPtr<IPhoneQSTileHost> pt;
                mQsPanel->GetHost((IPhoneQSTileHost**)&pt);
                AutoPtr<IUserSwitcherController> controller;
                pt->GetUserSwitcherController((IUserSwitcherController**)&controller);
                controller->GetCurrentUserName(mContext, &currentUser);
            }
            if (TextUtils::IsEmpty(currentUser)) {
                mContext->GetString(R::string::accessibility_multi_user_switch_switcher, &text);
            }
            else {
                AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                AutoPtr<ICharSequence> obj;
                CString::New(currentUser, (ICharSequence**)&obj);
                args->Set(0, obj);
                mContext->GetString(R::string::accessibility_multi_user_switch_switcher_with_current,
                        args, &text);
            }
        }
        else {
            mContext->GetString(R::string::accessibility_multi_user_switch_quick_contact, &text);
        }
        if (!TextUtils::IsEmpty(text)) {
            AutoPtr<IList> container;
            IAccessibilityRecord::Probe(event)->GetText((IList**)&container);
            AutoPtr<ICharSequence> obj;
            CString::New(text, (ICharSequence**)&obj);
            container->Add(obj);
        }
    }
    return NOERROR;
}

ECode CMultiUserSwitch::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

Boolean CMultiUserSwitch::OpensUserSwitcherWhenClicked()
{
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);

    AutoPtr<IUserManager> um;
    helper->Get(ctx, (IUserManager**)&um);
    Boolean bv1 = FALSE, bv2 = FALSE;
    helper->SupportsMultipleUsers(&bv1);
    return bv1 && (um->IsUserSwitcherEnabled(&bv2), bv2);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
