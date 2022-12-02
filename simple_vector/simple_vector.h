
#pragma once

#include <cassert>
#include <initializer_list>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"

class ReserveProxyObj {
public:

    ReserveProxyObj(size_t capacity)
    :capacity_to_reserve(capacity) 
    {
    }

    size_t GetCapacity() const {
        return capacity_to_reserve;
    }

private:
        size_t capacity_to_reserve = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    constexpr SimpleVector() noexcept = default;
    
    SimpleVector(SimpleVector&& other)
    {
        non_copy_swap(std::move(other));
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        swap(rhs);
        return *this;
    }
    
    void non_copy_swap(SimpleVector&& other) {
        this->items_ = std::move(other.items_);
        this->size_ = std::exchange(other.size_, this->size_);
        this->capacity_ = std::exchange(other.capacity_, this->capacity_);
    }
    
    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> tmp(other.size_);
        std::copy(other.begin(), other.end(), &tmp[0]);
        swap(tmp);
    }
    
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            auto rhscopy(rhs);
            swap(rhscopy);
        }
        return *this;
    }
    
    explicit SimpleVector(size_t size)
    : SimpleVector(size, Type())
    {
    }
    
    SimpleVector(size_t size, const Type& value)
    : size_(size), capacity_(size) 
    {
        if(capacity_ != 0) {
            ArrayPtr<Type> temp(size);
            items_.swap(temp);
        }
        std::fill(begin(), end(), value);
    }    

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
    : SimpleVector(init.size())
    {
        if(capacity_ != 0) {
            std::copy(init.begin(), init.end(), begin());
            size_ = capacity_;
        }
    }
    
    explicit SimpleVector(ReserveProxyObj obj) {
        ArrayPtr<Type> temp(obj.GetCapacity());
        items_.swap(temp);
        capacity_ = obj.GetCapacity();
    }
    
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::copy(begin(), end(), &tmp[0]);
            items_.swap(tmp);
            capacity_ = new_capacity;
        }
    }
    
    void PushBack(const Type& item) {
        if(size_ < capacity_) {
            items_[size_] = item;
            ++size_;
        } else {
            ArrayPtr<Type> temp(std::max(size_t(1), capacity_ * 2));
            if(capacity_ != 0) {
                std::copy(begin(), end(), &temp[0]);
                items_.swap(temp);
                capacity_ *= 2;
                items_[size_] = item;
                ++size_;
            } else {
                temp[0] = item;
                items_.swap(temp);
                capacity_ = 1, size_ = 1;
            }
        }
    }

    void PushBack(Type&& item) {
        if(size_ < capacity_) {
            items_[size_] = std::move(item);
            ++size_;
        } else {
            ArrayPtr<Type> temp(std::max(size_t(1), capacity_ * 2));
            if(capacity_ != 0) {
                std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), &temp[0]);
                items_.swap(temp);
                capacity_ *= 2;
                items_[size_] = std::move(item);
                ++size_;
            } else {
                temp[0] = std::move(item);
                items_.swap(temp);
                capacity_ = 1, size_ = 1;
            }
        }
    }
    
    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos <= end());
        Iterator iter = const_cast<Type*>(pos);
        std::move(iter+1, end(), iter);
        assert(size_ > 0);
        size_--;
        return iter;
    }
    
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        Iterator iter = const_cast<Type*>(pos);
        if (iter == end()) {
            PushBack(value);
            return end() - 1;
        }
        if(size_ < capacity_) {
            std::copy_backward(iter, end(), end()+1);
            *iter = value;
            size_++;
        } else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::copy(begin(), iter, &temp[0]);
            for (size_t i = 0; i < size_; ++i) {
                if(&items_[i] == iter) {
                    temp[i] = value;
                    std::copy(iter, end(), &temp[i+1]);
                }
            }
            items_.swap(temp);
            size_++;
            capacity_ = std::max(size_, capacity_ * 2); 
        }
        return iter;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        Iterator iter = const_cast<Type*>(pos);
        if (iter == end()) {
            PushBack(std::move(value));
            return end() - 1;
        }
        if(size_ < capacity_) {
            std::copy_backward(std::make_move_iterator(iter), std::make_move_iterator(end()), end()+1);
            *iter = std::move(value);
            size_++;
        } else {
            ArrayPtr<Type> temp(capacity_ * 2);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(iter), &temp[0]);
            for (size_t i = 0; i < size_; ++i) {
                if(&items_[i] == iter) {
                    temp[i] = std::move(value);
                    std::copy(std::make_move_iterator(iter), std::make_move_iterator(end()), &temp[i+1]);
                }
            }
            items_.swap(temp);
            size_++;
            capacity_ = std::max(size_, capacity_ * 2); 
        }
        return iter;
    }
    
    void PopBack() noexcept {
        assert(size_ != 0);
        size_--;
    }
    
    
    void swap(SimpleVector& other) noexcept {
        this->items_.swap(other.items_);
        std::swap(this->size_, other.size_);
        std::swap(this->capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }
    
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("out_of_range");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
    
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        
        if (new_size > capacity_) {
            ArrayPtr<Type> temp(new_size);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), &temp[0]);
            items_.swap(temp);
            capacity_ = std::max(new_size, capacity_*2);
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            ArrayPtr<Type> temp(new_size);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), &temp[0]);
            items_.swap(temp);
            size_ = new_size;
        }
        
    }
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }
   
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }
    
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !operator==(lhs, rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.cbegin(), rhs.cend(), lhs.cbegin(), lhs.cend());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
} 

