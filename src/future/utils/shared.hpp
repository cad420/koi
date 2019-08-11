#pragma once

#include <type_traits>

#include <future/future.hpp>
#include <utils/nonnull.hpp>

namespace koi
{
namespace future
{
namespace utils
{
namespace _
{
using namespace std;
using namespace traits::concepts;
using namespace koi::utils;

template <typename F, typename O = void>
struct Shared;

template <typename F>
struct Shared<F> : Future<typename F::Output>
{
	bool poll() override { return _->poll(); }

	Shared( F &&_ ) :
	  _( new F( std::forward<F>( _ ) ) )
	{
	}

private:
	Arc<F> _;
};

template <typename F>
struct Shared<
  F, typename enable_if<
	   !is_same<void, typename F::Output>::value,
	   typename F::Output>::type> : Future<typename F::Output>
{
	bool poll() override { return _->poll(); }
	typename F::Output get() override
	{
		return this->_->get();
	}

	Shared( F &&_ ) :
	  _( new F( std::forward<F>( _ ) ) )
	{
	}

private:
	Arc<F> _;
};

}  // namespace _

using _::Shared;

}  // namespace utils

}  // namespace future

}  // namespace koi
