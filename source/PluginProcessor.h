#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <cstddef>

class CSPiky64AudioProcessor final : public juce::AudioProcessor
{
public:
    CSPiky64AudioProcessor();
    ~CSPiky64AudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::MidiKeyboardState& getKeyboardState() noexcept { return keyboardState; }
    juce::AudioProcessorValueTreeState& getParameters() noexcept { return parameters; }
    static constexpr std::size_t scopeSampleCount = 128;
    void copyScopeSamples (std::array<float, scopeSampleCount>& destination) const noexcept;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void pushScopeSamples (const juce::AudioBuffer<float>& buffer) noexcept;

    juce::AudioProcessorValueTreeState parameters;
    juce::Synthesiser synthesiser;
    juce::MidiKeyboardState keyboardState;
    juce::Reverb reverb;
    std::atomic<float>* reverbMixParameter = nullptr;
    std::atomic<float>* outputVolumeDbParameter = nullptr;
    std::atomic<float>* filterPositionParameter = nullptr;
    juce::SmoothedValue<float> outputGain;
    juce::SmoothedValue<float> filterPosition;
    std::array<std::array<float, 4>, 2> filterState {};
    std::array<std::atomic<float>, scopeSampleCount> scopeSamples;
    std::atomic<std::size_t> scopeWritePosition { 0 };
    double processingSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CSPiky64AudioProcessor)
};
