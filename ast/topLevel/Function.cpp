#include "Function.h"

bool Function::addParameter(Token token, std::string paramName, TypeBase *type) {
    if (parameterIndices.find(paramName) != parameterIndices.end())
        return false;

    auto param = new FunctionParameter(
        std::move(token),
        paramName,
        type,
        this
    );

    parameters.push_back(param);
    parameterIndices[paramName] = param->index = parameters.size() - 1;

    return true;
}

FunctionParameter *Function::getParameter(const std::string &parameterName) const {
    auto it = parameterIndices.find(parameterName);
    if (it == parameterIndices.end())
        return nullptr;

    return parameters[it->second];
}

int Function::getParameterIndex(const std::string &parameterName) {
    auto it = parameterIndices.find(parameterName);
    if (it == parameterIndices.end())
        return -1;

    return it->second;
}