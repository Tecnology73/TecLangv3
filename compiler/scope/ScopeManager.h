#pragma once

#include <stack>
#include "Scope.h"
#include "../../Console.h"
#include "../../context/Context.h"

class ScopeManager {
public:
    explicit ScopeManager();

    static ScopeManager* get();

    template<typename T>
    T* enter(const std::string& name, T* context) {
        static_assert(std::is_base_of_v<Context, T>, "T must be a subclass of Context");
        // printDebug("[Scope] Enter: %s > %s", current->name.c_str(), name.c_str());

        auto scope = std::make_shared<Scope>(name);
        scope->parent = current;
        current = scope;

        if (context)
            pushContext(context);

        return context;
    }

    void leave(const std::string& name);

    std::shared_ptr<Scope> GetParentScope(unsigned int levels = 1) const;

    /*
     * Add
     */

    void Add(const VariableDeclaration* var) const;

    void Add(const std::string& name, const Node* node, const TypeReference* narrowedType) const;

    /*
     * Has
     */

    bool HasVar(const std::string& name) const;

    /*
     * Get
     */

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

    std::shared_ptr<Scope> current;
    std::stack<Context *> contextStack;
};
