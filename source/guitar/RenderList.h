#pragma once

#include "guitar/Keys.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <optional>
#include <vector>

// THE RENDER LIST (PR7-I 0.2.0; PR7-N 0.1.0 for the bass): every note PR7-N needs from the owner's reference bass
// instrument, as MIDI files to render through it (one file per string and articulation), and where each note lands in the
// rendered audio. Our own design: because PR7-N writes the MIDI, it knows exactly when each note starts and which string,
// fret, dynamic and stroke it is, so the import can cut the render without guessing.
//
// Each file, at 120 BPM:
//   - every take in turn: each fret 0-24, each dynamic, then each stroke's round-robins IN A ROW (every down, then every
//     up: PR7-I's checks found the reference's sister guitar playing all its takes of a stroke only when that stroke
//     repeats; alternating strokes left out half of them);
//   - each take in its own slot of `slotSeconds`: its keyswitch first (Keys.h: the articulation with its stroke, Main
//     Down / Up or Palm Mute Down / Up), then `noteDelaySeconds` later the note, held `holdSeconds`, then silence for its
//     tail to die away;
//   - the string's force-string key (Keys.h: 127 the low A ... 123 the G) held from the file's start to its end, so it
//     works whether the reference latches it or holds it; `leadInSeconds` of silence before the first slot.
namespace guitar::renders
{
    inline constexpr double bpm = 120.0;
    inline constexpr double leadInSeconds = 2.0;
    inline constexpr double noteDelaySeconds = 0.25;   // keyswitches first, then the note

    // EACH ARTICULATION'S LIST (PR7-N 0.1.0: our first guess, to be checked by the owner's round-robin check render
    // (RoundRobinCheck.h) before the full lists are rendered; the reference's dynamics and round-robins aren't known:
    // there is no manual). Copies are left out by the import, so asking for more round-robins only costs render time.
    //   - SUSTAIN (MAIN): 3 dynamics at 45 / 85 / 118; 4 round-robins of each stroke (PR7-I's sister reference has 2 for
    //     sustain); each note held 3.5 s in a 4.5 s slot (a bass rings longer than its palm mutes, a little less than
    //     PR7-I's 4 s: the take streams from disk and fades over RELEASE whatever its length).
    //   - MUTE (PALM MUTE): velocity at 13 / 38 / 64 / 89 / 115 (PR7-I's sister reference's five degrees of muting; if the
    //     bass has fewer, the import keeps the same take only once); 8 round-robins of each stroke; 1 s in a 1.6 s slot.
    // (PR7-I's lists, for its history:
    //   - SUSTAIN (fixed: the owner renders these lists): 3 dynamics at velocities 45 / 85 / 118 (below the reference's
    //     rake, 120-126, and pinch harmonic, 127); 2 round-robins of each stroke (what the reference has: its round-robin
    //     check, November 2026); each note held 4 s in a 5.5 s slot.
    //   - MUTE (palm mute, 0.3.0): velocity sets how muted (the reference's way): its five degrees (Jupiter's Mute Layer
    //     table) at 13 / 38 / 64 / 89 / 115; 8 round-robins of each stroke (Jupiter has 6 up strokes and 10 down strokes;
    //     the import leaves copies out); a palm mute is gone within a second, so each held 1 s in a 1.6 s slot.)
    inline constexpr int maxLayers = 6, maxRoundRobins = 8;
    struct Spec
    {
        int layers = 1;
        std::array<int, maxLayers> velocities {};
        int roundRobins = 1;
        double holdSeconds = 1.0;   // how long a take's note is held (the longest it can ring)
        double slotSeconds = 2.0;   // from one take's keyswitches to the next's
        int key = keys::mainAlternate;   // the articulation's own key (its alternating one)
    };
    const Spec& spec (Articulation);
    int layerFor (Articulation, int velocity) noexcept;   // halfway between the list's velocities

    struct Take
    {
        int string = 0, fret = 0, layer = 0;
        Stroke stroke = Stroke::down;
        int roundRobin = 0;
        double noteSeconds = 0.0;   // when its note starts in the file
        Articulation articulation = Articulation::sustain;
        int note() const noexcept { return noteOf (string, fret); }
        int velocity() const noexcept { return spec (articulation).velocities[(size_t) layer]; }
    };

    std::vector<Take> takes (Articulation, int string);
    double lengthSeconds (Articulation, int string);   // to the end of the last take's slot
    juce::MidiFile toMidi (Articulation, int string);
    juce::String fileName (Articulation, int string);   // "PR7-N Sustain - String 5 (low A)" (no extension)

    // Where an imported take's file goes in a guitar's folder: "<Sustain>/s0_f05_l2_d_r1.wav", and back.
    juce::String takeFileName (Articulation, const Take&);
    struct TakeId { Articulation articulation = Articulation::sustain; int string = 0, fret = 0, layer = 0; Stroke stroke = Stroke::down; int roundRobin = 0; };
    std::optional<TakeId> parseTakeFile (const juce::File&);
}
