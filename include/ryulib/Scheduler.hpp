#ifndef RYU_SCHEDULER_HPP
#define RYU_SCHEDULER_HPP


#include <ryulib/SimpleThread.hpp>
#include <ryulib/ThreadQueue.hpp>

using namespace std;

const int TASK_STRING = -1;

class TaskOfScheduler
{
public:
	int task;
	void* data;
	int size;
	int tag;
	string text;

	TaskOfScheduler(int t, void* d, int s, int g) {
		task = t;
		data = d;
		size = s;
		tag = g;
	}
};

typedef function<void(int, void*, int, int)> TaskEvent;
typedef function<void()> RepeatEvent;
typedef function<void(const string)> StringEvent;

class Scheduler {
public:
	Scheduler() {
		thread_ = new SimpleThread(on_thread_execute);
	}

	~Scheduler() {
		stop();
		delete thread_;
	}

	void terminateAndWait()
	{
		stop();
		thread_->terminateAndWait();
	}

	void terminateNow()
	{
		stop();
		thread_->terminateNow();
	}

	void start() {
		started_ = true;
	}

	void stop() {
		started_ = false;
	}
	
	void add(string text) {
		TaskOfScheduler* t = new TaskOfScheduler(TASK_STRING, nullptr, 0, 0);
		t->text = text;
		queue_.push(t);
		thread_->wakeUp();
	}

	void add(int task) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfScheduler* t = new TaskOfScheduler(task, nullptr, 0, 0);
		queue_.push(t);
		thread_->wakeUp();
	}

	void add(int task, void* data) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfScheduler* t = new TaskOfScheduler(task, data, 0, 0);
		queue_.push(t);
		thread_->wakeUp();
	}

	void add(int task, void* data, int size, int tag) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfScheduler* t = new TaskOfScheduler(task, data, size, tag);
		queue_.push(t);
		thread_->wakeUp();
	}

	void sleep(int millis)
	{
		thread_->sleep(millis);
	}

	void setOnTask(const TaskEvent& value) { on_task_ = value; }
	void setOnRepeat(const RepeatEvent& value) { on_repeat_ = value; }

private:
	bool started_ = false;
	ThreadQueue<TaskOfScheduler*> queue_;
	SimpleThread* thread_;
	SimpleThreadEvent on_thread_execute = [&](SimpleThread * simpleThread) {
		while (simpleThread->isTerminated() == false) {
			TaskOfScheduler* t = queue_.pop();
			if (t != NULL) {
				if (t->task == TASK_STRING) {
					if (on_string_ != nullptr) on_string_(t->text);
				} else  if (on_task_ != nullptr) {
					on_task_(t->task, t->data, t->size, t->tag);
				}
				delete t;
			}

			if (started_) {
				if (on_repeat_ != nullptr) {
					on_repeat_();
				} else {
					thread_->sleep(1);
				}
			}
		}
	};

	TaskEvent on_task_ = nullptr;
	RepeatEvent on_repeat_ = nullptr;
	StringEvent on_string_ = nullptr;
};


#endif  // RYU_SCHEDULER_HPP