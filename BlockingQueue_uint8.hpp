#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <assert.h>
#include <vector>
#include <stdint.h>

#define DEFAULT_CAPACITY 64

namespace doip_service {

// Singleton class
//template<typename T>
class BlockingQueue {

	using QueueType = std::shared_ptr<std::vector<uint8_t>>;
	using QueueDefine = std::queue<QueueType> ;

private:
	BlockingQueue() : mtx(), full_(), empty_(), capacity_(DEFAULT_CAPACITY) {}

	BlockingQueue(const BlockingQueue &rhs) = delete;

	BlockingQueue &operator=(const BlockingQueue &rhs) = delete;

public:
	static BlockingQueue &GetInstance() {
		static BlockingQueue instance;
		return instance;
	}

	virtual ~BlockingQueue() = default;

	/*void Push(const T &task) {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.size() == capacity_) {
			full_.wait(lock);
		}
		assert(queue_.size() < capacity_);
		queue_.push(task);
		empty_.notify_all();
	}*/

	void Push(QueueType task, const std::chrono::milliseconds &timeout = std::chrono::milliseconds::zero()) {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.size() == capacity_) {
			if (timeout == std::chrono::milliseconds::zero()) {
				full_.wait(lock);
			} else {
				if (!full_.wait_for(lock, timeout, [this]() { return queue_.size() < capacity_; })) {
					throw std::runtime_error("Push timeout");
				}
			}
		}
		assert(queue_.size() < capacity_);
		queue_.push(task);
		empty_.notify_all();
	}

	/*T Take() {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.empty()) {
			empty_.wait(lock);
		}
		assert(!queue_.empty());
		T front(queue_.front());
		queue_.pop();
		full_.notify_all();
		return front;
	}
*/

	int Take(QueueType value,const std::chrono::milliseconds &timeout = std::chrono::milliseconds::zero()) {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.empty()) {
			if (timeout == std::chrono::milliseconds::zero()) {
				empty_.wait(lock);
			} else {
				if (!empty_.wait_for(lock, timeout, [this]() { return !queue_.empty(); })) {
					//throw std::runtime_error("Take timeout");
					return -1;
				}
			}
		}
		assert(!queue_.empty());
		value = queue_.front();
		queue_.pop();
		full_.notify_all();
		return 0;
	}

	QueueType Front() {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.empty()) {
			empty_.wait(lock);
		}
		assert(!queue_.empty());
		QueueType front(queue_.front());
		return front;
	}

	QueueType Back() {
		std::unique_lock<std::mutex> lock(mtx);
		while (queue_.empty()) {
			empty_.wait(lock);
		}
		assert(!queue_.empty());
		QueueType back(queue_.back());
		return back;
	}

	size_t Size() {
		std::lock_guard<std::mutex> lock(mtx);
		return queue_.size();
	}

	bool IsEmpty() {
		std::lock_guard<std::mutex> lock(mtx);
		return queue_.empty();
	}

	void SetCapacity(const size_t capacity) {
		capacity_ = (capacity > 0 ? capacity : DEFAULT_CAPACITY);
	}

private:
	mutable std::mutex mtx;
	std::condition_variable full_;
	std::condition_variable empty_;
	QueueDefine queue_;
	size_t capacity_;

};//class BlockingQueue
}//namespace doip_service

#endif  //BLOCKING_QUEUE_HPP
