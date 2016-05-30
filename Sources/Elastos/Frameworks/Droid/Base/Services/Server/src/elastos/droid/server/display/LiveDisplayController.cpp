
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Text.h>
#include "elastos/droid/server/display/LiveDisplayController.h"
#include "elastos/droid/server/accessibility/DisplayAdjustmentUtils.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::EIID_IPowerManagerInternal;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::Accessibility::DisplayAdjustmentUtils;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Server::Twilight::EIID_ITwilightManager;
using Elastos::Droid::Server::Twilight::EIID_ITwilightListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

//==============================================================================
//            LiveDisplayController::SettingsObserver
//==============================================================================

AutoPtr<IUri> InitURI(
    /* [in] */ const String& name)
{
    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IUri> uri;
    settingsSys->GetUriFor(name, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_TEMPERATURE_DAY_URI =
        InitURI(ISettingsSystem::DISPLAY_TEMPERATURE_DAY);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_TEMPERATURE_NIGHT_URI =
        InitURI(ISettingsSystem::DISPLAY_TEMPERATURE_NIGHT);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_TEMPERATURE_MODE_URI =
        InitURI(ISettingsSystem::DISPLAY_TEMPERATURE_MODE);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_AUTO_OUTDOOR_MODE_URI =
        InitURI(ISettingsSystem::DISPLAY_AUTO_OUTDOOR_MODE);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_LOW_POWER_URI =
        InitURI(ISettingsSystem::DISPLAY_LOW_POWER);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_COLOR_ENHANCE_URI =
        InitURI(ISettingsSystem::DISPLAY_COLOR_ENHANCE);
const AutoPtr<IUri> LiveDisplayController::SettingsObserver::DISPLAY_COLOR_ADJUSTMENT_URI =
        InitURI(ISettingsSystem::DISPLAY_COLOR_ADJUSTMENT);

LiveDisplayController::SettingsObserver::SettingsObserver(
    /* [in] */ LiveDisplayController* host)
    : mHost(host)
{
    ContentObserver::constructor(mHost->mHandler);
}

void LiveDisplayController::SettingsObserver::Register(
    /* [in] */ Boolean r)
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    if (r) {
        cr->RegisterContentObserver(DISPLAY_TEMPERATURE_DAY_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_TEMPERATURE_NIGHT_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_TEMPERATURE_MODE_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_AUTO_OUTDOOR_MODE_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_LOW_POWER_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_COLOR_ENHANCE_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
        cr->RegisterContentObserver(DISPLAY_COLOR_ADJUSTMENT_URI, FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    }
    else {
        cr->UnregisterContentObserver((IContentObserver*)this);
    }
}

ECode LiveDisplayController::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    ContentObserver::OnChange(selfChange,  uri);
    mHost->UpdateSettings();
    return NOERROR;
}


//==============================================================================
//            LiveDisplayController::AnimatorUpdateListener
//==============================================================================

CAR_INTERFACE_IMPL(LiveDisplayController::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode LiveDisplayController::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Int32 integer;
    IInteger32::Probe(value)->GetValue(&integer);
    mHost->SetDisplayTemperature(integer);
    return NOERROR;
}


//==============================================================================
//            LiveDisplayController::TwilightListener
//==============================================================================

CAR_INTERFACE_IMPL(LiveDisplayController::TwilightListener, Object, ITwilightListener)

ECode LiveDisplayController::TwilightListener::OnTwilightStateChanged()
{
    mHost->UpdateLiveDisplay(mHost->mCurrentLux);
    return NOERROR;
}


//==============================================================================
//            LiveDisplayController::LiveDisplayHandler
//==============================================================================

LiveDisplayController::LiveDisplayHandler::LiveDisplayHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ LiveDisplayController* host)
    : mHost(host)
{
    Handler::constructor(looper, NULL, TRUE /*async*/);
}

ECode LiveDisplayController::LiveDisplayHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_UPDATE_LIVE_DISPLAY:
            AutoPtr<ITwilightState> twilight;
            if (mHost->mTwilightManager == NULL) {
                Slogger::E(TAG, "TODO mTwilightManager is NULL");
                return NOERROR;
            }
            mHost->mTwilightManager->GetCurrentState((ITwilightState**)&twilight);

            mHost->UpdateColorTemperature(twilight);
            mHost->UpdateOutdoorMode(twilight);
            mHost->UpdateColorEnhancement(twilight);
            mHost->UpdateLowPowerMode();
            mHost->UpdateUserHint(twilight);

            Boolean transition = mHost->mMode == MODE_AUTO &&
                    mHost->mColorTemperature != mHost->mDayTemperature &&
                    mHost->mColorTemperature != mHost->mNightTemperature;
            if (transition) {
                // fire again in a minute
                Boolean result;
                SendEmptyMessageDelayed(MSG_UPDATE_LIVE_DISPLAY,
                        IDateUtils::MINUTE_IN_MILLIS, &result);
            }
            break;
    }
    return NOERROR;
}


//==============================================================================
//            LiveDisplayController
//==============================================================================

const Int32 LiveDisplayController::MODE_OFF;
const Int32 LiveDisplayController::MODE_NIGHT;
const Int32 LiveDisplayController::MODE_AUTO;
const Int32 LiveDisplayController::MODE_OUTDOOR;
const Int32 LiveDisplayController::MODE_DAY;
const String LiveDisplayController::TAG("LiveDisplayController");
const Int64 LiveDisplayController::TWILIGHT_ADJUSTMENT_TIME = IDateUtils::HOUR_IN_MILLIS * 1;
const Int32 LiveDisplayController::OFF_TEMPERATURE;
const Int32 LiveDisplayController::MSG_UPDATE_LIVE_DISPLAY;

AutoPtr<ArrayOf<Double> > InitColorTable()
{
    Double table[579] = {
        1.00000000, 0.18172716, 0.00000000,
        1.00000000, 0.25503671, 0.00000000,
        1.00000000, 0.30942099, 0.00000000,
        1.00000000, 0.35357379, 0.00000000,
        1.00000000, 0.39091524, 0.00000000,
        1.00000000, 0.42322816, 0.00000000,
        1.00000000, 0.45159884, 0.00000000,
        1.00000000, 0.47675916, 0.00000000,
        1.00000000, 0.49923747, 0.00000000,
        1.00000000, 0.51943421, 0.00000000,
        1.00000000, 0.54360078, 0.08679949,
        1.00000000, 0.56618736, 0.14065513,
        1.00000000, 0.58734976, 0.18362641,
        1.00000000, 0.60724493, 0.22137978,
        1.00000000, 0.62600248, 0.25591950,
        1.00000000, 0.64373109, 0.28819679,
        1.00000000, 0.66052319, 0.31873863,
        1.00000000, 0.67645822, 0.34786758,
        1.00000000, 0.69160518, 0.37579588,
        1.00000000, 0.70602449, 0.40267128,
        1.00000000, 0.71976951, 0.42860152,
        1.00000000, 0.73288760, 0.45366838,
        1.00000000, 0.74542112, 0.47793608,
        1.00000000, 0.75740814, 0.50145662,
        1.00000000, 0.76888303, 0.52427322,
        1.00000000, 0.77987699, 0.54642268,
        1.00000000, 0.79041843, 0.56793692,
        1.00000000, 0.80053332, 0.58884417,
        1.00000000, 0.81024551, 0.60916971,
        1.00000000, 0.81957693, 0.62893653,
        1.00000000, 0.82854786, 0.64816570,
        1.00000000, 0.83717703, 0.66687674,
        1.00000000, 0.84548188, 0.68508786,
        1.00000000, 0.85347859, 0.70281616,
        1.00000000, 0.86118227, 0.72007777,
        1.00000000, 0.86860704, 0.73688797,
        1.00000000, 0.87576611, 0.75326132,
        1.00000000, 0.88267187, 0.76921169,
        1.00000000, 0.88933596, 0.78475236,
        1.00000000, 0.89576933, 0.79989606,
        1.00000000, 0.90198230, 0.81465502,
        1.00000000, 0.90963069, 0.82838210,
        1.00000000, 0.91710889, 0.84190889,
        1.00000000, 0.92441842, 0.85523742,
        1.00000000, 0.93156127, 0.86836903,
        1.00000000, 0.93853986, 0.88130458,
        1.00000000, 0.94535695, 0.89404470,
        1.00000000, 0.95201559, 0.90658983,
        1.00000000, 0.95851906, 0.91894041,
        1.00000000, 0.96487079, 0.93109690,
        1.00000000, 0.97107439, 0.94305985,
        1.00000000, 0.97713351, 0.95482993,
        1.00000000, 0.98305189, 0.96640795,
        1.00000000, 0.98883326, 0.97779486,
        1.00000000, 0.99448139, 0.98899179,
        1.00000000, 1.00000000, 1.00000000,
        0.98947904, 0.99348723, 1.00000000,
        0.97940448, 0.98722715, 1.00000000,
        0.96975025, 0.98120637, 1.00000000,
        0.96049223, 0.97541240, 1.00000000,
        0.95160805, 0.96983355, 1.00000000,
        0.94303638, 0.96443333, 1.00000000,
        0.93480451, 0.95923080, 1.00000000,
        0.92689056, 0.95421394, 1.00000000,
        0.91927697, 0.94937330, 1.00000000,
        0.91194747, 0.94470005, 1.00000000,
        0.90488690, 0.94018594, 1.00000000,
        0.89808115, 0.93582323, 1.00000000,
        0.89151710, 0.93160469, 1.00000000,
        0.88518247, 0.92752354, 1.00000000,
        0.87906581, 0.92357340, 1.00000000,
        0.87315640, 0.91974827, 1.00000000,
        0.86744421, 0.91604254, 1.00000000,
        0.86191983, 0.91245088, 1.00000000,
        0.85657444, 0.90896831, 1.00000000,
        0.85139976, 0.90559011, 1.00000000,
        0.84638799, 0.90231183, 1.00000000,
        0.84153180, 0.89912926, 1.00000000,
        0.83682430, 0.89603843, 1.00000000,
        0.83225897, 0.89303558, 1.00000000,
        0.82782969, 0.89011714, 1.00000000,
        0.82353066, 0.88727974, 1.00000000,
        0.81935641, 0.88452017, 1.00000000,
        0.81530175, 0.88183541, 1.00000000,
        0.81136180, 0.87922257, 1.00000000,
        0.80753191, 0.87667891, 1.00000000,
        0.80380769, 0.87420182, 1.00000000,
        0.80018497, 0.87178882, 1.00000000,
        0.79665980, 0.86943756, 1.00000000,
        0.79322843, 0.86714579, 1.00000000,
        0.78988728, 0.86491137, 1.00000000,
        0.78663296, 0.86273225, 1.00000000,
        0.78346225, 0.86060650, 1.00000000,
        0.78037207, 0.85853224, 1.00000000,
        0.77735950, 0.85650771, 1.00000000,
        0.77442176, 0.85453121, 1.00000000,
        0.77155617, 0.85260112, 1.00000000,
        0.76876022, 0.85071588, 1.00000000,
        0.76603147, 0.84887402, 1.00000000,
        0.76336762, 0.84707411, 1.00000000,
        0.76076645, 0.84531479, 1.00000000,
        0.75822586, 0.84359476, 1.00000000,
        0.75574383, 0.84191277, 1.00000000,
        0.75331843, 0.84026762, 1.00000000,
        0.75094780, 0.83865816, 1.00000000,
        0.74863017, 0.83708329, 1.00000000,
        0.74636386, 0.83554194, 1.00000000,
        0.74414722, 0.83403311, 1.00000000,
        0.74197871, 0.83255582, 1.00000000,
        0.73985682, 0.83110912, 1.00000000,
        0.73778012, 0.82969211, 1.00000000,
        0.73574723, 0.82830393, 1.00000000,
        0.73375683, 0.82694373, 1.00000000,
        0.73180765, 0.82561071, 1.00000000,
        0.72989845, 0.82430410, 1.00000000,
        0.72802807, 0.82302316, 1.00000000,
        0.72619537, 0.82176715, 1.00000000,
        0.72439927, 0.82053539, 1.00000000,
        0.72263872, 0.81932722, 1.00000000,
        0.72091270, 0.81814197, 1.00000000,
        0.71922025, 0.81697905, 1.00000000,
        0.71756043, 0.81583783, 1.00000000,
        0.71593234, 0.81471775, 1.00000000,
        0.71433510, 0.81361825, 1.00000000,
        0.71276788, 0.81253878, 1.00000000,
        0.71122987, 0.81147883, 1.00000000,
        0.70972029, 0.81043789, 1.00000000,
        0.70823838, 0.80941546, 1.00000000,
        0.70678342, 0.80841109, 1.00000000,
        0.70535469, 0.80742432, 1.00000000,
        0.70395153, 0.80645469, 1.00000000,
        0.70257327, 0.80550180, 1.00000000,
        0.70121928, 0.80456522, 1.00000000,
        0.69988894, 0.80364455, 1.00000000,
        0.69858167, 0.80273941, 1.00000000,
        0.69729688, 0.80184943, 1.00000000,
        0.69603402, 0.80097423, 1.00000000,
        0.69479255, 0.80011347, 1.00000000,
        0.69357196, 0.79926681, 1.00000000,
        0.69237173, 0.79843391, 1.00000000,
        0.69119138, 0.79761446, 1.00000000,
        0.69003044, 0.79680814, 1.00000000,
        0.68888844, 0.79601466, 1.00000000,
        0.68776494, 0.79523371, 1.00000000,
        0.68665951, 0.79446502, 1.00000000,
        0.68557173, 0.79370830, 1.00000000,
        0.68450119, 0.79296330, 1.00000000,
        0.68344751, 0.79222975, 1.00000000,
        0.68241029, 0.79150740, 1.00000000,
        0.68138918, 0.79079600, 1.00000000,
        0.68038380, 0.79009531, 1.00000000,
        0.67939381, 0.78940511, 1.00000000,
        0.67841888, 0.78872517, 1.00000000,
        0.67745866, 0.78805526, 1.00000000,
        0.67651284, 0.78739518, 1.00000000,
        0.67558112, 0.78674472, 1.00000000,
        0.67466317, 0.78610368, 1.00000000,
        0.67375872, 0.78547186, 1.00000000,
        0.67286748, 0.78484907, 1.00000000,
        0.67198916, 0.78423512, 1.00000000,
        0.67112350, 0.78362984, 1.00000000,
        0.67027024, 0.78303305, 1.00000000,
        0.66942911, 0.78244457, 1.00000000,
        0.66859988, 0.78186425, 1.00000000,
        0.66778228, 0.78129191, 1.00000000,
        0.66697610, 0.78072740, 1.00000000,
        0.66618110, 0.78017057, 1.00000000,
        0.66539706, 0.77962127, 1.00000000,
        0.66462376, 0.77907934, 1.00000000,
        0.66386098, 0.77854465, 1.00000000,
        0.66310852, 0.77801705, 1.00000000,
        0.66236618, 0.77749642, 1.00000000,
        0.66163375, 0.77698261, 1.00000000,
        0.66091106, 0.77647551, 1.00000000,
        0.66019791, 0.77597498, 1.00000000,
        0.65949412, 0.77548090, 1.00000000,
        0.65879952, 0.77499315, 1.00000000,
        0.65811392, 0.77451161, 1.00000000,
        0.65743716, 0.77403618, 1.00000000,
        0.65676908, 0.77356673, 1.00000000,
        0.65610952, 0.77310316, 1.00000000,
        0.65545831, 0.77264537, 1.00000000,
        0.65481530, 0.77219324, 1.00000000,
        0.65418036, 0.77174669, 1.00000000,
        0.65355332, 0.77130560, 1.00000000,
        0.65293404, 0.77086988, 1.00000000,
        0.65232240, 0.77043944, 1.00000000,
        0.65171824, 0.77001419, 1.00000000,
        0.65112144, 0.76959404, 1.00000000,
        0.65053187, 0.76917889, 1.00000000,
        0.64994941, 0.76876866, 1.00000000,
        0.64937392, 0.76836326, 1.00000000};
    AutoPtr<ArrayOf<Double> > temp = ArrayOf<Double>::Alloc(table, 579);
    return temp;
}
const AutoPtr<ArrayOf<Double> > LiveDisplayController::sColorTable = InitColorTable();

LiveDisplayController::LiveDisplayController(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper)
    : mColorTemperature(OFF_TEMPERATURE)
    , mCurrentLux(0.0)
    , mHintCounter(0)
    , mMode(0)
    , mOutdoorMode(FALSE)
    , mColorEnhancement(FALSE)
    , mLowPower(FALSE)
    , mDayTemperature(0)
    , mNightTemperature(0)
    , mUseOutdoorMode(FALSE)
    , mUseColorEnhancement(FALSE)
    , mUseLowPower(FALSE)
    , mSunset(FALSE)
    , mDefaultDayTemperature(0)
    , mDefaultNightTemperature(0)
    , mDefaultOutdoorLux(0)
    , mLowPerformance(FALSE)
{
    mColorAdjustment = ArrayOf<Float>::Alloc(3);
    (*mColorAdjustment)[0] = 1.0;
    (*mColorAdjustment)[1] = 1.0;
    (*mColorAdjustment)[2] = 1.0;

    mRGB = ArrayOf<Float>::Alloc(3);
    (*mRGB)[0] = 0.0;
    (*mRGB)[1] = 0.0;
    (*mRGB)[2] = 0.0;

    mObserver = new SettingsObserver(this);
    mTwilightListener = new TwilightListener(this);

    mContext = context;
    mHandler = new LiveDisplayHandler(looper, this);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::CMHW_SERVICE, (IInterface**)&service);
    mCmHardwareManager = ICmHardwareManager::Probe(service);

    mTwilightManager = ITwilightManager::Probe(LocalServices::GetService(EIID_ITwilightManager));
    if (mTwilightManager == NULL) {
        Slogger::E(TAG, "TODO can not get Twilight service, maybe result from location service not ready");
    }
    else {
        mTwilightManager->RegisterListener(mTwilightListener, mHandler);
    }

    Boolean sunlightEnhancementSupported;
    mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT, &sunlightEnhancementSupported);

    if (sunlightEnhancementSupported) {
        mCmHardwareManager->Get(ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT, &mOutdoorMode);
    }

    if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_COLOR_ENHANCEMENT,
            &sunlightEnhancementSupported), sunlightEnhancementSupported) {
        mCmHardwareManager->Get(ICmHardwareManager::FEATURE_COLOR_ENHANCEMENT, &mColorEnhancement);
    }

    if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_ADAPTIVE_BACKLIGHT,
            &sunlightEnhancementSupported), sunlightEnhancementSupported) {
        mCmHardwareManager->Get(ICmHardwareManager::FEATURE_ADAPTIVE_BACKLIGHT, &mLowPower);
    }

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetInteger(Elastos::Droid::R::integer::config_dayColorTemperature, &mDefaultDayTemperature);
    res->GetInteger(Elastos::Droid::R::integer::config_nightColorTemperature, &mDefaultNightTemperature);
    res->GetInteger(Elastos::Droid::R::integer::config_outdoorAmbientLux, &mDefaultOutdoorLux);

    // Counter used to determine when we should tell the user about this feature.
    // If it's not used after 3 sunsets, we'll show the hint once.
    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    settingsSys->GetInt32ForUser(cr, ISettingsSystem::LIVE_DISPLAY_HINTED, -3,
            IUserHandle::USER_CURRENT, &mHintCounter);

    // Workaround for CMHW - remove once the new patches are merged
    if (!sunlightEnhancementSupported) {
        Boolean result;
        settingsSys->PutInt32ForUser(cr, ISettingsSystem::DISPLAY_AUTO_OUTDOOR_MODE,
                -1, IUserHandle::USER_CURRENT, &result);
    }
    UpdateSettings();
    mObserver->Register(TRUE);

    AutoPtr<IPowerManagerInternal> pmi = IPowerManagerInternal::Probe(LocalServices::GetService(EIID_IPowerManagerInternal));
    pmi->RegisterLowPowerModeObserver(mLowPowerModeListener);
    pmi->GetLowPowerModeEnabled(&mLowPerformance);
}

void LiveDisplayController::UpdateSettings()
{
    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_TEMPERATURE_DAY, mDefaultDayTemperature,
            IUserHandle::USER_CURRENT, &mDayTemperature);
    settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_TEMPERATURE_NIGHT, mDefaultNightTemperature,
            IUserHandle::USER_CURRENT, &mNightTemperature);
    settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_TEMPERATURE_MODE, MODE_OFF,
            IUserHandle::USER_CURRENT, &mMode);
    Boolean isSupported;
    if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT, &isSupported), !isSupported) {
        mUseOutdoorMode = FALSE;
    }
    else {
        Int32 value;
        settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_AUTO_OUTDOOR_MODE, 1,
                IUserHandle::USER_CURRENT, &value);
        mUseOutdoorMode = value == 1;
    }
    if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_ADAPTIVE_BACKLIGHT, &isSupported), !isSupported) {
        mUseLowPower = FALSE;
    }
    else {
        Int32 value;
        settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_LOW_POWER, 1,
                IUserHandle::USER_CURRENT, &value);
        mUseLowPower = value == 1;
    }
    if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_COLOR_ENHANCEMENT, &isSupported), !isSupported) {
        mColorEnhancement = FALSE;
    }
    else {
        Int32 value;
        settingsSys->GetInt32ForUser(cr, ISettingsSystem::DISPLAY_COLOR_ENHANCE, 1,
                IUserHandle::USER_CURRENT, &value);
        mColorEnhancement = value == 1;
    }

    // Clear the hint forever
    if (mMode != MODE_OFF) {
        SaveUserHint(1);
    }

    // Manual color adjustment will be set as a space separated string of float values
    String colorAdjustmentTemp;
    settingsSys->GetStringForUser(cr, ISettingsSystem::DISPLAY_COLOR_ADJUSTMENT,
            IUserHandle::USER_CURRENT, &colorAdjustmentTemp);
    AutoPtr<ArrayOf<String> > colorAdjustment;
    if (!colorAdjustmentTemp.IsNull()) {
        StringUtils::Split(colorAdjustmentTemp, " ", (ArrayOf<String>**)&colorAdjustment);
    }
    if (colorAdjustment == NULL || colorAdjustment->GetLength() != 3) {
        colorAdjustment = ArrayOf<String>::Alloc(3);
        (*colorAdjustment)[0] = String("1.0");
        (*colorAdjustment)[1] = String("1.0");
        (*colorAdjustment)[2] = String("1.0");
    }
    // try {
    (*mColorAdjustment)[0] = StringUtils::ParseFloat((*colorAdjustment)[0]);
    (*mColorAdjustment)[1] = StringUtils::ParseFloat((*colorAdjustment)[1]);
    (*mColorAdjustment)[2] = StringUtils::ParseFloat((*colorAdjustment)[2]);
    // } catch (NumberFormatException e) {
    //     Slog.e(TAG, e.getMessage(), e);
    //     mColorAdjustment[0] = 1.0f;
    //     mColorAdjustment[1] = 1.0f;
    //     mColorAdjustment[2] = 1.0f;
    // }

    UpdateLiveDisplay(mCurrentLux);
}

void LiveDisplayController::UpdateLiveDisplay()
{
    UpdateLiveDisplay(mCurrentLux);
}

void LiveDisplayController::UpdateLiveDisplay(
    /* [in] */ Float lux)
{
    {    AutoLock syncLock(this);
        mCurrentLux = lux;
        mHandler->RemoveMessages(MSG_UPDATE_LIVE_DISPLAY);
        Boolean result;
        mHandler->SendEmptyMessage(MSG_UPDATE_LIVE_DISPLAY, &result);
    }
}

void LiveDisplayController::UpdateColorTemperature(
    /* [in] */ ITwilightState* twilight)
{
    {    AutoLock syncLock(this);
        Int32 temperature = mDayTemperature;
        if (mMode == MODE_OFF || mLowPerformance) {
            temperature = OFF_TEMPERATURE;
        }
        else if (mMode == MODE_NIGHT) {
            temperature = mNightTemperature;
        }
        else if (mMode == MODE_AUTO) {
            temperature = GetTwilightK(twilight);
        }

        if (mAnimator != NULL) {
            IAnimator::Probe(mAnimator)->Cancel();
        }
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        mAnimator = NULL;
        AutoPtr<ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(2);
        (*attrs)[0] = mColorTemperature;
        (*attrs)[1] = temperature;
        helper->OfInt32(attrs, (IValueAnimator**)&mAnimator);
        mAnimator->SetDuration(Elastos::Core::Math::Abs(mColorTemperature - temperature) / 2);
        AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
        mAnimator->AddUpdateListener(listener);
        IAnimator::Probe(mAnimator)->Start();
    }
}

void LiveDisplayController::SetDisplayTemperature(
    /* [in] */ Int32 temperature)
{
    {    AutoLock syncLock(this);
        mColorTemperature = temperature;

        AutoPtr<ArrayOf<Float> > rgb = TemperatureToRGB(temperature);

        if (!mLowPerformance) {
            (*rgb)[0] *= (*mColorAdjustment)[0];
            (*rgb)[1] *= (*mColorAdjustment)[1];
            (*rgb)[2] *= (*mColorAdjustment)[2];
        }

        if ((*rgb)[0] == (*mRGB)[0] && (*rgb)[1] == (*mRGB)[1] && (*rgb)[2] == (*mRGB)[2]) {
            // no changes
            return;
        }

        mRGB->Copy(rgb, 3);

        Slogger::D(TAG, "Adjust display temperature to %dK [r=%d g=%d b=%d]", temperature,
                (*rgb)[0], (*rgb)[1], (*rgb)[2]);

        Boolean isSupported;
        if (mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_DISPLAY_COLOR_CALIBRATION, &isSupported), isSupported) {
            // Clear this out in case of an upgrade
            AutoPtr<ISettingsSecure> settingsSecure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            Boolean result;
            settingsSecure->PutStringForUser(cr,
                    ISettingsSecure::LIVE_DISPLAY_COLOR_MATRIX,
                    String(NULL),
                    IUserHandle::USER_CURRENT, &result);

            Int32 max;
            mCmHardwareManager->GetDisplayColorCalibrationMax(&max);
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);
            (*array)[0] = (Int32)((*rgb)[0] * max);
            (*array)[1] = (Int32)((*rgb)[1] * max);
            (*array)[2] = (Int32)((*rgb)[2] * max);
            mCmHardwareManager->SetDisplayColorCalibration(array, &result);
            ScreenRefresh();
        }
        else {
            String colorMatrixStr(NULL);
            if ((*rgb)[0] != 1.0f || (*rgb)[1] != 1.0f || (*rgb)[2] != 1.0f) {
                AutoPtr<ArrayOf<IInterface*> > colorMatrix = ArrayOf<IInterface*>::Alloc(16);
                AutoPtr<IFloat> f;
                CFloat::New((*rgb)[0], (IFloat**)&f);
                colorMatrix->Set(0, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(1, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(2, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(3, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(4, f);
                f = NULL;
                CFloat::New((*rgb)[1], (IFloat**)&f);
                colorMatrix->Set(5, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(6, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(7, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(8, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(9, f);
                f = NULL;
                CFloat::New((*rgb)[2], (IFloat**)&f);
                colorMatrix->Set(10, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(11, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(12, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(13, f);
                f = NULL;
                CFloat::New(0.0, (IFloat**)&f);
                colorMatrix->Set(14, f);
                f = NULL;
                CFloat::New(1.0, (IFloat**)&f);
                colorMatrix->Set(15, f);
                AutoPtr<ICharSequence> cs;
                CString::New(String(" "), (ICharSequence**)&cs);
                colorMatrixStr = TextUtils::Join(cs, colorMatrix);
            }

            // For GPU color transform, go thru DisplayAdjustmentUtils in
            // order to coexist with accessibility settings
            AutoPtr<ISettingsSecure> settingsSecure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            Boolean result;
            settingsSecure->PutStringForUser(cr,
                    ISettingsSecure::LIVE_DISPLAY_COLOR_MATRIX,
                    colorMatrixStr,
                    IUserHandle::USER_CURRENT, &result);

           DisplayAdjustmentUtils::ApplyAdjustments(mContext, IUserHandle::USER_CURRENT);
        }
    }
}

void LiveDisplayController::UpdateOutdoorMode(
    /* [in] */ ITwilightState* twilight)
{
    {    AutoLock syncLock(this);
        Boolean isNight;
        Boolean enabled = !mLowPerformance &&
                ((mMode == MODE_OUTDOOR) ||
                (mUseOutdoorMode && mMode == MODE_AUTO &&
                twilight != NULL && (twilight->IsNight(&isNight), !isNight) &&
                mCurrentLux > mDefaultOutdoorLux));

        if (enabled == mOutdoorMode) {
            return;
        }

        Boolean result;
        mCmHardwareManager->Set(ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT, enabled, &result);
        mOutdoorMode = enabled;
    }
}

void LiveDisplayController::UpdateColorEnhancement(
    /* [in] */ ITwilightState* twilight)
{
    {    AutoLock syncLock(this);
        Boolean isNight;
        Boolean enabled = !mLowPerformance && (mUseColorEnhancement &&
                !(mMode == MODE_NIGHT ||
                (mMode == MODE_AUTO && twilight != NULL && (twilight->IsNight(&isNight), isNight))));

        if (enabled == mColorEnhancement) {
            return;
        }

        Boolean result;
        mCmHardwareManager->Set(ICmHardwareManager::FEATURE_COLOR_ENHANCEMENT, enabled, &result);
        mColorEnhancement = enabled;
    }
}

void LiveDisplayController::UpdateLowPowerMode()
{
    {    AutoLock syncLock(this);
        Boolean enabled = mUseLowPower && mCurrentLux < mDefaultOutdoorLux;

        if (enabled == mLowPower) {
            return;
        }

        Boolean result;
        mCmHardwareManager->Set(ICmHardwareManager::FEATURE_ADAPTIVE_BACKLIGHT, enabled, &result);
        mLowPower = enabled;
    }
}

AutoPtr<ArrayOf<Float> > LiveDisplayController::TemperatureToRGB(
    /* [in] */ Int32 degreesK)
{
    Int32 k = MathUtils::Constrain(degreesK, 1000, 20000);
    Float a = (k % 100) / 100.0;
    Int32 i = ((k - 1000)/ 100) * 3;

    AutoPtr<ArrayOf<Float> > rgb = ArrayOf<Float>::Alloc(3);
    (*rgb)[0] =  Interp(i, a);
    (*rgb)[1] =  Interp(i + 1, a);
    (*rgb)[2] =  Interp(i + 2, a);
    return rgb;
}

Float LiveDisplayController::Interp(
    /* [in] */ Int32 i,
    /* [in] */ Float a)
{
    return MathUtils::Lerp((Float)(*sColorTable)[i], (Float)(*sColorTable)[i + 3], a);
}

Float LiveDisplayController::Adj(
    /* [in] */ Int64 now,
    /* [in] */ Int64 sunset,
    /* [in] */ Int64 sunrise)
{
    if (sunset < 0 || sunrise < 0
            || now < sunset || now > sunrise) {
        return 1.0;
    }

    if (now < sunset + TWILIGHT_ADJUSTMENT_TIME) {
        return MathUtils::Lerp(1.0, 0.0,
                (Float)(now - sunset) / TWILIGHT_ADJUSTMENT_TIME);
    }

    if (now > sunrise - TWILIGHT_ADJUSTMENT_TIME) {
        return MathUtils::Lerp(1.0, 0.0,
                (Float)(sunrise - now) / TWILIGHT_ADJUSTMENT_TIME);
    }

    return 0.0;
}

Int32 LiveDisplayController::GetTwilightK(
    /* [in] */ ITwilightState* state)
{
    Float adjustment = 1.0;

    if (state != NULL) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 now;
        sys->GetCurrentTimeMillis(&now);
        Int64 yesterdaySunset, todaySunrise, todaySunset, tomorrowSunrise;
        state->GetYesterdaySunset(&yesterdaySunset);
        state->GetTodaySunrise(&todaySunrise);
        state->GetTodaySunset(&todaySunset);
        state->GetTomorrowSunrise(&tomorrowSunrise);
        adjustment = Adj(now, yesterdaySunset, todaySunrise) *
                Adj(now, todaySunset, tomorrowSunrise);
    }

    return (Int32)MathUtils::Lerp(mNightTemperature, mDayTemperature, adjustment);
}

void LiveDisplayController::ScreenRefresh()
{
    // try {
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> surfaceFlinger = sm->getService(android::String16("SurfaceFlinger"));

    if (surfaceFlinger != NULL) {
        Slogger::I(TAG, "******* TELLING SURFACE FLINGER WE ARE BOOTED!");
        android::Parcel data;
        data.writeInterfaceToken(android::String16("android.ui.ISurfaceComposer"));
        surfaceFlinger->transact(1004, data, NULL, 0);
        // data.recycle();
    }
    // } catch (RemoteException ex) {
    //     Slog.e(TAG, "Failed to refresh screen", ex);
    // }
}

void LiveDisplayController::SaveUserHint(
    /* [in] */ Int32 value)
{
    if (mHintCounter == value) {
        return;
    }
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean result;
    settingsSystem->PutInt32ForUser(cr,
            ISettingsSystem::LIVE_DISPLAY_HINTED,
            value,
            IUserHandle::USER_CURRENT, &result);
    mHintCounter = value;
}

void LiveDisplayController::UpdateUserHint(
    /* [in] */ ITwilightState* state)
{
    // check if we should send the hint only once after sunset
    if (state == NULL || mHintCounter == 1) {
        return;
    }
    Boolean isNight;
    Boolean transition = (state->IsNight(&isNight), isNight) && !mSunset;
    mSunset = isNight;
    if (!transition) {
        return;
    }

    if (mHintCounter <= 0) {
        mHintCounter++;
        SaveUserHint(mHintCounter);
    }
    if (mHintCounter == 0) {
        //show the notification and don't come back here
        AutoPtr<IIntent> intent;
        CIntent::New(String("android.settings.LIVEDISPLAY_SETTINGS"), (IIntent**)&intent);
        AutoPtr<IPendingIntentHelper> helper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
        AutoPtr<IPendingIntent> result;
        helper->GetActivity(
                mContext, 0, intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&result);
        AutoPtr<INotificationBuilder> builder;
        CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String title;
        res->GetString(Elastos::Droid::R::string::live_display_title, &title);
        AutoPtr<ICharSequence> cs;
        CString::New(title, (ICharSequence**)&cs);
        builder->SetContentTitle(cs);
        String hint;
        res->GetString(Elastos::Droid::R::string::live_display_hint, &hint);
        cs = NULL;
        CString::New(hint, (ICharSequence**)&cs);
        builder->SetContentText(cs);
        builder->SetSmallIcon(Elastos::Droid::R::drawable::ic_livedisplay_notif);
        builder->SetContentIntent(result);

        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
        AutoPtr<INotificationManager> nm = INotificationManager::Probe(service);
        AutoPtr<INotification> notification;
        builder->Build((INotification**)&notification);
        AutoPtr<IUserHandleHelper> hhelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hhelper);
        AutoPtr<IUserHandle> handle;
        hhelper->GetCURRENT((IUserHandle**)&handle);
        nm->NotifyAsUser(String(NULL), 1, notification, handle);

        SaveUserHint(1);
    }
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
