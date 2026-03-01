#pragma once

#include <cstdint>

template <typename T, uint16_t Capacity>
class RingQueue {
    static_assert(Capacity > 0, "RingQueue Capacity must be > 0");

public:
    RingQueue() : head_(0), tail_(0), count_(0) {}

    bool isEmpty() const {
        return count_ == 0;
    }

    uint16_t itemCount() const {
        return count_;
    }

    void enqueue(const T& item) {
        if (count_ == Capacity) {
            head_ = (head_ + 1) % Capacity;
        } else {
            count_++;
        }

        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % Capacity;
    }

    // Dequeue into out. Returns false if empty.
    bool dequeue(T& out) {
        if (count_ == 0) {
            return false;
        }

        out = buffer_[head_];
        head_ = (head_ + 1) % Capacity;
        count_--;
        return true;
    }

private:
    T buffer_[Capacity];
    uint16_t head_;
    uint16_t tail_;
    uint16_t count_;
};