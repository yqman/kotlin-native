/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#ifndef RUNTIME_MM_EXTRA_OBJECT_DATA_H
#define RUNTIME_MM_EXTRA_OBJECT_DATA_H

#include <cstddef>
#include <cstdint>

#include "Memory.h"
#include "TypeInfo.h"
#include "Utils.hpp"

namespace kotlin {
namespace mm {

// Optional data that's lazily allocated only for objects that need it.
class ExtraObjectData : private Pinned {
public:
    MetaObjHeader* AsMetaObjHeader() noexcept { return reinterpret_cast<MetaObjHeader*>(this); }
    static ExtraObjectData& FromMetaObjHeader(MetaObjHeader* header) noexcept { return *reinterpret_cast<ExtraObjectData*>(header); }

    static ExtraObjectData& ForObjHeader(ObjHeader* object) noexcept;

    static ExtraObjectData& Install(ObjHeader* object) noexcept;
    static void Uninstall(ObjHeader* object) noexcept;

#ifdef KONAN_OBJC_INTEROP
    void** GetAssociatedObjectLocation() noexcept { return &associatedObject_; }
#endif

    ObjHeader** GetWeakCounterLocation() noexcept { return &weakReferenceCounter_; }

    bool IsFrozen() const noexcept;
    // Returns `false` if failed to freeze the object (if it was set to be never frozen).
    bool Freeze() noexcept;
    void Unfreeze() noexcept;
    // Returns `false` if the object is already frozen.
    bool EnsureNeverFrozen() noexcept;

private:
    enum Flags : uint32_t {
        kFrozen = 1 << 0,
        kNeverFrozen = 1 << 1,
    };

    explicit ExtraObjectData(const TypeInfo* typeInfo) noexcept : typeInfo_(typeInfo) {}
    ~ExtraObjectData();

    // Must be first to match `TypeInfo` layout.
    const TypeInfo* typeInfo_;

    std::atomic<uint32_t> flags_{0};

#ifdef KONAN_OBJC_INTEROP
    void* associatedObject_ = nullptr;
#endif

    // TODO: Need to respect when marking.
    ObjHeader* weakReferenceCounter_ = nullptr;
};

} // namespace mm
} // namespace kotlin

#endif // RUNTIME_MM_EXTRA_OBJECT_DATA_H
