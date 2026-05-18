#pragma once

#include "any.h"
#include <functional>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

namespace xhey::camera {

class Runloop;

class RunloopMsgCenter {
public:
    bool AddMsgObserver(const std::string &msg_key, Runloop *runloop, std::function<void(const Some &data)> handle);
    void RemoveMsgObserver(const std::string &msg_key, Runloop *runloop);
    void RemoveAllObservers(Runloop *runloop);

    bool PostMsg(const std::string &msg_key, const Some &data);

    template <class... Args>
    bool PostMsg(const std::string &msg_key, const Args &...data) {
        return PostMsg(msg_key, Some(data...));
    }

private:
    struct MsgHandle {
        Runloop *runloop;
        std::function<void(const Some &data)> func;
        bool operator<(const MsgHandle &o) const { return runloop < o.runloop; }
    };

    bool InsertMsgMap(const std::string &msg_key, Runloop *runloop, std::function<void(const Some &data)> handle);
    bool InsertRunloopMap(const std::string &msg_key, Runloop *runloop);
    void EraseMsgMap(const std::string &msg_key, Runloop *runloop);
    void EraseRunloopMap(const std::string &msg_key, Runloop *runloop);
    std::pair<bool, std::function<void(const Some &data)>> LockAndFindHandle(const std::string &msg_key, Runloop *runloop);

    std::recursive_mutex map_mutex_;
    std::unordered_map<std::string, std::set<MsgHandle>> msg_map_;
    std::unordered_map<Runloop *, std::set<std::string>> runloop_map_;
};

} // namespace xhey::camera
