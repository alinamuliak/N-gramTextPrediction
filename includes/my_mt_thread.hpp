#ifndef MY_MT_THREAD_HPP
#define MY_MT_THREAD_HPP

#include "iostream"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <map>
//#include "../includes/files.h"


template<typename T>
class my_mt_thread {
public:
    my_mt_thread(size_t m_size) {
        max_size = m_size;
    }
    ~my_mt_thread() = default;
    my_mt_thread(const my_mt_thread&) = delete;
    my_mt_thread& operator=(const my_mt_thread&) = delete;

    void push_back(const T& a) {
        {
            std::unique_lock<std::mutex> lock(m_member);
            while(que_m.size() >= max_size) {
                cv_m_full.wait(lock);
            }
            que_m.push_back(a);
        }
        cv_m_empty.notify_one();
    }

    void push_front(const T& a) {
        {
            std::unique_lock<std::mutex> lock(m_member);
            while(que_m.size() >= max_size) {
                cv_m_full.wait(lock);
            }
            que_m.push_front(a);
        }
        cv_m_empty.notify_one();
    }

    T pop_front() {
        T a;
        {
            std::unique_lock<std::mutex> lock(m_member);
            while(que_m.empty()) {
                cv_m_empty.wait(lock);
            }
            //cv_m.wait(lock, [this](){ return !que_m.empty(); }); те саме

            a = que_m.front();
            que_m.pop_front();
        }
        cv_m_full.notify_one();
        return a;
    }

    T front() {
        T a;
        std::unique_lock<std::mutex> lock(m_member);
        while(que_m.empty()) {
            cv_m_empty.wait(lock);
        }

        a = que_m.front();

        return a;
    }

    size_t get_size() const {
        std::lock_guard<std::mutex> lock(m_member);
        return que_m.size();
    }

    void print_que() {
        for (auto it = que_m.begin(); it != que_m.end(); ++it)
                std::cout << "'" << *it  <<"', " << std::endl;
        }

    bool get_status() {
        std::lock_guard<std::mutex> lock(m_member);
        return pill;
    }

    void set_status(bool status) {
        std::lock_guard<std::mutex> lock(m_member);
        pill = status;
    }

    size_t get_counter() {
        std::lock_guard<std::mutex> lock(m_member);
        return counter;
    }

    void set_counter_inc(size_t num) {
        std::lock_guard<std::mutex> lock(m_member);
        counter += num;
    }

    void close() {
        closed = true;
    }

    bool is_closed() {
        return closed;
    }

private:
    std::deque<T> que_m;
    mutable std::mutex m_member;
    std::condition_variable cv_m_empty;
    std::condition_variable cv_m_full;
    size_t max_size;
    size_t counter = 0;
    bool pill = false;
    bool closed = false;
};


template<typename K, typename V>
class my_mt_map {
public:
    my_mt_map() = default;
    ~my_mt_map() = default;
    my_mt_map(const my_mt_map&) = delete;
    my_mt_map& operator=(const my_mt_map&) = delete;
    V& operator[](K key) {
        return map_mult[key];
    }

    void insert(const std::pair<K, V> &p) {
        std::lock_guard<std::mutex> lock(m_m);
        map_mult.insert({p.first, p.second});
    }

    void merge(const std::map<K, V>& local) {
        std::lock_guard<std::mutex> lock(m_m);
        for (auto const& el: local){
            auto itr = map_mult.find(el.first);
            if (itr!=map_mult.end()) {
                map_mult[el.first]+= el.second;
            }
            else{
                map_mult.emplace(std::pair<std::string, int>(el.first, 1));
            }
        }
    }


    std::map<K, V> cast_to_map() {
        return map_mult;
    }


private:
    std::map<K, V> map_mult;
    mutable std::mutex m_m;
};


#endif // MY_MT_THREAD_HPP
