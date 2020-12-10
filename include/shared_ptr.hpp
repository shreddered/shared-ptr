// Copyright 2018 Dmitry Kalinin <x.shreddered.x@gmail.com>

#ifndef INCLUDE_SHARED_PTR_HPP_
#define INCLUDE_SHARED_PTR_HPP_

#include <atomic>
#include <cstdint>
#include <utility>

template<typename T>
class SharedPtr {
public:
    SharedPtr() : _ptr(nullptr), _cb(nullptr) {}
    explicit SharedPtr(T* ptr) : _ptr(ptr), _cb(new ControlBlock(ptr)) {}
    SharedPtr(const SharedPtr& other) : _ptr(other._ptr), _cb(other._cb) {
        if (_cb) {
            _cb->refCount.fetch_add(1);
        }
    }
    SharedPtr(SharedPtr&& other) : _ptr(other._ptr), _cb(other._cb) {
        other.invalidate();
    }
    virtual ~SharedPtr() noexcept {
        if (_cb && _cb->fetch_sub(1) == 1) {
            delete _ptr;
            delete _cb;
        }
    }
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            SharedPtr<T>(other).swap(*this);
        }
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if (this != &other) {
            SharedPtr<T>(other).swap(*this);
        }
        return *this;
    }
    operator bool() const {
        return _ptr != nullptr;
    }
    T& operator *() {
        return *_ptr;
    }
    T* operator->() {
        return _ptr;
    }
    T* get() {
        return _ptr;
    }
    void reset() noexcept {
        SharedPtr<T>().swap(*this);
    }
    void reset(T* ptr) {
        SharedPtr<T>(ptr).swap(*this);
    }
    void swap(SharedPtr& other) {
        std::swap(_ptr, other._ptr);
        std::swap(_cb, other._cb);
    }
    std::size_t use_count() const {
        return (_cb) ? static_cast<std::size_t>(_cb->refCount) : 0;
    }

private:
    inline void invalidate() noexcept {
        _ptr = nullptr;
        _cb = nullptr;
    }
    T* _ptr;
    struct ControlBlock;
    ControlBlock* _cb;
}; // class SharedPtr

struct SharedPtr<T>::ControlBlock final {
    std::atomic_size_t refCount;
}; // struct SharedPtr::ControlBlock

#endif // INCLUDE_SHARED_PTR_HPP_
