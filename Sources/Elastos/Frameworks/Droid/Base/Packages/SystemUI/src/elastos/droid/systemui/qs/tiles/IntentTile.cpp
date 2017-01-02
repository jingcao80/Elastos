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

#include "elastos/droid/systemui/qs/tiles/IntentTile.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

static const String TAG("IntentTile");

IntentTile::Receiver::Receiver(
    /* [in] */ IntentTile* host)
    : mHost(host)
{}

ECode IntentTile::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->RefreshState(intent);
    return NOERROR;
}

const String IntentTile::PREFIX("intent(");

IntentTile::IntentTile(
    /* [in] */ IQSTileHost* host,
    /* [in] */ const String& action)
    : mCurrentUserId(0)
{
    QSTile::constructor(host);
    mReceiver = new Receiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(action, (IIntentFilter**)&filter);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&i);
}

void IntentTile::HandleDestroy()
{
    QSTile::HandleDestroy();
    mContext->UnregisterReceiver(mReceiver);
}

ECode IntentTile::Create(
    /* [in] */ IQSTileHost* host,
    /* [in] */ const String& spec,
    /* [out] */ IQSTile** tile)
{
    VALIDATE_NOT_NULL(tile);
    if (spec == NULL || !spec.StartWith(PREFIX) || !spec.EndWith(")")) {
        tile = NULL;
        // throw new IllegalArgumentException("Bad intent tile spec: " + spec);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    const String action = spec.Substring(PREFIX.GetLength(), spec.GetLength() - 1);
    if (action.IsEmpty()) {
        tile = NULL;
        // throw new IllegalArgumentException("Empty intent tile spec action");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *tile = new IntentTile(host, action);
    REFCOUNT_ADD(*tile);
    return NOERROR;
}

ECode IntentTile::SetListening(
    /* [in] */ Boolean listening)
{
    return NOERROR;
}

AutoPtr<QSTile::State> IntentTile::NewTileState()
{
    return new State();
}

void IntentTile::HandleUserSwitch(
    /* [in] */ Int32 newUserId)
{
    QSTile::HandleUserSwitch(newUserId);
    mCurrentUserId = newUserId;
}

void IntentTile::HandleClick()
{
    ECode ec = NOERROR;
    do {
        if (mOnClick != NULL) {
            ec = mOnClick->Send();
            if (FAILED(ec)) {
                break;
            }
        }
        else if (mOnClickUri != NULL) {
            AutoPtr<IIntentHelper> helper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
            AutoPtr<IIntent> intent;
            ec = helper->ParseUri(mOnClickUri, IIntent::URI_INTENT_SCHEME, (IIntent**)&intent);
            if (FAILED(ec)) {
                break;
            }
            AutoPtr<IUserHandle> uh;
            CUserHandle::New(mCurrentUserId, (IUserHandle**)&uh);
            ec = mContext->SendBroadcastAsUser(intent, uh);
        }
    } while (0);
    if (FAILED(ec)) {
        Logger::W(TAG, "Error sending click intent");
    }
}

void IntentTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    if (IIntent::Probe(arg) == NULL) return;
    AutoPtr<IIntent> intent = IIntent::Probe(arg);
    intent->GetBooleanExtra(String("visible"), TRUE, &state->mVisible);
    intent->GetStringExtra(String("contentDescription"), &state->mContentDescription);
    intent->GetStringExtra(String("label"), &state->mLabel);
    state->mIconId = 0;
    state->mIcon = NULL;
    AutoPtr<ArrayOf<Byte> > iconBitmap;
    intent->GetByteArrayExtra(String("iconBitmap"), (ArrayOf<Byte>**)&iconBitmap);
    if (iconBitmap != NULL) {
        ECode ec = NOERROR;
        do {
            AutoPtr<IBitmapFactory> helper;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
            AutoPtr<IBitmap> b;
            ec = helper->DecodeByteArray(iconBitmap, 0, iconBitmap->GetLength(), (IBitmap**)&b);
            if (FAILED(ec)) {
                break;
            }
            AutoPtr<IResources> res;
            mContext->GetResources((IResources**)&res);
            ec = CBitmapDrawable::New(res, b, (IDrawable**)&state->mIcon);
        } while (0);

        if (FAILED(ec)) {
            Logger::W(TAG, "Error loading icon bitmap, length %d", iconBitmap->GetLength());
        }
    }
    else {
        Int32 iconId = 0;
        intent->GetInt32Extra(String("iconId"), 0, &iconId);
        if (iconId != 0) {
            String iconPackage;
            intent->GetStringExtra(String("iconPackage"), &iconPackage);
            if (!TextUtils::IsEmpty(iconPackage)) {
                state->mIcon = GetPackageDrawable(iconPackage, iconId);
            }
            else {
                state->mIconId = iconId;
            }
        }
    }

    AutoPtr<IParcelable> p;
    intent->GetParcelableExtra(String("onClick"), (IParcelable**)&p);
    mOnClick = IPendingIntent::Probe(p);
    intent->GetStringExtra(String("onClickUri"), &mOnClickUri);
}

AutoPtr<IDrawable> IntentTile::GetPackageDrawable(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 id)
{
    ECode ec = NOERROR;
    do {
        AutoPtr<IContext> ctx;
        ec = mContext->CreatePackageContext(pkg, 0, (IContext**)&ctx);
        if (FAILED(ec)) {
            break;
        }
        AutoPtr<IDrawable> d;
        if (!FAILED(ctx->GetDrawable(id, (IDrawable**)&d))) {
            return d;
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::W(TAG, "Error loading package drawable pkg=%s id=%d", pkg.string(), id);
    }
    return NULL;
}

String IntentTile::GetSimpleName()
{
    return String("IntentTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
