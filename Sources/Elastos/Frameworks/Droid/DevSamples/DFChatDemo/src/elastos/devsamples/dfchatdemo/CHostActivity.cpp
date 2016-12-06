#include "CHostActivity.h"
#include "CChatApplication.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DFChatDemo {

static const String TAG("DFChatDemo.CHostActivity");

//========================================================
// CHostActivity::MyHandler
//========================================================
CHostActivity::MyHandler::MyHandler()
    : mHost(NULL)
{}

ECode CHostActivity::MyHandler::constructor(
    /* [in] */ CHostActivity* host)
{
    mHost = host;
    return NOERROR;
}

ECode CHostActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

//========================================================
// CHostActivity
//========================================================
const Int32 CHostActivity::DIALOG_ALLJOYN_ERROR_ID = 3;
const Int32 CHostActivity::DIALOG_SET_NAME_ID = 0;
const Int32 CHostActivity::DIALOG_START_ID = 1;
const Int32 CHostActivity::DIALOG_STOP_ID = 2;
const Int32 CHostActivity::HANDLE_APPLICATION_QUIT_EVENT = 0;
const Int32 CHostActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT = 1;
const Int32 CHostActivity::HANDLE_ALLJOYN_ERROR_EVENT = 2;

CHostActivity::CHostActivity()
    : mChatApplication(NULL)
{}

CHostActivity::~CHostActivity()
{}

ECode CHostActivity::constructor()
{
    return Activity::constructor();
}

ECode CHostActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, "onCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::host);

    return NOERROR;
}

ECode CHostActivity::OnDestroy()
{
    return Activity::OnDestroy();
}

ECode CHostActivity::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    return NOERROR;
}

AutoPtr<IDialog> CHostActivity::OnCreateDialog(
    /* [in] */ Int32 id)
{
    AutoPtr<IDialog> dialog;
    return dialog;
}

void CHostActivity::UpdateChannelState()
{
}

void CHostActivity::AlljoynError()
{
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos