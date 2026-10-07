// LeakStack.h

#ifndef LEAK_STACK
#define LEAK_STACK

#include <climits>
#include <utility>
#include <stdexcept>
#include <algorithm>

namespace leak_stack {

template <typename TYPE>
class LeakStack {
private:
    unsigned long long _MaxSize = ULLONG_MAX;
    unsigned long long _Capacity = 0;
    unsigned long long _Size = 0;
    TYPE* _Datas = nullptr;

    void _Rebuild(unsigned long long newCapacity) {
        TYPE* _newDatas = new TYPE[newCapacity];
        for (unsigned long long i = 0; i < _Size; ++i) {
            _newDatas[i] = std::move(_Datas[i]);
        }
        delete[] _Datas;
        _Datas = _newDatas;
        _Capacity = newCapacity;
    }

    template <typename U>
    void _PushImpl(U&& val) {
        if (_Size >= _MaxSize) {
            throw std::length_error("LeakStack: reached MaxSize");
        }
        if (_Size == _Capacity) {
            unsigned long long newCap = _Size + 1;
            if (newCap > _MaxSize) {
                newCap = _MaxSize;
            }
            _Rebuild(newCap);
        }
        _Datas[_Size] = std::forward<U>(val);
        ++_Size;
    }

public:

    LeakStack() = default;

    LeakStack(unsigned long long size, const TYPE& val) {
        _Size = size;
        _Capacity = size;
        _Datas = new TYPE[size];
        for (unsigned long long i = 0; i < size; ++i) {
            _Datas[i] = val;
        }
    }

    LeakStack(const TYPE* start, const TYPE* end) {
        _Size = static_cast<unsigned long long>(end - start);
        _Capacity = _Size;
        _Datas = new TYPE[_Capacity];
        for (unsigned long long i = 0; i < _Size; ++i) {
            _Datas[i] = start[i];
        }
    }

    ~LeakStack() {
        delete[] _Datas;
    }

    LeakStack(const LeakStack&) = delete;

    LeakStack& operator=(const LeakStack&) = delete;

    LeakStack(LeakStack&& other) noexcept
        : _MaxSize(other._MaxSize),
          _Capacity(other._Capacity),
          _Size(other._Size),
          _Datas(other._Datas) {
        other._Datas = nullptr;
        other._Size = 0;
        other._Capacity = 0;
    }

    LeakStack& operator=(LeakStack&& other) noexcept {
        if (this != &other) {
            delete[] _Datas;
            _MaxSize = other._MaxSize;
            _Capacity = other._Capacity;
            _Size = other._Size;
            _Datas = other._Datas;
            other._Datas = nullptr;
            other._Size = 0;
            other._Capacity = 0;
        }
        return *this;
    }

    void SetMaxSize(unsigned long long maxSize) {
        if (maxSize < _Size) {
            throw std::length_error("LeakStack: MaxSize < current Size");
        }
        _MaxSize = maxSize;
        if (_Capacity > _MaxSize) {
            _Rebuild(_MaxSize);
        }
    }

    unsigned long long MaxSize() const {
        return _MaxSize;
    }

    void PrepareCapacity(unsigned long long cap) {
        if (cap < _Size) {
            throw std::length_error("LeakStack: PrepareCapacity < current Size");
        }
        if (cap > _MaxSize) {
            throw std::length_error("LeakStack: PrepareCapacity > MaxSize");
        }
        if (cap == _Capacity) {
            return;
        }
        _Rebuild(cap);
    }

    TYPE Leakage() const {
        if (_Size == 0) {
            throw std::out_of_range("LeakStack is empty");
        }
        return _Datas[_Size / 2];
    }

    void Push(const TYPE& val) {
        _PushImpl(val);
    }

    void Push(TYPE&& val) {
        _PushImpl(std::move(val));
    }

    void Take() {
        if (_Size == 0) {
            throw std::out_of_range("LeakStack is empty");
        }

        unsigned long long mid = _Size / 2;

        for (unsigned long long i = mid; i + 1 < _Size; ++i) {
            _Datas[i] = std::move(_Datas[i + 1]);
        }

        _Datas[_Size - 1].~TYPE();
        --_Size;
    }

    void Clear() {
        for (unsigned long long i = 0; i < _Size; ++i) {
            _Datas[i].~TYPE();
        }
        _Size = 0;
    }

    unsigned long long Size() const {
        return _Size;
    }

    unsigned long long Capacity() const {
        return _Capacity;
    }

    bool Empty() const {
        return _Size == 0;
    }

    bool Full() const {
        return _Size == _MaxSize;
    }
};

}

#endif