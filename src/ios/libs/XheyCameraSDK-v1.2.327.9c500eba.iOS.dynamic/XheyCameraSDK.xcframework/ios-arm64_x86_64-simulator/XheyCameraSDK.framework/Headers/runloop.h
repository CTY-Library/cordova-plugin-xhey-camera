#pragma once

#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// #include <boost/lockfree/queue.hpp>
#include "any.h"
#include "event.h"
#include "runloop_msg_center.h"
#include "task_queue.h"

#if defined(_MSC_VER)
#define XHEY_MEDIAKIT_DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__)
#define XHEY_MEDIAKIT_DEPRECATED __attribute__((__deprecated__))
#else
#define XHEY_MEDIAKIT_DEPRECATED
#endif

namespace xhey::camera {

static const int32_t INFINITE_TIMEOUT = -1;

class Runloop {
public:
    Runloop(const std::string &name = "Runloop", std::shared_ptr<RunloopMsgCenter> msg_center = nullptr,
            size_t max_size = 102400);
    virtual ~Runloop();

    static Runloop *SharedInstance();

    bool Post(std::function<void()> func);
    bool PostDelayed(std::function<void()> task, uint32_t delay);
    // !!!WARNING!!!: if msec < 0, will be blocked until func() done
    XHEY_MEDIAKIT_DEPRECATED bool PostAndWait(std::function<void()> func, int32_t wait_ms = INFINITE_TIMEOUT);

    long AddTimer(std::function<void()> func, uint32_t interval, bool repeat);
    void RemoveTimer(long tid);
    void RefreshTimer(long tid);
    std::thread::id thread_id();

    bool AddMsgObserver(const std::string &msg_key, std::function<void(const Some &data)> handle);
    void RemoveMsgObserver(const std::string &msg_key);
    bool PostMsgToObservers(const std::string &msg_key, const Some &data);

    template <class... Args>
    bool PostMsgToObservers(const std::string &msg_key, const Args &...a) {
        auto ptr = msg_center_.lock();
        if (!ptr)
            return false;

        return ptr->PostMsg(msg_key, Some(a...));
    }

    void ClearTaskQueue();
    void Stop();

    bool IsInRunloopThread();

    enum SchedPriorityLevel {
        kSchedPriorityLevelDefault = 0,
        kSchedPriorityLevelMin = 1,
        kSchedPriorityLevelLower = 2,
        kSchedPriorityLevelLow = 3,
        kSchedPriorityLevelSlightlyLow = 4,
        kSchedPriorityLevelSlightlyHigh = 5,
        kSchedPriorityLevelHigh = 6,
        kSchedPriorityLevelHigher = 7,
        kSchedPriorityLevelMax = 8,
    };

    bool SetSchedPriorityLevel(const SchedPriorityLevel level);

protected:
    Runloop(const std::string &name, std::shared_ptr<RunloopMsgCenter> msg_center, size_t max_size,
            TaskQueue<std::function<void()> *> *task_queue);
    virtual void WaitFor(int32_t msec);
    virtual void Wakeup();

    std::thread thread_;
    std::atomic<bool> stop_;
    bool *stop_flag_on_runloop_thread_;
    std::unique_ptr<TaskQueue<std::function<void()> *>> tasks_;
    std::string name_;

private:
    struct Timer {
        long tid;
        std::function<void()> func;
        uint32_t interval;
        uint32_t repeat;
        uint64_t next_fire_time;

        bool operator<(const Timer &t) const { return next_fire_time < t.next_fire_time; }

        bool operator==(long tid) const { return this->tid == tid; }
    };

    struct TaskObject {
        std::function<void()> func;
    };

    struct IdleTimeStatistician {
        std::deque<std::pair<bool, uint64_t>> time_points;
        uint64_t time_window_size = 0;

        void AddTimeSlice(bool idle, uint64_t t) {
            if (time_window_size == 0)
                return;

            if (!time_points.empty() && time_points.front() == std::make_pair(idle, t))
                return;

            time_points.push_front(std::make_pair(idle, t));
            uint64_t available_time = t - time_window_size;
            for (auto it = time_points.begin(); it != time_points.end(); ++it) {
                if (it->second < available_time) {
                    time_points.erase(++it, time_points.end());
                    break;
                }
            }
        }

        std::pair<uint64_t, uint64_t> CalculateIdleTime(uint64_t now) const {
            if (time_window_size == 0)
                return {0, 1};

            uint64_t available_time = now - time_window_size;
            uint64_t idle_time = 0;

            for (auto it = time_points.begin(); it != time_points.end(); ++it) {
                if (it->first)
                    idle_time += now - std::max(it->second, available_time);

                now = it->second;

                if (it->second <= available_time)
                    break;
            }

            return {idle_time, time_window_size};
        }
    };

    void Run(const std::string &name);

    Event post_event_;
    std::list<Timer> timers_;
    std::atomic_long timer_id_generator_;
    std::weak_ptr<RunloopMsgCenter> msg_center_;
#if defined(__APPLE__)
    int default_sched_priority_ = -1;
#endif
    SchedPriorityLevel sched_priority_level_ = kSchedPriorityLevelDefault;

    IdleTimeStatistician idle_statistician_;
};

} // namespace xhey::camera
