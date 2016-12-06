#include "CUseActivity.h"
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

static const String TAG("DFChatDemo.CUseActivity");

//========================================================
// CUseActivity::MyHandler
//========================================================

CUseActivity::MyHandler::MyHandler()
    : mHost(NULL)
{}

ECode CUseActivity::MyHandler::constructor(
    /* [in] */ CUseActivity* host)
{
    mHost = host;
    return NOERROR;
}

ECode CUseActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

//========================================================
// CUseActivity
//========================================================
const Int32 CUseActivity::DIALOG_JOIN_ID = 0;
const Int32 CUseActivity::DIALOG_LEAVE_ID = 1;
const Int32 CUseActivity::DIALOG_ALLJOYN_ERROR_ID = 2;

const Int32 CUseActivity::HANDLE_APPLICATION_QUIT_EVENT = 0;
const Int32 CUseActivity::HANDLE_HISTORY_CHANGED_EVENT = 1;
const Int32 CUseActivity::HANDLE_CHANNEL_STATE_CHANGED_EVENT = 2;
const Int32 CUseActivity::HANDLE_ALLJOYN_ERROR_EVENT = 3;

CUseActivity::CUseActivity()
    : mChatApplication(NULL)
{}

CUseActivity::~CUseActivity()
{}

ECode CUseActivity::constructor()
{
    return Activity::constructor();
}

ECode CUseActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, "onCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::use);

    return NOERROR;
}

ECode CUseActivity::OnDestroy()
{
    return Activity::OnDestroy();
}

ECode CUseActivity::Update(
    /* [in] */ IObservable* o,
    /* [in] */ IInterface* arg)
{
    return NOERROR;
}

AutoPtr<IDialog> CUseActivity::OnCreateDialog(
    /* [in] */ Int32 id)
{
    AutoPtr<IDialog> dialog;
    return dialog;
}

void CUseActivity::UpdateHistory()
{
}

void CUseActivity::UpdateChannelState()
{
}

void CUseActivity::AlljoynError()
{
}

} // namespace DFChatDemo
} // namespace DevSamples
} // namespace Elastos