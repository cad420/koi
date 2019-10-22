#pragma once

#include <functional>
#include <traits/concepts.hpp>

namespace koi
{
namespace utils
{
namespace _
{
using namespace std;
using namespace traits::concepts;

template <typename F>
struct Bomb final : NoCopy, NoHeap
{
	Bomb( F &&fn ) :
	  fn( std::move( fn ) )
	{
	}
	Bomb( Bomb && ) = default;
	Bomb &operator=( Bomb && ) = default;

	~Bomb() { fn(); }

private:
	F fn;
};

template <typename F>
auto make_bomb( F &&fn )
{
	return Bomb<F>( std::forward<F>( fn ) );
}

}  // namespace _

using _::make_bomb;

}  // namespace utils

}  // namespace koi
