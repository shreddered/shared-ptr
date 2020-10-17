// Copyright 2018 Dmitry Kalinin <x.shreddered.x@gmail.com>

#ifndef INCLUDE_SHARED_PTR_HPP_
#define INCLUDE_SHARED_PTR_HPP_

#include <atomic>
#include <cstdint>

template<typename T>
class SharedPtr {
public:
    SharedPtr() : _ptr(nullptr), _cb(nullptr) {}
    SharedPtr(T* ptr) : _ptr(ptr), _cb(new ControlBlock(ptr)) {}
    SharedPtr(const SharedPtr& other) : _ptr(other._ptr), _cb(other._cb) {
        if (_cb) {
            ++_cb->refCount;
        }
    }
    SharedPtr(SharedPtr&& other) : _ptr(other._ptr), _cb(other._cb) {
        other.invalidate();
    }
    virtual ~SharedPtr() noexcept {
        if (_cb) {
            _cb->release();
            delete _cb;
        }
    }
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            _ptr = other._ptr;
            _cb = other._cb;
            if (_cb) {
                ++_cb->refCount;
            }
        }
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        _ptr = other._ptr;
        _cb = other._cb;
        other.invalidate();
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
    void reset() {
        SharedPtr<T>().swap(*this);
    }
    void reset(T* ptr) {
        SharedPtr<T>(ptr).swap(*this);
    }
    void swap(SharedPtr& ptr) {
        SharedPtr tmp = std::move(ptr);
        ptr = std::move(*this);
        *this = std::move(tmp);
    }
    std::size_t use_count() const {
        return _cb->refCount;
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

template<typename T>
struct SharedPtr<T>::ControlBlock final {
    std::atomic_size_t refCount;
    std::atomic<T*> ptr;
    explicit ControlBlock(T* _ptr) : ptr(_ptr), refCount(1) {}
    inline void release() {
        if(--refCount == 0) {
            delete ptr;
        }
    }
}; // struct SharedPtr::ControlBlock

#endif // INCLUDE_SHARED_PTR_HPP_
