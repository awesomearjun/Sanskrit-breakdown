#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <variant>

// ----- NOMINAL TRACK (Nouns, Adjectives, Pronouns) -----

enum class NominalGender {
    MASCULINE,
    FEMININE,
    NEUTER
};

enum class NominalCase {
    SUBJECT,       // Nominative: The entity performing the action
    OBJECT,        // Accusative: The direct target of the action
    INSTRUMENTAL,  // Instrumental: The means, tool, or agent using which an action is done
    RECIPIENT,     // Dative: The person or entity for whom the action is performed
    ORIGIN,        // Ablative: The starting point or source from which something moves/separates
    RELATION,      // Genitive: Denotes possession, belonging, or association ("of whom")
    CONTEXT,       // Locative: The location, time, or setting where the action occurs
    ADDRESS        // Vocative: Used when directly calling out to someone ("O Rama!")
};

enum class NominalNumber {
    SINGULAR,      // Exactly one entity
    DUAL,          // Exactly two entities
    PLURAL         // Three or more entities
};

struct NominalMetadata {
    NominalCase nominalCase;
    NominalNumber number;
    NominalGender gender;
    std::string suffix;           // The specific suffix string that was matched to this verb form
    std::string prefix;           // The specific prefix string that was matched to this verb form
};

// ----- VERB TRACK -----

enum class VerbPerson {
    THIRD,   // He, She, It, They (Default starting point in Sanskrit logic)
    SECOND,  // You, You all
    FIRST    // I, We
};

enum class VerbNumber {
    SINGULAR, // One actor executing the action
    DUAL,     // Exactly two actors executing the action
    PLURAL    // Three or more actors executing the action
};

enum class VerbTenseOrMood {
    PRESENT,                    // Actions happening right now (e.g., "He goes")
    IMPERFECT_PAST,             // General past tense actions (e.g., "He went")
    IMPERATIVE,                 // Commands, requests, or instructions (e.g., "Go!")
    POTENTIAL,                  // Duties, advice, possibilities, or expectations (e.g., "He should go")
    PERFECT_PAST,               // Remote or historical past, usually unwitnessed (e.g., "Once upon a time, he went")
    PERIPHRASTIC_FUTURE,        // Distant or definite future actions, but not occurring today (e.g., "He will go tomorrow")
    SIMPLE_FUTURE,              // General future actions (e.g., "He will go")
    VEDIC_SUBJUNCTIVE,          // Intentions, prayers, or urgent requests (Exclusive to the ancient Vedas)
    BENEDICTION,                // Blessings, prayers, or intense longings (e.g., "May you live long")
    AORIST_PAST,                // Immediate, recent, or absolute past actions (e.g., "He just left")
    CONDITIONAL_FUTURE_PAST     // Hypotheses or missed outcomes (e.g., "If it had rained, he would have stayed")
};

enum class VerbVoice {
    ACTIVE,     // Action is directed outwards toward another target
    REFLEXIVE  // Action loops back to benefit or affect the person doing it
};

struct VerbMetadata {
    VerbTenseOrMood tenseOrMood; // The specific timeframe or emotional intent of the action
    VerbPerson person;           // The grammatical perspective of the actor
    VerbNumber number;           // The count of actors executing the verb
    VerbVoice voice;             // The structural direction/beneficiary of the action's outcome
    std::string suffix;           // The specific suffix string that was matched to this verb form
    std::string prefix;           // The specific prefix string that was matched to this verb form
};

// ----- ROOT -----

enum class ConjugationClass {
    CLASS_1_BASE_A,               // Takes a simple '-a-' linking vowel (e.g., bhū -> bhavati)
    CLASS_2_DIRECT_ATTACH,        // Suffixes lock straight onto the root without a vowel (e.g., ad -> atti)
    CLASS_3_DUPLICATED,           // Involves repeating/doubling the root syllable (e.g., hu -> juhoti)
    CLASS_4_YA_INFIX,             // Inserts a '-ya-' sound before the suffix (e.g., div -> dīvyati)
    CLASS_5_NU_INFIX,             // Inserts a '-nu-' sound before the suffix (e.g., su -> sunoti)
    CLASS_6_ACCENTED_A,           // Inserts an accented '-a-' vowel (e.g., tud -> tudati)
    CLASS_7_INTERNAL_NASAL,       // Splices a nasal consonant straight inside the root itself (e.g., rudh -> ruṇaddhi)
    CLASS_8_U_INFIX,              // Inserts a '-u-' sound before the suffix (e.g., tan -> tanoti)
    CLASS_9_NA_INFIX,             // Inserts a '-nā-' sound before the suffix (e.g., krī -> krīṇāti)
    CLASS_10_CAUSATIVE_AYA,       // Inserts a causative or structural '-aya-' sound (e.g., cur -> corayati)
    UNKNOWN                       // The root's conjugation class is not yet determined or is ambiguous
};

enum class RootVoiceType {
    ACTIVE_VOICE_MARKERS,         // The root naturally uses Parasmaipada terminal suffixes
    REFLEXIVE_VOICE_MARKERS,      // The root naturally uses Ātmanepada terminal suffixes
    SHARED_VOICE_MARKERS,         // The root naturally accepts either structural terminal set
    UNKNOWN                       // The root's natural voice type is not yet determined or is ambiguous
};

enum class InternalVowelRule {
    ALLOWS_LINKING_VOWEL,         // Root allows internal structural 'i' insertions (Sa-iṭ)
    BLOCKS_LINKING_VOWEL,         // Root strictly forbids internal structural 'i' insertions (An-iṭ)
    VARIABLE_LINKING_VOWEL,       // Vowel insertion behavior is completely conditional (Veti)
    UNKNOWN                       // The root's internal vowel behavior is not yet determined or is ambiguous
};

struct SanskritRoot {
    std::string originalTagForm = "";    // The raw dictionary entry containing historical marker tags (e.g., L"डुकृञ्")
    std::string cleanLookupForm = "";    // The clean, stripped form used for algorithmic string matching (e.g., L"कृ")

    ConjugationClass conjugationClass = ConjugationClass::UNKNOWN;  // The internal morphological structural pattern it follows
    RootVoiceType naturalVoiceType = RootVoiceType::UNKNOWN;     // The default inflection suffix set it is pre-wired to use
    InternalVowelRule internalVowelRule = InternalVowelRule::UNKNOWN;// Internal structural chemistry behavior for vowel insertions

    std::string traditionalMeaning = "";    // The semantic definition text from the ancient root lists (e.g., L"सत्तायाम्")
    std::string englishMeaning = "";        // The literal concept translation (e.g., L"to be / to exist")
};

// ---- ETC -----

struct IndeclinableMetadata {
    // Unchanging words (particles, conjunctions) don't need inflected details
};

struct UnknownMetadata {};

using WordMetadata = std::variant<UnknownMetadata, NominalMetadata, VerbMetadata, IndeclinableMetadata>;

// ----- WORD -----

enum class WordType {
    UNKNOWN,
    VERB,
    NOMINAL,
    INDECLINABLE
};

struct WordAnalysis {
    bool success = false;
    std::string original = "";
    std::vector<std::string> components = {}; // e.g., ["उप", "जाय", "ते"] or ["नर", "ः"]
    std::optional<VerbMetadata> verbInfo;
    std::optional<NominalMetadata> nominalInfo;
    std::string matchType = "none"; // "indeclinable", "prefix", "verb", "stem", or "nominal"
};

struct Word {
    std::string text;
    std::string cleanForm;
    std::vector<SanskritRoot> roots; // Holds full structural definition objects for compound tracking

    std::vector<WordAnalysis> analyses;

    WordType type;
    WordMetadata metadata = UnknownMetadata{};
};

struct WordAnalysisNode
{
    WordAnalysis analysis;

    std::vector<std::shared_ptr<WordAnalysisNode>> children;
};

struct WordTreeNode
{
    Word word;

    std::vector<std::shared_ptr<WordTreeNode>> children;
};

struct WordListNode
{
    Word word;

    std::shared_ptr<WordListNode> next;
};