#pragma once

#include <stack>
#include <llvm/IR/LLVMContext.h>
#include "Scope.h"
#include "../context/CodegenContext.h"

class ScopeManager {
public:
    explicit ScopeManager(llvm::LLVMContext &context);

    static ScopeManager *get();

    template<typename T>
    T *enter(std::string name, T *context) {
        static_assert(std::is_base_of<CodegenContext, T>::value, "T must be a subclass of CodegenContext");
        std::cout << "[Scope] Enter: " << current->name << " > " << name << std::endl;

        auto scope = new Scope(std::move(name));
        scope->parent = current;
        current = scope;

        if (context)
            pushContext(context);

        return context;
    }

    void leave(const std::string &name);

    /*
     * Add
     */

    void add(TypeDefinition *typeDef) const;

    void addCompiledType(TypeDefinition *typeDef) const;

    void add(Enum *anEnum) const;

    void add(Function *function) const;

    void add(VariableDeclaration *var) const;

    /*
     * Has
     */

    bool hasType(const std::string &typeName) const;

    bool hasEnum(const std::string &enumName) const;

    bool hasFunction(const std::string &funcName) const;

    bool hasVar(const std::string &varName) const;

    /*
     * Get
     */

    TypeDefinition *getType(const std::string &typeName) const;

    TypeDefinition *getType(llvm::Type *llvmType) const;

    Enum *getEnum(const std::string &enumName) const;

    Function *getFunction(const std::string &funcName) const;

    VariableDeclaration *getVar(const std::string &varName) const;

    /*
     * Codegen Context
     */

    void pushContext(CodegenContext *context);

    void popContext();

    CodegenContext *getContext() const;

    template<typename T>
    T *findContext() const {
        if (contextStack.empty())
            return nullptr;

        auto context = contextStack.top();
        while (context != nullptr && dynamic_cast<T *>(context) == nullptr)
            context = context->parent;

        return dynamic_cast<T *>(context);
    }

    bool hasContext() const;

    template<typename T>
    bool isInContext() const {
        return !contextStack.empty() && dynamic_cast<T *>(contextStack.top()) != nullptr;
    }

private:
    static ScopeManager *instance;

    RootScope *root;
    Scope *current;

    std::stack<CodegenContext *> contextStack;

    void addBuiltinType(const std::string &name, llvm::LLVMContext &context);
};