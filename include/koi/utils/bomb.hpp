#pragma once

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
struct Bomb final : NoHeap
{
	Bomb( F &&fn ) :
	  fn( std::move( fn ) ) {}
	
	Bomb(const Bomb &) = delete;
	Bomb(Bomb &&) = default;
	Bomb & operator=(const Bomb &) = delete;
	Bomb & operator=(Bomb &&) = default;

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
