#ifndef RYULIB_SUSPENSIONQUEUE_HPP
#define RYULIB_SUSPENSIONQUEUE_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SuspensionQueue
{
private:
	bool is_terminated_;
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable condition_;
public:
	SuspensionQueue() :
		is_terminated_(false)
	{

	}

	void push(T item)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(item);
		lock.unlock();

		condition_.notify_all();
	}

	T pop() 
	{
		std::unique_lock<std::mutex> lock(mutex_);

		while (queue_.empty()) {
			if (is_terminated_) return NULL;
			condition_.wait(lock);
		}

		auto result = queue_.front();
		queue_.pop();
		return result;
	}

	bool pop(T &item)
	{
		item = NULL;

		std::unique_lock<std::mutex> lock(mutex_);

		while (queue_.empty()) {
			if (is_terminated_) return false;
			condition_.wait(lock);
		}

		item = queue_.front();
		queue_.pop();
		return item;
	}

	void terminate() 
	{
		is_terminated_ = true;
		condition_.notify_all();
	}
public:
	bool is_terminated() { return is_terminated_;  }
	
	bool is_empty() 
	{
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.empty();
	}

	int size() 
	{
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size();
	}
};

#endif  // RYULIB_SUSPENSIONQUEUE_HPP