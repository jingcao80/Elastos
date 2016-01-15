#include "elastos/droid/view/CDispList.h"
#include "elastos/droid/view/CDispListHelper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/StringBuffer.h>
#include "elastos/droid/R.h"

using Elastos::Core::StringBuffer;
using Elastos::Core::CInteger32;
using Elastos::Droid::R;
using Elastos::Droid::Widget::EIID_IToast;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 CDispList::MSG_SHOW_ITEM  = 0;
const Int32 CDispList::MSG_SHOW_NEXT  = 1;
const Int32 CDispList::DURATION  = IToast::LENGTH_SHORT;

CAR_INTERFACE_IMPL(CDispList::MyToast, IToast)
IHANDLER_METHODS_IMPL(CDispList, Handler)

CDispList::MyToast::MyToast(
    /* [in] */ IContext* ctx,
    /* [in] */ CDispList* host) : mHost(host)
{
    Toast::Init(ctx);
}

ECode CDispList::MyToast::OnHide()
{
    Logger::D(IDispList::TAG, "on Hide in MyToast. \n");
    AutoPtr<CDispListHelper> helper = mHost->GetHelper();
    AutoPtr<IInteger32> key;
    CInteger32::New(mHost->mCurItem, (IInteger32**)&key);
    HashMap<AutoPtr<IInteger32>, AutoPtr<IDispListDispFormat> >::Iterator it =
        helper->mShortCutFormatMap.Find(key);
    AutoPtr<IDispListDispFormat> value;
    if (it != helper->mShortCutFormatMap.End()) {
        value = it->mSecond;
    }
    mHost->OnHide(value);
    return NOERROR;
}

ECode CDispList::MyToast::IsShowing(
        /* [out] */ Boolean* showing)
{
    return Toast::IsShowing(showing);
}

ECode CDispList::MyToast::Show()
{
    return Toast::Show();
}

ECode CDispList::MyToast::Cancel()
{
    return Toast::Cancel();
}

ECode CDispList::MyToast::SetView(
    /* [in] */ IView* view)
{
    return Toast::SetView(view);
}

ECode CDispList::MyToast::GetView(
    /* [out] */ IView** view)
{
    return Toast::GetView(view);
}

ECode CDispList::MyToast::SetDuration(
    /* [in] */ Int32 duration)
{
    return Toast::SetDuration(duration);
}

ECode CDispList::MyToast::GetDuration(
    /* [out] */ Int32* duration)
{
    return Toast::GetDuration(duration);
}

ECode CDispList::MyToast::SetMargin(
    /* [in] */ Float horizontalMargin,
    /* [in] */ Float verticalMargin)
{
    return Toast::SetMargin(horizontalMargin, verticalMargin);
}

ECode CDispList::MyToast::GetHorizontalMargin(
    /* [out] */ Float* horizontalMargin)
{
    return Toast::GetHorizontalMargin(horizontalMargin);
}

ECode CDispList::MyToast::GetVerticalMargin(
    /* [out] */ Float* verticalMargin)
{
    return Toast::GetVerticalMargin(verticalMargin);
}

ECode CDispList::MyToast::SetGravity(
    /* [in] */ Int32 gravity,
    /* [in] */ Int32 xOffset,
    /* [in] */ Int32 yOffset)
{
    return Toast::SetGravity(gravity, xOffset, yOffset);
}

ECode CDispList::MyToast::GetGravity(
    /* [out] */ Int32* gravity)
{
    return Toast::GetGravity(gravity);
}

ECode CDispList::MyToast::GetXOffset(
    /* [out] */ Int32* xOffset)
{
    return Toast::GetXOffset(xOffset);
}

ECode CDispList::MyToast::GetYOffset(
    /* [out] */ Int32* yOffset)
{
    return Toast::GetYOffset(yOffset);
}

ECode CDispList::MyToast::SetText(
    /* [in] */ Int32 resId)
{
    return Toast::SetText(resId);
}

ECode CDispList::MyToast::SetText(
    /* [in] */ ICharSequence* s)
{
    return Toast::SetText(s);
}


//========================================================================
// CDispList
//========================================================================
CDispList::CDispList()
    : mCurItem(0)
    , mToastIsInited(FALSE)
{
}

PInterface CDispList::Probe(
    /* [in] */ REIID riid)
{
    return _CDispList::Probe(riid);
}

ECode CDispList::constructor(
    /* [in] */ IContext* ctx)
{
    Handler::Init();

    mContext = ctx;
    mCurItem = IDispList::ADVANCED_DISPLAY_TYPE_UNKNOWN;
    // create a toast
    mToast = new MyToast(ctx, this);
    // initToast();
    return NOERROR;
}

ECode CDispList::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case MSG_SHOW_ITEM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IDispListDispFormat* format = IDispListDispFormat::Probe(obj);
            OnShowItem(format);
            break;
        }
        case MSG_SHOW_NEXT: {
            OnShowNext();
            break;
        }
        default: {
            Logger::W(TAG, "unsupported message = %d", what);
            break;
        }
    }
    return NOERROR;
}

ECode CDispList::ShowItem(
        /* [in] */ IDispListDispFormat* item)
{
    InitToast();

    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_SHOW_ITEM, item, (IMessage**)&msg);
    Boolean result;
    return SendMessage(msg, &result);
}

ECode CDispList::ShowNext()
{
    InitToast();
    Boolean result;
    return SendEmptyMessage(MSG_SHOW_NEXT, &result);
}

ECode CDispList::IsShowing(
        /* [out] */ Boolean* showing)
{
    InitToast();
    return mToast->IsShowing(showing);
}

ECode CDispList::OnHide(
        /* [in] */ IDispListDispFormat* item)
{
    Logger::D(IDispList::TAG, "on Hide");
    return NOERROR;
}


ECode CDispList::OnShowItem(
        /* [in] */ IDispListDispFormat* item)
{
    // get a item's string id
    AutoPtr<CDispListHelper> helper = GetHelper();
    Int32 advType;
    helper->GetAdvancedDisplayType(item, &advType);
    AutoPtr<IInteger32> key;
    CInteger32::New(advType, (IInteger32**)&key);

    AutoPtr<IInteger32> str_id;
    HashMap<AutoPtr<IInteger32>, AutoPtr<IInteger32> >::Iterator it =
        helper->mShortCutStrMap.Find(key);
    if(it != helper->mShortCutStrMap.End())
    {
        str_id = it->mSecond;
    }
    if( str_id == NULL ){
        String itemStr;
        item->ToString(&itemStr);
        StringBuffer sb;
        sb += "Fail in searching item = ";
        sb += itemStr;
        sb.ToString(&itemStr);
        Logger::D(IDispList::TAG, itemStr);
        return NOERROR;
    }

    // redraw TextView and show toast
    Int32 value;
    str_id->GetValue(&value);
    mToast->SetText(value);
    mToast->Show();
    // record the current showing item
    mCurItem = advType;
    return NOERROR;
}

ECode CDispList::OnShowNext()
{
    // get the current showing item's index in the array.
    Int32 index = -1;
    AutoPtr<CDispListHelper> helper = GetHelper();
    AutoPtr<ArrayOf<IInteger32*> > array = helper->mShortCutArray;
    for(Int32 i = 0; i < array->GetLength(); i++)
    {
        AutoPtr<IInteger32> temp = (*array)[i];
        Int32 v;
        temp->GetValue(&v);
        if(v == mCurItem)
        {
            index = i;
            break;
        }
    }

    {
        String str;
        StringBuffer sb;
        sb += "current index = ";
        sb += index;
        sb.ToString(&str);
        Logger::D(IDispList::TAG, str);
    }

    if( index == -1 )
        return NOERROR;

    // get the index of the next item to be shown
    index++;
    if( index == array->GetLength() )
        index = 0;

    AutoPtr<IInteger32> key = (*array)[index];
    AutoPtr<IDispListDispFormat> format;
    HashMap<AutoPtr<IInteger32>, AutoPtr<IDispListDispFormat> >::Iterator it =
        helper->mShortCutFormatMap.Find(key);
    if(it != helper->mShortCutFormatMap.End())
    {
        format = it->mSecond;
    }
    return OnShowItem(format);
}

void CDispList::InitToast()
{
    if(!mToastIsInited)
    {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        AutoPtr<IView> v;
        inflater->Inflate(R::layout::transient_notification, NULL, (IView**)&v);

        mToast->SetGravity(IGravity::TOP | IGravity::LEFT, 100, 0 );
        mToast->SetView(v);
        mToast->SetDuration(DURATION);
        mToastIsInited = TRUE;
    }
}

AutoPtr<CDispListHelper> CDispList::GetHelper()
{
    AutoPtr<IDispListHelper> helper;
    CDispListHelper::AcquireSingleton((IDispListHelper**)&helper);
    CDispListHelper* cHelper = (CDispListHelper*)(helper.Get());
    return cHelper;
}


}// namespace View
}// namespace Droid
}// namespace View
