#include "Enum.h"
#include "../../ast/literals/Integer.h"
#include "../../compiler/Compiler.h"

Enum* EnumAnalyzer::lastEnum = nullptr;

void EnumAnalyzer::Analyze() {
    lastEnum = node;

    unsigned lastValue = 0;
    for (const auto& item: node->fieldOrder) {
        const auto field = node->fields[item];
        if (auto node = static_cast<Integer *>(field->expression)) {
            lastValue = node->value + 1;
            continue;
        }

        field->expression = new Integer(field->token, lastValue++);
    }

    // Calculate how many bits are required for the enum.
    unsigned bits = 8;
    while ((1 << bits) < lastValue)
        bits++;

    node->setLlvmType(Compiler::getScopeManager().getType("i" + std::to_string(bits))->getLlvmType());
}
