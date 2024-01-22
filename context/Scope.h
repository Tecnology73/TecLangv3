#pragma once

class Scope {
public:
    Scope() {
        current = this;
    }

    explicit Scope(Scope* parent) : parent(parent) {
    }

private:
    static Scope* root;

    Scope* current = nullptr;
    Scope* parent = nullptr;
};
