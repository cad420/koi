#pragma once

#include <future/future.hpp>
#include <future/decorator/decorated.hpp>
#include <utils/option.hpp>
#include <traits/function.hpp>

namespace koi
{
namespace future
{
namespace utils
{
namespace _
{
using namespace std;
using namespace traits;

using namespace koi::utils;
using namespace koi::utils::_;

// template <typename R>
// Lazy<R> lazy( function<R()> &&fn );

template <typename R = void>
struct Lazy;

template <>
struct Lazy<> : Future<>
{
	void poll() { fn(); }

	Lazy( function<void()> &&fn ) :
	  fn( std::move( fn ) )
	{
	}

private:
	function<void()> fn;
};

template <typename R>
struct Lazy : Future<R>
{
	void poll() override { _ = fn(); }
	R poll_result() override
	{
		this->poll();
		return std::move( _.value() );
	}

	// private:
	Lazy( function<R()> &&fn ) :
	  fn( std::move( fn ) )
	{
	}

private:
	// template <typename F>
	// friend Lazy<F> lazy();
	function<R()> fn;
	Option<R> _;
};

template <typename F>
Decorated<
  Lazy<
	typename InvokeResultOf<F>::type>>
  lazy( F &&fn )
{
	using TaskFn = typename InferFunction<F>::type;
	using R = typename InvokeResultOf<TaskFn>::type;
	return Decorated<Lazy<R>>(
	  Lazy<R>( std::move( static_cast<TaskFn &&>( fn ) ) ) );
}

}  // namespace _

using _::lazy;

}  // namespace utils

}  // namespace future

}  // namespace koi
