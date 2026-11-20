# Changelog

Each version's notes appear on its GitHub Release page. (PR7-N started from PR7-I 0.7.2: PR7-I's own history is in its
repository's changelog.)

## 0.1.0

The first Paragon PR7-N: the PR7 family's bass instrument, a 5-string in drop A played from MIDI. It starts from PR7-I
0.7.2 (the guitar instrument): the same GUITAR page, rack, engine, render lists and import, made into a bass.

- **Its own plugin.** Paragon PR7-N installs beside PR7-I, PR7-A and the rest of the family, with its own presets folder
  and settings. The logo says -N.
- **The bass.** Five strings in drop A: A0 E1 A1 D2 G2, played from the same keys as the reference bass instrument (an
  octave above where it sounds, as bass parts are written), so the parts from your studio play unchanged. Frets 0-24.
- **The picture.** Our own 3D bass: the body traced from a multiscale 5-string's outline (no maker's name or logo),
  fanned frets with the 7th square, the pickups slanted with them, its own headstock, at 85% like the guitars, all 24
  frets in the window. Notes light up between the slanted frets, where they cross each string.
- **The keyswitches: the reference bass's map.** Main alternating / up / down (C-2, C#-2, D-2) plays sustain with that
  picking; palm mute alternating / up / down (F-1, F#-1, G-1) plays palm mute, velocity setting how muted; the
  force-string keys G8 (the low A) to D#8 (the G) force their string while held. The map's other articulations (dead
  notes, slap, pop, harmonic, hammer / pull, tapping, slide, fret-hand mute, dead strum, dead palm rest) aren't rendered
  yet: they're recognised, and the rack's screen says so and what plays instead. The rack names the key latched.
- **The render lists for the bass**, played with the reference bass's own keys: sustain (three dynamics, four takes of
  each stroke, 45 minutes a string) and palm mute (five velocities, eight takes of each stroke, 53 minutes a string),
  each stroke's takes in a row; the releases (one list a string) and the unpitched noises. **A round-robin check** (12
  minutes) to render first: it measures how many takes the bass really has, so the lists can be cut down to match.
- **Gone from PR7-I** (they were the guitar's): the KEY MAP (Split / Factory), power chords, the hand-position and
  picking keys, F#0 fret noise and its knob, the power-chord and fret-noise lists, and the A/B suite (made for the
  guitar reference).
