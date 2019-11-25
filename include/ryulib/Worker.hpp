#ifndef RYU_WORKER_HPP
#define RYU_WORKER_HPP


#include <ryulib/base.hpp>
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

	TaskOfWorker(int t, string txt, void* d, int s, int g) {
		task = t;
		text = txt;
		data = d;
		size = s;
		tag = g;
	}
};

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

	void add(int task) {
		TaskOfWorker* t = new TaskOfWorker(task, "", nullptr, 0, 0);
		queue_.push(t);
	}

	void add(string text) {
		TaskOfWorker* t = new TaskOfWorker(TASK_STRING, text, nullptr, 0, 0);
		queue_.push(t);
	}

	void add(int task, void* data) {
		TaskOfWorker* t = new TaskOfWorker(task, "", data, 0, 0);
		queue_.push(t);
	}

	void add(int task, void* data, int size, int tag) {
		TaskOfWorker* t = new TaskOfWorker(task, "", data, size, tag);
		queue_.push(t);
	}

	void setOnTask(const TaskEvent& value) { on_task_ = value; }

	bool is_empty() { return queue_.is_empty(); }

private:
	bool started_ = false;
	SuspensionQueue<TaskOfWorker*> queue_;
	SimpleThread* thread_;
	SimpleThreadEvent on_thread_execute = [&](SimpleThread * simpleThread) {
		while (simpleThread->isTerminated() == false) {
			TaskOfWorker* t = queue_.pop();
			if (on_task_ != nullptr) {
				on_task_(t->task, t->text, t->data, t->size, t->tag);
			}

			delete t;
		}
	};

	TaskEvent on_task_ = nullptr;
};


#endif  // RYU_WORKER_HPP