#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Wave2Tables.h"

#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

namespace
{
constexpr auto wave1StrongId = "wave1Strong";
constexpr auto wave2EnabledId = "wave2Enabled";
constexpr auto wave2ShapeId = "wave2Shape";
constexpr auto wave2DetunedSawMixId = "wave2DetunedSawMix";
constexpr auto characterNervousId = "characterNervous";
constexpr auto decaySecondsId = "decaySeconds";
constexpr auto releaseSecondsId = "releaseSeconds";
constexpr auto octaveUpId = "octaveUp";
constexpr auto reverbMixId = "reverbMix";
constexpr auto outputVolumeDbId = "outputVolumeDb";
// Keep the Stage 0.22 parameter ID so hosts retain the same automation lane.
constexpr auto filterPositionId = "filterCutoffHz";
constexpr auto scopeColourId = "scopeColour";
constexpr auto filterBypassStart = 0.98f;

double getMeasuredFilterCutoff (float position) noexcept
{
    // The supplied recordings match four cascaded one-pole low-pass stages.
    // Stage 0.25 stretches the original lower 40% across 98% of this knob.
    constexpr std::array<double, 5> cutoffAnchorsHz {
        436.0, 983.0, 2482.0, 7225.0, 50000.0
    };

    const auto scaledPosition = static_cast<double> (
        juce::jlimit (0.0f, filterBypassStart, position))
        * (4.0 / static_cast<double> (filterBypassStart));
    const auto segment = juce::jlimit (0, 3,
                                       static_cast<int> (std::floor (scaledPosition)));
    const auto fraction = scaledPosition - static_cast<double> (segment);
    const auto lowerCutoff = cutoffAnchorsHz[static_cast<std::size_t> (segment)];
    const auto upperCutoff = cutoffAnchorsHz[static_cast<std::size_t> (segment + 1)];

    // Geometric interpolation preserves the measured exponential slider curve.
    return lowerCutoff * std::pow (upperCutoff / lowerCutoff, fraction);
}

class PikySound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override       { return true; }
    bool appliesToChannel (int) override    { return true; }
};

class PikyVoice final : public juce::SynthesiserVoice
{
public:
    PikyVoice (std::atomic<float>* wave1StrongParameter,
               std::atomic<float>* wave2EnabledParameter,
               std::atomic<float>* wave2ShapeParameter,
               std::atomic<float>* wave2DetunedSawMixParameter,
               std::atomic<float>* characterNervousParameter,
               std::atomic<float>* decaySecondsParameter,
               std::atomic<float>* releaseSecondsParameter,
               std::atomic<float>* octaveUpParameter)
        : wave1Strong (wave1StrongParameter),
          wave2Enabled (wave2EnabledParameter),
          wave2Shape (wave2ShapeParameter),
          wave2DetunedSawMix (wave2DetunedSawMixParameter),
          characterNervous (characterNervousParameter),
          decaySeconds (decaySecondsParameter),
          releaseSeconds (releaseSecondsParameter),
          octaveUp (octaveUpParameter)
    {
        jassert (wave1Strong != nullptr);
        jassert (wave2Enabled != nullptr);
        jassert (wave2Shape != nullptr);
        jassert (wave2DetunedSawMix != nullptr);
        jassert (characterNervous != nullptr);
        jassert (decaySeconds != nullptr);
        jassert (releaseSeconds != nullptr);
        jassert (octaveUp != nullptr);
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<PikySound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float, juce::SynthesiserSound*, int) override
    {
        const auto octaveOffset = octaveUp->load() >= 0.5f ? 12 : 0;
        const auto transposedNote = juce::jlimit (0, 127, midiNoteNumber + octaveOffset);
        const auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (transposedNote);
        phaseDelta = juce::MathConstants<double>::twoPi * cyclesPerSecond / getSampleRate();
        wave2PhaseDelta = phaseDelta;
        wave2PhaseDeltaA = phaseDelta * 0.9965;
        wave2PhaseDeltaB = phaseDelta * 1.0035;
        phase = 0.0;
        wave2Phase = 0.0;
        wave2PhaseA = 0.0;
        wave2PhaseB = 0.0;
        envelope = 1.0;
        releasing = false;
        // MIDI velocity remains intentionally ignored for original-behavior compatibility.
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
            releasing = true;
        else
        {
            clearCurrentNote();
            envelope = 0.0;
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        if (phaseDelta <= 0.0 || envelope <= 0.0)
            return;

        const auto strong = wave1Strong->load() >= 0.5f;
        const auto wave2IsEnabled = wave2Enabled->load() >= 0.5f;
        const auto shape = juce::jlimit (0, 11, juce::roundToInt (wave2Shape->load()));
        const auto detunedSawMix = juce::jlimit (0.0, 1.0,
                                                 static_cast<double> (wave2DetunedSawMix->load()));
        const auto nervous = characterNervous->load() >= 0.5f;
        const auto currentDecaySeconds = juce::jlimit (0.08, 5.0,
                                                       static_cast<double> (decaySeconds->load()));
        const auto currentReleaseSeconds = juce::jlimit (0.03, 8.0,
                                                         static_cast<double> (releaseSeconds->load()));
        const auto currentDecayMultiplier = std::pow (0.001,
                                                       1.0 / (currentDecaySeconds * getSampleRate()));
        const auto currentReleaseMultiplier = std::pow (0.001,
                                                         1.0 / (currentReleaseSeconds * getSampleRate()));

        while (--numSamples >= 0)
        {
            auto wave1 = std::sin (phase)
                       + 0.05 * std::sin (phase * 3.0);
            if (strong)
                wave1 += 0.24 * std::sin (phase * 2.0)
                       + 0.10 * std::sin (phase * 3.0);

            auto mixed = wave1 * 0.82;

            if (wave2IsEnabled)
            {
                mixed += renderWave2Shape (shape, wave2Phase);

                if (detunedSawMix > 0.0)
                {
                    const auto detunedSawA = renderSawWave (wave2PhaseA);
                    const auto detunedSawB = renderSawWave (wave2PhaseB);
                    mixed += 0.18 * detunedSawMix * 0.5 * (detunedSawA + detunedSawB);
                }
            }

            if (nervous)
                mixed = 2.75 * std::tanh (mixed * 5.0);
            else
                mixed *= 2.50;

            const auto sample = static_cast<float> (0.225 * envelope * mixed);

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample (channel, startSample, sample);

            advancePhase (phase, phaseDelta);
            advancePhase (wave2Phase, wave2PhaseDelta);
            advancePhase (wave2PhaseA, wave2PhaseDeltaA);
            advancePhase (wave2PhaseB, wave2PhaseDeltaB);

            envelope *= releasing ? currentReleaseMultiplier : currentDecayMultiplier;
            ++startSample;

            if (envelope < 0.00001)
            {
                clearCurrentNote();
                phaseDelta = 0.0;
                envelope = 0.0;
                break;
            }
        }
    }

private:
    static void advancePhase (double& currentPhase, double delta) noexcept
    {
        currentPhase += delta;
        if (currentPhase >= juce::MathConstants<double>::twoPi)
            currentPhase -= juce::MathConstants<double>::twoPi;
    }

    double renderWave2Shape (int shape, double currentPhase) noexcept
    {
        const auto position = static_cast<std::size_t> (juce::jlimit (0, 11, shape));
        const auto tablePosition = currentPhase
                                 * static_cast<double> (PikyWave2Tables::tableSize)
                                 / juce::MathConstants<double>::twoPi;
        const auto index = static_cast<std::size_t> (tablePosition)
                         % PikyWave2Tables::tableSize;
        const auto nextIndex = (index + 1) % PikyWave2Tables::tableSize;
        const auto fraction = tablePosition - std::floor (tablePosition);
        const auto& table = PikyWave2Tables::values[position];

        return table[index] + fraction * (table[nextIndex] - table[index]);
    }

    static double renderSawWave (double currentPhase) noexcept
    {
        return currentPhase / juce::MathConstants<double>::pi - 1.0;
    }

    std::atomic<float>* wave1Strong = nullptr;
    std::atomic<float>* wave2Enabled = nullptr;
    std::atomic<float>* wave2Shape = nullptr;
    std::atomic<float>* wave2DetunedSawMix = nullptr;
    std::atomic<float>* characterNervous = nullptr;
    std::atomic<float>* decaySeconds = nullptr;
    std::atomic<float>* releaseSeconds = nullptr;
    std::atomic<float>* octaveUp = nullptr;

    double phase = 0.0;
    double phaseDelta = 0.0;
    double wave2Phase = 0.0;
    double wave2PhaseDelta = 0.0;
    double wave2PhaseA = 0.0;
    double wave2PhaseB = 0.0;
    double wave2PhaseDeltaA = 0.0;
    double wave2PhaseDeltaB = 0.0;
    double envelope = 0.0;
    bool releasing = false;
};
}

juce::AudioProcessorValueTreeState::ParameterLayout
CSPiky64AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> layout;

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { wave1StrongId, 1 }, "Wave 1 Strong", true));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { wave2EnabledId, 1 }, "Wave 2 Enabled", true));

    layout.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { wave2ShapeId, 1 }, "Wave 2 Shape",
        juce::StringArray { "Position 01", "Position 02", "Position 03",
                            "Position 04", "Position 05", "Position 06",
                            "Position 07", "Position 08", "Position 09",
                            "Position 10", "Position 11", "Position 12" },
        0));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { wave2DetunedSawMixId, 1 }, "Wave 2 Detuned Saws",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("mix")));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { characterNervousId, 1 }, "Character Nervous", false));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { decaySecondsId, 1 }, "Decay",
        juce::NormalisableRange<float> { 0.08f, 5.0f, 0.001f, 0.79f }, 1.65f,
        juce::AudioParameterFloatAttributes().withLabel ("s")));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { releaseSecondsId, 1 }, "Release",
        juce::NormalisableRange<float> { 0.03f, 8.0f, 0.001f, 0.67f }, 0.20f,
        juce::AudioParameterFloatAttributes().withLabel ("s")));

    layout.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { octaveUpId, 1 }, "Octave +1", false));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { reverbMixId, 1 }, "Reverb Mix",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("mix")));

    auto outputVolumeRange = juce::NormalisableRange<float> { -60.0f, 6.0f, 0.1f };
    outputVolumeRange.setSkewForCentre (0.0f);

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { outputVolumeDbId, 1 }, "Output Volume",
        outputVolumeRange, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { filterPositionId, 1 }, "Filter Position",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 1.0f,
        juce::AudioParameterFloatAttributes().withLabel ("position")));

    layout.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { scopeColourId, 1 }, "Scope Color",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f }, 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("position")));

    return { layout.begin(), layout.end() };
}

CSPiky64AudioProcessor::CSPiky64AudioProcessor()
    : AudioProcessor (BusesProperties()
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    auto* wave1StrongParameter = parameters.getRawParameterValue (wave1StrongId);
    auto* wave2EnabledParameter = parameters.getRawParameterValue (wave2EnabledId);
    auto* wave2ShapeParameter = parameters.getRawParameterValue (wave2ShapeId);
    auto* wave2DetunedSawMixParameter = parameters.getRawParameterValue (wave2DetunedSawMixId);
    auto* characterNervousParameter = parameters.getRawParameterValue (characterNervousId);
    auto* decaySecondsParameter = parameters.getRawParameterValue (decaySecondsId);
    auto* releaseSecondsParameter = parameters.getRawParameterValue (releaseSecondsId);
    auto* octaveUpParameter = parameters.getRawParameterValue (octaveUpId);
    reverbMixParameter = parameters.getRawParameterValue (reverbMixId);
    outputVolumeDbParameter = parameters.getRawParameterValue (outputVolumeDbId);
    filterPositionParameter = parameters.getRawParameterValue (filterPositionId);
    jassert (reverbMixParameter != nullptr);
    jassert (outputVolumeDbParameter != nullptr);
    jassert (filterPositionParameter != nullptr);

    for (auto& sample : scopeSamples)
        sample.store (0.0f, std::memory_order_relaxed);

    for (int voice = 0; voice < 12; ++voice)
        synthesiser.addVoice (new PikyVoice (wave1StrongParameter,
                                             wave2EnabledParameter,
                                             wave2ShapeParameter,
                                             wave2DetunedSawMixParameter,
                                             characterNervousParameter,
                                             decaySecondsParameter,
                                             releaseSecondsParameter,
                                             octaveUpParameter));

    synthesiser.addSound (new PikySound());
}

const juce::String CSPiky64AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CSPiky64AudioProcessor::acceptsMidi() const       { return true; }
bool CSPiky64AudioProcessor::producesMidi() const      { return false; }
bool CSPiky64AudioProcessor::isMidiEffect() const      { return false; }
double CSPiky64AudioProcessor::getTailLengthSeconds() const { return 14.0; }

int CSPiky64AudioProcessor::getNumPrograms()           { return 1; }
int CSPiky64AudioProcessor::getCurrentProgram()        { return 0; }
void CSPiky64AudioProcessor::setCurrentProgram (int)   {}
const juce::String CSPiky64AudioProcessor::getProgramName (int) { return "Initial Pluck"; }
void CSPiky64AudioProcessor::changeProgramName (int, const juce::String&) {}

void CSPiky64AudioProcessor::prepareToPlay (double sampleRate, int)
{
    processingSampleRate = sampleRate;
    synthesiser.setCurrentPlaybackSampleRate (sampleRate);
    reverb.setSampleRate (sampleRate);
    reverb.reset();
    for (auto& sample : scopeSamples)
        sample.store (0.0f, std::memory_order_relaxed);
    scopeWritePosition.store (0, std::memory_order_release);
    for (auto& channelState : filterState)
        channelState.fill (0.0f);
    filterPosition.reset (sampleRate, 0.02);
    filterPosition.setCurrentAndTargetValue (
        filterPositionParameter != nullptr ? filterPositionParameter->load() : 1.0f);
    outputGain.reset (sampleRate, 0.02);
    outputGain.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (
        outputVolumeDbParameter != nullptr ? outputVolumeDbParameter->load() : 0.0f));
}

void CSPiky64AudioProcessor::releaseResources()
{
    reverb.reset();
    for (auto& sample : scopeSamples)
        sample.store (0.0f, std::memory_order_relaxed);
    scopeWritePosition.store (0, std::memory_order_release);
    for (auto& channelState : filterState)
        channelState.fill (0.0f);
}

bool CSPiky64AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono()
        || output == juce::AudioChannelSet::stereo();
}

void CSPiky64AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    synthesiser.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    const auto filterTarget = juce::jlimit (0.0f, 1.0f,
                                            filterPositionParameter != nullptr
                                                ? filterPositionParameter->load()
                                                : 1.0f);
    filterPosition.setTargetValue (filterTarget);

    for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex)
    {
        const auto position = filterPosition.getNextValue();

        // The measured active region is deliberately stretched to 98% for much
        // finer adjustment. Track the dry sample while bypassed so re-entering
        // the filtered region cannot reveal stale state or cause a discontinuity.
        if (position >= filterBypassStart)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                const auto input = buffer.getSample (channel, sampleIndex);
                filterState[static_cast<std::size_t> (channel)].fill (input);
            }

            continue;
        }

        const auto cutoff = getMeasuredFilterCutoff (position);
        const auto pole = static_cast<float> (
            std::exp (-juce::MathConstants<double>::twoPi
                      * cutoff / processingSampleRate));

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto filtered = buffer.getSample (channel, sampleIndex);
            auto& channelState = filterState[static_cast<std::size_t> (channel)];

            for (auto& stageState : channelState)
            {
                stageState = filtered + pole * (stageState - filtered);
                filtered = stageState;
            }

            buffer.setSample (channel, sampleIndex, filtered);
        }
    }

    const auto mix = juce::jlimit (0.0f, 1.0f,
                                   reverbMixParameter != nullptr
                                       ? reverbMixParameter->load()
                                       : 0.0f);

    const auto wetGain = mix * mix;

    juce::Reverb::Parameters reverbParameters;
    reverbParameters.roomSize = 1.0f;
    reverbParameters.damping = 0.50f;
    reverbParameters.wetLevel = 0.16f * wetGain;
    reverbParameters.dryLevel = 0.5f;
    reverbParameters.width = 1.0f;
    reverbParameters.freezeMode = 0.0f;
    reverb.setParameters (reverbParameters);

    if (buffer.getNumChannels() == 1)
        reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());
    else if (buffer.getNumChannels() >= 2)
        reverb.processStereo (buffer.getWritePointer (0),
                              buffer.getWritePointer (1),
                              buffer.getNumSamples());

    if (buffer.getNumSamples() > 0)
    {
        const auto outputDb = outputVolumeDbParameter != nullptr
                                  ? outputVolumeDbParameter->load()
                                  : 0.0f;
        outputGain.setTargetValue (juce::Decibels::decibelsToGain (outputDb));

        const auto startGain = outputGain.getCurrentValue();
        const auto endGain = outputGain.skip (buffer.getNumSamples());
        buffer.applyGainRamp (0, buffer.getNumSamples(), startGain, endGain);
    }

    pushScopeSamples (buffer);
}

void CSPiky64AudioProcessor::pushScopeSamples (const juce::AudioBuffer<float>& buffer) noexcept
{
    if (buffer.getNumChannels() <= 0 || buffer.getNumSamples() <= 0)
        return;

    auto writePosition = scopeWritePosition.load (std::memory_order_relaxed);

    for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex)
    {
        auto sample = buffer.getSample (0, sampleIndex);
        if (buffer.getNumChannels() > 1)
            sample = 0.5f * (sample + buffer.getSample (1, sampleIndex));

        scopeSamples[writePosition].store (sample, std::memory_order_relaxed);
        if (++writePosition == scopeSampleCount)
            writePosition = 0;
    }

    scopeWritePosition.store (writePosition, std::memory_order_release);
}

void CSPiky64AudioProcessor::copyScopeSamples (
    std::array<float, scopeSampleCount>& destination) const noexcept
{
    auto readPosition = scopeWritePosition.load (std::memory_order_acquire);

    for (auto& sample : destination)
    {
        sample = scopeSamples[readPosition].load (std::memory_order_relaxed);
        if (++readPosition == scopeSampleCount)
            readPosition = 0;
    }
}

bool CSPiky64AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CSPiky64AudioProcessor::createEditor()
{
    return new CSPiky64AudioProcessorEditor (*this);
}

void CSPiky64AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto stateXml = parameters.copyState().createXml())
        copyXmlToBinary (*stateXml, destData);
}

void CSPiky64AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto stateXml = getXmlFromBinary (data, sizeInBytes))
        if (stateXml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*stateXml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CSPiky64AudioProcessor();
}
