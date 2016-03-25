
#include "CActivityTwo.h"
#include "R.h"
#include <elautoptr.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IActionProvider;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ActivityTest {

CActivityTwo::MyListener::MyListener(
    /* [in] */ CActivityTwo* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityTwo::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CActivityTwo::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityTwo::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityTwo::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityTwo::MyListener::OnClick(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CActivityTwo::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main2);

    mReceiveText = ITextView::Probe(FindViewById(R::id::receivetext));
    assert(mReceiveText != NULL);
Slogger::D("CActivityTwo", "===============================OnCreate===========1");
    // Get intent, action and MIME type
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    String type;
    intent->GetType(&type);
Slogger::D("CActivityTwo", "===============================OnCreate===========2");
    if (IIntent::ACTION_SEND.Equals(action) && type != NULL) {
        if (String("text/plain").Equals(type)) {
            HandleSendText(intent); // Handle text being sent
        } else if (type.StartWith("image/")) {
            // HandleSendImage(intent); // Handle single image being sent
        }
    } else if (IIntent::ACTION_SEND_MULTIPLE.Equals(action) && type != NULL) {
        if (type.StartWith("image/")) {
            // HandleSendMultipleImages(intent); // Handle multiple images being sent
        }
    } else {
        // Handle other intents, such as being started from the home screen
    }
Slogger::D("CActivityTwo", "===============================OnCreate===========4");
    return NOERROR;
}

ECode CActivityTwo::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    // Inflate menu resource file.
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::share_menu, menu);

    // Locate MenuItem with ShareActionProvider
    AutoPtr<IMenuItem> item;
    menu->FindItem(R::id::menu_item_share, (IMenuItem**)&item);
    assert(item != NULL);

    // Fetch and store ShareActionProvider
    item->GetActionProvider((IActionProvider**)&mShareActionProvider);

    // Return true to display menu
    *allowToShow = TRUE;
    return NOERROR;
}

// Call to update the share intent
void CActivityTwo::SetShareIntent(
    /* [in] */ IIntent* shareIntent)
{
    if (mShareActionProvider != NULL) {
        mShareActionProvider->SetShareIntent(shareIntent);
    }
}

void CActivityTwo::HandleSendText(
    /* [in] */ IIntent* intent)
{
    String sharedText;
    intent->GetStringExtra(IIntent::EXTRA_TEXT, &sharedText);
Slogger::D("CActivityTwo", "HandleSendText=====================================,sharedText=[%s]",sharedText.string());
    if (sharedText != NULL) {
        // Update UI to reflect text being shared
        AutoPtr<ICharSequence> text;
        CStringWrapper::New(sharedText, (ICharSequence**)&text);
        mReceiveText->SetText(text);
        mReceiveText->SetTextSize(30);
        mReceiveText->SetTextColor(IColor::RED);
    }
}

ECode CActivityTwo::OnStart()
{
Slogger::D("CActivityTwo", "=============================================OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnResume()
{
Slogger::D("CActivityTwo", "=============================================OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnPause()
{
Slogger::D("CActivityTwo", "=============================================OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnStop()
{
Slogger::D("CActivityTwo", "=============================================OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnDestroy()
{
Slogger::D("CActivityTwo", "=============================================OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace ActivityTest
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
