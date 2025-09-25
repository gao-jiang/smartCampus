#pragma once

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class MyQueue {
public:
    MyQueue() : head_(0) {}

    MyQueue(const MyQueue& other)
        : data_(other.data_.begin() + other.head_, other.data_.end()), head_(0) {}

    MyQueue& operator=(const MyQueue& other) {
        if (this != &other) {
            data_.assign(other.data_.begin() + other.head_, other.data_.end());
            head_ = 0;
        }
        return *this;
    }

    MyQueue(MyQueue&& other) noexcept
        : data_(std::move(other.data_)), head_(other.head_) {
        other.head_ = 0;
    }

    MyQueue& operator=(MyQueue&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            head_ = other.head_;
            other.head_ = 0;
        }
        return *this;
    }

    void push(const T& value) {
        data_.push_back(value);
    }

    void push(T&& value) {
        data_.push_back(std::move(value));
    }

    std::size_t size() const {
        return data_.size() - head_;
    }

    bool empty() const {
        return size() == 0;
    }

    void pop() {
        if (empty()) {
            throw std::out_of_range("MyQueue::pop on empty queue");
        }
        ++head_;
        if (head_ > data_.size() / 2) {
            std::vector<T> tmp(data_.begin() + head_, data_.end());
            data_.swap(tmp);
            head_ = 0;
        }
    }

    T& front() {
        if (empty()) {
            throw std::out_of_range("MyQueue::front on empty queue");
        }
        return data_[head_];
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("MyQueue::front on empty queue");
        }
        return data_[head_];
    }

    void clear() {
        data_.clear();
        head_ = 0;
    }

    void print() const {
        std::cout << "队列内容: ";
        if (empty()) {
            std::cout << "(空)\n";
            return;
        }
        for (std::size_t i = head_; i < data_.size(); ++i) {
            std::cout << data_[i] << ' ';
        }
        std::cout << '\n';
    }

private:
    std::vector<T> data_;
    std::size_t head_;
};
