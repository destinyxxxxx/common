#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <functional>

class ThreadPool 
{
private:
	typedef std::function<void(void)> Task;

public:
	ThreadPool(uint32_t size = 10, uint32_t maxSize = -1) : maxSize_(maxSize)
	{
		resize(size);
	}
	~ThreadPool()
	{
		stop();
	}

public:
	template<class F, class ...Args>
	void commit(F&& f, Args&&... args)
	{
		if (available() == 0 && size() < maxSize_) {
			resize(std::max<size_t>(threads_.size() + threads_.size() / 2, 1));
		}
		std::lock_guard<std::mutex> locker(mutex_);
		tasks_.push(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		cv_.notify_one();
	}

	void resize(size_t size)
	{
		std::unique_lock<std::mutex> locker(mutex_);
		if (size > threads_.size()) {
			for (size_t i = size - threads_.size(); i != 0; i--) {
				threads_.push_back(std::thread(std::bind(&ThreadPool::run, this)));
			}
		}
	}

	uint32_t available() const
	{
		std::unique_lock<std::mutex> locker(mutex_);
		return threads_.size() - load_;
	}

	uint32_t size() const
	{
		std::unique_lock<std::mutex> locker(mutex_);
		return threads_.size();
	}

	void stop()
	{
		std::unique_lock<std::mutex> locker(mutex_);
		stop_ = true;
		cv_.notify_all();
		locker.unlock();
		for (auto&& future : threads_) {
			future.join();
		}
	}

private:
	void run()
	{
		while (!stop_) {
			std::unique_lock<std::mutex> locker(mutex_);
			cv_.wait(locker, [this]() {
				return !tasks_.empty() || stop_;
			});
			if (stop_) {
				break;
			}
			load_++;
			auto task = tasks_.front();
			tasks_.pop();
			locker.unlock();
			task();
			load_--;
		}
	}

private:
	std::vector<std::thread> threads_;
	std::queue<Task> tasks_;
	mutable std::mutex mutex_;
	std::condition_variable cv_;
	std::atomic_bool stop_{ false };
	uint32_t load_{ 0 };
	uint32_t maxSize_{ (uint32_t) - 1 };
};