/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "ExtraObjectData.hpp"

#include "PointerBits.h"
#include "Weak.h"

#ifdef KONAN_OBJC_INTEROP
#include "ObjCMMAPI.h"
#endif

using namespace kotlin;

// static
mm::ExtraObjectData& mm::ExtraObjectData::ForObjHeader(ObjHeader* object) noexcept {
    return mm::ExtraObjectData::FromMetaObjHeader(object->meta_object());
}

// static
mm::ExtraObjectData& mm::ExtraObjectData::Install(ObjHeader* object) noexcept {
    if (object->has_meta_object()) {
        return mm::ExtraObjectData::FromMetaObjHeader(object->meta_object());
    }

    TypeInfo* typeInfo = object->typeInfoOrMeta_;
    RuntimeCheck(!hasPointerBits(typeInfo, OBJECT_TAG_MASK), "Object must not be tagged");

    auto* data = new ExtraObjectData(typeInfo);

    TypeInfo* old = __sync_val_compare_and_swap(&object->typeInfoOrMeta_, typeInfo, reinterpret_cast<TypeInfo*>(data));
    if (old != typeInfo) {
        // Somebody else created `mm::ExtraObjectData` for this object
        delete data;
        return *reinterpret_cast<mm::ExtraObjectData*>(old);
    }

    return *data;
}

// static
void mm::ExtraObjectData::Uninstall(ObjHeader* object) noexcept {
    RuntimeAssert(object->has_meta_object(), "Object must have a meta object set");

    auto& data = ExtraObjectData::FromMetaObjHeader(object->meta_object());

    *const_cast<const TypeInfo**>(&object->typeInfoOrMeta_) = data.typeInfo_;

    delete &data;
}

bool mm::ExtraObjectData::IsFrozen() const noexcept {
    return (flags_ & Flags::kFrozen) != 0;
}

bool mm::ExtraObjectData::Freeze() noexcept {
    do {
        auto flags = flags_.load();

        if (flags & Flags::kFrozen) return true;

        if (flags & Flags::kNeverFrozen) return false;

        auto newFlags = flags | Flags::kFrozen;
        if (flags_.compare_exchange_weak(flags, newFlags)) return true;
    } while (true);
}

void mm::ExtraObjectData::Unfreeze() noexcept {
    do {
        auto flags = flags_.load();

        if (!(flags & Flags::kFrozen)) return;

        auto newFlags = flags & ~Flags::kFrozen;
        if (flags_.compare_exchange_weak(flags, newFlags)) return;
    } while (true);
}

bool mm::ExtraObjectData::EnsureNeverFrozen() noexcept {
    do {
        auto flags = flags_.load();

        if (flags & Flags::kNeverFrozen) return true;

        if (flags & Flags::kFrozen) return false;

        auto newFlags = flags | Flags::kNeverFrozen;
        if (flags_.compare_exchange_weak(flags, newFlags)) return true;
    } while (true);
}

mm::ExtraObjectData::~ExtraObjectData() {
    if (weakReferenceCounter_) {
        WeakReferenceCounterClear(weakReferenceCounter_);
        ZeroHeapRef(&weakReferenceCounter_);
    }

#ifdef KONAN_OBJC_INTEROP
    Kotlin_ObjCExport_releaseAssociatedObject(associatedObject_);
#endif
}
