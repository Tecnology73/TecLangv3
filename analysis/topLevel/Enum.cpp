#include "Enum.h"
#include "../../ast/literals/Integer.h"

Enum *EnumAnalyzer::lastEnum = nullptr;

void EnumAnalyzer::Analyze() {
    EnumAnalyzer::lastEnum = node;

    unsigned lastValue = 0;
    for (const auto &item: node->fieldOrder) {
        auto field = node->fields[item];
        if (field->expression) {
            lastValue = static_cast<Integer *>(field->expression)->value + 1;
            continue;
        }

        field->expression = new Integer(field->token, lastValue++);
    }
}
