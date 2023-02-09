#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class VariablePrinterVisitor
    : public RecursiveASTVisitor<VariablePrinterVisitor> {
 public:
  explicit VariablePrinterVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitVarDecl(VarDecl *Decl) {
    llvm::outs() << Decl->getNameAsString() << "\n";
    return true;
  }

 private:
  ASTContext *Context;
};

class VariablePrinterConsumer : public clang::ASTConsumer {
 public:
  explicit VariablePrinterConsumer(ASTContext *Context)
      : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  VariablePrinterVisitor Visitor;
};

class VariablePrinterAction : public clang::PluginASTAction {
 protected:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<VariablePrinterConsumer>(&Compiler.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &Args) {
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<VariablePrinterAction>
    X("print-vars", "print all variable names");
