#include "PluginEditor.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

#if JUCE_WINDOWS
 #include <windows.h>
#endif

namespace
{
constexpr int designWidth = 302;
constexpr int designHeight = 214;
constexpr int defaultEditorWidth = 378;
constexpr int defaultEditorHeight = 268;
constexpr float minimumEditorScale = 0.75f;
constexpr float maximumEditorScale = 2.0f;
constexpr auto octaveUpId = "octaveUp";
constexpr auto currentPresetNameProperty = "currentPresetName";

constexpr std::array<const char*, 12> presetParameterIds {
    "wave1Strong",
    "wave2Enabled",
    "wave2Shape",
    "wave2DetunedSawMix",
    "characterNervous",
    "decaySeconds",
    "releaseSeconds",
    "octaveUp",
    "reverbMix",
    "outputVolumeDb",
    "filterCutoffHz",
    "scopeColour"
};

juce::File getPluginDataDirectory()
{
#if JUCE_WINDOWS
    static const int moduleAddressAnchor = 0;
    HMODULE moduleHandle = nullptr;

    if (GetModuleHandleExW (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                                | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR> (&moduleAddressAnchor),
                            &moduleHandle) != 0)
    {
        std::array<wchar_t, 32768> modulePath {};
        const auto pathLength = GetModuleFileNameW (
            moduleHandle, modulePath.data(), static_cast<DWORD> (modulePath.size()));

        if (pathLength > 0 && pathLength < modulePath.size())
            return juce::File (juce::String (modulePath.data()))
                .getParentDirectory()
                .getChildFile ("Data");
    }
#endif

    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("SynthMaker")
        .getChildFile ("CSPiky64")
        .getChildFile ("Data");
}

juce::StringPairArray readIniFile (const juce::File& file)
{
    juce::StringPairArray values (true);
    juce::StringArray lines;
    lines.addLines (file.loadFileAsString());

    for (auto line : lines)
    {
        line = line.trim();

        if (line.isEmpty() || line.startsWithChar (';') || line.startsWithChar ('#')
            || line.startsWithChar ('['))
            continue;

        const auto separator = line.indexOfChar ('=');
        if (separator <= 0)
            continue;

        values.set (line.substring (0, separator).trim(),
                    line.substring (separator + 1).trim());
    }

    return values;
}

juce::String legalPresetName (juce::String requestedName)
{
    requestedName = requestedName.trim();
    if (requestedName.endsWithIgnoreCase (".ini"))
        requestedName = requestedName.dropLastCharacters (4).trim();

    return juce::File::createLegalFileName (requestedName).trim();
}

juce::Colour getScopeColour (double position)
{
    constexpr std::array<juce::uint32, 9> colourAnchors {
        0xff3c3c3a, // Grey (the original Stage 0.28 trace colour)
        0xffd92f2f, // Red
        0xffff6b20, // Orange
        0xffd9b400, // Yellow
        0xff24a43a, // Green
        0xff00a6b8, // Cyan
        0xff2374d8, // Blue
        0xff4b43b8, // Indigo
        0xffa12db5  // Violet
    };

    const auto scaledPosition = juce::jlimit (0.0, 1.0, position)
                              * static_cast<double> (colourAnchors.size() - 1);
    const auto lowerIndex = juce::jlimit (
        0, static_cast<int> (colourAnchors.size()) - 2,
        static_cast<int> (std::floor (scaledPosition)));
    const auto fraction = static_cast<float> (
        scaledPosition - static_cast<double> (lowerIndex));

    return juce::Colour (colourAnchors[static_cast<std::size_t> (lowerIndex)])
        .interpolatedWith (
            juce::Colour (colourAnchors[static_cast<std::size_t> (lowerIndex + 1)]),
            fraction);
}
}

void CSPiky64AudioProcessorEditor::CompactTextButton::paintButton (juce::Graphics& g,
                                                                   bool isMouseOverButton,
                                                                   bool isButtonDown)
{
    auto textColour = juce::Colour (0xffff632f);
    if (isMouseOverButton)
        textColour = textColour.brighter (0.18f);
    if (isButtonDown)
        textColour = textColour.darker (0.15f);
    if (! isEnabled())
        textColour = textColour.withAlpha (0.45f);

    g.setColour (textColour);
    g.setFont (juce::FontOptions (getHeight() * 0.52f, juce::Font::bold));
    g.drawText (getButtonText(), getLocalBounds(), juce::Justification::centred);
}

void CSPiky64AudioProcessorEditor::PresetButton::paintButton (juce::Graphics& g,
                                                               bool isMouseOverButton,
                                                               bool isButtonDown)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    auto background = juce::Colour (0xffdadad5);

    if (isMouseOverButton)
        background = background.brighter (0.10f);
    if (isButtonDown)
        background = background.darker (0.12f);
    if (! isEnabled())
        background = background.withMultipliedAlpha (0.55f);

    g.setColour (background);
    g.fillRoundedRectangle (bounds, 1.5f);
    g.setColour (juce::Colour (0xff686864).withMultipliedAlpha (isEnabled() ? 1.0f : 0.45f));
    g.drawRoundedRectangle (bounds, 1.5f, 1.0f);

    g.setColour (juce::Colour (0xff242424).withMultipliedAlpha (isEnabled() ? 1.0f : 0.45f));
    g.setFont (juce::FontOptions (getHeight() * 0.44f, juce::Font::bold));
    g.drawFittedText (getButtonText(), getLocalBounds().reduced (1, 0),
                      juce::Justification::centred, 1, 0.70f);
}

void CSPiky64AudioProcessorEditor::PresetNameDisplay::setText (const juce::String& newText)
{
    if (text != newText)
    {
        text = newText;
        repaint();
    }
}

void CSPiky64AudioProcessorEditor::PresetNameDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (juce::Colour (0xffedede8));
    g.fillRoundedRectangle (bounds, 1.8f);
    g.setColour (juce::Colour (0xff686864));
    g.drawRoundedRectangle (bounds, 1.8f, 1.0f);
    g.setColour (juce::Colour (0xff242424));
    g.setFont (juce::FontOptions (getHeight() * 0.48f, juce::Font::bold));
    g.drawFittedText (text, getLocalBounds().reduced (3, 0),
                      juce::Justification::centred, 1, 0.72f);
}

void CSPiky64AudioProcessorEditor::PresetNameDisplay::mouseWheelMove (
    const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    const auto movement = std::abs (wheel.deltaY) >= std::abs (wheel.deltaX)
                              ? wheel.deltaY
                              : wheel.deltaX;

    if (movement == 0.0f)
        return;

    const auto now = juce::Time::getMillisecondCounter();
    if (lastWheelTime != 0 && now - lastWheelTime < 100)
        return;

    lastWheelTime = now;
    if (onStep != nullptr)
        onStep (movement > 0.0f ? -1 : 1);
}

void CSPiky64AudioProcessorEditor::ProportionalRotaryLookAndFeel::drawRotarySlider (
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosition, float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
    const auto diameter = static_cast<float> (juce::jmin (width, height)) * 0.84f;
    const auto centreX = static_cast<float> (x) + static_cast<float> (width) * 0.5f;
    const auto centreY = static_cast<float> (y) + static_cast<float> (height) * 0.5f;
    const auto radius = diameter * 0.5f;
    const auto lineWidth = diameter * 0.075f;
    const auto angle = rotaryStartAngle
                     + sliderPosition * (rotaryEndAngle - rotaryStartAngle);
    const auto alpha = slider.isEnabled() ? 1.0f : 0.38f;
    const auto knobBounds = juce::Rectangle<float> (diameter, diameter)
                                .withCentre (juce::Point<float> { centreX, centreY });

    g.setColour (juce::Colour (0xffecece7).withMultipliedAlpha (alpha));
    g.fillEllipse (knobBounds);

    g.setColour (slider.findColour (juce::Slider::rotarySliderOutlineColourId)
                        .withMultipliedAlpha (alpha));
    g.drawEllipse (knobBounds, lineWidth);

    juce::Path activeArc;
    activeArc.addCentredArc (centreX, centreY,
                             radius - lineWidth * 1.15f,
                             radius - lineWidth * 1.15f,
                             0.0f, rotaryStartAngle, angle, true);

    const auto activeColour = slider.findColour (juce::Slider::rotarySliderFillColourId)
                                  .withMultipliedAlpha (alpha);
    g.setColour (activeColour);
    g.strokePath (activeArc,
                  juce::PathStrokeType (lineWidth,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    const auto pointerThickness = diameter * 0.075f;
    const auto pointerLength = radius * 0.48f;
    juce::Path pointer;
    pointer.addRoundedRectangle (-pointerThickness * 0.5f,
                                 -radius * 0.72f,
                                 pointerThickness,
                                 pointerLength,
                                 pointerThickness * 0.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle)
                                .translated (centreX, centreY));
    g.fillPath (pointer);

    if (slider.getName() == "Wave 2 shape")
    {
        const auto shapeNumber = juce::roundToInt (slider.getValue()) + 1;
        g.setColour (juce::Colour (0xff343434).withMultipliedAlpha (alpha));
        g.setFont (juce::FontOptions (diameter * 0.34f, juce::Font::bold));
        g.drawText (juce::String (shapeNumber),
                    juce::roundToInt (knobBounds.getX()),
                    juce::roundToInt (knobBounds.getY()),
                    juce::roundToInt (knobBounds.getWidth()),
                    juce::roundToInt (knobBounds.getHeight()),
                    juce::Justification::centred,
                    false);
    }
}

CSPiky64AudioProcessorEditor::OctaveSelector::OctaveSelector (
    juce::AudioProcessorValueTreeState& parameterState)
    : octaveValue (parameterState.getRawParameterValue (octaveUpId)),
      octaveParameter (parameterState.getParameter (octaveUpId))
{
    jassert (octaveValue != nullptr);
    jassert (octaveParameter != nullptr);

    lastOctaveUp = octaveValue != nullptr && octaveValue->load() >= 0.5f;
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
    startTimerHz (20);
}

CSPiky64AudioProcessorEditor::OctaveSelector::~OctaveSelector()
{
    stopTimer();
}

void CSPiky64AudioProcessorEditor::OctaveSelector::paint (juce::Graphics& g)
{
    const auto width = static_cast<float> (getWidth());
    const auto height = static_cast<float> (getHeight());
    const auto centreX = width * 0.5f;
    const auto halfArrowWidth = width * 0.27f;
    const auto halfArrowHeight = height * 0.09f;
    const auto lineWidth = juce::jmax (1.0f, juce::jmin (width, height) * 0.075f);
    const auto octaveIsUp = octaveValue != nullptr && octaveValue->load() >= 0.5f;
    const auto orange = juce::Colour (0xffff632f);

    juce::Path upArrow;
    upArrow.startNewSubPath (centreX - halfArrowWidth, height * 0.27f + halfArrowHeight);
    upArrow.lineTo (centreX, height * 0.27f - halfArrowHeight);
    upArrow.lineTo (centreX + halfArrowWidth, height * 0.27f + halfArrowHeight);

    juce::Path downArrow;
    downArrow.startNewSubPath (centreX - halfArrowWidth, height * 0.73f - halfArrowHeight);
    downArrow.lineTo (centreX, height * 0.73f + halfArrowHeight);
    downArrow.lineTo (centreX + halfArrowWidth, height * 0.73f - halfArrowHeight);

    g.setColour (octaveIsUp ? orange : orange.withAlpha (0.38f));
    g.strokePath (upArrow, juce::PathStrokeType (lineWidth,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

    g.setColour (octaveIsUp ? orange.withAlpha (0.38f) : orange);
    g.strokePath (downArrow, juce::PathStrokeType (lineWidth,
                                                    juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
}

void CSPiky64AudioProcessorEditor::OctaveSelector::mouseDown (const juce::MouseEvent& event)
{
    if (octaveParameter == nullptr)
        return;

    const auto selectUpperOctave = event.position.y < static_cast<float> (getHeight()) * 0.5f;
    octaveParameter->beginChangeGesture();
    octaveParameter->setValueNotifyingHost (selectUpperOctave ? 1.0f : 0.0f);
    octaveParameter->endChangeGesture();
    repaint();
}

void CSPiky64AudioProcessorEditor::OctaveSelector::timerCallback()
{
    const auto octaveIsUp = octaveValue != nullptr && octaveValue->load() >= 0.5f;
    if (octaveIsUp != lastOctaveUp)
    {
        lastOctaveUp = octaveIsUp;
        repaint();
    }
}

CSPiky64AudioProcessorEditor::GridKeyboard::GridKeyboard (juce::MidiKeyboardState& keyboardState)
    : state (keyboardState)
{
    startTimerHz (30);
}

CSPiky64AudioProcessorEditor::GridKeyboard::~GridKeyboard()
{
    stopActiveNote();
}

CSPiky64AudioProcessorEditor::ScopeDisplay::ScopeDisplay (
    CSPiky64AudioProcessor& audioProcessor)
    : processor (audioProcessor)
{
    setInterceptsMouseClicks (false, false);
    startTimerHz (30);
}

CSPiky64AudioProcessorEditor::ScopeDisplay::~ScopeDisplay()
{
    stopTimer();
}

void CSPiky64AudioProcessorEditor::ScopeDisplay::setTraceColour (juce::Colour newColour)
{
    if (traceColour != newColour)
    {
        traceColour = newColour;
        repaint();
    }
}

void CSPiky64AudioProcessorEditor::ScopeDisplay::timerCallback()
{
    processor.copyScopeSamples (samples);
    repaint();
}

void CSPiky64AudioProcessorEditor::ScopeDisplay::paint (juce::Graphics& g)
{
    const auto width = static_cast<float> (getWidth());
    const auto height = static_cast<float> (getHeight());
    const auto centreY = height * 0.5f;
    const auto amplitude = height * 0.43f;

    auto peak = 0.0f;
    for (const auto sample : samples)
        peak = juce::jmax (peak, std::abs (sample));

    const auto displayGain = peak >= 0.001f
                                 ? amplitude / juce::jmax (peak, 0.02f)
                                 : 0.0f;

    juce::Path trace;
    for (std::size_t index = 0; index < samples.size(); ++index)
    {
        const auto x = static_cast<float> (index)
                     * (width - 1.0f)
                     / static_cast<float> (samples.size() - 1);
        const auto offset = juce::jlimit (-amplitude, amplitude,
                                          samples[index] * displayGain);
        const auto y = centreY - offset;

        if (index == 0)
            trace.startNewSubPath (x, y);
        else
            trace.lineTo (x, y);
    }

    const auto displayScale = juce::jmin (width / 286.0f, height / 20.0f);
    g.setColour (traceColour);
    g.strokePath (trace,
                  juce::PathStrokeType (juce::jmax (1.0f, 1.6f * displayScale),
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
}

void CSPiky64AudioProcessorEditor::GridKeyboard::timerCallback()
{
    repaint();
}

void CSPiky64AudioProcessorEditor::GridKeyboard::resized()
{
    rebuildKeys();
}

void CSPiky64AudioProcessorEditor::GridKeyboard::rebuildKeys()
{
    keys.clear();

    constexpr std::array<int, 7> naturalOffsets { 0, 2, 4, 5, 7, 9, 11 };
    constexpr std::array<int, 5> sharpOffsets   { 1, 3, 6, 8, 10 };
    constexpr std::array<float, 5> sharpSlots  { 0.5f, 1.5f, 3.5f, 4.5f, 5.5f };

    const std::array<juce::Colour, 7> naturalColours
    {
        juce::Colour (0xff128d02),
        juce::Colour (0xffae0606),
        juce::Colour (0xff9b0383),
        juce::Colour (0xff943501),
        juce::Colour (0xff0387b7),
        juce::Colour (0xffa56902),
        juce::Colour (0xff943501)
    };

    const std::array<juce::Colour, 5> sharpColours
    {
        juce::Colour (0xffa56902),
        juce::Colour (0xff0387b7),
        juce::Colour (0xff128d02),
        juce::Colour (0xffae0606),
        juce::Colour (0xff9b0383)
    };

    const auto width = static_cast<float> (getWidth());
    const auto scale = width / 286.0f;
    const auto keySize = 14.0f * scale;
    const auto leftMargin = 12.0f * scale;
    const auto rightMargin = 6.0f * scale;
    const auto cell = (width - leftMargin - rightMargin - keySize) / 13.0f;
    const auto secondRowTop = 54.0f * scale;
    const auto leftOctaveShift = 4.0f * scale;

    for (int row = 0; row < 2; ++row)
    {
        const auto rowTop = row == 0 ? 0.0f : secondRowTop;

        for (int octave = 0; octave < 2; ++octave)
        {
            const auto baseNote = 48 + row * 24 + octave * 12;

            for (int index = 0; index < 7; ++index)
            {
                const auto slot = static_cast<float> (octave * 7 + index);
                const auto x = leftMargin + slot * cell
                             - (octave == 0 ? leftOctaveShift : 0.0f);
                const auto y = rowTop + 25.0f * scale;
                keys.push_back ({ { x, y, keySize, keySize },
                                  baseNote + naturalOffsets[static_cast<size_t> (index)],
                                  naturalColours[static_cast<size_t> (index)] });
            }

            for (int index = 0; index < 5; ++index)
            {
                const auto slot = static_cast<float> (octave * 7)
                                + sharpSlots[static_cast<size_t> (index)];
                const auto x = leftMargin + slot * cell
                             - (octave == 0 ? leftOctaveShift : 0.0f);
                const auto y = rowTop + 7.0f * scale;
                keys.push_back ({ { x, y, keySize, keySize },
                                  baseNote + sharpOffsets[static_cast<size_t> (index)],
                                  sharpColours[static_cast<size_t> (index)] });
            }
        }
    }
}

void CSPiky64AudioProcessorEditor::GridKeyboard::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffd9d9d4));

    const auto scale = static_cast<float> (getWidth()) / 286.0f;

    for (const auto& key : keys)
    {
        auto colour = key.colour;
        if (state.isNoteOnForChannels (0xffff, key.midiNote))
            colour = colour.withSaturation (1.0f).withBrightness (
                juce::jmin (1.0f, colour.getBrightness() * 1.45f));

        g.setColour (juce::Colours::black.withAlpha (0.20f));
        g.fillRect (key.bounds.translated (1.1f * scale, 1.3f * scale));
        g.setColour (colour);
        g.fillRect (key.bounds);
        g.setColour (juce::Colours::black.withAlpha (0.75f));
        g.drawRect (key.bounds, 0.8f * scale);
    }
}

int CSPiky64AudioProcessorEditor::GridKeyboard::noteAt (juce::Point<float> point) const
{
    for (auto key = keys.rbegin(); key != keys.rend(); ++key)
        if (key->bounds.contains (point))
            return key->midiNote;

    return -1;
}

void CSPiky64AudioProcessorEditor::GridKeyboard::startNote (int midiNote)
{
    if (midiNote == activeMouseNote)
        return;

    stopActiveNote();

    if (midiNote >= 0)
    {
        activeMouseNote = midiNote;
        state.noteOn (1, midiNote, 1.0f);
        repaint();
    }
}

void CSPiky64AudioProcessorEditor::GridKeyboard::stopActiveNote()
{
    if (activeMouseNote >= 0)
    {
        state.noteOff (1, activeMouseNote, 1.0f);
        activeMouseNote = -1;
        repaint();
    }
}

void CSPiky64AudioProcessorEditor::GridKeyboard::mouseDown (const juce::MouseEvent& event)
{
    startNote (noteAt (event.position));
}

void CSPiky64AudioProcessorEditor::GridKeyboard::mouseDrag (const juce::MouseEvent& event)
{
    startNote (noteAt (event.position));
}

void CSPiky64AudioProcessorEditor::GridKeyboard::mouseUp (const juce::MouseEvent&)
{
    stopActiveNote();
}

void CSPiky64AudioProcessorEditor::GridKeyboard::mouseExit (const juce::MouseEvent& event)
{
    if (! event.mods.isAnyMouseButtonDown())
        stopActiveNote();
}

CSPiky64AudioProcessorEditor::CSPiky64AudioProcessorEditor (CSPiky64AudioProcessor& owner)
    : AudioProcessorEditor (&owner),
      processor (owner),
      dataDirectory (getPluginDataDirectory()),
      presetDirectory (dataDirectory.getChildFile ("Preset")),
      gridKeyboard (owner.getKeyboardState()),
      scopeDisplay (owner),
      octaveSelector (owner.getParameters())
{
    dataDirectory.createDirectory();
    presetDirectory.createDirectory();

    addAndMakeVisible (gridKeyboard);
    addAndMakeVisible (scopeDisplay);
    addAndMakeVisible (wave1ModeButton);
    addAndMakeVisible (wave2EnabledButton);
    addAndMakeVisible (wave2ShapeKnob);
    addAndMakeVisible (wave2DetunedSawKnob);
    addAndMakeVisible (characterButton);
    addAndMakeVisible (decayKnob);
    addAndMakeVisible (releaseKnob);
    addAndMakeVisible (filterKnob);
    addAndMakeVisible (octaveSelector);
    addAndMakeVisible (reverbKnob);
    addAndMakeVisible (volumeKnob);
    addAndMakeVisible (scopeColourKnob);
    addAndMakeVisible (presetNameDisplay);
    addAndMakeVisible (previousPresetButton);
    addAndMakeVisible (nextPresetButton);
    addAndMakeVisible (renamePresetButton);
    addAndMakeVisible (presetMenuButton);

    previousPresetButton.setButtonText ("<");
    nextPresetButton.setButtonText (">");
    renamePresetButton.setButtonText ("NAME");
    presetMenuButton.setButtonText ("MENU");

    presetNameDisplay.setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
    previousPresetButton.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    nextPresetButton.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    renamePresetButton.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    presetMenuButton.setMouseCursor (juce::MouseCursor::PointingHandCursor);

    const auto configureModeButton = [] (juce::TextButton& button)
    {
        button.setClickingTogglesState (true);
    };

    configureModeButton (wave1ModeButton);
    configureModeButton (wave2EnabledButton);
    configureModeButton (characterButton);

    wave2ShapeKnob.setName ("Wave 2 shape");
    wave2ShapeKnob.setLookAndFeel (&rotaryLookAndFeel);
    wave2ShapeKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    wave2ShapeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    wave2ShapeKnob.setRange (0.0, 11.0, 1.0);
    wave2ShapeKnob.setScrollWheelEnabled (true);
    wave2ShapeKnob.setMouseDragSensitivity (90);
    wave2ShapeKnob.setDoubleClickReturnValue (true, 0.0);
    wave2ShapeKnob.setColour (juce::Slider::rotarySliderFillColourId,
                              juce::Colour (0xffff632f));
    wave2ShapeKnob.setColour (juce::Slider::rotarySliderOutlineColourId,
                              juce::Colour (0xff858580));
    wave2ShapeKnob.setColour (juce::Slider::thumbColourId, juce::Colour (0xffff632f));

    wave2DetunedSawKnob.setName ("Wave 2 detuned saw mix");
    wave2DetunedSawKnob.setLookAndFeel (&rotaryLookAndFeel);
    wave2DetunedSawKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    wave2DetunedSawKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    wave2DetunedSawKnob.setRange (0.0, 1.0, 0.001);
    wave2DetunedSawKnob.setScrollWheelEnabled (true);
    wave2DetunedSawKnob.setMouseDragSensitivity (120);
    wave2DetunedSawKnob.setDoubleClickReturnValue (true, 0.0);
    wave2DetunedSawKnob.setColour (juce::Slider::rotarySliderFillColourId,
                                   juce::Colour (0xff168fc0));
    wave2DetunedSawKnob.setColour (juce::Slider::rotarySliderOutlineColourId,
                                   juce::Colour (0xff858580));
    wave2DetunedSawKnob.setColour (juce::Slider::thumbColourId,
                                   juce::Colour (0xff168fc0));

    const auto configureOrangeKnob = [] (juce::Slider& knob, const juce::String& name)
    {
        knob.setName (name);
        knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        knob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffff632f));
        knob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff858580));
        knob.setColour (juce::Slider::thumbColourId, juce::Colour (0xffff632f));
    };

    configureOrangeKnob (decayKnob, "Decay");
    configureOrangeKnob (releaseKnob, "Release");
    configureOrangeKnob (reverbKnob, "Reverb Mix");
    decayKnob.setLookAndFeel (&rotaryLookAndFeel);
    releaseKnob.setLookAndFeel (&rotaryLookAndFeel);
    reverbKnob.setLookAndFeel (&rotaryLookAndFeel);
    reverbKnob.setScrollWheelEnabled (true);
    reverbKnob.setMouseDragSensitivity (120);
    reverbKnob.setDoubleClickReturnValue (true, 0.0);

    filterKnob.setName ("Filter Position");
    filterKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    filterKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    filterKnob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff8d2bb0));
    filterKnob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff858580));
    filterKnob.setColour (juce::Slider::thumbColourId, juce::Colour (0xff8d2bb0));
    filterKnob.setLookAndFeel (&rotaryLookAndFeel);
    filterKnob.setScrollWheelEnabled (true);
    filterKnob.setMouseDragSensitivity (120);
    filterKnob.setDoubleClickReturnValue (true, 1.0);

    volumeKnob.setName ("Output Volume");
    volumeKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    volumeKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    volumeKnob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff2ca83f));
    volumeKnob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff858580));
    volumeKnob.setColour (juce::Slider::thumbColourId, juce::Colour (0xff2ca83f));
    volumeKnob.setLookAndFeel (&rotaryLookAndFeel);
    volumeKnob.setScrollWheelEnabled (true);
    volumeKnob.setMouseDragSensitivity (120);
    volumeKnob.setDoubleClickReturnValue (true, 0.0);

    scopeColourKnob.setName ("Scope Color");
    scopeColourKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    scopeColourKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    scopeColourKnob.setRange (0.0, 1.0, 0.001);
    scopeColourKnob.setColour (juce::Slider::rotarySliderOutlineColourId,
                               juce::Colour (0xff858580));
    scopeColourKnob.setLookAndFeel (&rotaryLookAndFeel);
    scopeColourKnob.setScrollWheelEnabled (true);
    scopeColourKnob.setMouseDragSensitivity (120);
    scopeColourKnob.setDoubleClickReturnValue (true, 0.0);

    auto& parameterState = owner.getParameters();
    wave1ModeAttachment = std::make_unique<ButtonAttachment> (
        parameterState, "wave1Strong", wave1ModeButton);
    wave2EnabledAttachment = std::make_unique<ButtonAttachment> (
        parameterState, "wave2Enabled", wave2EnabledButton);
    wave2ShapeAttachment = std::make_unique<SliderAttachment> (
        parameterState, "wave2Shape", wave2ShapeKnob);
    wave2DetunedSawAttachment = std::make_unique<SliderAttachment> (
        parameterState, "wave2DetunedSawMix", wave2DetunedSawKnob);
    characterAttachment = std::make_unique<ButtonAttachment> (
        parameterState, "characterNervous", characterButton);
    decayAttachment = std::make_unique<SliderAttachment> (
        parameterState, "decaySeconds", decayKnob);
    releaseAttachment = std::make_unique<SliderAttachment> (
        parameterState, "releaseSeconds", releaseKnob);
    filterAttachment = std::make_unique<SliderAttachment> (
        parameterState, "filterCutoffHz", filterKnob);
    reverbAttachment = std::make_unique<SliderAttachment> (
        parameterState, "reverbMix", reverbKnob);
    volumeAttachment = std::make_unique<SliderAttachment> (
        parameterState, "outputVolumeDb", volumeKnob);
    scopeColourAttachment = std::make_unique<SliderAttachment> (
        parameterState, "scopeColour", scopeColourKnob);

    wave1ModeButton.onStateChange = [this] { updateControlText(); };
    wave2EnabledButton.onStateChange = [this] { updateControlText(); };
    characterButton.onStateChange = [this] { updateControlText(); };
    scopeColourKnob.onValueChange = [this] { updateScopeColour(); };
    presetNameDisplay.onStep = [this] (int delta) { selectRelativePreset (delta); };
    previousPresetButton.onClick = [this] { selectRelativePreset (-1); };
    nextPresetButton.onClick = [this] { selectRelativePreset (1); };
    renamePresetButton.onClick = [this] { showPresetNamePrompt (true); };
    presetMenuButton.onClick = [this] { showPresetMenu(); };
    updateControlText();
    updateScopeColour();
    refreshPresetFiles();
    restorePresetLabelFromState();

    setResizable (true, true);
    setResizeLimits (juce::roundToInt (defaultEditorWidth * minimumEditorScale),
                     juce::roundToInt (defaultEditorHeight * minimumEditorScale),
                     juce::roundToInt (defaultEditorWidth * maximumEditorScale),
                     juce::roundToInt (defaultEditorHeight * maximumEditorScale));

    if (auto* boundsConstrainer = getConstrainer())
        boundsConstrainer->setFixedAspectRatio (static_cast<double> (designWidth)
                                                / static_cast<double> (designHeight));

    const auto editorSize = loadEditorSize();
    setSize (editorSize.x, editorSize.y);
}

CSPiky64AudioProcessorEditor::~CSPiky64AudioProcessorEditor()
{
    saveEditorSize();
    presetNameWindow.reset();
}

void CSPiky64AudioProcessorEditor::refreshPresetFiles()
{
    presetFiles.clear();
    presetDirectory.createDirectory();

    const auto files = presetDirectory.findChildFiles (juce::File::findFiles,
                                                        false, "*.ini");
    for (const auto& file : files)
        presetFiles.push_back (file);

    std::sort (presetFiles.begin(), presetFiles.end(),
               [] (const juce::File& first, const juce::File& second)
               {
                   return first.getFileNameWithoutExtension().compareIgnoreCase (
                              second.getFileNameWithoutExtension()) < 0;
               });
}

void CSPiky64AudioProcessorEditor::restorePresetLabelFromState()
{
    const auto storedName = processor.getParameters().state
                                .getProperty (currentPresetNameProperty, "INIT")
                                .toString()
                                .trim();

    if (storedName.isEmpty() || storedName.equalsIgnoreCase ("INIT"))
    {
        setCurrentPreset ("INIT", {});
        return;
    }

    for (const auto& file : presetFiles)
    {
        if (file.getFileNameWithoutExtension().equalsIgnoreCase (storedName))
        {
            setCurrentPreset (file.getFileNameWithoutExtension(), file);
            return;
        }
    }

    // A host project already contains the parameter values, so retain its label
    // even if the separate user-preset file has since been moved or deleted.
    setCurrentPreset (storedName, {});
}

void CSPiky64AudioProcessorEditor::setCurrentPreset (const juce::String& presetName,
                                                      const juce::File& presetFile)
{
    currentPresetName = presetName.isNotEmpty() ? presetName : "INIT";
    currentPresetFile = presetFile;
    presetNameDisplay.setText (currentPresetName);
    renamePresetButton.setEnabled (currentPresetFile.existsAsFile());
    processor.getParameters().state.setProperty (currentPresetNameProperty,
                                                  currentPresetName, nullptr);
}

void CSPiky64AudioProcessorEditor::selectRelativePreset (int delta)
{
    refreshPresetFiles();

    const auto presetCount = static_cast<int> (presetFiles.size()) + 1;
    auto currentIndex = 0;

    if (currentPresetFile.existsAsFile())
    {
        for (int index = 0; index < static_cast<int> (presetFiles.size()); ++index)
        {
            if (presetFiles[static_cast<std::size_t> (index)] == currentPresetFile)
            {
                currentIndex = index + 1;
                break;
            }
        }
    }

    auto nextIndex = (currentIndex + delta) % presetCount;
    if (nextIndex < 0)
        nextIndex += presetCount;

    if (nextIndex == 0)
        loadInitialPreset();
    else
        loadPresetFile (presetFiles[static_cast<std::size_t> (nextIndex - 1)]);
}

void CSPiky64AudioProcessorEditor::loadInitialPreset()
{
    auto& parameterState = processor.getParameters();

    for (const auto* parameterId : presetParameterIds)
    {
        if (auto* parameter = parameterState.getParameter (parameterId))
        {
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost (parameter->getDefaultValue());
            parameter->endChangeGesture();
        }
    }

    setCurrentPreset ("INIT", {});
}

bool CSPiky64AudioProcessorEditor::loadPresetFile (const juce::File& file)
{
    if (! file.existsAsFile())
    {
        showPresetError ("The selected preset file no longer exists.");
        return false;
    }

    const auto values = readIniFile (file);
    auto& parameterState = processor.getParameters();
    std::vector<std::pair<juce::RangedAudioParameter*, float>> pendingValues;
    pendingValues.reserve (presetParameterIds.size());

    for (const auto* parameterId : presetParameterIds)
    {
        auto* parameter = parameterState.getParameter (parameterId);
        if (parameter == nullptr || ! values.containsKey (parameterId))
        {
            showPresetError ("The preset is incomplete or incompatible:\n"
                             + file.getFileName());
            return false;
        }

        const auto actualValue = values[parameterId].getDoubleValue();
        if (! std::isfinite (actualValue))
        {
            showPresetError ("The preset contains an invalid value:\n"
                             + file.getFileName());
            return false;
        }

        const auto normalisedValue = juce::jlimit (
            0.0f, 1.0f,
            parameter->convertTo0to1 (static_cast<float> (actualValue)));
        pendingValues.emplace_back (parameter, normalisedValue);
    }

    for (const auto& pendingValue : pendingValues)
    {
        pendingValue.first->beginChangeGesture();
        pendingValue.first->setValueNotifyingHost (pendingValue.second);
        pendingValue.first->endChangeGesture();
    }

    auto presetName = values.getValue ("Name", file.getFileNameWithoutExtension()).trim();
    if (presetName.isEmpty())
        presetName = file.getFileNameWithoutExtension();

    setCurrentPreset (presetName, file);
    return true;
}

bool CSPiky64AudioProcessorEditor::savePresetFile (const juce::File& file,
                                                    const juce::String& presetName)
{
    if (presetDirectory.createDirectory().failed())
    {
        showPresetError ("The Data/Preset folder could not be created.");
        return false;
    }

    juce::String contents;
    contents << "; CSPiky64 user preset\n"
             << "[CSPiky64Preset]\n"
             << "Name=" << presetName << "\n"
             << "FormatVersion=1\n";

    auto& parameterState = processor.getParameters();
    for (const auto* parameterId : presetParameterIds)
    {
        auto* parameter = parameterState.getParameter (parameterId);
        if (parameter == nullptr)
        {
            showPresetError ("An internal preset parameter could not be found.");
            return false;
        }

        const auto actualValue = parameter->convertFrom0to1 (parameter->getValue());
        contents << parameterId << "=" << juce::String (actualValue, 9) << "\n";
    }

    if (! file.replaceWithText (contents))
    {
        showPresetError ("The preset could not be written:\n" + file.getFullPathName());
        return false;
    }

    return true;
}

void CSPiky64AudioProcessorEditor::saveCurrentPreset()
{
    if (! currentPresetFile.existsAsFile())
    {
        showPresetNamePrompt (false);
        return;
    }

    if (savePresetFile (currentPresetFile, currentPresetName))
    {
        refreshPresetFiles();
        setCurrentPreset (currentPresetName, currentPresetFile);
    }
}

void CSPiky64AudioProcessorEditor::savePresetAs (const juce::String& requestedName)
{
    const auto presetName = legalPresetName (requestedName);
    if (presetName.isEmpty())
    {
        showPresetError ("Please enter a preset name.");
        return;
    }

    const auto targetFile = presetDirectory.getChildFile (presetName + ".ini");
    if (targetFile.existsAsFile())
    {
        showPresetError ("A preset with that name already exists.\n"
                         "Use SAVE to replace the currently selected preset.");
        return;
    }

    if (savePresetFile (targetFile, presetName))
    {
        refreshPresetFiles();
        setCurrentPreset (presetName, targetFile);
    }
}

void CSPiky64AudioProcessorEditor::renameCurrentPreset (
    const juce::String& requestedName)
{
    if (! currentPresetFile.existsAsFile())
    {
        showPresetError ("INIT is built in and cannot be renamed.");
        return;
    }

    const auto presetName = legalPresetName (requestedName);
    if (presetName.isEmpty())
    {
        showPresetError ("Please enter a preset name.");
        return;
    }

    if (presetName.equalsIgnoreCase (currentPresetFile.getFileNameWithoutExtension()))
    {
        setCurrentPreset (currentPresetFile.getFileNameWithoutExtension(), currentPresetFile);
        return;
    }

    const auto targetFile = presetDirectory.getChildFile (presetName + ".ini");
    if (targetFile.existsAsFile())
    {
        showPresetError ("A preset with that name already exists.");
        return;
    }

    if (! currentPresetFile.moveFileTo (targetFile))
    {
        showPresetError ("The preset file could not be renamed.");
        return;
    }

    currentPresetFile = targetFile;
    if (! savePresetFile (targetFile, presetName))
        return;

    refreshPresetFiles();
    setCurrentPreset (presetName, targetFile);
}

void CSPiky64AudioProcessorEditor::showPresetNamePrompt (bool renameExistingPreset)
{
    if (renameExistingPreset && ! currentPresetFile.existsAsFile())
    {
        showPresetError ("INIT is built in and cannot be renamed.");
        return;
    }

    presetNameWindow = std::make_unique<juce::AlertWindow> (
        renameExistingPreset ? "Rename Preset" : "Save Preset As",
        renameExistingPreset ? "Enter the new preset name."
                             : "Enter a name for this preset.",
        juce::MessageBoxIconType::NoIcon, this);

    auto* window = presetNameWindow.get();

    const auto initialName = renameExistingPreset ? currentPresetName : "New Preset";
    window->addTextEditor ("presetName", initialName, "Name:");
    window->addButton (renameExistingPreset ? "Rename" : "Save", 1);
    window->addButton ("Cancel", 0);

    if (auto* editor = window->getTextEditor ("presetName"))
        editor->selectAll();

    juce::Component::SafePointer<CSPiky64AudioProcessorEditor> safeEditor (this);
    window->enterModalState (
        true,
        juce::ModalCallbackFunction::create (
            [safeEditor, renameExistingPreset] (int result)
            {
                if (auto* editor = safeEditor.getComponent())
                {
                    auto name = juce::String();
                    if (result == 1 && editor->presetNameWindow != nullptr)
                        name = editor->presetNameWindow->getTextEditorContents ("presetName");

                    editor->presetNameWindow.reset();

                    if (result == 1)
                    {
                        if (renameExistingPreset)
                            editor->renameCurrentPreset (name);
                        else
                            editor->savePresetAs (name);
                    }
                }
            }),
        false);
}

void CSPiky64AudioProcessorEditor::showPresetMenu()
{
    refreshPresetFiles();

    constexpr int savePresetId = 1;
    constexpr int savePresetAsId = 2;
    constexpr int initialPresetId = 1000;
    constexpr int firstUserPresetId = 1001;

    juce::PopupMenu loadMenu;
    loadMenu.addItem (initialPresetId, "INIT", true,
                      ! currentPresetFile.existsAsFile()
                          && currentPresetName.equalsIgnoreCase ("INIT"));

    for (int index = 0; index < static_cast<int> (presetFiles.size()); ++index)
    {
        const auto& file = presetFiles[static_cast<std::size_t> (index)];
        loadMenu.addItem (firstUserPresetId + index,
                          file.getFileNameWithoutExtension(), true,
                          file == currentPresetFile);
    }

    juce::PopupMenu menu;
    menu.addSubMenu ("LOAD", loadMenu);
    menu.addItem (savePresetId, "SAVE");
    menu.addItem (savePresetAsId, "SAVE AS...");

    juce::Component::SafePointer<CSPiky64AudioProcessorEditor> safeEditor (this);
    menu.showMenuAsync (
        juce::PopupMenu::Options().withTargetComponent (&presetMenuButton),
        [safeEditor, savePresetId, savePresetAsId,
         initialPresetId, firstUserPresetId] (int result)
        {
            auto* editor = safeEditor.getComponent();
            if (editor == nullptr || result == 0)
                return;

            if (result == savePresetId)
                editor->saveCurrentPreset();
            else if (result == savePresetAsId)
                editor->showPresetNamePrompt (false);
            else if (result == initialPresetId)
                editor->loadInitialPreset();
            else if (result >= firstUserPresetId)
            {
                const auto index = result - firstUserPresetId;
                if (index >= 0 && index < static_cast<int> (editor->presetFiles.size()))
                    editor->loadPresetFile (
                        editor->presetFiles[static_cast<std::size_t> (index)]);
            }
        });
}

void CSPiky64AudioProcessorEditor::showPresetError (const juce::String& message) const
{
    juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                             "CSPiky64 Preset", message);
}

juce::Point<int> CSPiky64AudioProcessorEditor::loadEditorSize() const
{
    const auto settingsFile = dataDirectory.getChildFile ("Settings.ini");
    if (! settingsFile.existsAsFile())
        return { defaultEditorWidth, defaultEditorHeight };

    const auto values = readIniFile (settingsFile);
    auto storedWidth = values.getValue ("Width", {}).getIntValue();
    const auto storedHeight = values.getValue ("Height", {}).getIntValue();

    if (storedWidth <= 0 && storedHeight > 0)
        storedWidth = juce::roundToInt (static_cast<double> (storedHeight)
                                       * static_cast<double> (designWidth)
                                       / static_cast<double> (designHeight));

    if (storedWidth <= 0)
        return { defaultEditorWidth, defaultEditorHeight };

    const auto minimumWidth = juce::roundToInt (defaultEditorWidth * minimumEditorScale);
    const auto maximumWidth = juce::roundToInt (defaultEditorWidth * maximumEditorScale);
    const auto width = juce::jlimit (minimumWidth, maximumWidth, storedWidth);
    const auto height = juce::roundToInt (static_cast<double> (width)
                                          * static_cast<double> (designHeight)
                                          / static_cast<double> (designWidth));
    return { width, height };
}

void CSPiky64AudioProcessorEditor::saveEditorSize() const
{
    if (dataDirectory.createDirectory().failed())
        return;

    juce::String contents;
    contents << "; CSPiky64 editor settings\n"
             << "[GUI]\n"
             << "Width=" << getWidth() << "\n"
             << "Height=" << getHeight() << "\n";
    dataDirectory.getChildFile ("Settings.ini").replaceWithText (contents);
}

void CSPiky64AudioProcessorEditor::updateControlText()
{
    wave1ModeButton.setButtonText (wave1ModeButton.getToggleState() ? "STRONG" : "SOFT");
    wave2EnabledButton.setButtonText (wave2EnabledButton.getToggleState() ? "ON" : "OFF");
    characterButton.setButtonText (characterButton.getToggleState() ? "NERVOUS" : "CALM");
    wave2ShapeKnob.setEnabled (wave2EnabledButton.getToggleState());
    wave2DetunedSawKnob.setEnabled (wave2EnabledButton.getToggleState());
    repaint();
}

void CSPiky64AudioProcessorEditor::updateScopeColour()
{
    const auto colour = getScopeColour (scopeColourKnob.getValue());
    scopeColourKnob.setColour (juce::Slider::rotarySliderFillColourId, colour);
    scopeColourKnob.setColour (juce::Slider::thumbColourId, colour);
    scopeDisplay.setTraceColour (colour);
    scopeColourKnob.repaint();
}

void CSPiky64AudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto scaleX = static_cast<float> (getWidth()) / static_cast<float> (designWidth);
    const auto scaleY = static_cast<float> (getHeight()) / static_cast<float> (designHeight);

    g.addTransform (juce::AffineTransform::scale (scaleX, scaleY));

    juce::ColourGradient background (juce::Colour (0xfff3f3ee), 0.0f, 96.0f,
                                       juce::Colour (0xffd1d1cc), 0.0f,
                                       static_cast<float> (designHeight),
                                       false);
    g.setGradientFill (background);
    g.fillRect (0, 0, designWidth, designHeight);

    // The original frame is a uniform five-pixel ring split diagonally into
    // dark and light sections. The diagonal is hidden by the centre panel and
    // is therefore visible only as angled joins on the top and lower edges.
    juce::Path darkFrameSection;
    darkFrameSection.startNewSubPath (0.0f, 0.0f);
    darkFrameSection.lineTo (220.0f, 0.0f);
    darkFrameSection.lineTo (95.0f, 200.0f);
    darkFrameSection.lineTo (0.0f, 200.0f);
    darkFrameSection.closeSubPath();

    juce::ColourGradient darkFrameGradient (juce::Colour (0xff4d4d55), 0.0f, 0.0f,
                                             juce::Colour (0xff363636), 0.0f, 200.0f,
                                             false);
    g.setGradientFill (darkFrameGradient);
    g.fillPath (darkFrameSection);

    juce::Path lightFrameSection;
    lightFrameSection.startNewSubPath (220.0f, 0.0f);
    lightFrameSection.lineTo (static_cast<float> (designWidth), 0.0f);
    lightFrameSection.lineTo (static_cast<float> (designWidth), 200.0f);
    lightFrameSection.lineTo (95.0f, 200.0f);
    lightFrameSection.closeSubPath();

    juce::ColourGradient lightFrameGradient (juce::Colour (0xff969696), 0.0f, 0.0f,
                                              juce::Colour (0xff7a7a7a), 0.0f, 200.0f,
                                              false);
    g.setGradientFill (lightFrameGradient);
    g.fillPath (lightFrameSection);

    g.setGradientFill (background);
    g.fillRect (5, 5, designWidth - 10, 190);

    g.setColour (juce::Colour (0xff242424));
    g.drawHorizontalLine (105, 8.0f, 294.0f);

    g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
    g.drawText ("WAVE 1", 6, 108, 48, 14, juce::Justification::centred);
    g.drawText ("WAVE 2", 52, 108, 48, 14, juce::Justification::centred);
    g.drawText ("CHARACTER", 116, 108, 52, 14, juce::Justification::centred);
    g.drawText ("DEC / REL", 169, 108, 68, 14, juce::Justification::centred);
    g.drawText ("OCT", 239, 108, 22, 14, juce::Justification::centred);
    g.drawText ("REV", 263, 108, 28, 14, juce::Justification::centred);

    auto sawLabelColour = juce::Colour (0xff168fc0);
    if (! wave2EnabledButton.getToggleState())
        sawLabelColour = sawLabelColour.withAlpha (0.38f);
    g.setColour (sawLabelColour);
    g.drawText ("+SAWS", 52, 150, 48, 13, juce::Justification::centred);

    g.setColour (juce::Colour (0xff242424));
    g.drawText ("VOL", 257, 150, 40, 13, juce::Justification::centred);

    g.drawText ("FILTER", 196, 150, 48, 13, juce::Justification::centred);

    g.setColour (juce::Colour (0xff242424));
    g.drawText ("SCOPE", -4, 142, 66, 11, juce::Justification::centred);
    g.drawText ("COLOUR", -4, 150, 66, 13, juce::Justification::centred);

    juce::ColourGradient statusBar (juce::Colour (0xff363636), 0.0f, 200.0f,
                                    juce::Colour (0xff1c1c1c), 0.0f, 208.0f,
                                    false);
    g.setGradientFill (statusBar);
    g.fillRect (0, 200, designWidth, 14);

    g.setColour (juce::Colour (0xffc6df52));
    g.setFont (juce::FontOptions (9.0f));
    g.drawText ("CSPiky64", 6, 200, 44, 14, juce::Justification::centredLeft);

    g.setColour (juce::Colours::white);
    g.drawText ("v0.33", 50, 200, 44, 14, juce::Justification::centredLeft);

    const auto drawStatusRing = [&g] (juce::Rectangle<float> bounds,
                                      juce::Colour topColour,
                                      juce::Colour bottomColour)
    {
        juce::ColourGradient ringGradient (topColour, bounds.getCentreX(), bounds.getY(),
                                           bottomColour, bounds.getCentreX(), bounds.getBottom(),
                                           false);
        g.setGradientFill (ringGradient);
        g.drawEllipse (bounds, 1.2f);
    };

    drawStatusRing ({ 259.0f, 202.0f, 10.0f, 10.0f },
                    juce::Colour (0xffe5b3a5), juce::Colour (0xffb52525));
    drawStatusRing ({ 265.0f, 202.0f, 10.0f, 10.0f },
                    juce::Colour (0xffda90fd), juce::Colour (0xff9702cc));
    drawStatusRing ({ 271.0f, 202.0f, 10.0f, 10.0f },
                    juce::Colour (0xff8ee5ef), juce::Colour (0xff01a8d1));
}

void CSPiky64AudioProcessorEditor::resized()
{
    const auto scaleX = static_cast<float> (getWidth()) / static_cast<float> (designWidth);
    const auto scaleY = static_cast<float> (getHeight()) / static_cast<float> (designHeight);

    gridKeyboard.setBounds (juce::roundToInt (8.0f * scaleX),
                            juce::roundToInt (8.0f * scaleY),
                            juce::roundToInt (286.0f * scaleX),
                            juce::roundToInt (96.0f * scaleY));

    const auto setScaledBounds = [scaleX, scaleY] (juce::Component& component,
                                                   float x, float y, float width, float height)
    {
        component.setBounds (juce::roundToInt (x * scaleX),
                             juce::roundToInt (y * scaleY),
                             juce::roundToInt (width * scaleX),
                             juce::roundToInt (height * scaleY));
    };

    setScaledBounds (wave1ModeButton, 4.0f, 124.0f, 50.0f, 21.0f);
    setScaledBounds (scopeDisplay, 8.0f, 48.0f, 286.0f, 20.0f);
    setScaledBounds (wave2EnabledButton, 94.0f, 108.0f, 20.0f, 16.0f);
    setScaledBounds (wave2ShapeKnob, 62.0f, 122.0f, 28.0f, 28.0f);
    setScaledBounds (wave2DetunedSawKnob, 62.0f, 163.0f, 28.0f, 28.0f);
    setScaledBounds (characterButton, 116.0f, 124.0f, 53.0f, 21.0f);
    setScaledBounds (decayKnob, 172.0f, 122.0f, 28.0f, 28.0f);
    setScaledBounds (releaseKnob, 206.0f, 122.0f, 28.0f, 28.0f);
    setScaledBounds (filterKnob, 206.0f, 163.0f, 28.0f, 28.0f);
    setScaledBounds (octaveSelector, 239.0f, 119.0f, 22.0f, 34.0f);
    setScaledBounds (reverbKnob, 263.0f, 122.0f, 28.0f, 28.0f);
    setScaledBounds (volumeKnob, 263.0f, 163.0f, 28.0f, 28.0f);
    setScaledBounds (scopeColourKnob, 15.0f, 163.0f, 28.0f, 28.0f);
    setScaledBounds (presetNameDisplay, 101.0f, 150.0f, 95.0f, 15.0f);
    setScaledBounds (previousPresetButton, 101.0f, 168.0f, 17.0f, 20.0f);
    setScaledBounds (nextPresetButton, 120.0f, 168.0f, 17.0f, 20.0f);
    setScaledBounds (renamePresetButton, 139.0f, 168.0f, 27.0f, 20.0f);
    setScaledBounds (presetMenuButton, 168.0f, 168.0f, 28.0f, 20.0f);
}
