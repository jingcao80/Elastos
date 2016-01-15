
#ifndef __ETL_STACK_H__
#define __ETL_STACK_H__

#include <elastos/utility/etl/etl_def.h>
#include <elastos/utility/etl/ConceptCheck.h>
#include <elrefbase.h>

using Elastos::ElLightRefBase;

_ETL_NAMESPACE_BEGIN

// Forward declarations of operators == and <, needed for friend declaration.

template <class Tp,
          class Sequence = Deque<Tp> >
class Stack;

template <class Tp, class Seq>
Boolean operator==(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y);

template <class Tp, class Seq>
Boolean operator<(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y);


template <class Tp, class Sequence>
class Stack : public ElLightRefBase
{
  // concept requirements
    Concept_ClassRequires(Tp, SGIAssignableConcept)
    Concept_ClassRequires(Sequence, BackInsertionSequenceConcept)
    typedef typename Sequence::ValueType SequenceValueType;
    Concept_ClassRequires2(Tp, SequenceValueType, SameTypeConcept);

    template <class Tp1, class Seq1>
    friend Boolean operator== (const Stack<Tp1, Seq1>&,
                          const Stack<Tp1, Seq1>&);
    template <class Tp1, class Seq1>
    friend Boolean operator< (const Stack<Tp1, Seq1>&,
                         const Stack<Tp1, Seq1>&);
public:
    typedef typename Sequence::ValueType       ValueType;
    typedef typename Sequence::SizeType        SizeType;
    typedef           Sequence                   ContainerType;

    typedef typename Sequence::Reference       Reference;
    typedef typename Sequence::ConstReference ConstReference;

protected:
    Sequence c;

public:
    Stack() : c() {}
    explicit Stack(const Sequence& s) : c(s) {}

    Boolean IsEmpty() const { return c.IsEmpty(); }
    SizeType GetSize() const { return c.GetSize(); }
    Reference GetTop() { return c.GetBack(); }
    ConstReference GetTop() const { return c.GetBack(); }
    void Push(const ValueType& x) { c.PushBack(x); }
    void Pop() { c.PopBack(); }
};

template <class Tp, class Seq>
Boolean operator==(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return x.c == y.c;
}

template <class Tp, class Seq>
Boolean operator<(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return x.c < y.c;
}

template <class Tp, class Seq>
Boolean operator!=(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return !(x == y);
}

template <class Tp, class Seq>
Boolean operator>(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return y < x;
}

template <class Tp, class Seq>
Boolean operator<=(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return !(y < x);
}

template <class Tp, class Seq>
Boolean operator>=(const Stack<Tp, Seq>& x, const Stack<Tp, Seq>& y)
{
  return !(x < y);
}

_ETL_NAMESPACE_END


#endif //__ETL_STACK_H__
