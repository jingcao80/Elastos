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

#include "elastos/droid/server/CConsumerIrService.h"
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Hardware.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Manifest;
using Elastos::Droid::Hardware::EIID_IIConsumerIrService;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CConsumerIrService::TAG("ConsumerIrService");

const Int32 CConsumerIrService::MAX_XMIT_TIME = 2000000; /* in microseconds */

CAR_INTERFACE_IMPL_2(CConsumerIrService, Object, IIConsumerIrService, IBinder)

CAR_OBJECT_IMPL(CConsumerIrService)

CConsumerIrService::CConsumerIrService()
    : mNativeHal(0)
{
}

CConsumerIrService::~CConsumerIrService()
{}

ECode CConsumerIrService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(TRUE);

    mNativeHal = HalOpen();
    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    Boolean bval;
    pkgMgr->HasSystemFeature(IPackageManager::FEATURE_CONSUMER_IR, &bval);
    if (bval) {
        if (mNativeHal == 0) {
            Slogger::E(TAG, "FEATURE_CONSUMER_IR present, but no IR HAL loaded!");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mNativeHal != 0) {
        Slogger::E(TAG, "IR HAL present, but FEATURE_CONSUMER_IR is not set!");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

//@Override
ECode CConsumerIrService::HasIrEmitter(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNativeHal != 0;
    return NOERROR;
}

ECode CConsumerIrService::ThrowIfNoIrEmitter()
{
    if (mNativeHal == 0) {
        Slogger::E(TAG, "IR emitter not available");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

//@Override
ECode CConsumerIrService::Transmit(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 carrierFrequency,
    /* [in] */ ArrayOf<Int32>* pattern)
{
    Int32 perm;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::TRANSMIT_IR, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Requires TRANSMIT_IR permission");
        return E_SECURITY_EXCEPTION;
    }

    Int64 totalXmitTime = 0;
    for (Int32 i = 0; i < pattern->GetLength(); ++i) {
        Int32 slice = (*pattern)[i];
        if (slice <= 0) {
            Slogger::E(TAG, "Non-positive IR slice");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        totalXmitTime += slice;
    }

    if (totalXmitTime > MAX_XMIT_TIME ) {
        Slogger::E(TAG, "IR pattern too Int64");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(ThrowIfNoIrEmitter())

    // Right now there is no mechanism to ensure fair queing of IR requests
    {
        AutoLock syncLock(mHalLock);
        Int32 err = HalTransmit(mNativeHal, carrierFrequency, pattern);

        if (err < 0) {
            Slogger::E(TAG, "Error transmitting: %d", err);
        }
    }
    return NOERROR;
}

//@Override
ECode CConsumerIrService::GetCarrierFrequencies(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 perm;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::TRANSMIT_IR, &perm);
    if (perm != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, "Requires TRANSMIT_IR permission");
        return E_SECURITY_EXCEPTION;
    }

    FAIL_RETURN(ThrowIfNoIrEmitter())

    {    AutoLock syncLock(mHalLock);
        AutoPtr<ArrayOf<Int32> > array = HalGetCarrierFrequencies(mNativeHal);
        *result = array;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

//=============================================================================
// native
//=============================================================================
#include <stdlib.h>
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/consumerir.h>

Int64 CConsumerIrService::HalOpen()
{
    hw_module_t const* module;
    consumerir_device_t *dev;
    int err;

    err = hw_get_module(CONSUMERIR_HARDWARE_MODULE_ID, &module);
    if (err != 0) {
        ALOGE("Can't open consumer IR HW Module, error: %d", err);
        return 0;
    }

    err = module->methods->open(module, CONSUMERIR_TRANSMITTER,
            (hw_device_t **) &dev);
    if (err < 0) {
        ALOGE("Can't open consumer IR transmitter, error: %d", err);
        return 0;
    }

    return reinterpret_cast<Int64>(dev);
}

Int32 CConsumerIrService::HalTransmit(
    /* [in] */ Int64 halObject,
    /* [in] */ Int32 carrierFrequency,
    /* [in] */ ArrayOf<Int32>* pattern)
{
    consumerir_device_t *dev = reinterpret_cast<consumerir_device_t*>(halObject);

    Int32 patternLength = pattern->GetLength();

    return dev->transmit(dev, carrierFrequency, (Int32*)pattern->GetPayload(), patternLength);
}

AutoPtr<ArrayOf<Int32> > CConsumerIrService::HalGetCarrierFrequencies(
    /* [in] */ Int64 halObject)
{
    consumerir_device_t *dev = reinterpret_cast<consumerir_device_t*>(halObject);
    consumerir_freq_range_t *ranges;
    int len;

    len = dev->get_num_carrier_freqs(dev);
    if (len <= 0)
        return NULL;

    ranges = new consumerir_freq_range_t[len];

    len = dev->get_carrier_freqs(dev, len, ranges);
    if (len <= 0) {
        delete[] ranges;
        return NULL;
    }

    int i;
    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(len * 2);
    if (arr == NULL) {
        delete[] ranges;
        return NULL;
    }

    for (i = 0; i < len; i++) {
        arr->Set(i * 2, ranges[i].min);
        arr->Set(i * 2 + 1, ranges[i].max);
    }

    delete[] ranges;
    return arr;
}

ECode CConsumerIrService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}


} // Server
} // Droid
} // Elastos
