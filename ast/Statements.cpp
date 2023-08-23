#include "Statements.h"
#include "../compiler/statements/TypeDefinition.h"
#include "../compiler/Compiler.h"

void Function::addParameter(FunctionParameter *parameter) {
    parameter->function = this;
    parameters.push_back(parameter);
    parameterIndices[parameter->name] = parameter->index = parameters.size() - 1;
}

FunctionParameter *Function::getParameter(const std::string &parameterName) const {
    if (parameterIndices.find(parameterName) == parameterIndices.end())
        return nullptr;

    return parameters[parameterIndices.at(parameterName)];
}

int Function::getParameterIndex(const std::string &parameterName) {
    if (parameterIndices.find(parameterName) == parameterIndices.end())
        return -1;

    return parameterIndices[parameterName];
}

/*
 * Variable Declaration
 */

TypeDefinition *VariableDeclaration::GetCompiledType(Visitor *v) {
    if (!type) {
        type = inferType(v, this);
        if (!type) return nullptr;
    }

    if (!type->llvmType) {
        if (auto typeDef = dynamic_cast<TypeDefinition *>(type))
            generateTypeDefinition(v, typeDef);
        else
            type->llvmType = Compiler::getScopeManager().getType("i32")->llvmType;
    }

    return dynamic_cast<TypeDefinition *>(type);
}
