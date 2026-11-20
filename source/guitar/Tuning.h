#pragma once

#include <juce_core/juce_core.h>

#include <array>

// THE BASS (PR7-N 0.1.0; the code keeps PR7-I's "guitar" names, as the family keeps its targets' names): the owner's
// band's 5-string in drop A, A0 E1 A1 D2 G2. Strings are numbered from 0, the low A, up to 4, the G (a bassist calls the
// G string 1 and the low A string 5: `playerNumber`). Frets 0 (open) to 24.
//
// THE KEYS: the reference bass instrument (Djinn Bass 2) is played an octave above where it sounds, as bass parts are
// written, so its MIDI notes are the guitar's: the low A is note 33 (named A0 with middle C as C3, the way the owner's
// studio PR7-S and the reference name it), sounding at 27.5 Hz, an octave under PR7-I's low A. PR7-S's Djinn profile
// plays the low A as note 33, so the owner's parts play here unchanged. Where a pitch is measured (the imports), a take
// is checked an octave under its key (`soundingFrequency`), and at the key's own pitch too in case a render sounds there:
// which one the reference really sounds at hasn't been measured here.
namespace guitar
{
    inline constexpr int numStrings = 5;
    inline constexpr int numFrets = 24;   // the highest fret; 25 positions with the open string
    inline constexpr std::array<int, numStrings> openNotes { 33, 40, 45, 50, 55 };   // A0 E1 A1 D2 G2 (keys, C3 naming)
    inline constexpr int lowestNote = 33, highestNote = 55 + numFrets;                // the low A to the G's 24th fret
    inline constexpr int soundingOffset = -12;                                         // sounds an octave under its key

    inline int noteOf (int string, int fret) noexcept { return openNotes[(size_t) string] + fret; }
    inline int playerNumber (int string) noexcept { return numStrings - string; }   // 0 (low A) -> 5
    inline juce::String stringName (int string)
    {
        static const char* names[] { "A", "E", "A", "D", "G" };
        return juce::String (names[juce::jlimit (0, numStrings - 1, string)]);
    }
    // "STRING 5 (LOW A)", as the render list's rows and files name a string.
    inline juce::String stringLabel (int string)
    {
        const auto n = juce::String (playerNumber (string));
        if (string == 0) return "STRING " + n + " (LOW A)";
        return "STRING " + n + " (" + stringName (string) + ")";
    }
    // A string's name in file names and rows: "low A", "E" ... "G".
    inline juce::String stringWhich (int string) { return string == 0 ? juce::String ("low A") : stringName (string); }
    inline double frequencyOf (double note) noexcept { return 440.0 * std::pow (2.0, (note - 69.0) / 12.0); }
    inline double soundingFrequency (double note) noexcept { return frequencyOf (note + soundingOffset); }

    // How a string is played: SUSTAIN (the reference's MAIN) and MUTE (its PALM MUTE). Append only (saved by index).
    enum class Articulation { sustain, mute };
    inline constexpr int numArticulations = 2;
    inline juce::String articulationName (Articulation a) { return a == Articulation::mute ? "Mute" : "Sustain"; }

    enum class Stroke { down, up };
}
