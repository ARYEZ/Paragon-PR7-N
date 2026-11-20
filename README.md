# Paragon PR7-N

[![Build](https://github.com/ARYEZ/Paragon-PR7-N/actions/workflows/build.yml/badge.svg)](https://github.com/ARYEZ/Paragon-PR7-N/actions/workflows/build.yml)
[![Latest release](https://img.shields.io/github/v/release/ARYEZ/Paragon-PR7-N)](https://github.com/ARYEZ/Paragon-PR7-N/releases/latest)

The bass instrument of the Paragon PR7 family: a VST3 instrument that plays a 5-string bass from MIDI, for tight djent
and progressive metal parts. Its own plugin: no sampler host needed. Built with [JUCE](https://juce.com).

It plays a clean DI bass. For amps, cabinets and pedals, put [PR7-G](https://github.com/ARYEZ/Paragon-PR7-G) after it in
your DAW.

Paragon PR7-N started from [Paragon PR7-I](https://github.com/ARYEZ/Paragon-PR7-I) 0.7.2, the family's guitar instrument:
the same page, rack, engine, render lists and import, with a bass. The bass is a 5-string in drop A (A0 E1 A1 D2 G2)
with fanned frets (the 7th square), its body our own, traced from a multiscale 5-string's outline (no maker's name or
logo).

## Status

| Stage | What | Now |
|---|---|---|
| **0** | Its own plugin from PR7-I 0.7.2: the bass picture, 5 strings in drop A, the reference bass's keyswitch map, the bass's render lists | **Done (0.1.0)** |
| **1** | Render and import the takes; fit the lists to what the round-robin check finds | Next |
| **2** | The other articulations: dead notes, slap and pop, harmonics, hammer-ons and pull-offs, slides, fret-hand mute | Planned |

## Using it

1. **Render the round-robin check first.** On the RENDERS page, SAVE ALL LISTS writes every list and the round-robin
   check (12 minutes). Render it through your bass instrument as a clean DI (effects off, drop A, no doubling,
   anti-repetition, ringing or release noises) and bounce it to a WAV from bar 1: it shows how many different takes
   the bass has, so the lists ask for no more than that.
2. **Render the lists.** Each string has a sustain list (about 45 minutes: every fret 0-24, three dynamics, four takes of
   each stroke) and a palm-mute list (about 53 minutes: five velocities, eight takes of each stroke).
3. **Import.** Drop each WAV on its string's row. PR7-N finds the notes, cuts them, checks their pitch and saves them in
   the bass's folder (CHOOSE FOLDER... to put it elsewhere). A wrong file is refused and the row says why.
4. **Play.** Notes go where a bassist would put them; the GUITAR page lights each one between the slanted frets.

**Keyswitches** (the reference bass's keyswitch map, so parts written for it play the same): main alternating / up /
down C-2 / C#-2 / D-2; palm mute alternating / up / down F-1 / F#-1 / G-1 (velocity: how muted); force string G8 (the
low A) to D#8 (the G), held. The map's other articulations (dead notes, slap, pop, harmonic, hammer / pull, tapping,
slide, fret-hand mute, dead strum, dead palm rest) aren't rendered yet. Note names as in Cubase (middle C = C3);
SETTINGS can switch to C4.

## Download

Get the latest version from the **[Releases](https://github.com/ARYEZ/Paragon-PR7-N/releases/latest)** page: download
the zip for your computer. (Builds of every push also appear under **Actions** → a run → **Artifacts**.)

Unzip it and copy `Paragon PR7-N.vst3` to your VST3 folder:

| OS | VST3 folder |
|---|---|
| Windows | `C:\Program Files\Common Files\VST3` |
| Linux | `~/.vst3` |

There's no macOS build for now (only Windows and Linux).

Then rescan plugins in your DAW: PR7-N shows up as an instrument.

## Making a release

1. Set the new version in `CMakeLists.txt` (`project(ParagonX1 VERSION x.y.z)`).
2. Add a `## x.y.z` section to `CHANGELOG.md`.
3. Commit and push.

When a push carries a version that hasn't been released yet, GitHub Actions builds Windows and Linux, tags the commit
`vx.y.z` and publishes the release with notes from the changelog.

## Building from source

Requirements: CMake 3.24+, a C++20 compiler (Visual Studio 2022, or GCC 11+/Clang 14+), and Git.
JUCE and Catch2 are downloaded automatically at pinned versions.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

The plugin is written to `build/ParagonX1_artefacts/Release/VST3/` (the build target keeps its family name).

On Linux, install JUCE's dependencies first (the list is in `.github/workflows/build.yml`).

To render the editor to an image without a DAW:

```bash
PARAGON_SNAPSHOT=editor.png ./build/tests/ParagonX1Tests "[snapshot]"
```

## License

Paragon PR7-N is free software under the [GNU Affero General Public License v3](LICENSE.md),
which is required by JUCE's open-source licence.

It uses:
- [JUCE](https://github.com/juce-framework/JUCE): AGPLv3
- The Inter typeface (in `resources/fonts`) by Rasmus Andersson, under the SIL Open Font License

PR7-N is our own design and code, inspired by how commercial bass instruments play. It ships no third-party
sounds: it plays your own recordings from your computer.
