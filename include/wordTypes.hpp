#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// ----- FUNDAMENTAL -----

struct Prefix
{
    std::string text;
    std::string englishMeaning;
};

struct Suffix
{
    std::string text;
    std::string englishMeaning;
};

// ----- NOMINAL TRACK (Nouns, Adjectives, Pronouns) -----

enum class NominalGender
{
    MASCULINE,
    FEMININE,
    NEUTER,
    ANY
};

enum class NominalCase
{
    SUBJECT,      // Nominative: The entity performing the action
    OBJECT,       // Accusative: The direct target of the action
    INSTRUMENTAL, // Instrumental: The means, tool, or agent using which an
                  // action is done
    RECIPIENT, // Dative: The person or entity for whom the action is performed
    ORIGIN,    // Ablative: The starting point or source from which something
               // moves/separates
    RELATION,  // Genitive: Denotes possession, belonging, or association ("of
               // whom")
    CONTEXT, // Locative: The location, time, or setting where the action occurs
    ADDRESS  // Vocative: Used when directly calling out to someone ("O Rama!")
};

enum class NominalNumber
{
    SINGULAR, // Exactly one entity
    DUAL,     // Exactly two entities
    PLURAL    // Three or more entities
};

struct SecondaryEnding
{
    Suffix suffix;
};

struct NominalMetadata
{
    NominalCase nominalCase;
    NominalNumber number;
    NominalGender gender;
    std::string
        suffix; // The specific suffix string that was matched to this verb form
    std::string stem = "";
    Prefix
        prefix; // The specific prefix string that was matched to this verb form
    SecondaryEnding secondaryEnding = {};
};

// ----- VERB TRACK -----

enum class VerbPerson
{
    THIRD,  // He, She, It, They (Default starting point in Sanskrit logic)
    SECOND, // You, You all
    FIRST   // I, We
};

enum class VerbNumber
{
    SINGULAR, // One actor executing the action
    DUAL,     // Exactly two actors executing the action
    PLURAL    // Three or more actors executing the action
};

enum class VerbTenseOrMood
{
    PRESENT,        // Actions happening right now (e.g., "He goes")
    IMPERFECT_PAST, // General past tense actions (e.g., "He went")
    IMPERATIVE,     // Commands, requests, or instructions (e.g., "Go!")
    POTENTIAL,      // Duties, advice, possibilities, or expectations (e.g., "He
                    // should go")
    PERFECT_PAST, // Remote or historical past, usually unwitnessed (e.g., "Once
                  // upon a time, he went")
    PERIPHRASTIC_FUTURE, // Distant or definite future actions, but not
                         // occurring today (e.g., "He will go tomorrow")
    SIMPLE_FUTURE,       // General future actions (e.g., "He will go")
    VEDIC_SUBJUNCTIVE, // Intentions, prayers, or urgent requests (Exclusive to
                       // the ancient Vedas)
    BENEDICTION, // Blessings, prayers, or intense longings (e.g., "May you live
                 // long")
    AORIST_PAST, // Immediate, recent, or absolute past actions (e.g., "He just
                 // left")
    CONDITIONAL_FUTURE_PAST // Hypotheses or missed outcomes (e.g., "If it had
                            // rained, he would have stayed")
};

enum class VerbVoice
{
    ACTIVE,   // Action is directed outwards toward another target
    REFLEXIVE // Action loops back to benefit or affect the person doing it
};

struct VerbMetadata
{
    VerbTenseOrMood
        tenseOrMood; // The specific timeframe or emotional intent of the action
    VerbPerson person; // The grammatical perspective of the actor
    VerbNumber number; // The count of actors executing the verb
    VerbVoice
        voice; // The structural direction/beneficiary of the action's outcome
    std::string
        suffix; // The specific suffix string that was matched to this verb form
    std::string stem = "";
    Prefix
        prefix; // The specific prefix string that was matched to this verb form
};

// ----- ROOT/STEM -----

enum class ConjugationClass
{
    CLASS_1_BASE_A, // Takes a simple '-a-' linking vowel (e.g., bhū -> bhavati)
    CLASS_2_DIRECT_ATTACH, // Suffixes lock straight onto the root without a
                           // vowel (e.g., ad -> atti)
    CLASS_3_DUPLICATED, // Involves repeating/doubling the root syllable (e.g.,
                        // hu -> juhoti)
    CLASS_4_YA_INFIX, // Inserts a '-ya-' sound before the suffix (e.g., div ->
                      // dīvyati)
    CLASS_5_NU_INFIX, // Inserts a '-nu-' sound before the suffix (e.g., su ->
                      // sunoti)
    CLASS_6_ACCENTED_A, // Inserts an accented '-a-' vowel (e.g., tud -> tudati)
    CLASS_7_INTERNAL_NASAL, // Splices a nasal consonant straight inside the
                            // root itself (e.g., rudh -> ruṇaddhi)
    CLASS_8_U_INFIX,  // Inserts a '-u-' sound before the suffix (e.g., tan ->
                      // tanoti)
    CLASS_9_NA_INFIX, // Inserts a '-nā-' sound before the suffix (e.g., krī ->
                      // krīṇāti)
    CLASS_10_CAUSATIVE_AYA, // Inserts a causative or structural '-aya-' sound
                            // (e.g., cur -> corayati)
    UNKNOWN // The root's conjugation class is not yet determined or is
            // ambiguous
};

enum class RootVoiceType
{
    ACTIVE_VOICE_MARKERS,    // The root naturally uses Parasmaipada terminal
                             // suffixes
    REFLEXIVE_VOICE_MARKERS, // The root naturally uses Ātmanepada terminal
                             // suffixes
    SHARED_VOICE_MARKERS,    // The root naturally accepts either structural
                             // terminal set
    UNKNOWN // The root's natural voice type is not yet determined or is
            // ambiguous
};

enum class InternalVowelRule
{
    ALLOWS_LINKING_VOWEL,   // Root allows internal structural 'i' insertions
                            // (Sa-iṭ)
    BLOCKS_LINKING_VOWEL,   // Root strictly forbids internal structural 'i'
                            // insertions (An-iṭ)
    VARIABLE_LINKING_VOWEL, // Vowel insertion behavior is completely
                            // conditional (Veti)
    UNKNOWN // The root's internal vowel behavior is not yet determined or is
            // ambiguous
};

struct Root
{
    std::string originalTagForm = ""; // The raw dictionary entry containing
                                      // historical marker tags (e.g., L"डुकृञ्")
    std::string cleanLookupForm =
        ""; // The clean, stripped form used for algorithmic string matching
            // (e.g., L"कृ")

    ConjugationClass conjugationClass =
        ConjugationClass::UNKNOWN; // The internal morphological structural
                                   // pattern it follows
    RootVoiceType naturalVoiceType =
        RootVoiceType::UNKNOWN; // The default inflection suffix set it is
                                // pre-wired to use
    InternalVowelRule internalVowelRule =
        InternalVowelRule::UNKNOWN; // Internal structural chemistry behavior
                                    // for vowel insertions

    std::string traditionalMeaning =
        ""; // The semantic definition text from the ancient root lists (e.g.,
            // L"सत्तायाम्")
    std::string englishMeaning =
        ""; // The literal concept translation (e.g., L"to be / to exist")

    bool isEmpty() const
    {
        return (originalTagForm.empty() && cleanLookupForm.empty()) ||
               (traditionalMeaning.empty() && englishMeaning.empty());
    }
};

enum class EndingType
{
    A_STEM,
    AA_STEM,
    I_STEM,
    II_STEM,
    U_STEM,
    CONSONANT
};
struct StemMeaning
{
    std::string english; // "discipline, humility, modesty"
    std::string
        traditional; // "दमन, शिष्टाचार" (Sanskrit gloss/Pāṇinian definition)
};

struct NominalStem
{
    std::string text;     // "विनय"
    EndingType ending;    // EndingType::A_STEM
    NominalGender gender; // Gender::MASCULINE
    StemMeaning meaning;  // English & traditional definitions
    bool isEmpty() const
    {
        return text.empty() || meaning.english.empty() ||
               meaning.traditional.empty();
    }
};

// ---- ETC -----

struct IndeclinableMetadata
{
    std::string text;
    std::string englishMeaning;
};

using WordMetadataType =
    std::variant<NominalMetadata, VerbMetadata, IndeclinableMetadata>;
using CoreMetadataType = std::variant<Root, NominalStem, SecondaryEnding>;

// ----- WORD -----

enum class WordMatchType
{
    NONE,         // No match found
    INDECLINABLE, // Indeclinable word (unchanging)
    VERB,         // Verb form
    NOMINAL,      // Nominal stem (noun/adjective/pronoun)
    IDENTITY      // not really identified but just the original word
};

enum class CoreMatchType
{
    NONE,            // we don't know
    ROOT,            // root (mostly for verb)
    STEM,            // stem (mostly for nouns)
    SECONDARY_ENDING // secondary ending (for nouns)
};

struct CoreMetadata
{
    bool success = false;
    std::string original = "";
    CoreMatchType matchType = CoreMatchType::NONE;
    std::optional<CoreMetadataType> metadata;

    [[nodiscard]]
    bool empty()
    {
        return original.empty() || !metadata.has_value();
    }
};

struct WordMetadata
{
    bool success = false;
    std::string original = "";
    std::vector<CoreMetadata> cores = {};
    std::optional<WordMetadataType> metadata = {};
    WordMatchType matchType = WordMatchType::NONE;

    [[nodiscard]]
    bool empty()
    {
        return original.empty() || !metadata.has_value();
    }
};

struct Word
{
    std::string text;
    std::vector<std::vector<WordMetadata>> metadata;
};

using WordList = std::vector<std::shared_ptr<Word>>;
