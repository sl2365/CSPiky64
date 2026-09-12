CSPiky64 - Version 1.38
=====================

PURPOSE
-------
This adds Icecream-compatible MIDI pitch bend and modulation-wheel performance
control while retaining the permanent sl23 identity, factory presets, sound
engine, parameter layout and interface from Version 1.37.

Version 1.38 includes:

  - MIDI pitch bend with a default range of plus/minus 2 semitones
  - MIDI RPN pitch-bend sensitivity support for host-selectable bend ranges
  - Modulation wheel (CC1) vibrato at 5.5 Hz and up to plus/minus 1 semitone
  - Five-millisecond smoothing for pitch bend and modulation-wheel movement
  - MIDI Reset All Controllers support for returning both controls to neutral

  - Manufacturer name set explicitly to sl23
  - Four-character manufacturer code set explicitly to sl23
  - Explicit bundle ID com.sl23.cspiky64
  - The existing unique CSPiky64 plug-in code Pk64 retained
  - A native Windows x64 VST3 build
  - A temporary playable pluck voice
  - A voice level that intentionally ignores MIDI velocity, followed by an
    independent master output-volume control
  - A four-octave coloured on-screen keyboard raised by one octave to match the
    original GUI, without transposing external MIDI input
  - A one-double-click build with Results.log
  - Proportional interface resizing from 75% to 200%
  - A fixed aspect ratio that preserves the original interface proportions
  - Saved and host-automatable Wave 1 Strong/Soft selection
  - Saved and host-automatable Wave 2 On/Off selection
  - Twelve saved and host-automatable Wave 2 Shape positions matching the twelve
    physical detents observed on the original plug-in
  - Wave 2 position data isolated by subtracting the supplied Wave 2 OFF recording
    from every numbered recording
  - Per-position waveform phase, harmonic structure, and relative output level
    measured from the supplied 48 kHz C5 recordings
  - The common original decay removed before measuring each single-cycle wave
  - Original oscillator phase aligned to the accepted CSPiky64 Wave 1 phase
  - Content retained through the measured 40th harmonic
  - Efficient 256-sample linearly interpolated tables for all twelve positions
  - All twelve Wave 2 detents available through mouse dragging and the mouse wheel
  - A compact 1-through-12 number centred inside the orange selector knob
  - Saved and host-automatable Character Calm/Nervous selection
  - A measured 5% third harmonic in the base Wave 1 signal for the slightly
    flattened original Calm waveform
  - Nervous produced by strong per-voice symmetrical soft squaring
  - The provisional separately added seventh harmonic removed from Nervous
  - Approximately 9 to 10 dB of measured Nervous level compensation
  - An 8.87 dB Calm level correction, raising its isolated Soft/Simple peak from
    approximately -16.04 dBFS to -7.17 dBFS
  - An isolated Soft/Simple Character difference of approximately 3 dB, with
    Nervous remaining louder without overwhelming Calm
  - Natural stepped and spiked Nervous shapes when square-like release voices
    overlap at different phases
  - Saved and host-automatable decay control
  - Saved and host-automatable release control
  - Faster DEC and REL response near the left side of their knob travel
  - An estimated DEC curve that moves the former 47.5% sound to about 9 o'clock
  - An estimated REL curve that raises the 9-o'clock release from approximately
    0.06 seconds to 0.69 seconds, restoring much more audible tail energy
  - Reduced Wave 2 level so it no longer overpowers Wave 1
  - A wider independent release range for short-note string-like tails
  - An orange rotary Wave 2 waveform selector that responds to the mouse wheel
  - A separate blue control that adds two detuned saws to Wave 2
  - Saved and host-automatable detuned-saw mix
  - The blue +SAWS control kept separate from the measured Shape tables
  - +SAWS recalibrated from eleven supplied 48 kHz recordings covering 0-100%
  - A dominant lower saw measured at approximately 97.72% of played frequency,
    about 40 cents below the note rather than the former approximately 6 cents
  - A quieter upper companion saw at approximately 102.28% of played frequency
  - Measured asymmetric levels of 0.64 and 0.08 for the lower and upper saws,
    producing the original's much stronger detuning and audible beating
  - The measured near-linear +SAWS response retained across the knob
  - A new zero default and double-click reset for +SAWS, matching the supplied
    Wave 2 recording setup and preventing it from colouring Shape comparisons
  - Approximately 3 dB more provisional overall output
  - Smaller mode controls and text
  - Wave 2 selector and +SAWS enlarged to match DEC, REL, FILTER, REV, and VOL
  - All eight rotary controls sized identically at every GUI scale
  - Pad colours sampled from the original plug-in reference
  - The left octave in each pad row shifted slightly left, creating a clearer
    vertical gap between the left and right octave groups
  - Saturated pressed-pad highlighting that preserves each pad's vivid colour
  - A live dark-grey waveform trace across the existing band between pad rows
  - A short rolling snapshot of the final mono-summed plug-in output
  - Real-time-safe atomic transfer from the audio processor to the editor
  - A 30 Hz scope refresh for fluid movement without burdening the audio thread
  - Automatic visual gain with a low-level gate, keeping normal notes readable
    while returning silence to a straight centre line
  - Scope thickness and geometry that scale proportionally from 75% to 200%
  - A saved and host-automatable Scope Color control directly beneath WAVE 1
  - Scope Color aligned to the horizontal centre of the WAVE 1 control above it
  - A fully-left grey default and double-click reset to the original grey trace
  - Smooth colour interpolation from grey through red, orange, yellow, green,
    cyan, blue, indigo, and violet
  - Matching live colours on the scope trace and the Scope Color knob
  - No change to the signal path or sound from the Scope Color control
  - A proportionally scaling charcoal and grey frame around the main GUI
  - A uniform five-design-pixel frame thickness on the top, left, right, and
    lower separator
  - Dark and light frame sections divided by the original-style diagonal
  - An angled join near the right side of the top frame edge
  - A corresponding angled join near the left side of the lower frame edge
  - The border's lower edge placed between the main GUI and status bar, matching
    the original rather than enclosing the bottom of the status bar
  - A dark, subtly shaded status bar based on the original colour treatment
  - CSPiky64 retained in yellow with the current shortened version in white
  - The trailing .0 removed from the visible status-bar version number
  - Three overlapping red, violet, and cyan rings modelled on the original emblem
  - The emblem positioned at the lower right but clear of the resize drag handle
  - The three-ring emblem raised by one design pixel from its Stage 0.30 position
  - A centred two-line SCOPE / COLOUR heading above the Scope Color knob
  - COLOUR aligned vertically with +SAWS on the lower control row
  - SCOPE lowered by two design pixels while COLOUR remains in its approved place
  - Four-pixel left movement of the complete WAVE 1 / SCOPE COLOUR column
  - Eight-pixel left movement of every control and label from WAVE 2 through VOL
  - Six design pixels of clear main-panel space between REV/VOL and the frame
  - Pad and live-scope positions retained exactly from Stage 0.31
  - Larger Strong/Soft, On/Off, and Nervous/Calm readouts
  - Fully proportional orange, blue, decay, release, filter, reverb, and volume
    knob artwork
  - Strong/Soft, On/Off, and Nervous/Calm text that scales with the GUI
  - A compact preset-name display in the unused lower-middle space
  - All four lower preset buttons reduced to 15 design pixels, exactly matching
    the preset-name field's height
  - The preset-name field lowered by one further design pixel, retaining the existing
    three-design-pixel gap above the shorter button row
  - Previous and next preset buttons beneath the preset name
  - Mouse-wheel preset selection over the name display, wrapping between the
    last user preset and the built-in INIT state
  - A NAME button for renaming the currently selected user preset
  - A MENU button with LOAD, SAVE, and SAVE AS functions
  - Human-readable .ini user presets in dist/Data/Presets
  - All twelve front-panel parameters stored in every user preset: WAVE 1,
    WAVE 2 on/off and shape, +SAWS, CHARACTER, DEC, REL, OCT, REV, VOL, FILTER,
    and SCOPE COLOUR
  - Complete validation before a preset is applied, preventing partial loads
  - Host-notified parameter changes when a preset is selected
  - A built-in INIT state that requires no external preset file
  - Fourteen original factory presets embedded directly into the plug-in:
    Cords, Dreamy, Happy, Hide, Infinite, Init 1, Init 2, Last, LikeDist, Mario,
    New, Plucked, Simple, and Tikk
  - Separate FACTORY and USER sections inside the LOAD menu
  - Factory presets available to previous/next buttons and mouse-wheel selection
    without installing separate factory-preset files
  - Automatic migration of existing .ini files from Data/Preset to Data/Presets,
    without overwriting a same-named file already in the new folder
  - The current preset label retained in the normal host project state
  - Editor Width and Height saved on GUI close to dist/Data/Settings.ini
  - Saved editor size clamped to the existing 75% through 200% limits on reopen
  - The build creates Data/Presets when absent and preserves it on later builds
  - Preset naming opens with the complete name selected and keyboard focus active
  - Enter is handled directly by the focused name field and confirms SAVE AS or
    NAME without requiring another mouse click
  - Factory preset names shown in gold and user preset names shown in blue
  - Slightly larger preset-name text for improved readability
  - INIT retained in the original neutral text colour
  - Preset scrolling returns safely to INIT if the selected user preset was
    deleted externally while the editor remained open
  - Immediate wheel response without the former timing filter dropping quick
    wheel movements
  - The obsolete Reference capture folder removed from the project package
  - Pointing-hand cursors on STRONG/SOFT, ON/OFF, and NERVOUS/CALM
  - FILTER and VOL headings changed to black while their knobs remain purple
    and green
  - ON/OFF positioned closely beside and on the same text line as WAVE 2
  - Both Wave 2 knobs centred beneath WAVE 2 and aligned with their relevant rows
  - +SAWS, FILTER, and VOL labels enlarged to match the WAVE 1 heading
  - +SAWS dimmed to the same 38% opacity as the disabled Wave 2 knobs
  - Unnecessary SHAPE, D, and R labels removed
  - A new 378 x 268 default interface, approximately 125% of the former default
  - Resizing from 75% to 200% calculated from the new larger default
  - A saved and host-automatable octave selector with normal (0) and upper (+1)
    positions
  - Proportional orange up/down octave arrows based on the original interface
  - The unnecessary large STAGE/version display removed from the control area
  - More space between the octave arrows
  - The octave selector raised so its arrow pair aligns with the top rotary row
  - A saved and host-automatable reverb amount control
  - A measured long reverb target of approximately 10 to 11 seconds RT60
  - A gradual squared wet curve that builds the reverb behind the direct sound
  - Full direct-sound level throughout the complete REV control range
  - Reverb output reduced by approximately 16 dB to match the supplied original
    fully wet recording instead of overpowering the direct pluck
  - Corrected fully dry gain so REV at minimum preserves the Stage 0.11 level
  - A darker, longer provisional room character for closer comparison with the
    original Piky tail
  - A green output-volume knob beneath REV, exactly the same size and rotary
    style as the REV knob
  - Saved and host-automatable Output Volume from -60 dB to +6 dB
  - Transparent 0 dB default output, preserving the Stage 0.18 sound and level
  - Double-click reset to 0 dB for quick repeatable comparisons
  - Smooth 20 ms gain changes to prevent control movement from clicking
  - Post-reverb placement so VOL controls the complete plug-in output, including
    the reverb tail
  - A perceptually curved VOL response instead of the Stage 0.19 linear-dB travel
  - 0 dB positioned at the centre of the VOL knob
  - Fine 0 dB to +6 dB adjustment across the complete upper half of the knob
  - A progressively steeper reduction below the centre, retaining near-silence
    at the fully-left position without making the middle of the knob too quiet
  - Existing saved dB values remain valid, but any Stage 0.19 VOL automation may
    need recreating because the knob-to-level mapping has intentionally changed
  - A purple FILTER knob directly beneath REL
  - FILTER sized exactly like DEC, REL, REV, and VOL at every GUI scale
  - A saved and host-automatable continuous FILTER position
  - Four cascaded one-pole stages matching the measured approximately 24 dB per
    octave attenuation of the original
  - Measured cutoff anchors of approximately 436 Hz, 983 Hz, 2.48 kHz, and
    7.23 kHz at the new 0%, 24.5%, 49%, and 73.5% knob positions
  - Geometric interpolation between measured anchors for continuous control
  - No added resonance, matching the supplied recordings
  - The original active 0-40% response stretched across 0-98% of the new knob
    for much finer adjustment without changing its measured sound
  - Exact signal bypass from 98% through 100%
  - A fully-right default and double-click reset
  - Smooth 20 ms position movement to prevent zipper noise and clicks
  - Filter placement after the synthesiser voices but before reverb, so the room
    receives the filtered instrument sound while VOL still controls everything
  - No lower-end control snap when FILTER approaches its fully-left position


IMPORTANT: ONE-TIME PLUG-IN IDENTITY CHANGE
-------------------------------------------
Version 1.37 changes the manufacturer code from SyMk to sl23. JUCE incorporates
the manufacturer and plug-in codes into the VST3 class identity, so a host may
list Version 1.37 as a different plug-in and may not automatically substitute it
for Version 1.36 in an existing project.

Human-readable CSPiky64 .ini presets remain compatible because all twelve
parameter IDs and their value formats are unchanged. Save important Version
1.36 sounds as user presets before migration, then copy those .ini files into
the Version 1.37 Data\Presets folder and load them normally.

MIDI notes and MIDI clips are unchanged. After inserting Version 1.37 on an old
track, check the track's instrument routing and any host-specific MIDI-learn or
controller assignments.

DAW automation belongs to the old plug-in instance and is not guaranteed to be
transferred when the host sees Version 1.37 as a new plug-in. Parameter IDs are
unchanged, so a DAW that supports copying automation between plug-in instances
may allow it, but this is host-specific and should be verified before removing
the Version 1.36 instance.

Keep a backup of the Version 1.36 VST3 and the original DAW project until each
project has been migrated and saved under a new filename.

Version 1.37 does not yet include:
  - Final verification of the measured Wave 2 positions at notes other than the
    supplied C5 reference note
  - Deliberate per-note random Nervous shaping; first verify whether overlapping
    release voices reproduce the original variation naturally
  - Final level balancing for Strong, Wave 2, chords, or multiple overlapping
    notes; Stage 0.18 calibrates the isolated Soft/Simple comparison first
  - Final measured decay and release ranges or curves
  - A separate release-tail gain boost; first confirm whether the corrected
    release duration also corrects its perceived loudness
  - Final subjective filter fine-tuning if the approximate original slider
    positions reveal a small audible offset during direct comparison
  - A final reverb match across multiple sounds
  - Other final interface refinements or control menus
  - Authentic descriptive names for the twelve Wave 2 positions; numeric labels
    avoid inventing names not supported by the recordings or manual

The former eight-choice Wave 2 parameter was provisional. Existing automation
or saved values for that selector may choose a different position in Stage 0.21
and should be recreated.

The FILTER parameter keeps its existing VST3 ID so hosts retain the automation
lane, but Stage 0.24 changes its value mapping from provisional frequency in Hz to
measured knob position. Recreate any Stage 0.22/0.23 FILTER automation. A former
fully-open saved value safely clamps to the new fully-open default.

Stage 0.25 retains that parameter ID but deliberately expands its travel. Existing
Stage 0.24 FILTER automation below 100% should be recreated. Fully-right saved
values remain fully open.


MIDI PERFORMANCE CONTROL TEST
-----------------------------

1. Open CSPiky64 in PHI or another VST3 host and hold a note.
2. Move pitch bend fully down and up. With the host's default MIDI setting, the
   note should move smoothly down and up by two semitones.
3. Return pitch bend to its centre. The note should return exactly to pitch.
4. Move the modulation wheel from minimum to maximum while holding a note.
   Vibrato should increase smoothly from none to a clearly audible one-semitone
   depth at 5.5 Hz.
5. Release the note while either control is moving and confirm the release tail
   continues to respond without clicks or steps.

PRESET MANAGEMENT TEST
----------------------
1. Build and open CSPiky64, then confirm a compact INIT display and the four
   buttons <, >, NAME, and MENU appear in the lower-middle space.
2. Set every front-panel control to an obvious non-default value, including
   SCOPE COLOUR, FILTER, VOL, OCT, and both Wave 2 knobs.
3. Click MENU and choose SAVE AS. Confirm "New Preset" is already selected and
   focused, type a distinctive name, press Enter, and confirm it is saved.
4. Check dist\Data\Presets and confirm a matching .ini file exists and is readable
   in a text editor.
5. Change every control, choose INIT, then load the saved preset from MENU and
   confirm all twelve controls and the sound return together.
6. Open MENU > LOAD > FACTORY and confirm all fourteen named factory presets are
   available. Confirm their displayed names are gold, then load several and
   confirm every front-panel control changes.
7. Create a second user preset. Use < and >, then scroll over the preset-name
   display; confirm all three methods cycle through INIT, factory presets, and
   user presets with wraparound. Confirm user names are blue and INIT remains
   the original neutral colour.
8. Select a user preset, click NAME, type a replacement name without clicking the
   text field, press Enter, and confirm both the display and
   its .ini filename change. NAME is intentionally disabled for built-in INIT
   and all embedded factory presets.
9. Change a selected user preset and choose MENU > SAVE. Reload INIT and the user
   preset to confirm SAVE replaced that preset's values.
10. Resize the GUI, close its window, and confirm dist\Data\Settings.ini contains
   Width and Height. Reopen it and confirm the saved size returns.
11. Build again and confirm the existing preset INI files and Settings.ini remain
    untouched.
12. Save and reopen the host project and confirm the current parameter state and
    preset label return even without manually loading a preset.
13. Confirm FILTER and VOL labels are black while their knobs remain purple and
    green.
14. If a former dist\Data\Preset folder contains user .ini files, open the plug-in
    and confirm they appear in dist\Data\Presets without replacing newer files.
15. Select a disposable user preset, delete its .ini file while the editor is
    open, then scroll once over the displayed name. Confirm the display returns
    cleanly to INIT rather than jumping unpredictably. Scroll again and confirm
    normal selection resumes immediately.


FILTER CONTROL TEST
-------------------
1. Double-click the purple FILTER knob and confirm it moves fully right.
2. Play several familiar Wave 1 and Wave 2 sounds.
3. Confirm fully right sounds identical to the approved unfiltered Stage 0.24.
4. Move FILTER to about 73.5% and confirm only the upper harmonics become subtly
   softer, corresponding to approximately 7.23 kHz.
5. Move it to about 49%, 24.5%, and fully left. These positions correspond to
   approximately 2.48 kHz, 983 Hz, and 436 Hz respectively.
6. Confirm the response becomes progressively darker and steeper without a
   resonant whistle or peak near the cutoff.
7. Move FILTER slowly throughout its active lower 98% while holding or repeatedly
   playing a note and confirm there are no clicks, steps, or zipper noise.
8. Confirm the lowest part remains continuous and does not snap to zero across
   its final 5-7%.
9. Move the knob from 98% to 100% and confirm it remains fully open throughout
   this small final region.
10. Add an obvious REV tail and confirm the filtered tone feeds the reverb.
11. Confirm the purple FILTER knob is directly beneath REL and exactly the same
    size as REL, REV, and VOL at 75%, 100%, and 200% GUI sizes.
12. Leave FILTER at an obvious non-default position, save and reopen the host
    project, and confirm its position and sound return correctly.
13. If the host exposes automation, confirm Filter Position is available and moves
    smoothly.


MEASURED FILTER SUMMARY
-----------------------
The recordings were mono, 48 kHz, 24-bit C5 notes made with all controls fixed
except FILTER. Harmonic transfer was calculated relative to FILTER_100.wav.

Original position    Current knob       Effective cutoff
  0%                    0%               436 Hz
 10%                   24.5%             983 Hz
 20%                   49%               2.48 kHz
 30%                   73.5%             7.23 kHz
 40%                   98%               Fully open / exact bypass

Four identical cascaded digital one-pole stages fit the measured harmonic losses
closely. The response therefore falls at approximately 24 dB per octave and does
not require a resonance stage.


LIVE SCOPE TEST
---------------
1. Open the plug-in without playing and confirm a straight dark-grey line rests
   midway between the two pad rows.
2. Confirm the SCOPE / COLOUR knob is directly beneath WAVE 1 and centred to it.
3. Turn SCOPE COLOR slowly clockwise and confirm the trace and knob smoothly move
   together through grey, red, orange, yellow, green, cyan, blue, indigo, and
   violet.
4. Double-click SCOPE COLOR and confirm both it and the trace return to grey.
5. Play individual low and high notes and confirm the trace changes shape with
   the audible output.
6. Play several random notes and overlapping releases as in the supplied GIF;
   confirm the trace continues moving rather than showing a fixed decoration.
7. Compare CALM and NERVOUS, several Wave 2 positions, and FILTER settings;
   confirm visibly different waveform shapes are shown.
8. Add reverb and confirm the trace continues during its audible tail, then
   returns to the straight centre line as the output reaches silence.
9. Turn VOL fully left and confirm the scope settles to its centre line.
10. Confirm the trace remains entirely inside the gap and never covers a pad.
11. Resize the GUI to 75%, 100%, and 200%; confirm its position, height, line,
   Scope Color knob, and SCOPE / COLOUR label scale with the interface.
12. Click the pads immediately above and below the scope and confirm the display
   does not block their mouse input.
13. Leave SCOPE COLOR at a distinctive colour, save and reopen the host project,
   and confirm the knob and trace return to the same colour.


STATUS BAR AND BORDER TEST
--------------------------
1. Confirm the main interface has a five-pixel frame of consistent thickness on
   its top, left, right, and lower edges at the native design scale.
2. Confirm the dark and light frame colours meet at an angled join near the
   right side of the top edge and near the left side of the lower edge.
3. Confirm the lower frame runs between the main GUI and the status bar rather
   than around the outer bottom edge.
4. Confirm CSPiky64 is yellow and v1.37 is white, with no trailing .0.
5. Confirm three overlapping red, violet, and cyan rings appear at the lower
   right of the status bar.
6. Confirm the rings remain separate from and to the left of the resize handle
   and sit one pixel higher than in Stage 0.30.
7. Resize the GUI to 75%, 100%, and 200% and confirm the border, status bar,
   split-colour text, rings, and resize handle remain correctly aligned.


WAVE 2 SELECTOR TEST
--------------------
1. Set VOL to its centre 0 dB position.
2. Select Wave 1 SOFT, Character CALM, REV fully left, and +SAWS fully left.
3. Turn Wave 2 OFF and play the same C5 note used for the recordings.
4. Confirm both Wave 2 knobs and the +SAWS label dim together.
5. Confirm this still has the accepted Wave 1-only sound.
6. Turn Wave 2 ON and double-click the orange selector knob.
7. Play C5 and compare it with W2_01.wav; position 01 is deliberately subtle
   and mainly adds high harmonics rather than a loud fundamental.
8. Move through all twelve detents using mouse dragging or the wheel and confirm
   the number centred inside the orange knob runs from 1 through 12 without a
   leading zero.
9. Compare each detent with its same-numbered recording. Do not adjust VOL between
   positions because their different relative levels are part of the measurement.
10. Confirm the more strongly stepped and pulse-like later positions remain
    distinct rather than sounding like renamed sine, triangle, or saw waves.
11. Turn the blue +SAWS knob upward and confirm the added saw texture is much
    stronger than Version 0.34, with clearly audible detuning/beating, without
    changing the selected orange-knob detent.
12. Compare approximately 10%, 50%, and 100% against the supplied recordings and
    confirm the strength rises naturally across the complete knob.
13. Double-click +SAWS and confirm it returns fully left for clean comparisons.
14. Confirm both Wave 2 knobs match the other knobs in size and remain vertically
    centred beneath WAVE 2 at 75%, 100%, and 200% GUI sizes.
15. Save and reopen the host project and confirm the selected position returns.
16. If the host exposes automation, confirm all twelve positions are available.


OUTPUT VOLUME TEST
------------------
1. Open CSPiky64 and double-click the green VOL knob.
2. Confirm the knob moves to its centre and the output matches Stage 0.18 at
   0 dB.
3. Turn VOL to approximately one quarter and confirm the sound remains useful,
   at roughly -5.5 dB rather than the extremely quiet Stage 0.19 response.
4. Turn VOL fully left and confirm the complete plug-in output becomes nearly
   silent.
5. Return VOL to the centre, then carefully turn it fully right and confirm the
   output becomes approximately 6 dB louder without changing the tone.
6. Confirm the complete upper half now provides fine adjustment between 0 dB
   and +6 dB.
7. Add an obvious REV tail and move VOL while the tail is sounding.
8. Confirm both the direct sound and reverb tail change together.
9. Move VOL during a held or releasing note and confirm there are no clicks.
10. Resize the interface to 75%, 100%, and 200%; confirm the green knob stays the
   same size as REV and scales proportionally with it.
11. Leave VOL at an obvious non-default position, save and reopen the host project,
   and confirm its position and output level return correctly.
12. If your host exposes automation, move Output Volume and confirm it follows
    smoothly.


RESIZING TEST
-------------
1. Open CSPiky64 in your VST3 host.
2. Confirm its normal opening size is approximately 378 x 268 pixels and looks
   about 25% larger than Stage 0.9.
3. Drag the lower-right corner of the plug-in window inward.
4. Confirm it stops at approximately 284 x 201 pixels, which is 75% of the new
   default rather than 75% of the former smaller default.
5. Drag the lower-right corner outward.
6. Confirm it stops at approximately 756 x 536 pixels, which is 200% of the new
   default.
7. Confirm the interface keeps its original proportions throughout.
8. At both limits, click several coloured keys and confirm they still play.


TEXT READABILITY TEST
---------------------
1. Check STRONG/SOFT at the native interface size.
2. Confirm ON/OFF is closely beside WAVE 2, aligned with its text, and does not
   overlap that heading.
3. Check NERVOUS/CALM at the native interface size.
4. Confirm SCOPE / COLOUR, +SAWS, FILTER, and VOL are the same text size as WAVE 1.
5. Repeat these checks at 75%, 100%, and 200% interface sizes.
6. Confirm all readouts and labels scale with the rest of the interface.
7. Confirm the orange selector shows only 1 through 12 inside the knob, with no
   external POS text or leading zero.
8. Confirm SCOPE sits visibly closer to COLOUR while COLOUR remains aligned with
   +SAWS.
9. Confirm REV and VOL no longer touch the right frame.
10. Confirm the pads and live scope have not moved.


PROPORTIONAL KNOB TEST
----------------------
1. Compare the scope-colour, orange, blue, decay, release, filter, reverb, and
   volume knobs at 75%, 100%, and 200% sizes.
2. Confirm all eight knobs are exactly the same size at all three interface sizes.
3. Confirm the orange and blue knobs align horizontally with the corresponding
   top and bottom knob rows.
4. Confirm their circles, coloured arcs, outlines, and pointers all scale evenly.
5. Confirm the WAVE 2 heading is centred over its two knobs.
6. Confirm dragging and mouse-wheel adjustment still work at all three sizes.


OSCILLATOR CONTROL TEST
-----------------------
1. Select SOFT, turn Wave 2 OFF, turn REV fully left, and select CALM.
2. Play one note and confirm Calm remains mainly sine-like but is slightly flatter
   than Stage 0.16 rather than being a mathematically pure sine.
3. In SPAN, confirm a small third harmonic appears at three times the fundamental.
4. Change only CALM to NERVOUS and play one note after all earlier sound has ended.
5. Confirm the waveform becomes a clean, flat-topped soft square without the
   regular high-frequency ripple seen in Stage 0.16.
6. Confirm SPAN shows a descending odd-harmonic series without one abnormally
   strong seventh harmonic.
7. Confirm Nervous remains louder, but Calm is no longer overwhelmed; their
   isolated peak difference should be approximately 3 dB.
8. Tap the same note repeatedly before each release tail has ended.
9. Confirm overlapping square-like voices naturally produce changing stepped or
   one-sided spiked shapes on the scope.
10. Hold a note and confirm decay still damps it; compare this with a short tap,
    which should leave a stronger and longer release.
11. Play a few chords and report any audible clipping or harsh overload, because
    this stage calibrates both Character levels using a single Soft/Simple note.
12. Click STRONG and confirm it still changes the tone.
13. Turn Wave 2 ON and confirm all twelve measured positions and the separate
    +SAWS control remain selectable and audible.
14. Leave these controls at non-default settings for the state test below.


OCTAVE SELECTOR TEST
--------------------
1. Click the lower arrow beneath OCT and play a note.
2. Release the note.
3. Click the upper arrow and play the same note again.
4. Confirm the second note is exactly one octave higher.
5. Confirm the selected arrow is bright orange and the other arrow is dimmed.
6. Confirm there is now a clear visual gap between the two arrows.
7. Repeat the arrow clicks at 75%, 100%, and 200% interface sizes.
8. Leave the upper arrow selected, save the MuLab project, and reopen it.
9. Confirm the upper arrow remains selected and new notes still play one octave
   higher.
10. Confirm there is no longer a large STAGE/version display above the controls.
11. Confirm the current version remains visible in the bottom-left footer.


GUI PAD PITCH TEST
------------------
1. Select the lower OCT arrow.
2. Click the lowest coloured GUI pad and confirm it is one octave higher than it
   was in Stage 0.14.
3. Play a known note from an external MIDI keyboard or MuLab MIDI sequence.
4. Confirm the external MIDI note remains at exactly the same pitch as Stage
   0.14 and has not been transposed by this correction.
5. Select the upper OCT arrow and click the same GUI pad again.
6. Confirm the upper OCT position still raises that pad by one additional octave.


REVERB MIX TEST
---------------
1. Turn REV fully left and play several short notes.
2. Confirm the sound matches Stage 0.11 with no added reverb.
3. Turn REV to its middle position and repeat the notes.
4. Confirm the direct pluck remains at full strength while a long, restrained
   room tail is mixed behind it.
5. Turn REV to about 80% and confirm the direct pluck remains at full strength
   while the reverb becomes more prominent.
6. Move REV slowly from 80% to 100% and confirm the direct pluck no longer fades.
7. At 100%, confirm the full direct pluck remains and the quieter reverb has
   reached its maximum amount.
8. Confirm REV adds reverb without moving the decay or release
   knobs.
9. Check that the REV knob and artwork scale correctly at 75%, 100%, and 200%.
10. Leave REV at an obvious non-default position, save the MuLab project, and
   reopen it.
11. Confirm the REV position and reverb mix return correctly.

The decay length and wet output level are based on the supplied original
recordings. The exact room colour and stereo behavior remain provisional.


ENVELOPE CONTROL TEST
---------------------
The two knobs beneath DEC / REL are decay on the left and release on the right.

1. Set the original Piky DEC knob to 9 o'clock and play a repeated note.
2. Set CSPiky64 DEC to 9 o'clock and play the same note.
3. Confirm the two decay times are now close without moving CSPiky64 to 45% or
   50%.
4. Compare several additional DEC positions between fully left and 12 o'clock.
5. Set both plug-ins' REL knobs to 9 o'clock and play equal short notes.
6. Confirm CSPiky64 now has a substantially longer and more audible release tail
   at that position.
7. Compare the release-tail loudness after the two durations have been matched.
8. If CSPiky64 is still quieter, note whether the difference is slight, moderate,
   or large; that will guide a separate gain-only correction.
9. Confirm note-off produces no click or sudden upward jump in volume.
10. Confirm fully left remains very short and fully right remains very long for
    both DEC and REL.
11. Confirm both knobs still work at the 75% and 200% interface sizes.
12. Save and reopen the host project and confirm the DEC and REL settings return.

The parameter IDs and time limits are unchanged, but existing DEC/REL automation
may sound different because the knob response was intentionally remapped. The
curves remain provisional until more original-plug-in positions are measured.


PAD AND LAYOUT TEST
-------------------
1. Compare the coloured pads with the original plug-in.
2. Pay particular attention to the five sharp/black-note colours in each octave.
3. Confirm the left octave in both rows sits slightly farther left while both
   right-hand octaves remain in their Stage 0.25 positions.
4. Confirm this produces a slightly wider vertical gap between the octave groups.
5. Confirm the live scope remains inside the gap between the upper and lower pad
   rows and rests as a straight centre line during silence.
6. Click every pad and confirm each still responds at its correct pitch.
7. Confirm pressed pads become brighter and more saturated without turning pale
   or washed out.
8. Confirm the two OCT arrows sit slightly higher and align with the top knob row.


REQUIRED FOLDER POSITION
------------------------
Put the complete CSPiky64 folder beside your existing _Tools folder.

The folders should look like this:

  Your working folder
    _Tools
      cmake
        _4.4.2
      JUCE
        _8.0.15
    CSPiky64
      - Build.bat
      source
        Presets


BUILD
-----
1. Open the CSPiky64 folder.
2. Double-click "- Build.bat".
3. Wait for the BUILD SUMMARY.
4. If every line says PASS, press any key to close the window.
5. Open the dist folder.
6. The compiled file will be:

     CSPiky64.vst3

The build does not install the plug-in anywhere.

If the build fails, send Results.log instead of repeating the build.
