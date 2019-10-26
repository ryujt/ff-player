#ifndef RYULIB_SIMPLETHREAD_HPP
#define RYULIB_SIMPLETHREAD_HPP

#ifdef __linux__ 
	#include <signal.h>
#elif _WIN32
	#include <windows.h>
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

class SimpleThread;

typedef function<void(SimpleThread*)> SimpleThreadEvent;

class SimpleThread
{
public:
	SimpleThread(const SimpleThreadEvent& event)
		: is_terminated_(false), on_execute_(event)
	{
		thread_ = std::thread([&]() {
			on_execute_(this);
		});
	}
		
	~SimpleThread()
	{
		thread_.detach();
	}

	void sleep(int millis)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait_for(lock, std::chrono::milliseconds(millis));
	}

	void sleepTight()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.wait(lock);
	}

	void wakeUp()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		condition_.notify_all();
	}

	void terminate()
	{
		is_terminated_ = true;
		wakeUp();
	}

	void terminateAndWait()
	{
		is_terminated_ = true;
		wakeUp();
		thread_.join();
	}

	void terminateNow()
	{
		is_terminated_ = true;
		#ifdef __linux__ 
			pthread_cancel(thread_.native_handle());
		#elif _WIN32
			TerminateThread(thread_.native_handle(), 0);
		#endif	
	}

	bool isTerminated() { return is_terminated_;  }
private:
	std::thread thread_;
	std::mutex mutex_;
	std::condition_variable_any condition_;

	bool is_terminated_;

	SimpleThreadEvent on_execute_;
};

#endif  // RYULIB_SIMPLETHREAD_HPP
