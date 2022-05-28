// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>

#ifndef N_GRAMS_MYQUEUE_HPP
#define N_GRAMS_MYQUEUE_HPP


// this code credited (mostly) by Oleg Farenyuk
template<typename T>
class safe_que {
public:
    safe_que() {
        max_q_size = 1000000000;
        curr_q_size = 0;
        lockable = false;
    }

    explicit safe_que(size_t size) {
        max_q_size = size;
        curr_q_size = 0;
        lockable = false;
    }

    explicit safe_que(size_t size, bool l) {
        max_q_size = size;
        curr_q_size = 0;
        lockable = l;
    }

    ~safe_que() = default;

    safe_que(const safe_que &) = delete;

    safe_que &operator=(const safe_que &) = delete;

    template<typename X>
    void push_end(X &&a, size_t size, const std::string &type) {
        std::unique_lock<std::mutex> lock(mute_m);
        while (max_q_size < curr_q_size + size) {
            cv_popped.wait(lock);
        }
        que_m.push_back(que_e{std::forward<X>(a), size, type});
        curr_q_size += size;
        cv_pushed.notify_one();
    }

    template<typename X>
    void push_start(X &&a, size_t size, const std::string &type) {
        std::unique_lock<std::mutex> lock(mute_m);
        while (max_q_size < curr_q_size + size || (lockable && is_close() && type != "map_m")) {
            cv_popped.wait(lock);
        }
        que_m.push_front(que_e{std::forward<X>(a), size, type});
        curr_q_size += size;
        cv_pushed.notify_one();
    }

    std::pair<T, std::string> pop() {
        std::unique_lock<std::mutex> lock(mute_m);
        while (que_m.empty()) {
            cv_pushed.wait(lock);
        }
        auto a = std::move(que_m.front());
        que_m.pop_front();
        cv_popped.notify_one();

        curr_q_size -= a.size;
        return std::pair<T, std::string>(std::move(a.content), a.type);
    }

    size_t get_size() const {
        std::lock_guard<std::mutex> lock(mute_m);
        return que_m.size();
    }

    bool is_close() {
        if (curr_q_size > max_q_size * 0.9) {
            return true;
        }
        return false;
    }

private:
    struct que_e {
        T content;
        size_t size{};
        std::string type;
    };
    bool lockable;
    size_t max_q_size;
    size_t curr_q_size;
    std::deque<que_e> que_m;
    mutable std::mutex mute_m;
    std::condition_variable cv_pushed;
    std::condition_variable cv_popped;
};

#endif //N_GRAMS_MYQUEUE_HPP