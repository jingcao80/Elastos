
#include "elastos/droid/systemui/statusbar/phone/QSTileHost.h"
#include "elastos/droid/systemui/qs/QSTile.h"
#include "elastos/droid/systemui/qs/tiles/AirplaneModeTile.h"
#include "elastos/droid/systemui/qs/tiles/BluetoothTile.h"
#include "elastos/droid/systemui/qs/tiles/CastTile.h"
#include "elastos/droid/systemui/qs/tiles/CellularTile.h"
#include "elastos/droid/systemui/qs/tiles/ColorInversionTile.h"
#include "elastos/droid/systemui/qs/tiles/FlashlightTile.h"
#include "elastos/droid/systemui/qs/tiles/HotspotTile.h"
#include "elastos/droid/systemui/qs/tiles/IntentTile.h"
#include "elastos/droid/systemui/qs/tiles/LocationTile.h"
#include "elastos/droid/systemui/qs/tiles/RotationLockTile.h"
#include "elastos/droid/systemui/qs/tiles/WifiTile.h"
#include "../R.h"
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::SystemUI::Qs::EIID_IQSTileHost;
using Elastos::Droid::SystemUI::Qs::EIID_IQSTile;
using Elastos::Droid::SystemUI::Qs::QSTile;
using Elastos::Droid::SystemUI::Qs::Tiles::AirplaneModeTile;
using Elastos::Droid::SystemUI::Qs::Tiles::BluetoothTile;
using Elastos::Droid::SystemUI::Qs::Tiles::CastTile;
using Elastos::Droid::SystemUI::Qs::Tiles::CellularTile;
using Elastos::Droid::SystemUI::Qs::Tiles::ColorInversionTile;
using Elastos::Droid::SystemUI::Qs::Tiles::FlashlightTile;
using Elastos::Droid::SystemUI::Qs::Tiles::HotspotTile;
using Elastos::Droid::SystemUI::Qs::Tiles::IIntentTile;
using Elastos::Droid::SystemUI::Qs::Tiles::IntentTile;
using Elastos::Droid::SystemUI::Qs::Tiles::LocationTile;
using Elastos::Droid::SystemUI::Qs::Tiles::RotationLockTile;
using Elastos::Droid::SystemUI::Qs::Tiles::WifiTile;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const String QSTileHost::TAG("QSTileHost");
const Boolean QSTileHost::DEBUG = FALSE;//Logger::IsLoggable(TAG, Logger::___DEBUG);
const String QSTileHost::TILES_SETTING("sysui_qs_tiles");

QSTileHost::Observer::Observer(
    /* [in] */ QSTileHost* host)
    : mRegistered(FALSE)
    , mHost(host)
{
    AutoPtr<IHandler> handler;
    CHandler::New(Looper::GetMainLooper(), (IHandler**)&handler);
    ContentObserver::constructor(handler);
}

ECode QSTileHost::Observer::Register()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    if (mRegistered) {
        cr->UnregisterContentObserver(this);
    }

    AutoPtr<IUri> uri;
    Elastos::Droid::Provider::Settings::Secure::GetUriFor(TILES_SETTING, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, this, mHost->mUserTracker->GetCurrentUserId());
    mRegistered = TRUE;
    return NOERROR;
}

ECode QSTileHost::Observer::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->RecreateTiles();
    return NOERROR;
}

QSTileHost::UserTracker::UserTracker(
    /* [in] */ QSTileHost* host)
    : CurrentUserTracker(host->mContext)
    , mHost(host)
{}

void QSTileHost::UserTracker::OnUserSwitched(
    /* [in] */ Int32 newUserId)
{
    mHost->RecreateTiles();
    AutoPtr<ICollection> collection;
    mHost->mTiles->GetValues((ICollection**)&collection);

    AutoPtr<IIterator> ator;
    collection->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tile;
        ator->GetNext((IInterface**)&tile);
        IQSTile::Probe(tile)->UserSwitch(newUserId);
    }

    mHost->mSecurity->OnUserSwitched(newUserId);
    mHost->mObserver->Register();
}

CAR_INTERFACE_IMPL_2(QSTileHost, Object, IPhoneQSTileHost, IQSTileHost);
QSTileHost::QSTileHost(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneStatusBar* statusBar,
    /* [in] */ IBluetoothController* bluetooth,
    /* [in] */ ILocationController* location,
    /* [in] */ IRotationLockController* rotation,
    /* [in] */ INetworkController* network,
    /* [in] */ IZenModeController* zen,
    /* [in] */ IHotspotController* hotspot,
    /* [in] */ ICastController* cast,
    /* [in] */ IFlashlightController* flashlight,
    /* [in] */ IUserSwitcherController* userSwitcher,
    /* [in] */ IKeyguardMonitor* keyguard,
    /* [in] */ ISecurityController* security)
{
    CLinkedHashMap::New((ILinkedHashMap**)&mTiles);
    mObserver = new Observer(this);
    mContext = context;
    mStatusBar = statusBar;
    mBluetooth = bluetooth;
    mLocation = location;
    mRotation = rotation;
    mNetwork = network;
    mZen = zen;
    mHotspot = hotspot;
    mCast = cast;
    mFlashlight = flashlight;
    mUserSwitcherController = userSwitcher;
    mKeyguard = keyguard;
    mSecurity = security;

    AutoPtr<IHandlerThread> ht;
    CHandlerThread::New(String("QSTileHost.class.getSimpleName()"), (IHandlerThread**)&ht);
    IThread::Probe(ht)->Start();
    ht->GetLooper((ILooper**)&mLooper);

    mUserTracker = new UserTracker(this);
    RecreateTiles();

    mUserTracker->StartTracking();
    mObserver->Register();
}

ECode QSTileHost::SetCallback(
    /* [in] */ IQSTileHostCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode QSTileHost::GetTiles(
    /* [out] */ ICollection** tiles)
{
    VALIDATE_NOT_NULL(tiles);
    return mTiles->GetValues(tiles);
}

ECode QSTileHost::StartSettingsActivity(
    /* [in] */ IIntent* intent)
{
    mStatusBar->PostStartSettingsActivity(intent, 0);
    return NOERROR;
}

ECode QSTileHost::Warn(
    /* [in] */ const String& message)
{
    // already logged
    return NOERROR;
}

ECode QSTileHost::CollapsePanels()
{
    mStatusBar->PostAnimateCollapsePanels();
    return NOERROR;
}

ECode QSTileHost::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    *looper = mLooper;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode QSTileHost::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    *ctx = mContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode QSTileHost::GetBluetoothController(
    /* [out] */ IBluetoothController** bc)
{
    VALIDATE_NOT_NULL(bc);
    *bc = mBluetooth;
    REFCOUNT_ADD(*bc);
    return NOERROR;
}

ECode QSTileHost::GetLocationController(
    /* [out] */ ILocationController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLocation;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetRotationLockController(
    /* [out] */ IRotationLockController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRotation;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetNetworkController(
    /* [out] */ INetworkController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNetwork;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetZenModeController(
    /* [out] */ IZenModeController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mZen;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetHotspotController(
    /* [out] */ IHotspotController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHotspot;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetCastController(
    /* [out] */ ICastController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCast;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetFlashlightController(
    /* [out] */ IFlashlightController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFlashlight;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetKeyguardMonitor(
    /* [out] */ IKeyguardMonitor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKeyguard;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetUserSwitcherController(
    /* [out] */ IUserSwitcherController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUserSwitcherController;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode QSTileHost::GetSecurityController(
    /* [out] */ ISecurityController** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSecurity;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void QSTileHost::RecreateTiles()
{
    if (DEBUG) Logger::D(TAG, "Recreating tiles");
    AutoPtr<IList> tileSpecs = LoadTileSpecs();  /*<String*/

    AutoPtr<ISet> set;
    mTiles->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> ator;
    set->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE, tmp = FALSE;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> tile = IMapEntry::Probe(obj);
        assert(tile);

        AutoPtr<IInterface> key;
        tile->GetKey((IInterface**)&key);

        if (tileSpecs->Contains(key, &tmp), !tmp) {
            String s;
            ICharSequence::Probe(key)->ToString(&s);
            if (DEBUG) Logger::D(TAG, "Destroying tile: %s", s.string());

            AutoPtr<IInterface> value;
            tile->GetValue((IInterface**)&value);
            assert(IQSTile::Probe(value));
            IQSTile::Probe(value)->Destroy();
        }
    }

    AutoPtr<ILinkedHashMap> newTiles;  /*<String, QSTile<?>*/
    CLinkedHashMap::New((ILinkedHashMap**)&newTiles);
    ator = NULL;
    tileSpecs->GetIterator((IIterator**)&ator);
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICharSequence> tileSpec = ICharSequence::Probe(obj);
        if (mTiles->ContainsKey(tileSpec, &tmp), tmp) {
            AutoPtr<IInterface> v;
            mTiles->Get(tileSpec, (IInterface**)&v);
            newTiles->Put(tileSpec, v);
        }
        else {
            if (DEBUG) Logger::D(TAG, "Creating tile: %s", TO_CSTR(tileSpec));
            AutoPtr<IQSTile> values;
            String str;
            tileSpec->ToString(&str);
            ECode ec = CreateTile(str, (IQSTile**)&values);
            newTiles->Put(tileSpec, values);
            if (FAILED(ec)) {
                Logger::W(TAG, "Error creating tile for spec: %s", TO_CSTR(tileSpec));
            }
        }
    }

    if (mTiles->Equals(newTiles, &tmp), tmp) return;
    mTiles->Clear();
    mTiles->PutAll(IMap::Probe(newTiles));
    if (mCallback != NULL) {
        mCallback->OnTilesChanged();
    }
}

ECode QSTileHost::CreateTile(
    /* [in] */ const String& tileSpec,
    /* [out] */ IQSTile** values)
{
    VALIDATE_NOT_NULL(values);
    *values = NULL;

    AutoPtr<QSTile> qs;
    if (tileSpec.Equals("wifi")) {
        qs = new WifiTile(this);
    }
    else if (tileSpec.Equals("bt")) {
        qs = new BluetoothTile(this);
    }
    else if (tileSpec.Equals("inversion")) {
        qs = new ColorInversionTile(this);
    }
    else if (tileSpec.Equals("cell")) {
        qs = new CellularTile(this);
    }
    else if (tileSpec.Equals("airplane")) {
        qs = new AirplaneModeTile(this);
    }
    else if (tileSpec.Equals("rotation")) {
        qs = new RotationLockTile(this);
    }
    else if (tileSpec.Equals("flashlight")) {
        qs = new FlashlightTile(this);
    }
    else if (tileSpec.Equals("location")) {
        qs = new LocationTile(this);
    }
    else if (tileSpec.Equals("cast")) {
        qs = new CastTile(this);
    }
    else if (tileSpec.Equals("hotspot")) {
        qs = new HotspotTile(this);
    }
    else if (tileSpec.StartWith(IIntentTile::PREFIX)) {
        return IntentTile::Create(this, tileSpec, values);
    }
    else {
        // throw new IllegalArgumentException("Bad tile spec: " + tileSpec);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *values = (IQSTile*)qs->Probe(EIID_IQSTile);
    REFCOUNT_ADD(*values);
    return NOERROR;
}

AutoPtr<IList/*<String*/> QSTileHost::LoadTileSpecs()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String defaultTileList;
    res->GetString(R::string::quick_settings_tiles_default, &defaultTileList);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String tileList;
    Elastos::Droid::Provider::Settings::Secure::GetStringForUser(cr, TILES_SETTING,
            mUserTracker->GetCurrentUserId(), &tileList);
    if (tileList == NULL) {
        res->GetString(R::string::quick_settings_tiles, &tileList);
        if (DEBUG) Logger::D(TAG, "Loaded tile specs from config: %s", tileList.string());
    }
    else {
        if (DEBUG) Logger::D(TAG, "Loaded tile specs from setting: %s", tileList.string());
    }

    AutoPtr<IArrayList> tiles;  /*<String*/
    CArrayList::New((IArrayList**)&tiles);
    Boolean addedDefault = FALSE;
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(tileList, String(","), (ArrayOf<String>**)&values);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        String tile = (*values)[i];
        tile = tile.Trim();
        if (tile.IsEmpty()) continue;
        if (tile.Equals("default")) {
            if (!addedDefault) {
                AutoPtr<ArrayOf<String> > vs;
                StringUtils::Split(defaultTileList, String(","), (ArrayOf<String>**)&vs);
                AutoPtr<IList> outList;
                Arrays::AsList(vs, (IList**)&outList);
                tiles->AddAll(ICollection::Probe(outList));
                addedDefault = TRUE;
            }
        }
        else {
            AutoPtr<ICharSequence> t;
            CString::New(tile, (ICharSequence**)&t);
            tiles->Add(t);
        }
    }
    return IList::Probe(tiles);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
