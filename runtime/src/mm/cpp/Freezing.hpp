/*
 * Copyright 2010-2020 JetBrains s.r.o. Use of this source code is governed by the Apache 2.0 license
 * that can be found in the LICENSE file.
 */

#ifndef RUNTIME_MM_FREEZING_H
#define RUNTIME_MM_FREEZING_H

struct ObjHeader;

namespace kotlin {
namespace mm {

// Try to recursively freeze `object` along with its members. Returns `nullptr` on success.
// On failure returns the object that has disallowed freezing (by `EnsureNeverFrozen`). In case
// of a failure the `object` tree is left unchanged.
// Note: It's racy with respect to `object` modification.
ObjHeader* FreezeSubgraph(ObjHeader* object) noexcept;

// Make sure current object is never frozen. Returns `false` if current object is already frozen.
// Note: It's racy with respect to `object` modification.
bool EnsureNeverFrozen(ObjHeader* object) noexcept;

} // namespace mm
} // namespace kotlin

#endif // RUNTIME_MM_FREEZING_H
