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
    TypeBase(const Token& token, const std::string& name) : Node(token), name(name) {
    }

    virtual llvm::Value* getDefaultValue() const = 0;

    virtual bool canCastTo(TypeBase* other) const = 0;

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
        const std::vector<TypeReference *>& paramTypes
    ) const;

public:
    const std::string& name;
    BitFlag<TypeFlag> flags;
    bool isDeclared = false;

    // Because we allow overloading, we need to store an array of functions.
    std::unordered_map<std::string_view, std::vector<Function *>> functions;
    std::unordered_map<std::string_view, TypeField *> fields;
    std::vector<std::string_view> fieldOrder;

    llvm::Type* llvmType = nullptr;
    bool isValueType = false;
};

class TypeField : public Node {
public:
    TypeField(const Token& token, const std::string& name) : Node(token), name(name) {
    }

    void Accept(class Visitor* visitor) override;

public:
    const std::string& name;
    BitFlag<TypeFlag> flags;

    class TypeReference* type = nullptr; // Empty if we need to infer the type.
    Node* expression = nullptr;
    int index = 0;
};
