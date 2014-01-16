//===--- AbstractionPattern.h - SIL type abstraction pattersn ---*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
// This file defines the AbstractionPattern class, which is used to
// lower formal AST types into their SIL lowerings.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_SIL_ABSTRACTIONPATTERN_H
#define SWIFT_SIL_ABSTRACTIONPATTERN_H

#include "swift/AST/Types.h"

namespace swift {
namespace Lowering {

/// A pattern for the abstraction of a value.  See the large comment
/// in SILGenPoly.cpp.
///
/// An abstraction pattern is represented with an original,
/// unsubstituted type.  The archetypes or generic parameters
/// naturally fall at exactly the specified abstraction points.
class AbstractionPattern {
  CanType OrigType;
public:
  AbstractionPattern() {}
  explicit AbstractionPattern(Type origType)
    : AbstractionPattern(origType->getCanonicalType()) {}
  explicit AbstractionPattern(CanType origType) : OrigType(origType) {}
  CanType getAsType() const { return OrigType; }

  bool isNull() const { return OrigType.isNull(); }

  bool isOpaque() const {
    if (auto arch = dyn_cast<ArchetypeType>(OrigType))
      return !arch->requiresClass();
    // FIXME: Check class constraint of dependent types in their originating
    // context
    if (isa<GenericTypeParamType>(OrigType))
      return true;
    if (isa<DependentMemberType>(OrigType))
      return true;
    return false;
  }

  bool matchesTuple(CanTupleType substType) {
    if (auto tuple = dyn_cast<TupleType>(OrigType))
      return tuple->getNumElements() == substType->getNumElements();
    return isOpaque();
  }
  AbstractionPattern getTupleElementType(unsigned index) const {
    if (auto tuple = dyn_cast<TupleType>(OrigType))
      return AbstractionPattern(tuple.getElementType(index));
    assert(isOpaque());
    return AbstractionPattern(OrigType);
  }

  AbstractionPattern getLValueObjectType() const {
    if (auto iot = dyn_cast<InOutType>(OrigType))
      return AbstractionPattern(iot->getObjectType());
    assert(isOpaque());
    return AbstractionPattern(OrigType);
  }

  AbstractionPattern getFunctionResultType() const {
    if (auto fn = dyn_cast<AnyFunctionType>(OrigType))
      return AbstractionPattern(fn.getResult());
    assert(isOpaque());
    return AbstractionPattern(OrigType);
  }
  AbstractionPattern getFunctionInputType() const {
    if (auto fn = dyn_cast<AnyFunctionType>(OrigType))
      return AbstractionPattern(fn.getInput());
    assert(isOpaque());
    return AbstractionPattern(OrigType);
  }

  void dump() const LLVM_ATTRIBUTE_USED { OrigType.dump(); }
  void print(raw_ostream &OS) const { OrigType.print(OS); }
};

}
}


#endif
