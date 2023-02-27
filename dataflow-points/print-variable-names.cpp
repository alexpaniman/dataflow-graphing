#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Expr.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

using namespace clang;

class VariablePrinterVisitor: public RecursiveASTVisitor<VariablePrinterVisitor> {
public:
    explicit VariablePrinterVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitVarDecl(VarDecl *Decl) {
        printTransition(Decl->getSourceRange(), Decl->getLocation());
        return true;
    }

    // bool VisitBinaryOperator(BinaryOperator *BinOp) {
    //     printTransition(BinOp->getSourceRange(), BinOp->getOperatorLoc());
    //     return true;
    // }

    // bool VisitUnaryOperator(UnaryOperator *UnaryOp) {
    //     printTransition(UnaryOp->getSourceRange(), UnaryOp->getOperatorLoc());
    //     return true;
    // }

    bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr *BinOp) {
        printTransition(BinOp->getSourceRange(), BinOp->getOperatorLoc());
        return true;
    }

    bool VisitCallExpr(CallExpr *CallExpr) {
        SourceManager &srcMgr = Context->getSourceManager();
        if (!srcMgr.isInMainFile(CallExpr->getExprLoc()))
            return true;

        // if (!CallExpr->getCalleeDecl()->getAsFunction())
        //     return true;

        // if (!CallExpr->getCalleeDecl()->getAsFunction()->getParamDecl(0))
        //     return true;

        // printLocation(CallExpr->getSourceRange().getBegin());
        // printLocation(CallExpr->getCalleeDecl()->getAsFunction()->getParamDecl(0)->getLocation());
        printLocation(CallExpr->getSourceRange().getBegin());
        llvm::outs() << " ";
        printLocation(CallExpr->getExprLoc());

        llvm::outs() << "\n";
        return true;
    }

    bool VisitIntegerLiteral(IntegerLiteral *IntLiteral) {
        printTransition(IntLiteral->getSourceRange(), IntLiteral->getExprLoc());
        return true;
    }

private:
    ASTContext *Context;

    void printTransition(SourceRange range, SourceLocation target) {
        SourceManager &srcMgr = Context->getSourceManager();
        if (!srcMgr.isInMainFile(target))
            return;

        printLocation(range.getEnd());
        printLocation(target);

        llvm::outs() << "\n";
    }

    void printLocation(SourceLocation location) {
        SourceManager &srcMgr = Context->getSourceManager();

        int row = srcMgr.getSpellingLineNumber(location);
        int col = srcMgr.getSpellingColumnNumber(location);

        llvm::outs() << row << ":" << col << "\t";
    }
};

class VariablePrinterConsumer : public clang::ASTConsumer {
public:
    explicit VariablePrinterConsumer(ASTContext *Context) : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    VariablePrinterVisitor Visitor;
};

class VariablePrinterAction : public clang::PluginASTAction {
public:
    std::unique_ptr<clang::ASTConsumer>
    CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::make_unique<VariablePrinterConsumer>(&Compiler.getASTContext());
    }

    bool ParseArgs(const clang::CompilerInstance &Compiler,
                   const std::vector<std::string> &Args) {
        return true;
    }
};

static clang::FrontendPluginRegistry::Add<VariablePrinterAction>
    _("print-vars", "Print all variable names");
