#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include "PluginProcessor.h"

class CSPiky64AudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit CSPiky64AudioProcessorEditor (CSPiky64AudioProcessor&);
    ~CSPiky64AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class CompactTextButton final : public juce::TextButton
    {
    public:
        void paintButton (juce::Graphics&, bool isMouseOverButton, bool isButtonDown) override;
    };

    class PresetButton final : public juce::TextButton
    {
    public:
        void paintButton (juce::Graphics&, bool isMouseOverButton, bool isButtonDown) override;
    };

    class PresetNameDisplay final : public juce::Component
    {
    public:
        void setText (const juce::String&);
        void paint (juce::Graphics&) override;
        void mouseWheelMove (const juce::MouseEvent&,
                             const juce::MouseWheelDetails&) override;

        std::function<void (int)> onStep;

    private:
        juce::String text { "INIT" };
        juce::uint32 lastWheelTime = 0;
    };

    class ProportionalRotaryLookAndFeel final : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                               float sliderPosition, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider&) override;
    };

    class OctaveSelector final : public juce::Component,
                                 private juce::Timer
    {
    public:
        explicit OctaveSelector (juce::AudioProcessorValueTreeState&);
        ~OctaveSelector() override;

        void paint (juce::Graphics&) override;
        void mouseDown (const juce::MouseEvent&) override;

    private:
        void timerCallback() override;

        std::atomic<float>* octaveValue = nullptr;
        juce::RangedAudioParameter* octaveParameter = nullptr;
        bool lastOctaveUp = false;
    };

    class GridKeyboard final : public juce::Component,
                               private juce::Timer
    {
    public:
        explicit GridKeyboard (juce::MidiKeyboardState&);
        ~GridKeyboard() override;

        void paint (juce::Graphics&) override;
        void resized() override;
        void mouseDown (const juce::MouseEvent&) override;
        void mouseDrag (const juce::MouseEvent&) override;
        void mouseUp (const juce::MouseEvent&) override;
        void mouseExit (const juce::MouseEvent&) override;

    private:
        struct Key
        {
            juce::Rectangle<float> bounds;
            int midiNote = 0;
            juce::Colour colour;
        };

        void timerCallback() override;
        void rebuildKeys();
        int noteAt (juce::Point<float>) const;
        void startNote (int midiNote);
        void stopActiveNote();

        juce::MidiKeyboardState& state;
        std::vector<Key> keys;
        int activeMouseNote = -1;
    };

    class ScopeDisplay final : public juce::Component,
                               private juce::Timer
    {
    public:
        explicit ScopeDisplay (CSPiky64AudioProcessor&);
        ~ScopeDisplay() override;

        void setTraceColour (juce::Colour);
        void paint (juce::Graphics&) override;

    private:
        void timerCallback() override;

        CSPiky64AudioProcessor& processor;
        std::array<float, CSPiky64AudioProcessor::scopeSampleCount> samples {};
        juce::Colour traceColour { 0xff3c3c3a };
    };

    void updateControlText();
    void updateScopeColour();
    void refreshPresetFiles();
    void restorePresetLabelFromState();
    void selectRelativePreset (int delta);
    void loadInitialPreset();
    bool loadPresetFile (const juce::File&);
    bool savePresetFile (const juce::File&, const juce::String& presetName);
    void saveCurrentPreset();
    void savePresetAs (const juce::String& requestedName);
    void renameCurrentPreset (const juce::String& requestedName);
    void showPresetNamePrompt (bool renameExistingPreset);
    void showPresetMenu();
    void showPresetError (const juce::String&) const;
    void setCurrentPreset (const juce::String&, const juce::File&);
    juce::Point<int> loadEditorSize() const;
    void saveEditorSize() const;

    CSPiky64AudioProcessor& processor;
    juce::File dataDirectory;
    juce::File presetDirectory;
    std::vector<juce::File> presetFiles;
    juce::File currentPresetFile;
    juce::String currentPresetName { "INIT" };
    GridKeyboard gridKeyboard;
    ScopeDisplay scopeDisplay;
    OctaveSelector octaveSelector;
    ProportionalRotaryLookAndFeel rotaryLookAndFeel;

    CompactTextButton wave1ModeButton;
    CompactTextButton wave2EnabledButton;
    juce::Slider wave2ShapeKnob;
    juce::Slider wave2DetunedSawKnob;
    CompactTextButton characterButton;
    juce::Slider decayKnob;
    juce::Slider releaseKnob;
    juce::Slider filterKnob;
    juce::Slider reverbKnob;
    juce::Slider volumeKnob;
    juce::Slider scopeColourKnob;
    PresetNameDisplay presetNameDisplay;
    PresetButton previousPresetButton;
    PresetButton nextPresetButton;
    PresetButton renamePresetButton;
    PresetButton presetMenuButton;
    std::unique_ptr<juce::AlertWindow> presetNameWindow;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<ButtonAttachment> wave1ModeAttachment;
    std::unique_ptr<ButtonAttachment> wave2EnabledAttachment;
    std::unique_ptr<SliderAttachment> wave2ShapeAttachment;
    std::unique_ptr<SliderAttachment> wave2DetunedSawAttachment;
    std::unique_ptr<ButtonAttachment> characterAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;
    std::unique_ptr<SliderAttachment> filterAttachment;
    std::unique_ptr<SliderAttachment> reverbAttachment;
    std::unique_ptr<SliderAttachment> volumeAttachment;
    std::unique_ptr<SliderAttachment> scopeColourAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CSPiky64AudioProcessorEditor)
};
