/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#ifndef RUNTIME_OBJECT_ITERATORS_H
#define RUNTIME_OBJECT_ITERATORS_H

#include "KAssert.h"
#include "Memory.h"
#include "Natives.h"

namespace kotlin {

class ObjectFieldsIterable {
public:
    class Iterator {
    public:
        Iterator(ObjHeader* object, uint32_t index) noexcept : object_(object), index_(index) {}

        ObjHeader*& operator*() noexcept { return *Get(); }

        ObjHeader** operator->() noexcept { return Get(); }

        Iterator& operator++() noexcept {
            ++index_;
            return *this;
        }

        Iterator& operator--() noexcept {
            --index_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept { return object_ == rhs.object_ && index_ == rhs.index_; }

        bool operator!=(const Iterator& rhs) const noexcept { return !(*this == rhs); }

    private:
        ObjHeader** Get() noexcept {
            return reinterpret_cast<ObjHeader**>(reinterpret_cast<uint8_t*>(object_) + object_->type_info()->objOffsets_[index]);
        }

        ObjHeader* object_;
        uint32_t index_;
    };

    explicit ObjectFieldsIterable(ObjHeader* object) noexcept : object_(object) {
        RuntimeAssert(object->type_info()->IsArray(), "Must not be an array");
    }

    Iterator begin() noexcept { return Iterator(object_, 0); }
    Iterator end() noexcept { return Iterator(object_, object_->type_info()->objOffsetsCount_); }

private:
    static ObjHeader** AddressAt(ObjHeader* object, uint32_t index) noexcept {
        const auto* typeInfo = object->type_info();
        if (typeInfo->IsArray()) {
            return ArrayAddressOfElementAt(reinterpret_cast<ArrayHeader*>(object), index);
        } else {
        }
    }

    ObjHeader* object_;
};

class ArrayMemberIterable {
public:
    class Iterator {
    public:
        Iterator(ArrayHeader* array, uint32_t index) noexcept : array_(array), index_(index) {}

        ObjHeader*& operator*() noexcept { return *Get(); }

        ObjHeader** operator->() noexcept { return Get(); }

        Iterator& operator++() noexcept {
            ++index_;
            return *this;
        }

        Iterator& operator--() noexcept {
            --index_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept { return array_ == rhs.array_ && index_ == rhs.index_; }

        bool operator!=(const Iterator& rhs) const noexcept { return !(*this == rhs); }

    private:
        ObjHeader** Get() noexcept {
            return ArrayAddressOfElementAt(array_, index_);
        }

        ObjHeader* array_;
        uint32_t index_;
    };

    explicit ArrayMemberIterable(ArrayHeader* array) noexcept : array_(array) {
        RuntimeAssert(array->type_info()->IsArray(), "Must be an array");
    }

    Iterator begin() noexcept { return Iterator(array_, 0); }
    Iterator end() noexcept { return Iterator(array_, array_->count_); }

private:
    ArrayHeader* array_;
};

} // namespace kotlin

#endif // RUNTIME_OBJECT_ITERATORS_H
