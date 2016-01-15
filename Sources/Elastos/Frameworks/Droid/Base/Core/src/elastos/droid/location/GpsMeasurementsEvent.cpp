
#include "elastos/droid/location/GpsMeasurementsEvent.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL_2(GpsMeasurementsEvent, Object, IGpsMeasurementsEvent, IParcelable)

GpsMeasurementsEvent::GpsMeasurementsEvent()
{
}

ECode GpsMeasurementsEvent::constructor(
    /* [in] */ IGpsClock* clock,
    /* [in] */ ArrayOf<IGpsMeasurement*>* measurements)
{
    if (clock == NULL) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    if (measurements == NULL || measurements->GetLength() == 0) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    mClock = clock;
    AutoPtr<IList> list;
    Arrays::AsList(measurements, (IList**)&list);
    AutoPtr<ICollection> measurementCollection = ICollection::Probe(list);
    assert(0);//'Elastos::Utility::Collections::UnmodifiableCollection' has not been declared
    // Collections::UnmodifiableCollection(measurementCollection, (ICollection**)&mReadOnlyMeasurements);
    return NOERROR;
}

ECode GpsMeasurementsEvent::GetClock(
    /* [out] */ IGpsClock** clock)
{
    VALIDATE_NOT_NULL(clock)
    *clock = mClock;
    REFCOUNT_ADD(*clock)
    return NOERROR;
}

ECode GpsMeasurementsEvent::GetMeasurements(
    /* [out] */ ICollection** coll)
{
    VALIDATE_NOT_NULL(coll)
    *coll = mReadOnlyMeasurements;
    REFCOUNT_ADD(*coll)
    return NOERROR;
}

ECode GpsMeasurementsEvent::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    //TODO
    // ClassLoader classLoader = getClass().getClassLoader();

    // GpsClock clock = in.readParcelable(classLoader);

    // int measurementsLength = in.readInt();
    // GpsMeasurement[] measurementsArray = new GpsMeasurement[measurementsLength];
    // in.readTypedArray(measurementsArray, GpsMeasurement.CREATOR);

    // return new GpsMeasurementsEvent(clock, measurementsArray);
    return NOERROR;
}

ECode GpsMeasurementsEvent::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    //TODO
    // parcel.writeParcelable(mClock, flags);

    // GpsMeasurement[] measurementsArray = mReadOnlyMeasurements.toArray(new GpsMeasurement[0]);
    // parcel.writeInt(measurementsArray.length);
    // parcel.writeTypedArray(measurementsArray, flags);
    return NOERROR;
}

ECode GpsMeasurementsEvent::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder("[ GpsMeasurementsEvent:\n\n");
    String str;

    AutoPtr<IObject> o = IObject::Probe(mClock);
    o->ToString(&str);
    builder += str;
    builder += "\n";

    AutoPtr<IIterable> iterable = IIterable::Probe(mReadOnlyMeasurements);
    AutoPtr<IIterator> iterator;
    iterable->GetIterator((IIterator**)&iterator);
    Boolean hasNext = FALSE;
    AutoPtr<IGpsMeasurement> measurement;
    while(iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iterator->GetNext((IInterface**)&next);
        measurement = IGpsMeasurement::Probe(next);
        AutoPtr<IObject> o = IObject::Probe(measurement);
        o->ToString(&str);
        builder += str;
        builder += "\n";
    }
    builder += "]";
    *result = builder.ToString();
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
