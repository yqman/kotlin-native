/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#include "Freezing.hpp"

#include "ExtraObjectData.hpp"
#include "KAssert.h"
#include "Memory.h"
#include "ObjectIterators.hpp"

using namespace kotlin;

ObjHeader* mm::FreezeSubgraph(ObjHeader* object) noexcept {
    TODO();
}

bool mm::EnsureNeverFrozen(ObjHeader* object) noexcept {
    if (isPermanentOrFrozen(object)) {
        return false;
    }

    return mm::ExtraObjectData::ForObjHeader(object).EnsureNeverFrozen();
}
