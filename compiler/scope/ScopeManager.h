#pragma once

#include <stack>
#include <llvm/IR/LLVMContext.h>
#include "Scope.h"
#include "../../Console.h"
#include "../../context/Context.h"

class TypeVariant;

class ScopeManager {
public:
    explicit ScopeManager(llvm::LLVMContext& context);

    static ScopeManager* get();

    template<typename T>
    T* enter(const std::string& name, T* context) {
        static_assert(std::is_base_of_v<Context, T>, "T must be a subclass of Context");
        // printDebug("[Scope] Enter: %s > %s", current->name.c_str(), name.c_str());

        auto scope = new Scope(name);
        scope->parent = current;
        current = scope;

        if (context)
            pushContext(context);

        return context;
    }

    void leave(const std::string& name);

    Scope* GetParentScope(unsigned int levels = 1) const;

    /*
     * Add
     */

    void add(TypeDefinition* typeDef) const;

    void addCompiledType(TypeDefinition* typeDef) const;

    void add(Enum* anEnum) const;

    void add(Function* function) const;

    // void add(VariableDeclaration* var) const;

    void Add(const VariableDeclaration* var) const;

    void Add(const std::string& name, const Node* node, const TypeVariant* narrowedType) const;

    void addTypeUse(TypeVariant* variant, const TypeBase* type) const;

    /*
     * Has
     */

    bool hasType(const std::string& typeName) const;

    bool hasEnum(const std::string& enumName) const;

    bool hasFunction(const std::string& funcName) const;

    // bool hasVar(const std::string& varName) const;

    bool HasVar(const std::string& name) const;

    /*
     * Get
     */

    TypeBase* getType(const std::string& typeName, bool onlyDeclared = false) const;

    TypeDefinition* getType(const llvm::Type* llvmType) const;

    Enum* getEnum(const std::string& enumName, bool onlyDeclared = false) const;

    Function* getFunction(const std::string& funcName) const;

    // VariableDeclaration* getVar(const std::string& varName) const;

    std::unordered_map<std::string, std::shared_ptr<Symbol>>& GetVars() const;

    std::shared_ptr<Symbol> GetVar(const std::string& name) const;

    /*
     * Codegen Context
     */

    void pushContext(Context* context);

    void popContext();

    Context* getContext() const;

    template<typename T>
    T* getContext() const {
        if (contextStack.empty())
            return nullptr;

        if (auto context = dynamic_cast<T *>(contextStack.top()))
            return context;

        return nullptr;
    }

    template<typename T>
    T* findContext() const {
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
    static ScopeManager* instance;

    RootScope* root;
    Scope* current;

    std::stack<Context *> contextStack;

    void addBuiltinType(const std::string& name, llvm::LLVMContext& context);
};
