/* C++ code produced by gperf version 3.1 */
/* Command-line: gperf -L C++ -t keywords.gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#include "Token.h"

struct keyword {
    const char* name;
    enum Token::Type type;
};

#define TOTAL_KEYWORDS 21
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 44
/* maximum key range = 43, duplicates = 0 */

class Perfect_Hash {
private:
    static inline unsigned int hash(const std::string_view str);

public:
    static struct keyword* in_word_set(const std::string_view str);
};

inline unsigned int
Perfect_Hash::hash(const std::string_view str) {
    static unsigned char asso_values[] =
    {
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        5,
        0,
        10,
        45,
        10,
        5,
        45,
        20,
        0,
        45,
        45,
        25,
        45,
        0,
        0,
        20,
        45,
        0,
        10,
        15,
        0,
        45,
        20,
        5,
        15,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45,
        45
    };

    return str.length() +
           asso_values[static_cast<unsigned char>(str[1])] +
           asso_values[static_cast<unsigned char>(str[0])];
}

struct keyword*
Perfect_Hash::in_word_set(const std::string_view str) {
    static struct keyword wordlist[] =
    {
        {""},
        {""},
        {"in", Token::Type::In},
        {""},
        {"null", Token::Type::Null},
        {"break", Token::Type::Break},
        {""},
        {"if", Token::Type::If},
        {"for", Token::Type::For},
        {"func", Token::Type::Function},
        {""},
        {""},
        {"is", Token::Type::Is},
        {"new", Token::Type::New},
        {"enum", Token::Type::Enum},
        {"false", Token::Type::Boolean_False},
        {"return", Token::Type::Return},
        {"as", Token::Type::As},
        {"continue", Token::Type::Continue},
        {"true", Token::Type::Boolean_True},
        {""},
        {"extern", Token::Type::Extern},
        {""},
        {"pub", Token::Type::Public},
        {"priv", Token::Type::Private},
        {""},
        {""},
        {""},
        {""},
        {"step", Token::Type::Step},
        {""},
        {""},
        {""},
        {""},
        {"type", Token::Type::Type},
        {""},
        {""},
        {""},
        {""},
        {"else", Token::Type::Else},
        {""},
        {""},
        {""},
        {""},
        {"when", Token::Type::When}
    };

    if (str.length() <= MAX_WORD_LENGTH && str.length() >= MIN_WORD_LENGTH) {
        unsigned int key = hash(str);

        if (key <= MAX_HASH_VALUE) {
            const char* s = wordlist[key].name;
            if (str.length() == strlen(s) && !memcmp(str.data(), s, str.length()))
                return &wordlist[key];
        }
    }

    return nullptr;
}
