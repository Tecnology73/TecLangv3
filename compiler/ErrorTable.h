#pragma once

#include <map>
#include <string>

enum class ErrorCode {
    UNKNOWN_ERROR,
    TYPE_UNKNOWN,
    TYPE_UNKNOWN_CONSTRUCTOR,
    TYPE_UNKNOWN_FIELD,
    TYPE_UNKNOWN_FUNCTION,
    TYPE_COERCION_IMPLICIT_FAILED,
    TYPE_INFER_FAILED,
    BINARY_OP_UNKNOWN,
    FUNCTION_UNKNOWN,
    FUNCTION_MULTIPLE_RETURN_TYPES,
    VARIABLE_UNDECLARED,
    VARIABLE_INFER_FAILED,
    VARIABLE_ALREADY_DECLARED,
    SYNTAX_UNEXPECTED_TOKEN,
    STATIC_REF_UNKNOWN,
    ENUM_UNKNOWN_VALUE,
    SYNTAX_ERROR,
    WHEN_UNUSED_ENUM_VALUE,
    WHEN_MULTIPLE_RETURN_TYPES,
    VALUE_CANNOT_BE_NULL,
    VALUE_POSSIBLY_NULL,
    ASSIGN_TYPE_MISMATCH,
};

const std::map<ErrorCode, std::string> ErrorTable = {
    {ErrorCode::UNKNOWN_ERROR, "An unknown error has occurred."},
    {ErrorCode::TYPE_UNKNOWN, "Type '{}' does not exist."},
    {ErrorCode::TYPE_UNKNOWN_CONSTRUCTOR, "No constructor found matching {under}{}({})."},
    {ErrorCode::TYPE_UNKNOWN_FIELD, "Field '{}' does not exist in type '{}'."},
    {ErrorCode::TYPE_UNKNOWN_FUNCTION, "Function '{}' does not exist on type '{}'."},
    {ErrorCode::TYPE_COERCION_IMPLICIT_FAILED, "Cannot implicitly cast from '{}' to '{}'."},
    {ErrorCode::TYPE_INFER_FAILED, "Could not infer type of unsupported node '{}'."},
    {ErrorCode::BINARY_OP_UNKNOWN, "Unknown binary operation."},
    {ErrorCode::FUNCTION_UNKNOWN, "Function '{}' does not exist."},
    {ErrorCode::FUNCTION_MULTIPLE_RETURN_TYPES, "Function '{}' has multiple return types."},
    {ErrorCode::VARIABLE_UNDECLARED, "Variable '{}' does not exist in the current scope."},
    {ErrorCode::VARIABLE_INFER_FAILED, "Could not infer type of variable '{}'."},
    {ErrorCode::VARIABLE_ALREADY_DECLARED, "The variable '{}' has already been declared."},
    {ErrorCode::SYNTAX_UNEXPECTED_TOKEN, "Unexpected token '{}'. Expected {}."},
    {ErrorCode::STATIC_REF_UNKNOWN, "Unknown static reference '{}'."},
    {ErrorCode::ENUM_UNKNOWN_VALUE, "'{}' is not a valid expression for 'enum {}'."},
    {ErrorCode::SYNTAX_ERROR, "Syntax error: '{}' cannot be used in the current context or is invalid."},
    {ErrorCode::WHEN_UNUSED_ENUM_VALUE, "'else' is required when some enum values are not covered."},
    {ErrorCode::WHEN_MULTIPLE_RETURN_TYPES, "When statement has multiple return types."},
    {ErrorCode::VALUE_CANNOT_BE_NULL, "Value of '{}' cannot be null. Try marking the type as optional: '{}?'."},
    {ErrorCode::VALUE_POSSIBLY_NULL, "Value could be null."},
    {ErrorCode::ASSIGN_TYPE_MISMATCH, "Cannot assign value of type '{}' to type '{}'."},
};
