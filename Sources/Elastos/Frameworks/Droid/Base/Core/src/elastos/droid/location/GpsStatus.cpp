
#include "elastos/droid/location/GpsStatus.h"
#include "elastos/droid/location/CGpsSatellite.h"
#include <elastos/core/AutoLock.h>

using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::EIID_IIterator;

namespace Elastos {
namespace Droid {
namespace Location {

//---------------GpsStatus::SatelliteIterator-----------------//

CAR_INTERFACE_IMPL(GpsStatus::SatelliteIterator, Object, IIterator)

GpsStatus::SatelliteIterator::SatelliteIterator(
    /* [in] */ ArrayOf<IGpsSatellite* >* satellites)
    : mSatellites(satellites)
    , mIndex(0)
{
}

ECode GpsStatus::SatelliteIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    for (Int32 i = mIndex; i < mSatellites->GetLength(); i++) {
        Boolean valid = FALSE;
        (*mSatellites)[i]->IsValid(&valid);
        if (valid) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode GpsStatus::SatelliteIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    while (mIndex < mSatellites->GetLength()) {
        AutoPtr<IGpsSatellite> satellite = (*mSatellites)[mIndex++];
        Boolean valid = FALSE;
        satellite->IsValid(&valid);
        if (valid) {
            *object = satellite;
            REFCOUNT_ADD(*object)
            return NOERROR;
        }
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode GpsStatus::SatelliteIterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//---------------GpsStatus::StatelliteLists--------------------//

CAR_INTERFACE_IMPL(GpsStatus::StatelliteList, Object, IIterable)

GpsStatus::StatelliteList::StatelliteList(
    /* [in] */ GpsStatus* host)
    : mHost(host)
{}

ECode GpsStatus::StatelliteList::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    AutoPtr<SatelliteIterator> si = new SatelliteIterator(mHost->mSatellites);
    *it = IIterator::Probe(si.Get());
    REFCOUNT_ADD(*it)
    return NOERROR;
}

//---------------GpsStatus-----------------//
const Int32 GpsStatus::NUM_SATELLITES;

CAR_INTERFACE_IMPL(GpsStatus, Object, IGpsStatus)

GpsStatus::GpsStatus()
    : mTimeToFirstFix(0)
{
    mSatellites = ArrayOf<IGpsSatellite*>::Alloc(NUM_SATELLITES);
    for(Int32 i = 0; i < mSatellites->GetLength(); i++)
    {
        AutoPtr<IGpsSatellite> satellite;
        CGpsSatellite::New(i + 1, (IGpsSatellite**)&satellite);
        mSatellites->Set(i, satellite);
    }
    AutoPtr<StatelliteList> sl= new StatelliteList(this);
    mSatelliteList = IIterable::Probe(sl);
}

ECode GpsStatus::constructor()
{
    return NOERROR;
}

ECode GpsStatus::SetStatus(
    /* [in] */ Int32 svCount,
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ Int32 ephemerisMask,
    /* [in] */ Int32 almanacMask,
    /* [in] */ Int32 usedInFixMask)
{
    synchronized(this) {
        Int32 i;
        for (i = 0; i < mSatellites->GetLength(); i++) {
            (*mSatellites)[i]->SetValid(FALSE);
        }

        for (i = 0; i < svCount; i++) {
            Int32 prn = (*prns)[i] - 1;
            Int32 prnShift = (1 << prn);
            if (prn >= 0 && prn < mSatellites->GetLength()) {
                AutoPtr<IGpsSatellite> satellite = (*mSatellites)[prn];

                satellite->SetValid(TRUE);
                satellite->SetSnr((*snrs)[i]);
                satellite->SetElevation((*elevations)[i]);
                satellite->SetAzimuth((*azimuths)[i]);
                satellite->SetHasEphemeris((ephemerisMask & prnShift) != 0);
                satellite->SetHasAlmanac((almanacMask & prnShift) != 0);
                satellite->SetUsedInFix((usedInFixMask & prnShift) != 0);
            }
        }
    }
    return NOERROR;
}

ECode GpsStatus::SetStatus(
    /* [in] */ IGpsStatus* status)
{
    status->GetTimeToFirstFix(&mTimeToFirstFix);

    AutoPtr<GpsStatus> statusSrc = (GpsStatus*)status;
    for(Int32 i = 0; i < mSatellites->GetLength(); i++) {
        AutoPtr<IGpsSatellite> temp = (*mSatellites)[i];
        AutoPtr<CGpsSatellite> pcTemp = (CGpsSatellite*)(temp.Get());
        pcTemp->SetStatus((*(statusSrc->mSatellites))[i]);
    }
    return NOERROR;
}

ECode GpsStatus::SetTimeToFirstFix(
    /* [in] */ Int32 ttff)
{
    mTimeToFirstFix = ttff;
    return NOERROR;
}

ECode GpsStatus::GetTimeToFirstFix(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mTimeToFirstFix;
    return NOERROR;
}

ECode GpsStatus::GetSatellites(
    /* [out] */ IIterable** satellites)
{
    VALIDATE_NOT_NULL(satellites);
    *satellites = mSatelliteList.Get();
    REFCOUNT_ADD(*satellites)
    return NOERROR;
}

ECode GpsStatus::GetMaxSatellites(
    /* [out] */ Int32* maxNumber)
{
    VALIDATE_NOT_NULL(maxNumber);
    *maxNumber = NUM_SATELLITES;
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
