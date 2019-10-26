#ifndef RYULIB_THREADQUEUE_HPP
#define RYULIB_THREADQUEUE_HPP

#include <queue>
#include <mutex>

template <typename T>
class ThreadQueue
{
private:
	std::queue<T> queue_;
	std::mutex mutex_;
public:
	void push(T item) 
	{
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push(item);
	}

	T pop() 
	{
		std::unique_lock<std::mutex> lock(mutex_);

		if (queue_.empty()) return NULL;

		auto ressult = queue_.front();
		queue_.pop();

		return ressult;
	}

	bool pop(T &item)
	{
		item = NULL;

		std::unique_lock<std::mutex> lock(mutex_);

		if (queue_.empty()) return false;

		item = queue_.front();
		queue_.pop();

		return true;
	}

	T front()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (queue_.empty()) return NULL;
		return queue_.front();
	}

	T back()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (queue_.empty()) return NULL;
		return queue_.back();
	}
public:
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

#endif  // RYULIB_THREADQUEUE_HPP
