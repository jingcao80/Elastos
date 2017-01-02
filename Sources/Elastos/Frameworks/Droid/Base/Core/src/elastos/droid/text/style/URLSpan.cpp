//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.View.h"
#include "elastos/droid/text/style/URLSpan.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/content/CIntent.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
//using Elastos::Droid::Provider::IBrowser;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_2(URLSpan, ClickableSpan, IParcelableSpan, IParcelable)

URLSpan::URLSpan()
{}

URLSpan::~URLSpan()
{}

ECode URLSpan::constructor()
{
    return NOERROR;
}

ECode URLSpan::constructor(
    /* [in] */ const String& url)
{
    mURL = url;
    return NOERROR;
}

ECode URLSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::URL_SPAN;
    return NOERROR;
}

ECode URLSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadString(&mURL);
}

ECode URLSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteString(mURL);
}

ECode URLSpan::GetURL(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url)
    *url = mURL;
    return NOERROR;
}

ECode URLSpan::OnClick(
    /* [in] */ IView* widget)
{
    String strUrl;
    GetURL(&strUrl);

    AutoPtr<IUri> uri;
    Uri::Parse(strUrl, (IUri**)&uri);
    AutoPtr<IContext> context;
    widget->GetContext((IContext**)&context);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&intent);
    String strPackageName;
    context->GetPackageName(&strPackageName);
    intent->/*PutExtra*/PutExtra(/*IBrowser::EXTRA_APPLICATION_ID*/String("com.android.browser.application_id"), strPackageName);
    context->StartActivity(intent);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
