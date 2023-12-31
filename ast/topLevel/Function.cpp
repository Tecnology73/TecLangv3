#include "Function.h"

bool Function::AddParameter(const Token &token, std::string paramName, TypeBase *type) {
    if (parameters.find(paramName) != parameters.end())
        return false;

    auto param = new FunctionParameter(token, paramName, type, this);
    param->index = parameters.size();
    parameters.emplace(paramName, param);
    parameterOrder.push_back(paramName);

    return true;
}

FunctionParameter *Function::GetParameter(const std::string &parameterName) const {
    auto it = parameters.find(parameterName);
    if (it == parameters.end())
        return nullptr;

    return it->second;
}

FunctionParameter *Function::GetParameter(unsigned int index) const {
    if (index >= parameters.size())
        return nullptr;

    return parameters.at(parameterOrder[index]);
}

int Function::GetParameterIndex(const std::string &parameterName) {
    auto it = parameters.find(parameterName);
    if (it == parameters.end())
        return -1;

    return it->second->index;
}
