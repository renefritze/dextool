/// @copyright Boost License 1.0, http://boost.org/LICENSE_1_0.txt
/// @date 2017
/// @author Joakim Brännström (joakim.brannstrom@gmx.com)
///
/// All copied code from libclang is under the original license! Obviously.
//===- CXCursor.cpp - Routines for manipulating CXCursors -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines routines for manipulating CXCursors. It should be the
// only file that has internal knowledge of the encoding of the data in
// CXCursor.
//
//===----------------------------------------------------------------------===//
#include "libclang_interop.hpp"

// used by translateSourceLocation
#include "clang-c/Index.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"

#include "llvm_version.h"

namespace clang {
namespace cxcursor {
// See: CXCursor.cpp
const clang::Decl* getCursorParentDecl(CXCursor Cursor) {
    return static_cast<const clang::Decl*>(Cursor.data[0]);
}

// See: CXCursor.cpp
CXCursor dex_MakeCXCursor(const clang::Stmt* S, const clang::Decl* Parent, CXTranslationUnit TU,
                          clang::SourceRange RegionOfInterest) {
    assert(S && TU && "Invalid arguments!");
    CXCursorKind K = CXCursor_NotImplemented;

    switch (S->getStmtClass()) {
    case Stmt::NoStmtClass:
        break;

    case Stmt::CaseStmtClass:
        K = CXCursor_CaseStmt;
        break;

    case Stmt::DefaultStmtClass:
        K = CXCursor_DefaultStmt;
        break;

    case Stmt::IfStmtClass:
        K = CXCursor_IfStmt;
        break;

    case Stmt::SwitchStmtClass:
        K = CXCursor_SwitchStmt;
        break;

    case Stmt::WhileStmtClass:
        K = CXCursor_WhileStmt;
        break;

    case Stmt::DoStmtClass:
        K = CXCursor_DoStmt;
        break;

    case Stmt::ForStmtClass:
        K = CXCursor_ForStmt;
        break;

    case Stmt::GotoStmtClass:
        K = CXCursor_GotoStmt;
        break;

    case Stmt::IndirectGotoStmtClass:
        K = CXCursor_IndirectGotoStmt;
        break;

    case Stmt::ContinueStmtClass:
        K = CXCursor_ContinueStmt;
        break;

    case Stmt::BreakStmtClass:
        K = CXCursor_BreakStmt;
        break;

    case Stmt::ReturnStmtClass:
        K = CXCursor_ReturnStmt;
        break;

    case Stmt::GCCAsmStmtClass:
        K = CXCursor_GCCAsmStmt;
        break;

    case Stmt::MSAsmStmtClass:
        K = CXCursor_MSAsmStmt;
        break;

    case Stmt::ObjCAtTryStmtClass:
        K = CXCursor_ObjCAtTryStmt;
        break;

    case Stmt::ObjCAtCatchStmtClass:
        K = CXCursor_ObjCAtCatchStmt;
        break;

    case Stmt::ObjCAtFinallyStmtClass:
        K = CXCursor_ObjCAtFinallyStmt;
        break;

    case Stmt::ObjCAtThrowStmtClass:
        K = CXCursor_ObjCAtThrowStmt;
        break;

    case Stmt::ObjCAtSynchronizedStmtClass:
        K = CXCursor_ObjCAtSynchronizedStmt;
        break;

    case Stmt::ObjCAutoreleasePoolStmtClass:
        K = CXCursor_ObjCAutoreleasePoolStmt;
        break;

    case Stmt::ObjCForCollectionStmtClass:
        K = CXCursor_ObjCForCollectionStmt;
        break;

    case Stmt::CXXCatchStmtClass:
        K = CXCursor_CXXCatchStmt;
        break;

    case Stmt::CXXTryStmtClass:
        K = CXCursor_CXXTryStmt;
        break;

    case Stmt::CXXForRangeStmtClass:
        K = CXCursor_CXXForRangeStmt;
        break;

    case Stmt::SEHTryStmtClass:
        K = CXCursor_SEHTryStmt;
        break;

    case Stmt::SEHExceptStmtClass:
        K = CXCursor_SEHExceptStmt;
        break;

    case Stmt::SEHFinallyStmtClass:
        K = CXCursor_SEHFinallyStmt;
        break;

    case Stmt::SEHLeaveStmtClass:
        K = CXCursor_SEHLeaveStmt;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::CoroutineBodyStmtClass:
    case Stmt::CoreturnStmtClass:
        K = CXCursor_UnexposedStmt;
        break;

    case Stmt::ArrayTypeTraitExprClass:
    case Stmt::AsTypeExprClass:
    case Stmt::AtomicExprClass:
    case Stmt::BinaryConditionalOperatorClass:
    case Stmt::TypeTraitExprClass:
    case Stmt::CoawaitExprClass:
    case Stmt::DependentCoawaitExprClass:
    case Stmt::CoyieldExprClass:
    case Stmt::CXXBindTemporaryExprClass:
    case Stmt::CXXDefaultArgExprClass:
    case Stmt::CXXDefaultInitExprClass:
    case Stmt::CXXFoldExprClass:
    case Stmt::CXXRewrittenBinaryOperatorClass:
    case Stmt::CXXStdInitializerListExprClass:
    case Stmt::CXXScalarValueInitExprClass:
    case Stmt::CXXUuidofExprClass:
    case Stmt::ChooseExprClass:
    case Stmt::DesignatedInitExprClass:
    case Stmt::DesignatedInitUpdateExprClass:
    case Stmt::ArrayInitLoopExprClass:
    case Stmt::ArrayInitIndexExprClass:
    case Stmt::ExprWithCleanupsClass:
    case Stmt::ExpressionTraitExprClass:
    case Stmt::ExtVectorElementExprClass:
    case Stmt::ImplicitCastExprClass:
    case Stmt::ImplicitValueInitExprClass:
    case Stmt::NoInitExprClass:
    case Stmt::MaterializeTemporaryExprClass:
    case Stmt::ObjCIndirectCopyRestoreExprClass:
    case Stmt::OffsetOfExprClass:
    case Stmt::ParenListExprClass:
    case Stmt::PredefinedExprClass:
    case Stmt::ShuffleVectorExprClass:
    case Stmt::SourceLocExprClass:
    case Stmt::ConvertVectorExprClass:
    case Stmt::VAArgExprClass:
    case Stmt::ObjCArrayLiteralClass:
    case Stmt::ObjCDictionaryLiteralClass:
    case Stmt::ObjCBoxedExprClass:
    case Stmt::ObjCSubscriptRefExprClass:
    case Stmt::RecoveryExprClass:
    case Stmt::SYCLUniqueStableNameExprClass:
    case Stmt::EmbedExprClass:
    case Stmt::HLSLOutArgExprClass:
    case Stmt::OpenACCAsteriskSizeExprClass:
        K = CXCursor_UnexposedExpr;
        break;
#endif

    case Stmt::OpaqueValueExprClass:
        if (Expr* Src = cast<OpaqueValueExpr>(S)->getSourceExpr())
            return dex_MakeCXCursor(Src, Parent, TU, RegionOfInterest);
        K = CXCursor_UnexposedExpr;
        break;

    case Stmt::PseudoObjectExprClass:
        return dex_MakeCXCursor(cast<PseudoObjectExpr>(S)->getSyntacticForm(), Parent, TU,
                                RegionOfInterest);

    case Stmt::CompoundStmtClass:
        K = CXCursor_CompoundStmt;
        break;

    case Stmt::NullStmtClass:
        K = CXCursor_NullStmt;
        break;

    case Stmt::LabelStmtClass:
        K = CXCursor_LabelStmt;
        break;

    case Stmt::AttributedStmtClass:
        K = CXCursor_UnexposedStmt;
        break;

    case Stmt::DeclStmtClass:
        K = CXCursor_DeclStmt;
        break;

    case Stmt::CapturedStmtClass:
        K = CXCursor_UnexposedStmt;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::SYCLKernelCallStmtClass:
        K = CXCursor_UnexposedStmt;
        break;
#endif

    case Stmt::IntegerLiteralClass:
        K = CXCursor_IntegerLiteral;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::FixedPointLiteralClass:
        K = CXCursor_FixedPointLiteral;
        break;
#endif

    case Stmt::FloatingLiteralClass:
        K = CXCursor_FloatingLiteral;
        break;

    case Stmt::ImaginaryLiteralClass:
        K = CXCursor_ImaginaryLiteral;
        break;

    case Stmt::StringLiteralClass:
        K = CXCursor_StringLiteral;
        break;

    case Stmt::CharacterLiteralClass:
        K = CXCursor_CharacterLiteral;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::ConstantExprClass:
        return dex_MakeCXCursor(cast<ConstantExpr>(S)->getSubExpr(), Parent, TU, RegionOfInterest);
#endif

    case Stmt::ParenExprClass:
        K = CXCursor_ParenExpr;
        break;

    case Stmt::UnaryOperatorClass:
        K = CXCursor_UnaryOperator;
        break;

    case Stmt::UnaryExprOrTypeTraitExprClass:
    case Stmt::CXXNoexceptExprClass:
        K = CXCursor_UnaryExpr;
        break;

    case Stmt::MSPropertySubscriptExprClass:
    case Stmt::ArraySubscriptExprClass:
        K = CXCursor_ArraySubscriptExpr;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::MatrixSubscriptExprClass:
        // TODO: add support for MatrixSubscriptExpr.
        K = CXCursor_UnexposedExpr;
        break;

    case Stmt::ArraySectionExprClass:
        K = CXCursor_ArraySectionExpr;
        break;

    case Stmt::OMPArrayShapingExprClass:
        K = CXCursor_OMPArrayShapingExpr;
        break;

    case Stmt::OMPIteratorExprClass:
        K = CXCursor_OMPIteratorExpr;
        break;
#endif

    case Stmt::BinaryOperatorClass:
        K = CXCursor_BinaryOperator;
        break;

    case Stmt::CompoundAssignOperatorClass:
        K = CXCursor_CompoundAssignOperator;
        break;

    case Stmt::ConditionalOperatorClass:
        K = CXCursor_ConditionalOperator;
        break;

    case Stmt::CStyleCastExprClass:
        K = CXCursor_CStyleCastExpr;
        break;

    case Stmt::CompoundLiteralExprClass:
        K = CXCursor_CompoundLiteralExpr;
        break;

    case Stmt::InitListExprClass:
        K = CXCursor_InitListExpr;
        break;

    case Stmt::AddrLabelExprClass:
        K = CXCursor_AddrLabelExpr;
        break;

    case Stmt::StmtExprClass:
        K = CXCursor_StmtExpr;
        break;

    case Stmt::GenericSelectionExprClass:
        K = CXCursor_GenericSelectionExpr;
        break;

    case Stmt::GNUNullExprClass:
        K = CXCursor_GNUNullExpr;
        break;

    case Stmt::CXXStaticCastExprClass:
        K = CXCursor_CXXStaticCastExpr;
        break;

    case Stmt::CXXDynamicCastExprClass:
        K = CXCursor_CXXDynamicCastExpr;
        break;

    case Stmt::CXXReinterpretCastExprClass:
        K = CXCursor_CXXReinterpretCastExpr;
        break;

    case Stmt::CXXConstCastExprClass:
        K = CXCursor_CXXConstCastExpr;
        break;

    case Stmt::CXXFunctionalCastExprClass:
        K = CXCursor_CXXFunctionalCastExpr;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::CXXAddrspaceCastExprClass:
        K = CXCursor_CXXAddrspaceCastExpr;
        break;
#endif

    case Stmt::CXXTypeidExprClass:
        K = CXCursor_CXXTypeidExpr;
        break;

    case Stmt::CXXBoolLiteralExprClass:
        K = CXCursor_CXXBoolLiteralExpr;
        break;

    case Stmt::CXXNullPtrLiteralExprClass:
        K = CXCursor_CXXNullPtrLiteralExpr;
        break;

    case Stmt::CXXThisExprClass:
        K = CXCursor_CXXThisExpr;
        break;

    case Stmt::CXXThrowExprClass:
        K = CXCursor_CXXThrowExpr;
        break;

    case Stmt::CXXNewExprClass:
        K = CXCursor_CXXNewExpr;
        break;

    case Stmt::CXXDeleteExprClass:
        K = CXCursor_CXXDeleteExpr;
        break;

    case Stmt::ObjCStringLiteralClass:
        K = CXCursor_ObjCStringLiteral;
        break;

    case Stmt::ObjCEncodeExprClass:
        K = CXCursor_ObjCEncodeExpr;
        break;

    case Stmt::ObjCSelectorExprClass:
        K = CXCursor_ObjCSelectorExpr;
        break;

    case Stmt::ObjCProtocolExprClass:
        K = CXCursor_ObjCProtocolExpr;
        break;

    case Stmt::ObjCBoolLiteralExprClass:
        K = CXCursor_ObjCBoolLiteralExpr;
        break;

    case Stmt::ObjCAvailabilityCheckExprClass:
        K = CXCursor_ObjCAvailabilityCheckExpr;
        break;

    case Stmt::ObjCBridgedCastExprClass:
        K = CXCursor_ObjCBridgedCastExpr;
        break;

    case Stmt::BlockExprClass:
        K = CXCursor_BlockExpr;
        break;

    case Stmt::PackExpansionExprClass:
        K = CXCursor_PackExpansionExpr;
        break;

    case Stmt::SizeOfPackExprClass:
        K = CXCursor_SizeOfPackExpr;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::PackIndexingExprClass:
        K = CXCursor_PackIndexingExpr;
        break;
#endif

    case Stmt::DeclRefExprClass:
        K = CXCursor_DeclRefExpr;
        break;

    case Stmt::DependentScopeDeclRefExprClass:
    case Stmt::SubstNonTypeTemplateParmExprClass:
    case Stmt::SubstNonTypeTemplateParmPackExprClass:
    case Stmt::FunctionParmPackExprClass:
    case Stmt::UnresolvedLookupExprClass:
        K = CXCursor_DeclRefExpr;
        break;

    case Stmt::CXXDependentScopeMemberExprClass:
    case Stmt::CXXPseudoDestructorExprClass:
    case Stmt::MemberExprClass:
    case Stmt::MSPropertyRefExprClass:
    case Stmt::UnresolvedMemberExprClass:
        K = CXCursor_MemberRefExpr;
        break;

    case Stmt::CallExprClass:
    case Stmt::CXXOperatorCallExprClass:
    case Stmt::CXXMemberCallExprClass:
    case Stmt::CUDAKernelCallExprClass:
    case Stmt::CXXConstructExprClass:
    case Stmt::CXXInheritedCtorInitExprClass:
    case Stmt::CXXTemporaryObjectExprClass:
    case Stmt::CXXUnresolvedConstructExprClass:
    case Stmt::UserDefinedLiteralClass:
        K = CXCursor_CallExpr;
        break;

    case Stmt::LambdaExprClass:
        K = CXCursor_LambdaExpr;
        break;

#if LLVM_MAJOR_VERSION > 19
    case Stmt::ConceptSpecializationExprClass:
        K = CXCursor_ConceptSpecializationExpr;
        break;

    case Stmt::RequiresExprClass:
        K = CXCursor_RequiresExpr;
        break;

    case Stmt::CXXParenListInitExprClass:
        K = CXCursor_CXXParenListInitExpr;
        break;

    case Stmt::MSDependentExistsStmtClass:
        K = CXCursor_UnexposedStmt;
        break;
    case Stmt::OMPCanonicalLoopClass:
        K = CXCursor_OMPCanonicalLoop;
        break;
    case Stmt::OMPMetaDirectiveClass:
        K = CXCursor_OMPMetaDirective;
        break;
    case Stmt::OMPParallelDirectiveClass:
        K = CXCursor_OMPParallelDirective;
        break;
    case Stmt::OMPSimdDirectiveClass:
        K = CXCursor_OMPSimdDirective;
        break;
    case Stmt::OMPTileDirectiveClass:
        K = CXCursor_OMPTileDirective;
        break;
#endif
#if LLVM_MAJOR_VERSION > 20
    case Stmt::OMPStripeDirectiveClass:
        K = CXCursor_OMPStripeDirective;
        break;
#endif
#if LLVM_MAJOR_VERSION > 19
    case Stmt::OMPUnrollDirectiveClass:
        K = CXCursor_OMPUnrollDirective;
        break;
    case Stmt::OMPReverseDirectiveClass:
        K = CXCursor_OMPReverseDirective;
        break;
    case Stmt::OMPInterchangeDirectiveClass:
        K = CXCursor_OMPInterchangeDirective;
        break;
    case Stmt::OMPForDirectiveClass:
        K = CXCursor_OMPForDirective;
        break;
    case Stmt::OMPForSimdDirectiveClass:
        K = CXCursor_OMPForSimdDirective;
        break;
    case Stmt::OMPSectionsDirectiveClass:
        K = CXCursor_OMPSectionsDirective;
        break;
    case Stmt::OMPSectionDirectiveClass:
        K = CXCursor_OMPSectionDirective;
        break;
    case Stmt::OMPScopeDirectiveClass:
        K = CXCursor_OMPScopeDirective;
        break;
    case Stmt::OMPSingleDirectiveClass:
        K = CXCursor_OMPSingleDirective;
        break;
    case Stmt::OMPMasterDirectiveClass:
        K = CXCursor_OMPMasterDirective;
        break;
    case Stmt::OMPCriticalDirectiveClass:
        K = CXCursor_OMPCriticalDirective;
        break;
    case Stmt::OMPParallelForDirectiveClass:
        K = CXCursor_OMPParallelForDirective;
        break;
    case Stmt::OMPParallelForSimdDirectiveClass:
        K = CXCursor_OMPParallelForSimdDirective;
        break;
    case Stmt::OMPParallelMasterDirectiveClass:
        K = CXCursor_OMPParallelMasterDirective;
        break;
    case Stmt::OMPParallelMaskedDirectiveClass:
        K = CXCursor_OMPParallelMaskedDirective;
        break;
    case Stmt::OMPParallelSectionsDirectiveClass:
        K = CXCursor_OMPParallelSectionsDirective;
        break;
    case Stmt::OMPTaskDirectiveClass:
        K = CXCursor_OMPTaskDirective;
        break;
    case Stmt::OMPTaskyieldDirectiveClass:
        K = CXCursor_OMPTaskyieldDirective;
        break;
    case Stmt::OMPBarrierDirectiveClass:
        K = CXCursor_OMPBarrierDirective;
        break;
    case Stmt::OMPTaskwaitDirectiveClass:
        K = CXCursor_OMPTaskwaitDirective;
        break;
    case Stmt::OMPErrorDirectiveClass:
        K = CXCursor_OMPErrorDirective;
        break;
    case Stmt::OMPTaskgroupDirectiveClass:
        K = CXCursor_OMPTaskgroupDirective;
        break;
    case Stmt::OMPFlushDirectiveClass:
        K = CXCursor_OMPFlushDirective;
        break;
    case Stmt::OMPDepobjDirectiveClass:
        K = CXCursor_OMPDepobjDirective;
        break;
    case Stmt::OMPScanDirectiveClass:
        K = CXCursor_OMPScanDirective;
        break;
    case Stmt::OMPOrderedDirectiveClass:
        K = CXCursor_OMPOrderedDirective;
        break;
    case Stmt::OMPAtomicDirectiveClass:
        K = CXCursor_OMPAtomicDirective;
        break;
    case Stmt::OMPTargetDirectiveClass:
        K = CXCursor_OMPTargetDirective;
        break;
    case Stmt::OMPTargetDataDirectiveClass:
        K = CXCursor_OMPTargetDataDirective;
        break;
    case Stmt::OMPTargetEnterDataDirectiveClass:
        K = CXCursor_OMPTargetEnterDataDirective;
        break;
    case Stmt::OMPTargetExitDataDirectiveClass:
        K = CXCursor_OMPTargetExitDataDirective;
        break;
    case Stmt::OMPTargetParallelDirectiveClass:
        K = CXCursor_OMPTargetParallelDirective;
        break;
    case Stmt::OMPTargetParallelForDirectiveClass:
        K = CXCursor_OMPTargetParallelForDirective;
        break;
    case Stmt::OMPTargetUpdateDirectiveClass:
        K = CXCursor_OMPTargetUpdateDirective;
        break;
    case Stmt::OMPTeamsDirectiveClass:
        K = CXCursor_OMPTeamsDirective;
        break;
    case Stmt::OMPCancellationPointDirectiveClass:
        K = CXCursor_OMPCancellationPointDirective;
        break;
    case Stmt::OMPCancelDirectiveClass:
        K = CXCursor_OMPCancelDirective;
        break;
    case Stmt::OMPTaskLoopDirectiveClass:
        K = CXCursor_OMPTaskLoopDirective;
        break;
    case Stmt::OMPTaskLoopSimdDirectiveClass:
        K = CXCursor_OMPTaskLoopSimdDirective;
        break;
    case Stmt::OMPMasterTaskLoopDirectiveClass:
        K = CXCursor_OMPMasterTaskLoopDirective;
        break;
    case Stmt::OMPMaskedTaskLoopDirectiveClass:
        K = CXCursor_OMPMaskedTaskLoopDirective;
        break;
    case Stmt::OMPMasterTaskLoopSimdDirectiveClass:
        K = CXCursor_OMPMasterTaskLoopSimdDirective;
        break;
    case Stmt::OMPMaskedTaskLoopSimdDirectiveClass:
        K = CXCursor_OMPMaskedTaskLoopSimdDirective;
        break;
    case Stmt::OMPParallelMasterTaskLoopDirectiveClass:
        K = CXCursor_OMPParallelMasterTaskLoopDirective;
        break;
    case Stmt::OMPParallelMaskedTaskLoopDirectiveClass:
        K = CXCursor_OMPParallelMaskedTaskLoopDirective;
        break;
    case Stmt::OMPParallelMasterTaskLoopSimdDirectiveClass:
        K = CXCursor_OMPParallelMasterTaskLoopSimdDirective;
        break;
    case Stmt::OMPParallelMaskedTaskLoopSimdDirectiveClass:
        K = CXCursor_OMPParallelMaskedTaskLoopSimdDirective;
        break;
    case Stmt::OMPDistributeDirectiveClass:
        K = CXCursor_OMPDistributeDirective;
        break;
    case Stmt::OMPDistributeParallelForDirectiveClass:
        K = CXCursor_OMPDistributeParallelForDirective;
        break;
    case Stmt::OMPDistributeParallelForSimdDirectiveClass:
        K = CXCursor_OMPDistributeParallelForSimdDirective;
        break;
    case Stmt::OMPDistributeSimdDirectiveClass:
        K = CXCursor_OMPDistributeSimdDirective;
        break;
    case Stmt::OMPTargetParallelForSimdDirectiveClass:
        K = CXCursor_OMPTargetParallelForSimdDirective;
        break;
    case Stmt::OMPTargetSimdDirectiveClass:
        K = CXCursor_OMPTargetSimdDirective;
        break;
    case Stmt::OMPTeamsDistributeDirectiveClass:
        K = CXCursor_OMPTeamsDistributeDirective;
        break;
    case Stmt::OMPTeamsDistributeSimdDirectiveClass:
        K = CXCursor_OMPTeamsDistributeSimdDirective;
        break;
    case Stmt::OMPTeamsDistributeParallelForSimdDirectiveClass:
        K = CXCursor_OMPTeamsDistributeParallelForSimdDirective;
        break;
    case Stmt::OMPTeamsDistributeParallelForDirectiveClass:
        K = CXCursor_OMPTeamsDistributeParallelForDirective;
        break;
    case Stmt::OMPTargetTeamsDirectiveClass:
        K = CXCursor_OMPTargetTeamsDirective;
        break;
    case Stmt::OMPTargetTeamsDistributeDirectiveClass:
        K = CXCursor_OMPTargetTeamsDistributeDirective;
        break;
    case Stmt::OMPTargetTeamsDistributeParallelForDirectiveClass:
        K = CXCursor_OMPTargetTeamsDistributeParallelForDirective;
        break;
    case Stmt::OMPTargetTeamsDistributeParallelForSimdDirectiveClass:
        K = CXCursor_OMPTargetTeamsDistributeParallelForSimdDirective;
        break;
    case Stmt::OMPTargetTeamsDistributeSimdDirectiveClass:
        K = CXCursor_OMPTargetTeamsDistributeSimdDirective;
        break;
    case Stmt::OMPInteropDirectiveClass:
        K = CXCursor_OMPInteropDirective;
        break;
    case Stmt::OMPDispatchDirectiveClass:
        K = CXCursor_OMPDispatchDirective;
        break;
    case Stmt::OMPMaskedDirectiveClass:
        K = CXCursor_OMPMaskedDirective;
        break;
    case Stmt::OMPGenericLoopDirectiveClass:
        K = CXCursor_OMPGenericLoopDirective;
        break;
    case Stmt::OMPTeamsGenericLoopDirectiveClass:
        K = CXCursor_OMPTeamsGenericLoopDirective;
        break;
    case Stmt::OMPTargetTeamsGenericLoopDirectiveClass:
        K = CXCursor_OMPTargetTeamsGenericLoopDirective;
        break;
    case Stmt::OMPParallelGenericLoopDirectiveClass:
        K = CXCursor_OMPParallelGenericLoopDirective;
        break;
    case Stmt::OpenACCComputeConstructClass:
        K = CXCursor_OpenACCComputeConstruct;
        break;
    case Stmt::OpenACCLoopConstructClass:
        K = CXCursor_OpenACCLoopConstruct;
        break;
    case Stmt::OpenACCCombinedConstructClass:
        K = CXCursor_OpenACCCombinedConstruct;
        break;
    case Stmt::OpenACCDataConstructClass:
        K = CXCursor_OpenACCDataConstruct;
        break;
    case Stmt::OpenACCEnterDataConstructClass:
        K = CXCursor_OpenACCEnterDataConstruct;
        break;
    case Stmt::OpenACCExitDataConstructClass:
        K = CXCursor_OpenACCExitDataConstruct;
        break;
    case Stmt::OpenACCHostDataConstructClass:
        K = CXCursor_OpenACCHostDataConstruct;
        break;
    case Stmt::OpenACCWaitConstructClass:
        K = CXCursor_OpenACCWaitConstruct;
        break;
#endif
#if LLVM_MAJOR_VERSION > 20
    case Stmt::OpenACCCacheConstructClass:
        K = CXCursor_OpenACCCacheConstruct;
        break;
    case Stmt::OpenACCInitConstructClass:
        K = CXCursor_OpenACCInitConstruct;
        break;
    case Stmt::OpenACCShutdownConstructClass:
        K = CXCursor_OpenACCShutdownConstruct;
        break;
    case Stmt::OpenACCSetConstructClass:
        K = CXCursor_OpenACCSetConstruct;
        break;
    case Stmt::OpenACCUpdateConstructClass:
        K = CXCursor_OpenACCUpdateConstruct;
        break;
    case Stmt::OpenACCAtomicConstructClass:
        K = CXCursor_OpenACCAtomicConstruct;
        break;
#endif
#if LLVM_MAJOR_VERSION > 19
    case Stmt::OMPTargetParallelGenericLoopDirectiveClass:
        K = CXCursor_OMPTargetParallelGenericLoopDirective;
        break;
    case Stmt::BuiltinBitCastExprClass:
        K = CXCursor_BuiltinBitCastExpr;
        break;
    case Stmt::OMPAssumeDirectiveClass:
        K = CXCursor_OMPAssumeDirective;
        break;
#endif
    default:
        K = CXCursor_UnexposedExpr;
    }

    CXCursor C = {K, 0, {Parent, S, TU}};
    return C;
}

// See: CXCursor.cpp
CXCursor dex_MakeCursorVariableRef(const clang::VarDecl* Var, clang::SourceLocation Loc,
                                   CXTranslationUnit TU) {

    assert(Var && TU && "Invalid arguments!");
    void* RawLoc = Loc.getPtrEncoding();
    CXCursor C = {CXCursor_VariableRef, 0, {Var, RawLoc, TU}};
    return C;
}

} // namespace cxcursor
} // namespace clang

namespace dextool_clang_extension {

using ::llvm::dyn_cast_or_null;

// reimplementation of helper functions from libclang

// See: CXCursor.cpp
CXTranslationUnit getCursorTU(CXCursor Cursor) {
    return static_cast<CXTranslationUnit>(const_cast<void*>(Cursor.data[2]));
}

// See: CXCursor.cpp
clang::ASTUnit* getCursorASTUnit(CXCursor Cursor) {
    CXTranslationUnit TU = getCursorTU(Cursor);
    if (!TU) {
        return nullptr;
    }
    return TU->TheASTUnit;
}

// See: CXCursor.cpp
clang::ASTContext* getCursorContext(CXCursor Cursor) {
    return &getCursorASTUnit(Cursor)->getASTContext();
}

// See: CXCursor.cpp
const clang::Decl* getCursorDecl(CXCursor Cursor) {
    return static_cast<const clang::Decl*>(Cursor.data[0]);
}

// See: CXCursor.cpp
const clang::Expr* getCursorExpr(CXCursor Cursor) {
    return dyn_cast_or_null<clang::Expr>(getCursorStmt(Cursor));
}

// See: CXCursor.cpp
const clang::Stmt* getCursorStmt(CXCursor Cursor) {
    if (Cursor.kind == CXCursor_ObjCSuperClassRef || Cursor.kind == CXCursor_ObjCProtocolRef ||
        Cursor.kind == CXCursor_ObjCClassRef) {
        return nullptr;
    }

    return static_cast<const clang::Stmt*>(Cursor.data[1]);
}

// See: CXSourceLocation.h
/// \brief Translate a Clang source location into a CIndex source location.
CXSourceLocation translateSourceLocation(const clang::SourceManager& SM,
                                         const clang::LangOptions& LangOpts,
                                         clang::SourceLocation Loc) {
    if (Loc.isInvalid()) {
        clang_getNullLocation();
    }

    CXSourceLocation Result = {{
                                   &SM,
                                   &LangOpts,
                               },
                               Loc.getRawEncoding()};
    return Result;
}

// See: CXSourceLocation.h
CXSourceLocation translateSourceLocation(clang::ASTContext& Context, clang::SourceLocation Loc) {
    return translateSourceLocation(Context.getSourceManager(), Context.getLangOpts(), Loc);
}

// See: CIndex.cpp
CXSourceLocation getLocation(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const clang::Expr* expr = getCursorExpr(C);
// the API has changed from 4->8. getStartLoc where finally removed in
// libclang-8.
#if CINDEX_VERSION < 50
        clang::SourceLocation loc = expr->getLocStart();
#else
        clang::SourceLocation loc = expr->getBeginLoc();
#endif
        return translateSourceLocation(*getCursorContext(C), loc);
    }

    return clang_getNullLocation();
}

// Recurse past the implicit cast expression.
const clang::Expr* getUnderlyingExprNode(const clang::Expr* expr) {
    if (expr == nullptr) {
        return nullptr;
    }

    if (llvm::isa<clang::DeclRefExpr>(expr)) {
        return expr;
    }

    // See: clang/AST/Expr.h
    // IgnoreParenImpCasts - Ignore parentheses and implicit casts.  Strip off
    // any ParenExpr or ImplicitCastExprs, returning their operand.
    return expr->IgnoreParenImpCasts();
}

CXCursor dex_getUnderlyingExprNode(const CXCursor cx_expr) {
    const clang::Expr* expr = getCursorExpr(cx_expr);
    expr = getUnderlyingExprNode(expr);
    if (expr == nullptr) {
        return clang_getNullCursor();
    }

    const clang::Decl* parent = clang::cxcursor::getCursorParentDecl(cx_expr);
    CXTranslationUnit tu = getCursorTU(cx_expr);

    return clang::cxcursor::dex_MakeCXCursor(expr, parent, tu, expr->getSourceRange());
}

} // namespace dextool_clang_extension
