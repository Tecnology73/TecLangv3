#pragma once

template<typename T>
class BitFlag {
public:
    void Set(T flag) {
        flags |= static_cast<int>(flag);
    }

    void Clear(T flag) {
        flags &= ~static_cast<int>(flag);
    }

    bool Has(T flag) const {
        return (flags & static_cast<int>(flag)) != 0;
    }

    void Reset() {
        flags = 0;
    }

private:
    int flags = 0;
};
