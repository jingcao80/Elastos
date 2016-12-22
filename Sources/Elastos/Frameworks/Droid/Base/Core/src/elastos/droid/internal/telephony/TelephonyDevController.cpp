
#include "elastos/droid/internal/telephony/TelephonyDevController.h"
#include "elastos/droid/internal/telephony/CTelephonyDevController.h"
#include "elastos/droid/internal/telephony/HardwareConfig.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  TelephonyDevController::
//==============================================================
const String TelephonyDevController::LOGTAG("TDC");
const Boolean TelephonyDevController::DBG = TRUE;
Object TelephonyDevController::mLock;

const Int32 TelephonyDevController::EVENT_HARDWARE_CONFIG_CHANGED = 1;

AutoPtr<ITelephonyDevController> TelephonyDevController::sTelephonyDevController;

static AutoPtr<IArrayList> InitModem()
{
    AutoPtr<IArrayList> p;
    CArrayList::New((IArrayList**)&p);
    return p;
}
AutoPtr<IArrayList> TelephonyDevController::mModems = InitModem();

static AutoPtr<IArrayList> InitSims()
{
    AutoPtr<IArrayList> p;
    CArrayList::New((IArrayList**)&p);
    return p;
}
AutoPtr<IArrayList> TelephonyDevController::mSims = InitSims();

AutoPtr<IMessage> TelephonyDevController::sRilHardwareConfig;

CAR_INTERFACE_IMPL(TelephonyDevController, Handler, ITelephonyDevController)

void TelephonyDevController::Logd(
    /* [in] */ String s)
{
    // Rlog::D(LOG_TAG, s);
}

void TelephonyDevController::Loge(
    /* [in] */ String s)
{
    // Rlog::E(LOG_TAG, s);
}

AutoPtr<ITelephonyDevController> TelephonyDevController::Create()
{
    {
        AutoLock syncLock(mLock);
        if (sTelephonyDevController != NULL) {
            // throw new RuntimeException("TelephonyDevController already created!?!");
            return NULL;
        }
        CTelephonyDevController::New((ITelephonyDevController**)&sTelephonyDevController);
        return sTelephonyDevController;
    }
}

AutoPtr<ITelephonyDevController> TelephonyDevController::GetInstance()
{
    {
        AutoLock syncLock(mLock);
        if (sTelephonyDevController == NULL) {
            // throw new RuntimeException("TelephonyDevController not yet created!?!");
            return NULL;
        }
        return sTelephonyDevController;
    }
}

void TelephonyDevController::InitFromResource()
{
    AutoPtr<IResourcesHelper> hlp;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
    AutoPtr<IResources> resource;
    hlp->GetSystem((IResources**)&resource);
    AutoPtr<ArrayOf<String> > hwStrings;
    resource->GetStringArray(
        R::array::config_telephonyHardware, (ArrayOf<String>**)&hwStrings);
    if (hwStrings != NULL) {
        for (Int32 i = 0; i < hwStrings->GetLength(); i++) {
        String hwString = (*hwStrings)[i];
            AutoPtr<HardwareConfig> hw = new HardwareConfig(hwString);
            // CHardwareConfig::New(hwString, (IHardwareConfig**)&hw);
            if (hw != NULL) {
                if (hw->mType == IHardwareConfig::DEV_HARDWARE_TYPE_MODEM) {
                     UpdateOrInsert(hw, mModems);
                }
                else if (hw->mType == IHardwareConfig::DEV_HARDWARE_TYPE_SIM) {
                     UpdateOrInsert(hw, mSims);
                }
            }
        }
    }
}

ECode TelephonyDevController::constructor()
{
    InitFromResource();

    mModems->TrimToSize();
    mSims->TrimToSize();
    return NOERROR;
}

ECode TelephonyDevController::RegisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    /* get the current configuration from this ril... */
    cmdsIf->GetHardwareConfig(sRilHardwareConfig);
    /* ... process it ... */
    if (sRilHardwareConfig != NULL) {
        AutoPtr<IInterface> obj;
        sRilHardwareConfig->GetObj((IInterface**)&obj);
        AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
        if (ar->mException == NULL) {
            HandleGetHardwareConfigChanged(ar);
        }
    }
    /* and register for async device configuration change. */
    cmdsIf->RegisterForHardwareConfigChanged(IHandler::Probe(sTelephonyDevController),
                                            EVENT_HARDWARE_CONFIG_CHANGED,
                                            NULL);
    return NOERROR;
}

ECode TelephonyDevController::UnregisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    cmdsIf->UnregisterForHardwareConfigChanged(IHandler::Probe(sTelephonyDevController));
    return NOERROR;
}

ECode TelephonyDevController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<AsyncResult> ar;
    switch (what) {
        case EVENT_HARDWARE_CONFIG_CHANGED: {
            if (DBG) {
                Logd(String("handleMessage: received EVENT_HARDWARE_CONFIG_CHANGED"));
            }
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            HandleGetHardwareConfigChanged(ar);
        }
        break;
        default: {
            // String str("handleMessage: Unknown Event ");
            // str += what;
            // Loge(str);
        break;
        }
    }
    return NOERROR;
}

void TelephonyDevController::UpdateOrInsert(
    /* [in] */ IHardwareConfig* hw,
    /* [in] */ IArrayList* list)
{
    Int32 size = 0;
    AutoPtr<IHardwareConfig> item;
    {
        AutoLock syncLock(mLock);
        list->GetSize(&size);
        for (Int32 i = 0 ; i < size ; i++) {
            AutoPtr<IInterface> p;
            list->Get(i, (IInterface**)&p);
            item = IHardwareConfig::Probe(p);
            HardwareConfig* hwc = (HardwareConfig*)item.Get();
            if (hwc->mUuid.Equals(((HardwareConfig*)hw)->mUuid)) {
                if (DBG) {
                    String str("updateOrInsert: removing: ");
                    String itemStr;
                    IObject::Probe(item)->ToString(&itemStr);;
                    str += itemStr;
                    Logd(str);
                }
                list->Remove(i);
            }
        }
        if (DBG) {
            String str("updateOrInsert: inserting: ");
            String hwStr;
            IObject::Probe(hw)->ToString(&hwStr);
            str += hwStr;
            Logd(str);
        }
        list->Add(hw);
    }
}

void TelephonyDevController::HandleGetHardwareConfigChanged(
    /* [in] */ AsyncResult* ar)
{
    if ((ar->mException == NULL) && (ar->mResult != NULL)) {
        AutoPtr<IList> hwcfg = IList::Probe(ar->mResult);
        Int32 size = 0;
        hwcfg->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> p;
            hwcfg->Get(i, (IInterface**)&p);
            AutoPtr<IHardwareConfig> hw = IHardwareConfig::Probe(p);
            if (hw != NULL) {
                // if (hw->mType == IHardwareConfig::DEV_HARDWARE_TYPE_MODEM) {
                //     UpdateOrInsert(hw, mModems);
                // }
                // else if (hw->mType == IHardwareConfig::DEV_HARDWARE_TYPE_SIM) {
                //     UpdateOrInsert(hw, mSims);
                // }
            }
        }
    }
    else {
        /* error detected, ignore.  are we missing some real time configutation
         * at this point?  what to do...
         */
        Loge(String("handleGetHardwareConfigChanged - returned an error."));
    }
}

Int32 TelephonyDevController::GetModemCount()
{
    {
        AutoLock syncLock(mLock);
        Int32 count = 0;
        mModems->GetSize(&count);
        if (DBG) {
            // String str("getModemCount: ");
            // str += count;
            // Logd(str);
        }
        return count;
    }
}

ECode TelephonyDevController::GetModem(
    /* [in] */ Int32 index,
    /* [out] */ IHardwareConfig** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        Boolean bEmp = FALSE;
        mModems->IsEmpty(&bEmp);
        if (bEmp) {
            Loge(String("getModem: no registered modem device?!?"));
            *result = NULL;
            return NOERROR;
        }

        Int32 count = GetModemCount();
        if (index > count) {
            // String str("getModem: out-of-bounds access for modem device ");
            // str += index;
            // str += " max: ";
            // str += count;
            // Loge(str);
            *result = NULL;
            return NOERROR;
        }

        if (DBG) {
            // String str("getModem: ");
            // str += index;
            // Logd(str);
        }
        AutoPtr<IInterface> p;
        mModems->Get(index, (IInterface**)&p);
        *result = IHardwareConfig::Probe(p);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode TelephonyDevController::GetSimCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        Int32 count = 0;
        mSims->GetSize(&count);
        if (DBG) {
            // String str("getSimCount: ");
            // str += count;
            // Logd(str);
        }
        *result = count;
        return NOERROR;
    }
}

ECode TelephonyDevController::GetSim(
    /* [in] */ Int32 index,
    /* [out] */ IHardwareConfig** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        Boolean bEmp = FALSE;
        mSims->IsEmpty(&bEmp);
        if (bEmp) {
            Loge(String("getSim: no registered sim device?!?"));
            *result = NULL;
            return NOERROR;
        }

        Int32 count = 0;
        GetSimCount(&count);
        if (index > count) {
            // String str("getSim: out-of-bounds access for sim device ");
            // str += index;
            // str += " max: ";
            // str += count;
            // Loge(str);
            *result = NULL;
            return NOERROR;
        }

        if (DBG) {
            // String str("getSim: ");
            // str += index;
            // Logd(str);
        }
        AutoPtr<IInterface> p;
        mSims->Get(index, (IInterface**)&p);
        *result = IHardwareConfig::Probe(p);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode TelephonyDevController::GetModemForSim(
    /* [in] */ Int32 simIndex,
    /* [out] */ IHardwareConfig** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        Boolean bModemEmp = FALSE, bSimsEmp = FALSE;
        mModems->IsEmpty(&bModemEmp);
        mSims->IsEmpty(&bSimsEmp);
        if (bModemEmp || bSimsEmp) {
            Loge(String("getModemForSim: no registered modem/sim device?!?"));
            *result = NULL;
            return NOERROR;
        }

        Int32 count = 0;
        GetSimCount(&count);
        if (simIndex > count) {
            // String str("getModemForSim: out-of-bounds access for sim device ");
            // str += simIndex;
            // str += " max: ";
            // str += count;
            // Loge(str);
            *result = NULL;
            return NOERROR;
        }

        if (DBG) {
            // String str("getModemForSim ");
            // str += simIndex;
            // Logd(str);
        }

        AutoPtr<IHardwareConfig> sim;
        GetSim(simIndex, (IHardwareConfig**)&sim);
        AutoPtr<IIterator> it;
        mModems->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IHardwareConfig> modem = IHardwareConfig::Probe(p);
            assert(0 && "TODO");
            // if (modem->mUuid.Equals(sim->mModemUuid)) {
            //     *result = modem;
            //     return NOERROR;
            // }
        }

        *result = NULL;
        return NOERROR;
    }
}

ECode TelephonyDevController::GetAllSimsForModem(
    /* [in] */ Int32 modemIndex,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        Boolean bEmp = FALSE;
        mSims->IsEmpty(&bEmp);
        if (bEmp) {
            Loge(String("getAllSimsForModem: no registered sim device?!?"));
            *result = NULL;
            return NOERROR;
        }

        Int32 count = GetModemCount();
        if (modemIndex > count) {
            // String str("getAllSimsForModem: out-of-bounds access for modem device ");
            // str += modemIndex;
            // str += " max: ";
            // str += count;
            // Loge(str);
            *result = NULL;
            return NOERROR;
        }

        if (DBG) {
            // String str("getAllSimsForModem ");
            // str += modemIndex;
            // Logd(str);
        }

        AutoPtr<IArrayList> res;
        CArrayList::New((IArrayList**)&res);
        AutoPtr<IHardwareConfig> modem;
        GetModem(modemIndex, (IHardwareConfig**)&modem);
        AutoPtr<IIterator> it;
        mSims->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IHardwareConfig> sim = IHardwareConfig::Probe(p);
            assert(0 && "TODO");
            // if (sim->mModemUuid->Equals(modem->mUuid)) {
            //     res->Add(sim);
            // }
        }
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode TelephonyDevController::GetAllModems(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        AutoPtr<IArrayList> modems;
        CArrayList::New((IArrayList**)&modems);
        Boolean bEmp = FALSE;
        mModems->IsEmpty(&bEmp);
        if (bEmp) {
            if (DBG) {
                Logd(String("getAllModems: empty list."));
            }
        }
        else {
            AutoPtr<IIterator> it;
            mModems->GetIterator((IIterator**)&it);
            Boolean bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                // AutoPtr<IHardwareConfig> modem = IHardwareConfig::Probe(p);
                modems->Add(p);
            }
        }

        *result = modems;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode TelephonyDevController::GetAllSims(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(mLock);
        AutoPtr<IArrayList> sims;
        CArrayList::New((IArrayList**)&sims);
        Boolean bEmp = FALSE;
        mSims->IsEmpty(&bEmp);
        if (bEmp) {
            if (DBG) {
                Logd(String("getAllSims: empty list."));
            }
        }
        else {
            AutoPtr<IIterator> it;
            mSims->GetIterator((IIterator**)&it);
            Boolean bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> sim;
                it->GetNext((IInterface**)&sim);
                // HardwareConfig sim: mSims
                sims->Add(sim);
            }
        }

        *result = sims;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
