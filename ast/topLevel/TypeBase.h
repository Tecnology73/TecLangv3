#pragma once

#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include "../Node.h"
#include "../../misc/BitFlag.h"

class Function;

class TypeField;

enum class TypeFlag : int {
    PUBLIC = 1 << 0,
    PRIVATE = 1 << 1,
    OPTIONAL = 1 << 2,
};

class TypeBase : public Node {
public:
    TypeBase(const Token& token, std::string name)
        : Node(token),
          name(std::move(name)) {
    }

    virtual llvm::Value* getDefaultValue() const = 0;

    virtual bool canCastTo(TypeBase* other) const = 0;

    class TypeVariant* createVariant();

    class TypeReference* CreateReference() const;

    llvm::Type* getLlvmType();

    llvm::Type* setLlvmType(llvm::Type* type);

    void AddField(TypeField* field);

    void AddFunction(Function* function);

    TypeField* GetField(const std::string& fieldName) const;

    int GetFieldIndex(const std::string& fieldName) const;

    Function* GetFunction(const std::string& funcName) const;

    Function* FindFunction(
        const std::string& funcName,
        const std::vector<llvm::Value *>& parameters,
        const std::vector<const TypeVariant *>& paramTypes
    ) const;

public:
    std::string const name;
    BitFlag<TypeFlag> flags;
    bool isDeclared = false;

    // Because we allow overloading, we need to store an array of functions.
    std::unordered_map<std::string, std::vector<Function *>> functions;
    std::unordered_map<std::string, TypeField *> fields;
    std::vector<std::string> fieldOrder;

    llvm::Type* llvmType = nullptr;
    bool isValueType = false;
};

class TypeField : public Node {
public:
    explicit TypeField(const Token& token)
        : Node(token),
          name(token.value) {
    }

    TypeField(const Token& token, const std::string& name)
        : Node(token),
          name(name) {
    }

    void Accept(class Visitor* visitor) override;

public:
    std::string const name;
    BitFlag<TypeFlag> flags;

    // class TypeVariant* type = nullptr; // Empty if we need to infer the type.
    class TypeReference* type = nullptr; // Empty if we need to infer the type.
    Node* expression = nullptr;
    int index = 0;
};
