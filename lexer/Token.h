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
        Identifier,
        Integer,
        Double,
        String,
        Punctuation,
        EndOfFile,
        Unknown,

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

    std::string value;
    int intValue;
    double doubleValue;

    inline bool is(Type t) const {
        return type == t;
    }

    inline bool is(Type t, const std::string &v) const {
        return type == t && value == v;
    }

    inline bool is(Type t, int v) const {
        return type == t && intValue == v;
    }

    inline bool is(Type t, double v) const {
        return type == t && doubleValue == v;
    }

    inline bool isNot(Type t) const {
        return !is(t);
    }

    inline bool isNot(Type t, const std::string &v) const {
        return !is(t, v);
    }

    inline bool isNot(Type t, int v) const {
        return !is(t, v);
    }

    inline bool isNot(Type t, double v) const {
        return !is(t, v);
    }
};