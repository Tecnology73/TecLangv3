#pragma once

#include <string>

struct Position {
    long index;
    long line;
    long column;
    unsigned length;
};

struct Token {
    enum class Type {
        Unknown,
        Identifier,
        Integer,
        Double,
        String,
        Punctuation,
        EndOfFile,

        // Keywords
        Function,
        Return,
        If,
        Else,
        Boolean_True,
        Boolean_False,
        For,
        Continue,
        Break,
        In,
        Type,
        New,
        When,
        Extern,
        As,
        Step,
        Enum,
        Public,
        Private,
        Is,
        Null,

        // Symbols
        OpenParen,
        CloseParen,
        OpenCurly,
        CloseCurly,
        OpenBracket,
        CloseBracket,
        Comma,
        Colon,
        ColonColon,
        Semicolon,
        Period,
        Spread,
        TripleDots,
        Assign,
        Plus,
        PlusPlus,
        Minus,
        MinusMinus,
        Multiply,
        Divide,
        BackSlash,
        Percent,
        Ampersand,
        Pipe,
        Caret,
        Exclamation,
        Tilde,
        Question,
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual,
        Equal,
        NotEqual,
        LogicalAnd,
        LogicalOr,
        PlusEqual,
        MinusEqual,
        MultiplyEqual,
        DivideEqual,
        PercentEqual,
        AmpersandEqual,
        CaretEqual,
        PipeEqual,
        Arrow,
    };

    Type type = Type::Unknown;
    Position position;

    std::string_view value;
    int intValue = 0;
    double doubleValue = 0;

    inline bool is(const Type t) const {
        return type == t;
    }

    template<typename... Args>
    inline bool is(const Type t, Args... args) const {
        return is(t) || is(args...);
    }

    inline bool is(const Type t, const std::string& v) const {
        return type == t && value == v;
    }

    inline bool is(const Type t, const int v) const {
        return type == t && intValue == v;
    }

    inline bool is(const Type t, const double v) const {
        return type == t && doubleValue == v;
    }

    inline bool isNot(const Type t) const {
        return !is(t);
    }

    template<typename... Args>
    inline bool isNot(const Type t, Args... args) const {
        return isNot(t) && isNot(args...);
    }

    inline bool isNot(const Type t, const std::string& v) const {
        return !is(t, v);
    }

    inline bool isNot(const Type t, const int v) const {
        return !is(t, v);
    }

    inline bool isNot(const Type t, const double v) const {
        return !is(t, v);
    }
};
