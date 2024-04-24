#include "Enum.h"
#include "../../ast/literals/Integer.h"
#include "../../compiler/Compiler.h"

Enum* EnumAnalyzer::lastEnum = nullptr;

void EnumAnalyzer::Analyze() {
    lastEnum = node;

    int64_t largestValue = 0;
    for (const auto& item: node->fieldOrder) {
        const auto field = node->fields[item];
        if (auto node = static_cast<Integer*>(field->expression)) {
            largestValue = std::max(largestValue, node->value + 1);
            continue;
        }

        field->expression = new Integer(field->token, largestValue++);
    }

    // Calculate how many bits are required for the enum.
    unsigned bits = 8;
    while ((1 << bits) < largestValue)
        bits++;

    node->underlyingType = SymbolTable::GetInstance()->GetReference(std::format("i{}", bits));
}
