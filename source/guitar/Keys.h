#pragma once

#include "guitar/Tuning.h"

// THE KEYSWITCHES (PR7-N 0.1.0): the reference bass instrument's (Djinn Bass 2) KEYSWITCH MAP, as the owner's screenshot
// shows it (typed into their studio PR7-S as its "Djinn Bass 2" profile; there is no manual), so MIDI parts written for it
// play the same here and the render lists play it with its own keys. MIDI note numbers (C-2 = 0, middle C = C3 = 60):
//
//   0  C-2  Main Alternating      the plain picked note, strokes alternating     -> SUSTAIN, ALTERNATE picking
//   1  C#-2 Main Up / Index       up strokes (or the index finger)               -> SUSTAIN, UP
//   2  D-2  Main Down / Mid       down strokes (or the middle finger)            -> SUSTAIN, DOWN
//   3  D#-2 Dead Note             \
//   4  E-2  Slap                   |
//   5  F-2  Dead Slap              |
//   6  F#-2 Pop                    |  not rendered yet (later stages, each its own lists): latched, so a note after
//   7  G-2  Dead Pop               |  one of them plays as the last rendered articulation did, rather than wrongly as
//   8  G#-2 Harmonic               |  another; the screen names what's latched
//   9  A-2  Hammer / Pull          |
//  10  A#-2 Tapping                |
//  11  B-2  Slide Trigger         /
//  16  E-1  Fret Hand Mute        (not rendered yet)
//  17  F-1  Palm Mute Alternating                                                 -> MUTE, ALTERNATE
//  18  F#-1 Palm Mute Up                                                          -> MUTE, UP
//  19  G-1  Palm Mute Down                                                        -> MUTE, DOWN
//  20  G#-1 Dead Strum            (not rendered yet)
//  98  D6   Dead Palm Rest        (not rendered yet)
// 127  G8   Force string 5 (the low A; the map calls it "Force B String", Djinn's default tuning being B0 E1 A1 D2 G2)
// 126  F#8  Force string 4 (E) ... 123 D#8 Force string 1 (G)
//
// The map shows no "force string off" key, so PR7-N takes a force-string key as HELD: the string is forced while the key
// is down and let go with its note-off (PR7-S's composer holds the key over the notes it puts on a string). Our reading of
// the map, not checked against the reference.
namespace guitar::keys
{
    inline constexpr int mainAlternate = 0, mainUp = 1, mainDown = 2;
    inline constexpr int deadNote = 3, slap = 4, deadSlap = 5, pop = 6, deadPop = 7, harmonic = 8, hammerPull = 9,
                         tapping = 10, slideTrigger = 11, fretHandMute = 16;
    inline constexpr int palmMuteAlternate = 17, palmMuteUp = 18, palmMuteDown = 19;
    inline constexpr int deadStrum = 20, deadPalmRest = 98;
    inline constexpr int forceLowA = 127;   // minus the string: 127 the low A (string 0) .. 123 the G (string 4)

    inline constexpr int forceKey (int string) noexcept { return forceLowA - string; }
    inline constexpr int forcedStringOf (int note) noexcept   // -1: not a force-string key
    {
        return note <= forceLowA && note > forceLowA - numStrings ? forceLowA - note : -1;
    }

    // The key that plays an articulation with a stroke (the render lists press it before each take).
    inline constexpr int strokeKey (Articulation a, Stroke s) noexcept
    {
        if (a == Articulation::mute) return s == Stroke::down ? palmMuteDown : palmMuteUp;
        return s == Stroke::down ? mainDown : mainUp;
    }

    // Every key of the map (the rest of the keys below the low A and above the G's 24th fret do nothing).
    inline constexpr bool isMapKey (int note) noexcept
    {
        return (note >= mainAlternate && note <= slideTrigger) || (note >= fretHandMute && note <= deadStrum) || note == deadPalmRest
            || forcedStringOf (note) >= 0;
    }

    // What an articulation key latches: the articulation it plays (unrendered ones: none, see above) and its picking.
    struct Latched { bool rendered = false; Articulation articulation = Articulation::sustain; int pick = 0; };   // pick: 0 alternate, 1 down, 2 up
    inline constexpr Latched latchedBy (int note) noexcept
    {
        switch (note)
        {
            case mainAlternate:     return { true, Articulation::sustain, 0 };
            case mainDown:          return { true, Articulation::sustain, 1 };
            case mainUp:            return { true, Articulation::sustain, 2 };
            case palmMuteAlternate: return { true, Articulation::mute, 0 };
            case palmMuteDown:      return { true, Articulation::mute, 1 };
            case palmMuteUp:        return { true, Articulation::mute, 2 };
            default:                return {};
        }
    }
    inline constexpr bool isArticulationKey (int note) noexcept { return isMapKey (note) && forcedStringOf (note) < 0; }

    // The map's name for a key ("Main Alternating"), for the screen.
    inline const char* nameOf (int note) noexcept
    {
        switch (note)
        {
            case mainAlternate: return "Main Alternating";   case mainUp: return "Main Up";       case mainDown: return "Main Down";
            case deadNote: return "Dead Note";               case slap: return "Slap";            case deadSlap: return "Dead Slap";
            case pop: return "Pop";                          case deadPop: return "Dead Pop";     case harmonic: return "Harmonic";
            case hammerPull: return "Hammer / Pull";         case tapping: return "Tapping";      case slideTrigger: return "Slide Trigger";
            case fretHandMute: return "Fret Hand Mute";      case palmMuteAlternate: return "Palm Mute Alternating";
            case palmMuteUp: return "Palm Mute Up";          case palmMuteDown: return "Palm Mute Down";
            case deadStrum: return "Dead Strum";             case deadPalmRest: return "Dead Palm Rest";
            default: return "";
        }
    }
}
