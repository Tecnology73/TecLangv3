#include "Enum.h"
#include "../../compiler/Compiler.h"
#include "../../symbolTable/SymbolTable.h"

Enum* Enum::Create(const Token& token, const std::string& name) {
    // FIXME: Don't overwrite a TypeDef if that already exists with the same name.
    if (auto existingEnum = SymbolTable::GetInstance()->Get<Enum>(name); existingEnum && existingEnum->isDeclared)
        return existingEnum;

    auto anEnum = new Enum(token, name);
    anEnum->isDeclared = true;

    SymbolTable::GetInstance()->Add(anEnum);

    return anEnum;
}

TypeReference* Enum::CreateConstructorType(EnumConstructor* constructor) const {
    // This feels like a bit of a hack but in reality, these types will have to be turned into a struct for runtime support any ways.
    auto& typeName = StringInternTable::Intern(std::format("{}${}", name, constructor->name));
    auto type = new TypeDefinition(constructor->token, typeName);

    for (const auto& paramName: constructor->parameterOrder) {
        auto param = constructor->parameters[paramName];

        auto field = new TypeField(param->token, param->name);
        field->type = param->type;
        type->AddField(field);

        // If this is a named argument, also register the index, so we can do `var[0]`.
        if (!isdigit(paramName[0])) {
            auto field0 = new TypeField(param->token, StringInternTable::Intern(std::to_string(param->index)));
            field0->type = param->type;
            type->AddField(field0);
        }
    }

    type->isDeclared = true;
    SymbolTable::GetInstance()->Add(type);

    return type->CreateReference();
}
