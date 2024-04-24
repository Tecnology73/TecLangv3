#pragma once

#include <memory>
#include "../context/Context.h"
#include "Symbol.h"

class Scope : std::enable_shared_from_this<Scope> {
public:
    Scope(std::shared_ptr<Scope> parent, std::shared_ptr<Context> context);

    ~Scope() = default;

    static std::shared_ptr<Scope> GetScope();

    static std::shared_ptr<Context> GetContext();

    template<typename TContext>
    static std::shared_ptr<TContext> FindContext() {
        auto scope = current;
        while (scope) {
            if (auto context = std::dynamic_pointer_cast<TContext>(scope->context))
                return context;

            scope = scope->parent;
        }

        return nullptr;
    }

    static std::shared_ptr<Scope> GetParent();

    template<typename TContext, typename ...Args>
    static std::tuple<std::shared_ptr<Scope>, std::shared_ptr<TContext>> Enter(Args&& ... args) {
        static_assert(std::is_base_of_v<Context, TContext>, "TContext must be a subclass of Context.");

        auto context = std::make_shared<TContext>(std::forward<Args>(args)...);
        auto scope = std::make_shared<Scope>(current, context);

        current = scope;

        return {scope, context};
    }

    std::shared_ptr<Scope> Leave();

    void Add(class VariableDeclaration* node);

    std::shared_ptr<Symbol> Get(const std::string& name) const;

    std::unordered_map<std::string, std::shared_ptr<Symbol>>& List();

    bool Has(const std::string& name) const;

public:
    static std::shared_ptr<Scope> current;

private:
    std::shared_ptr<Scope> parent;
    std::shared_ptr<Context> context;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> vars;
};
