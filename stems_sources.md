# `stems.json` source and ranking notes

## Result

- Exactly 15,000 unique Devanagari nominal stems.
- Valid UTF-8 JSON with one top-level key: `stems`.
- Includes nouns, adjectives, pronouns, names, and other inflecting nominal stems; finite verb lemmas were excluded.

## Sources used

1. Gérard Huet's Sanskrit Heritage morphology XML (`SL_morph.xml`, header dated 15 March 2026): nominal analyses supplied the main validated stem inventory.
   - https://sanskrit.uohyd.ac.in/SKT/xml.html
2. Ambuda's corrected export of Oliver Hellwig's Digital Corpus of Sanskrit: noun/adjective lemma counts across 14 tagged Sanskrit text files supplied broad corpus-frequency estimates.
   - https://github.com/ambuda-org/dcs
3. A 700-verse Bhagavad Gita Sanskrit JSON with separated-word text supplied the highest-priority ranking signal.
   - https://gist.github.com/arangates/0db1aa5c0c97a4ca2073afaed0b0453a
4. The Rigveda Samhita pada-patha dataset supplied the secondary Vedic ranking signal.
   - https://github.com/deeplearningforsanskrit/rigveda-samhita

## Ranking and normalization

1. Bhagavad Gita-attested stems, ordered by estimated token frequency.
2. Rigveda-attested stems, ordered by estimated token frequency.
3. Remaining stems, ordered by Digital Corpus of Sanskrit frequency.
4. Sanskrit Heritage paradigm coverage, then Devanagari alphabetical order, used as deterministic tie-breakers.

Source transliteration was normalized from SLP1/IAST to NFC Devanagari. Sanskrit Heritage homonym markers such as `#1` and `#2` were removed, and duplicate Devanagari strings were collapsed.

## Ambiguities resolved

The Bhagavad Gita itself does not contain 15,000 distinct nominal stems. Its resolved nominal vocabulary was therefore used as the first ranking tier, while the remaining coverage came from the Rigveda and the broader tagged Sanskrit corpus. Ambiguous surface forms were assigned to the corpus-frequent nominal analysis to avoid promoting rare homonyms.
