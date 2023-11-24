#ifndef EVY_MUTATOR_FUNCTIONS_H
#define EVY_MUTATOR_FUNCTIONS_H

#include "../util/concepts/TupleLike.hpp"
#include "../util/concepts/ValueMatching.hpp"
#include "../util/concepts/IntType.hpp"

#include <random>
#include <type_traits>
#include <boost/pfr.hpp>

namespace evy
{

namespace detail
{
  template<typename ValueType>
  requires (evy::IntType<ValueType> || std::is_floating_point_v<ValueType>)
  auto getGenerator(ValueType min, ValueType max)
  {
    if constexpr(std::is_floating_point_v<ValueType>)
    {
      return std::uniform_real_distribution(min, max);
    }
    else if constexpr(evy::IntType<ValueType>)
    {
      return std::uniform_int_distribution(min, max);
    }
  }
}

template<std::size_t Index, typename ValueType> 
requires (evy::IntType<ValueType> || std::is_floating_point_v<ValueType>)
class MutateNumeric
{
public:
  MutateNumeric(ValueType min, ValueType max):
    min{min},
    max{max},
    rng(std::random_device{}()),
    generator{detail::getGenerator(min, max)}
  {

  }

  template<typename ChromosomeType>
  requires 
    TupleWithMemberTypeAtIndex<ChromosomeType, ValueType, Index> || 
    IndexableWithValue<ChromosomeType, ValueType> ||
    AggregateWithMemberTypeAtIndex<ChromosomeType, ValueType, Index>
  void operator()(ChromosomeType& chromosome)
  {
    if constexpr(AggregateWithMemberTypeAtIndex<ChromosomeType, ValueType, Index>)
    {
      boost::pfr::get<Index>(chromosome) = generator(rng);
    }
    else if constexpr(TupleWithMemberTypeAtIndex<ChromosomeType, ValueType, Index>)
    {
      using std::get;
      get<Index>(chromosome) = generator(rng);
    }
    else if constexpr( IndexableWithValue<ChromosomeType, ValueType> )
    {
      chromosome[Index] = generator(rng);
    }
  }

private:
  ValueType min;
  ValueType max;
  std::mt19937 rng;
  decltype(std::function{detail::getGenerator<ValueType>})::result_type generator;
};

}

#endif