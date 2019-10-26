#ifndef RYU_WORKER_HPP
#define RYU_WORKER_HPP


#include <ryulib/SimpleThread.hpp>
#include <ryulib/SuspensionQueue.hpp>

using namespace std;

const int TASK_STRING = -1;

class TaskOfWorker
{
public:
	int task;
	void* data;
	int size;
	int tag;
	string text;

	TaskOfWorker(int t, void* d, int s, int g) {
		task = t;
		data = d;
		size = s;
		tag = g;
	}
};

typedef function<void(int, void*, int, int)> TaskEvent;
typedef function<void(const string)> StringEvent;

class Worker {
public:
	Worker() {
		thread_ = new SimpleThread(on_thread_execute);
	}

	~Worker() {
		delete thread_;
	}

	void terminateAndWait()
	{
		thread_->terminateAndWait();
	}

	void terminateNow()
	{
		thread_->terminateNow();
	}

	void add(string text) {
		TaskOfWorker* t = new TaskOfWorker(TASK_STRING, nullptr, 0, 0);
		t->text = text;
		queue_.push(t);
	}

	void add(int task) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfWorker* t = new TaskOfWorker(task, nullptr, 0, 0);
		queue_.push(t);
	}

	void add(int task, void* data) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfWorker* t = new TaskOfWorker(task, data, 0, 0);
		queue_.push(t);
	}

	void add(int task, void* data, int size, int tag) {
		if (task < 0) throw "task must higher than 0.";

		TaskOfWorker* t = new TaskOfWorker(task, data, size, tag);
		queue_.push(t);
	}

	void setOnTask(const TaskEvent& value) { on_task_ = value; }
	void setOnString(const StringEvent& value) { on_string_ = value; }

private:
	bool started_ = false;
	SuspensionQueue<TaskOfWorker*> queue_;
	SimpleThread* thread_;
	SimpleThreadEvent on_thread_execute = [&](SimpleThread * simpleThread) {
		while (simpleThread->isTerminated() == false) {
			TaskOfWorker* t = queue_.pop();
			if (t->task == TASK_STRING) {
				if (on_string_ != nullptr) on_string_(t->text);
			} else  if (on_task_ != nullptr) {
				on_task_(t->task, t->data, t->size, t->tag);
			}

			delete t;
		}
	};

	TaskEvent on_task_ = nullptr;
	StringEvent on_string_ = nullptr;
};


#endif  // RYU_WORKER_HPP