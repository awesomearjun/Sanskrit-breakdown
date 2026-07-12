#include <string>
#include <vector>
#include <variant>

// ----- NOUN -----

enum class NounGender {
    MASCULINE,
    FEMININE,
    NEUTER
};

enum class NounCase {
    SUBJECT, // who does it
    OBJECT,  // who receives it
    INSTRUMENTAL, // with/using what
    RECIPIENT, // to/for whom
    ORIGIN, // from whom
    RELATION, // of whom
    CONTEXT, // Where/when
    ADDRESS // O! (vocative)
};

enum class Number {
    SINGULAR,
    DUAL,
    PLURAL
};

// 1. Define distinct, lightweight structs for each unique track
struct NounMetadata {
    NounCase vibhakti; // Case: 1 to 8 (Prathamā to Sambodhana)
    Number vacana;   // Number: 1=Singular, 2=Dual, 3=Plural
    NounGender linga;    // Gender: 1=Masculine, 2=Feminine, 3=Neuter
};

// ----- VERB -----

struct VerbMetadata {
    int lakara;   // Tense/Mood system (e.g., Lat, Lang, Lot)
    int purusha;  // Person: 1=3rd (Prathama), 2=2nd (Madhyama), 3=1st (Uttama)
    int vacana;   // Number: 1=Singular, 2=Dual, 3=Plural
    int pada;
};

// ---- ETC -----

struct IndeclinableMetadata {
};

struct UnknownMetadata {};

using WordMetadata = std::variant<UnknownMetadata, NounMetadata, VerbMetadata, IndeclinableMetadata>;

// ----- WORD -----

enum class WordType {
    UNKNOWN,
    NOUN,
    VERB,
    ADJECTIVE,
    PRONOUN,
    INDECLINABLE,
};

struct Word {
    std::wstring text;
    std::wstring cleanForm;
    std::vector<std::wstring> roots;

    WordType type;
    WordMetadata metadata = UnknownMetadata{};
};
