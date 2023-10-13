#pragma once

#include <cstdint>
#include <chrono>
#include <mutex>
#include <stdexcept>

// snowflake algorithm  ==> id generation algorithm
// 64-------------63----------------22--------------------12-------------------0
// symbol bit(1)  |   time bit(41)  |   machine bit(10)   |   increas bit(12)  |

// time bit store the difference between the start timestamp and the current timestamp
// machine bit include 5 bit worker id and 5 bit datacenter id

class SnowFlake final
{
    static constexpr int64_t TWEPOCH = 1672502400L;     // default start time 2023-01-01 00:00:00
    static constexpr int64_t WORKER_ID_BITS = 5L;
    static constexpr int64_t DATACENTER_ID_BITS = 5L;
    static constexpr int64_t MAX_WORKER_ID = (1 << WORKER_ID_BITS) - 1;
    static constexpr int64_t MAX_DATACENTER_ID = (1 << DATACENTER_ID_BITS) - 1;
    static constexpr int64_t SEQUENCE_BITS = 12L;
    static constexpr int64_t WORKER_ID_SHIFT = SEQUENCE_BITS;
    static constexpr int64_t DATACENTER_ID_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS;
    static constexpr int64_t TIMESTAMP_LEFT_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS + DATACENTER_ID_BITS;
    static constexpr int64_t SEQUENCE_MASK = (1 << SEQUENCE_BITS) - 1;

    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    time_point startTimePoint_ = std::chrono::steady_clock::now();
    int64_t startMillsecond_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int64_t lastTimestamp_ = -1;
    int64_t workerId_ = 0;
    int64_t datacenterId_ = 0;
    int64_t sequence_ = 0;
    std::mutex lock_;

public:
    static SnowFlake& instance()
    {
        static SnowFlake self;
        return self;
    }
    SnowFlake(SnowFlake&&) = delete;
    SnowFlake(const SnowFlake&) = delete;
    SnowFlake& operator=(const SnowFlake&) = delete;

    void setMachine(int64_t workerId = 0, int64_t datacenterId = 0)
    {
        setWorkerId(workerId);
        setDatacenterId(datacenterId);
    }
    void setWorkerId(int64_t workerId)
    {
        if (workerId > MAX_WORKER_ID || workerId < 0) {
            throw std::runtime_error("worker Id can't be greater than 31 or less than 0");
        }
        workerId_ = workerId;
    }
    void setDatacenterId(int64_t datacenterId)
    {
        if (datacenterId > MAX_DATACENTER_ID || datacenterId < 0) {
            throw std::runtime_error("datacenter Id can't be greater than 31 or less than 0");
        }
        datacenterId_ = datacenterId;
    }
    int64_t nextId()
    {
        std::lock_guard<std::mutex> lock(lock_);
        // std::chrono::steady_clock  cannot decrease as physical time moves forward
        auto timestamp = millsecond();
        if (lastTimestamp_ == timestamp) {
            sequence_ = (sequence_ + 1) & SEQUENCE_MASK;
            if (sequence_ == 0) {
                timestamp = waitNextMillis(lastTimestamp_);
            }
        } else {
            sequence_ = 0;
        }
        lastTimestamp_= timestamp;

        return ((timestamp - TWEPOCH) << TIMESTAMP_LEFT_SHIFT) 
            | (datacenterId_ << DATACENTER_ID_SHIFT) 
            | (workerId_ << WORKER_ID_SHIFT) 
            | sequence_;
    }

protected:
    int64_t millsecond() const noexcept
    {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTimePoint_);
        return startMillsecond_ + diff.count();
    }
    int64_t waitNextMillis(int64_t last) const noexcept
    {
        auto timestamp = millsecond();
        while (timestamp <= last) {
            timestamp = millsecond();
        }
        return timestamp;
    }

private:
    SnowFlake() = default;
    ~SnowFlake() = default;
};