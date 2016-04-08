
#ifndef __ELSWREFBASE_H__
#define __ELSWREFBASE_H__

#include <elastos.h>

using namespace Elastos;

class ElSWRefBase
{
public:
    class WeakRefType
    {
    public:
        ElSWRefBase* GetRefBase() const;

        void IncWeak(const void* id);
        void DecWeak(const void* id);

        Boolean AttemptIncStrong(const void* id);

        //! This is only safe if you have set OBJECT_LIFETIME_FOREVER.
        Boolean AttemptIncWeak(const void* id);

        //! DEBUGGING ONLY: Get current weak ref count.
        Int32 GetWeakCount() const;

        //! DEBUGGING ONLY: Print references held on object.
        void PrintRefs() const;

        //! DEBUGGING ONLY: Enable tracking for this object.
        // enable -- enable/disable tracking
        // retain -- when tracking is enable, if true, then we save a stack trace
        //           for each reference and dereference; when retain == false, we
        //           match up references and dereferences and keep only the
        //           outstanding ones.
        void TrackMe(Boolean enable, Boolean retain);
    };

public:
    void IncStrong(const void* id) const;
    void DecStrong(const void* id) const;

    void ForceIncStrong(const void* id) const;

    //! DEBUGGING ONLY: Get current strong ref count.
    Int32 GetStrongCount() const;

    WeakRefType* CreateWeak(const void* id) const;

    WeakRefType* GetWeakRefs() const;

    //! DEBUGGING ONLY: Print references held on object.
    inline void PrintRefs() const { GetWeakRefs()->PrintRefs(); }

    //! DEBUGGING ONLY: Enable tracking of object.
    inline void TrackMe(Boolean enable, Boolean retain)
    {
        GetWeakRefs()->TrackMe(enable, retain);
    }

    CARAPI_(UInt32) AddRef();
    CARAPI_(UInt32) Release();

protected:
    ElSWRefBase();
    virtual ~ElSWRefBase();

    //! Flags for extendObjectLifetime()
    enum {
        OBJECT_LIFETIME_STRONG  = 0x0000,
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_MASK    = 0x0001
    };

    void ExtendObjectLifetime(Int32 mode);

    //! Flags for onIncStrongAttempted()
    enum {
        FIRST_INC_STRONG = 0x0001
    };

    virtual void OnFirstRef();
    virtual void OnLastStrongRef(const void* id);
    virtual Boolean OnIncStrongAttempted(UInt32 flags, const void* id);
    virtual void OnLastWeakRef(const void* id);

// private:
//     friend class ReferenceMover;
//     static void moveReferences(void* d, void const* s, size_t n,
//             const ReferenceConverterBase& caster);

private:
    friend class WeakRefType;
    class WeakRefImpl;

    ElSWRefBase(const ElSWRefBase& other);
    ElSWRefBase& operator = (const ElSWRefBase& other);

    WeakRefImpl* const mRefs;
};

#endif //__ELSWREFBASE_H__
