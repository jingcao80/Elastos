
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Content.h>
#include "elastos/droid/server/lights/LightsService.h"
#include "elastos/droid/server/lights/CLegacyFlashlightHack.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <hardware/hardware.h>
#include <hardware/lights.h>
#include <utils/misc.h>
#include <utils/Log.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIHardwareService;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Server::Lights::CLegacyFlashlightHack;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

const String LightsService::TAG("LightsService");
const Boolean LightsService::DEBUG = FALSE;
const String LightsService::LegacyFlashlightHack::FLASHLIGHT_FILE("/sys/class/leds/spotlight/brightness");

//==============================================================================
//          LightImpl
//==============================================================================

LightImpl::LightImpl(
    /* [in] */ Int32 id,
    /* [in] */ LightsService* host)
    : mId(id)
    , mColor(0)
    , mMode(0)
    , mOnMS(0)
    , mOffMS(0)
    , mFlashing(FALSE)
    , mHost(host)
{
}

LightImpl::~LightImpl()
{
}

void LightImpl::SetBrightness(
    /* [in] */ Int32 brightness)
{
    SetBrightness(brightness, BRIGHTNESS_MODE_USER);
}

void LightImpl::SetBrightness(
    /* [in] */ Int32 brightness,
    /* [in] */ Int32 brightnessMode)
{
    synchronized (this) {
        Int32 color = brightness & 0x000000ff;
        color = 0xff000000 | (color << 16) | (color << 8) | color;
        SetLightLocked(color, LIGHT_FLASH_NONE, 0, 0, brightnessMode);
    }
}

void LightImpl::SetColor(
    /* [in] */ Int32 color)
{
    synchronized (this) {
        SetLightLocked(color, LIGHT_FLASH_NONE, 0, 0, 0);
    }
}

void LightImpl::SetFlashing(
    /* [in] */ Int32 color,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 onMS,
    /* [in] */ Int32 offMS)
{
    synchronized (this) {
        SetLightLocked(color, mode, onMS, offMS, BRIGHTNESS_MODE_USER);
    }
}

void LightImpl::Pulse()
{
    Pulse(0x00ffffff, 7);
}

void LightImpl::Pulse(
    /* [in] */ Int32 color,
    /* [in] */ Int32 onMS)
{
    synchronized (this) {
        if (mColor == 0 && !mFlashing) {
            SetLightLocked(color, LIGHT_FLASH_HARDWARE, onMS, 1000, BRIGHTNESS_MODE_USER);

            mColor = 0;

            AutoPtr<IMessage> msg;
            mHost->mH->ObtainMessage(1, THIS_PROBE(IInterface), (IMessage**)&msg);
            Boolean result;
            mHost->mH->SendMessageDelayed(msg, onMS, &result);
        }
    }
}

void LightImpl::TurnOff()
{
    synchronized (this) {
        SetLightLocked(0, LIGHT_FLASH_NONE, 0, 0, 0);
    }
}

void LightImpl::StopFlashing()
{
    synchronized (this) {
        SetLightLocked(mColor, LIGHT_FLASH_NONE, 0, 0, BRIGHTNESS_MODE_USER);
    }
}

void LightImpl::SetLightLocked(
    /* [in] */ Int32 color,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 onMS,
    /* [in] */ Int32 offMS,
    /* [in] */ Int32 brightnessMode)
{
    if (color != mColor || mode != mMode || onMS != mOnMS || offMS != mOffMS) {
        if (mHost->DEBUG) {
            Logger::V(mHost->TAG, "setLight # %d: color=#0x%x", mId, color);
        }

        mColor = color;
        mMode = mode;
        mOnMS = onMS;
        mOffMS = offMS;
        assert(0 && "TODO");
        //Trace.traceBegin(Trace.TRACE_TAG_POWER, "setLight(" + mId + ", " + color + ")");
        // try {
        mHost->SetLight_native(mId, color, mode, onMS, offMS, brightnessMode);
        // } finally {
        //Trace.traceEnd(Trace.TRACE_TAG_POWER);
        // }
    }
}

//==============================================================================
//          LightsService::LegacyFlashlightHack
//==============================================================================

CAR_INTERFACE_IMPL_2(LightsService::LegacyFlashlightHack, Object, IIHardwareService, IBinder);

LightsService::LegacyFlashlightHack::LegacyFlashlightHack()
{
}

LightsService::LegacyFlashlightHack::~LegacyFlashlightHack()
{}

ECode LightsService::LegacyFlashlightHack::constructor(
    /* [in] */ ISystemService* service)
{
    mHost = (LightsService*)service;
    return NOERROR;
}

ECode LightsService::LegacyFlashlightHack::GetFlashlightEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    //try {
    AutoPtr<IFileInputStream> fis;
    CFileInputStream::New(FLASHLIGHT_FILE, (IFileInputStream**)&fis);

    Int32 result;
    if (FAILED(IInputStream::Probe(fis)->Read(&result))) {
        *isEnabled = FALSE;
        return NOERROR;
    }
    IInputStream::Probe(fis)->Close();
    *isEnabled = (result != '0');
    return NOERROR;
    //} catch (Exception e) {
    //    return false;
    //}
}

ECode LightsService::LegacyFlashlightHack::SetFlashlightEnabled(
    /* [in] */ Boolean on)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    String permission1 = Elastos::Droid::Manifest::permission::FLASHLIGHT;
    Int32 result1;
    FAIL_RETURN(context->CheckCallingOrSelfPermission(permission1, &result1));
    String permission2 = Elastos::Droid::Manifest::permission::HARDWARE_TEST;
    Int32 result2;
    FAIL_RETURN(context->CheckCallingOrSelfPermission(permission2, &result2));
    if (result1 != IPackageManager::PERMISSION_GRANTED &&
            result2 != IPackageManager::PERMISSION_GRANTED) {
        Logger::E("LightsService::LegacyFlashlightHack", "Requires FLASHLIGHT or HARDWARE_TEST permission");
        return E_SECURITY_EXCEPTION;
        //throw new SecurityException("Requires FLASHLIGHT or HARDWARE_TEST permission");
    }
    //try {
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(FLASHLIGHT_FILE, (IFileOutputStream**)&fos);

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(2);
    (*bytes)[0] = (Byte)(on ? '1' : '0');
    (*bytes)[1] = '\n';
    IOutputStream::Probe(fos)->Write(bytes);
    IOutputStream::Probe(fos)->Close();
    //} catch (Exception e) {
        // fail silently
    //}
    return NOERROR;
}

ECode LightsService::LegacyFlashlightHack::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "LightsService::LegacyFlashlightHack";
    return NOERROR;
}

//==============================================================================
//          LightsService::MyLightsManager
//==============================================================================

LightsService::MyLightsManager::MyLightsManager(
    /* [in] */ LightsService* host)
    : mHost(host)
{
}

LightsService::MyLightsManager::~MyLightsManager()
{
}

AutoPtr<Light> LightsService::MyLightsManager::GetLight(
    /* [in] */ Int32 id)
{
    if (id < LIGHT_ID_COUNT) {
        return (*(mHost->mLights))[id];
    }
    else {
        return NULL;
    }
}

//==============================================================================
//          LightsService::MyHandler
//==============================================================================

ECode LightsService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IObject> object = IObject::Probe(obj);
    AutoPtr<LightImpl> light = (LightImpl*)object.Get();
    light->StopFlashing();
    return NOERROR;
}

//==============================================================================
//          LightsService
//==============================================================================

LightsService::LightsService()
{}

LightsService::~LightsService()
{
    Finalize_native();
}

ECode LightsService::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context));
    mService = new MyLightsManager(this);
    mH = new MyHandler(this);
    mH->constructor();

    CLegacyFlashlightHack::New(THIS_PROBE(ISystemService), (IIHardwareService**)&mLegacyFlashlightHack);

    Init_native();

    mLights = ArrayOf<LightImpl*>::Alloc(LightsManager::LIGHT_ID_COUNT);
    for (Int32 i = 0; i < LightsManager::LIGHT_ID_COUNT; i++) {
        mLights->Set(i, new LightImpl(i, this));
    }
    return NOERROR;
}

ECode LightsService::OnStart()
{
    PublishBinderService(String("hardware"), IBinder::Probe(mLegacyFlashlightHack));
    PublishLocalService(EIID_ILightsManager, TO_IINTERFACE(mService));
    return NOERROR;
}

// These values must correspond with the LIGHT_ID constants in
// LightsService.java
enum {
    LIGHT_INDEX_BACKLIGHT = 0,
    LIGHT_INDEX_KEYBOARD = 1,
    LIGHT_INDEX_BUTTONS = 2,
    LIGHT_INDEX_BATTERY = 3,
    LIGHT_INDEX_NOTIFICATIONS = 4,
    LIGHT_INDEX_ATTENTION = 5,
    LIGHT_INDEX_BLUETOOTH = 6,
    LIGHT_INDEX_WIFI = 7,
    LIGHT_COUNT
};

struct Devices {
    light_device_t* lights[LIGHT_COUNT];
};

static light_device_t* get_device(hw_module_t* module, char const* name)
{
    int err;
    hw_device_t* device;
    err = module->methods->open(module, name, &device);
    if (err == 0) {
        return (light_device_t*)device;
    }
    else {
        return NULL;
    }
}

void LightsService::Init_native()
{
    int err;
    hw_module_t* module;
    Devices* devices;

    devices = (Devices*)malloc(sizeof(Devices));

    err = hw_get_module(LIGHTS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (err == 0) {
        devices->lights[LIGHT_INDEX_BACKLIGHT]
                = get_device(module, LIGHT_ID_BACKLIGHT);
        devices->lights[LIGHT_INDEX_KEYBOARD]
                = get_device(module, LIGHT_ID_KEYBOARD);
        devices->lights[LIGHT_INDEX_BUTTONS]
                = get_device(module, LIGHT_ID_BUTTONS);
        devices->lights[LIGHT_INDEX_BATTERY]
                = get_device(module, LIGHT_ID_BATTERY);
        devices->lights[LIGHT_INDEX_NOTIFICATIONS]
                = get_device(module, LIGHT_ID_NOTIFICATIONS);
        devices->lights[LIGHT_INDEX_ATTENTION]
                = get_device(module, LIGHT_ID_ATTENTION);
        devices->lights[LIGHT_INDEX_BLUETOOTH]
                = get_device(module, LIGHT_ID_BLUETOOTH);
        devices->lights[LIGHT_INDEX_WIFI]
                = get_device(module, LIGHT_ID_WIFI);
    }
    else {
        memset(devices, 0, sizeof(Devices));
    }

    mNativePointer = (Int64)devices;
}

void LightsService::Finalize_native()
{
    Devices* devices = (Devices*)mNativePointer;
    if (devices == NULL) {
        return;
    }

    free(devices);
}

void LightsService::SetLight_native(
    /* [in] */ Int32 light,
    /* [in] */ Int32 color,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 onMS,
    /* [in] */ Int32 offMS,
    /* [in] */ Int32 brightnessMode)
{
    Devices* devices = (Devices*)mNativePointer;
    light_state_t state;

    if (light < 0 || light >= LIGHT_COUNT || devices->lights[light] == NULL) {
        return ;
    }

    memset(&state, 0, sizeof(light_state_t));
    state.color = color;
    state.flashMode = mode;
    state.flashOnMS = onMS;
    state.flashOffMS = offMS;
    state.brightnessMode = brightnessMode;

    {
        ALOGD_IF_SLOW(50, "Excessive delay setting light");
        devices->lights[light]->set_light(devices->lights[light], &state);
    }
}

} // Lights
} // Server
} // Droid
} // Elastos
