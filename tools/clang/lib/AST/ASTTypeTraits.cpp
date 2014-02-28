//===--- ASTTypeTraits.cpp --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  Provides a dynamic type identifier and a dynamically typed node container
//  that can be used to store an AST base node at runtime in the same storage in
//  a type safe way.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclCXX.h"

namespace clang {
namespace ast_type_traits {

const ASTNodeKind::KindInfo ASTNodeKind::AllKindInfo[] = {
  { NKI_None, "<None>" },
  { NKI_None, "CXXCtorInitializer" },
  { NKI_None, "TemplateArgument" },
  { NKI_None, "NestedNameSpecifier" },
  { NKI_None, "NestedNameSpecifierLoc" },
  { NKI_None, "QualType" },
  { NKI_None, "TypeLoc" },
  { NKI_None, "Decl" },
#define DECL(DERIVED, BASE) { NKI_##BASE, #DERIVED "Decl" },
#include "clang/AST/DeclNodes.inc"
  { NKI_None, "Stmt" },
#define STMT(DERIVED, BASE) { NKI_##BASE, #DERIVED },
#include "clang/AST/StmtNodes.inc"
  { NKI_None, "Type" },
#define TYPE(DERIVED, BASE) { NKI_##BASE, #DERIVED "Type" },
#include "clang/AST/TypeNodes.def"
};

bool ASTNodeKind::isBaseOf(ASTNodeKind Other, unsigned *Distance) const {
  return isBaseOf(KindId, Other.KindId, Distance);
}

bool ASTNodeKind::isSame(ASTNodeKind Other) const {
  return KindId != NKI_None && KindId == Other.KindId;
}

bool ASTNodeKind::isBaseOf(NodeKindId Base, NodeKindId Derived,
                           unsigned *Distance) {
  if (Base == NKI_None || Derived == NKI_None) return false;
  unsigned Dist = 0;
  while (Derived != Base && Derived != NKI_None) {
    Derived = AllKindInfo[Derived].ParentId;
    ++Dist;
  }
  if (Distance)
    *Distance = Dist;
  return Derived == Base;
}

StringRef ASTNodeKind::asStringRef() const { return AllKindInfo[KindId].Name; }

void DynTypedNode::print(llvm::raw_ostream &OS,
                         const PrintingPolicy &PP) const {
  if (const TemplateArgument *TA = get<TemplateArgument>())
    TA->print(PP, OS);
  else if (const NestedNameSpecifier *NNS = get<NestedNameSpecifier>())
    NNS->print(OS, PP);
  else if (const NestedNameSpecifierLoc *NNSL = get<NestedNameSpecifierLoc>())
    NNSL->getNestedNameSpecifier()->print(OS, PP);
  else if (const QualType *QT = get<QualType>())
    QT->print(OS, PP);
  else if (const TypeLoc *TL = get<TypeLoc>())
    TL->getType().print(OS, PP);
  else if (const Decl *D = get<Decl>())
    D->print(OS, PP);
  else if (const Stmt *S = get<Stmt>())
    S->printPretty(OS, 0, PP);
  else if (const Type *T = get<Type>())
    QualType(T, 0).print(OS, PP);
  else
    OS << "Unable to print values of type " << NodeKind.asStringRef() << "\n";
}

void DynTypedNode::dump(llvm::raw_ostream &OS, SourceManager &SM) const {
  if (const Decl *D = get<Decl>())
    D->dump(OS);
  else if (const Stmt *S = get<Stmt>())
    S->dump(OS, SM);
  else
    OS << "Unable to dump values of type " << NodeKind.asStringRef() << "\n";
}

SourceRange DynTypedNode::getSourceRange() const {
  if (const CXXCtorInitializer *CCI = get<CXXCtorInitializer>())
    return CCI->getSourceRange();
  if (const NestedNameSpecifierLoc *NNSL = get<NestedNameSpecifierLoc>())
    return NNSL->getSourceRange();
  if (const TypeLoc *TL = get<TypeLoc>())
    return TL->getSourceRange();
  if (const Decl *D = get<Decl>())
    return D->getSourceRange();
  if (const Stmt *S = get<Stmt>())
    return S->getSourceRange();
  return SourceRange();
}

} // end namespace ast_type_traits
} // end namespace clang
