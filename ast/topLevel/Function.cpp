#include "Function.h"
#include "../../compiler/Compiler.h"
#include "../StringInternTable.h"

bool Function::AddParameter(const Token& token, const std::string& paramName, TypeReference* type) {
    if (parameters.contains(paramName))
        return false;

    auto param = new FunctionParameter(token, paramName, type, this);
    param->index = parameters.size();
    parameters.emplace(paramName, param);
    parameterOrder.push_back(paramName);

    return true;
}

bool Function::AddParameter(const Token& token, TypeReference* type) {
    auto& paramName = StringInternTable::Intern(std::to_string(parameters.size()));
    if (parameters.contains(paramName))
        return false;

    auto param = new FunctionParameter(token, paramName, type, this);
    param->index = parameters.size();
    parameters.emplace(paramName, param);
    parameterOrder.push_back(paramName);

    return true;
}

FunctionParameter* Function::GetParameter(const std::string& parameterName) const {
    auto it = parameters.find(parameterName);
    if (it == parameters.end())
        return nullptr;

    return it->second;
}

FunctionParameter* Function::GetParameter(unsigned int index) const {
    if (isExternal)
        index = std::clamp(index, 0u, static_cast<unsigned>(parameters.size() - 1));

    if (index >= parameters.size())
        return nullptr;

    return parameters.at(parameterOrder[index]);
}

int Function::GetParameterIndex(const std::string& parameterName) {
    auto it = parameters.find(parameterName);
    if (it == parameters.end())
        return -1;

    return it->second->index;
}
