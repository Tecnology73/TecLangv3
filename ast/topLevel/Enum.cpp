#include "Enum.h"
#include "../../compiler/Compiler.h"

llvm::Value *Enum::getDefaultValue() const {
    return Compiler::getBuilder().getInt32(0);
}

bool Enum::canCastTo(TypeBase *other) const {
    return false;
}

void Enum::AddValue(EnumValue *value) {
    values.push_back(value);
    valueIndices[value->name] = values.size() - 1;
}

const std::vector<class EnumValue *> &Enum::GetValues() const {
    return values;
}

int Enum::GetValueIndex(const std::string &name) const {
    auto it = valueIndices.find(name);
    if (it == valueIndices.end()) return -1;

    return it->second;
}
