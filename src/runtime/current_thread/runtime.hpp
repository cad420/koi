#pragma once

#include <memory>
#include <utility>
#include <list>

#include <runtime/runtime.hpp>
#include <reactor/reactor.hpp>
#include <sync/queue.hpp>
#include <future/future.hpp>
#include <executor/executor.hpp>
#include <traits/concepts.hpp>
#include <utils/nonnull.hpp>

namespace koi
{
namespace runtime
{
namespace current_thread
{
namespace _
{
using namespace std;
using namespace chrono;
using namespace traits::concepts;
using namespace future;
using namespace reactor;
using namespace koi::utils;
using namespace sync;

struct Runtime;

struct Inner
{
	list<Box<Future<>>> tasks;
};

struct Scheduler
{
	void schedule( Box<Future<>> &&future )
	{
		// _->tasks.emplace_back( std::move( future ) );
	}
	size_t tick()
	{
		size_t nfut = 0;
		auto &tasks = _->tasks;
		for ( auto itr = tasks.begin(); itr != tasks.end(); ) {
			switch ( ( *itr )->poll() ) {
			case PollState::Pending:
				++itr;
				break;
			case PollState::Ok:
				nfut++;
			case PollState::Pruned:
				itr = tasks.erase( itr );
			}
		}
		return nfut;
	}
	bool idle() const { return _->tasks.empty(); }

	//private:
	Arc<Inner> _ = Arc<Inner>( new Inner );
};

struct Executor final : executor::Executor
{
	Executor() = default;

	void spawn( Box<Future<>> &&future ) override
	{
		scheduler._->tasks.emplace_back( std::move( future ) );
		scheduler.schedule( std::move( future ) );
	}
	void run( nanoseconds const *timeout = nullptr )
	{
		reactor.with( [=] {
			while ( true ) {
				scheduler.tick();
				if ( scheduler.idle() ) {
					return;
				}
				if ( !reactor.idle() ) {
					reactor.poll();
				}
			}
		} );
	}

private:
	Scheduler scheduler;
	Reactor reactor;
	friend struct Runtime;
};

struct Runtime : runtime::_::Runtime
{
	struct Builder final : NoCopy
	{
		Runtime build() { return Runtime{}; }

	private:
		friend struct Runtime;
	};

	Runtime() = default;

	void run() override
	{
		runtime::_::current_runtime().with( *this, [&] {
			this->executor.run();
		} );
	}

	void spawn( Box<Future<>> &&fut ) override
	{
		this->executor.spawn( std::move( fut ) );
	}

private:
	Executor executor;
	friend struct Builder;
};

}  // namespace _

using Runtime = runtime::_::TypedRuntime<_::Runtime>;

}  // namespace current_thread

}  // namespace runtime

}  // namespace koi
