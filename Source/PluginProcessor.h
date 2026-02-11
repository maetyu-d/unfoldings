#pragma once

#include <JuceHeader.h>

class MicrosoundSymphonyAudioProcessor : public juce::AudioProcessor
{
public:
    MicrosoundSymphonyAudioProcessor();
    ~MicrosoundSymphonyAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "unfoldings"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void renderNow();
    void applyBeautyScene();
    void applyPreset (int presetIndex);
    static juce::StringArray getPresetNames();
    static int getPresetMode (int presetIndex);
    bool exportLastRenderToWav (const juce::File& file) const;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioBuffer<float> renderMicroBurst (double microRate, double burstMs, int density) const;
    juce::AudioBuffer<float> unfoldGranular (const juce::AudioBuffer<float>& micro,
                                             double microRate,
                                             double outRate,
                                             double outSeconds,
                                             float grainOutMs,
                                             float overlap,
                                             int seed) const;

    juce::AudioBuffer<float> unfoldSpectral (const juce::AudioBuffer<float>& micro,
                                             double microRate,
                                             double outRate,
                                             double outSeconds,
                                             float stretch,
                                             float spectralWarp,
                                             float spectralChaos,
                                             int seed) const;
    juce::AudioBuffer<float> unfoldMorphogen (const juce::AudioBuffer<float>& micro,
                                              double microRate,
                                              double outRate,
                                              double outSeconds,
                                              float stretch,
                                              float spectralWarp,
                                              float spectralChaos,
                                              float hybridMix,
                                              int seed) const;
    juce::AudioBuffer<float> unfoldFennesz (const juce::AudioBuffer<float>& micro,
                                            double microRate,
                                            double outRate,
                                            double outSeconds,
                                            float stretch,
                                            float spectralWarp,
                                            float spectralChaos,
                                            float hybridMix,
                                            int seed) const;
    juce::AudioBuffer<float> unfoldNoto (const juce::AudioBuffer<float>& micro,
                                         double microRate,
                                         double outRate,
                                         double outSeconds,
                                         float stretch,
                                         float spectralWarp,
                                         float spectralChaos,
                                         int seed) const;
    juce::AudioBuffer<float> unfoldIkeda (const juce::AudioBuffer<float>& micro,
                                          double microRate,
                                          double outRate,
                                          double outSeconds,
                                          float stretch,
                                          float spectralWarp,
                                          float spectralChaos,
                                          int seed) const;

    static juce::AudioBuffer<float> toMono (const juce::AudioBuffer<float>& in);
    void setParameterValue (const juce::String& paramID, float plainValue);
    static float princArg (float x);
    static void sanitizeBufferInPlace (juce::AudioBuffer<float>& b);
    static void applyBloomInPlace (juce::AudioBuffer<float>& b, double sampleRate, int seed, float amount);
    static void normalizeInPlace (juce::AudioBuffer<float>& b, float peakTarget = 0.95f);

    juce::CriticalSection renderedLock;
    juce::AudioBuffer<float> renderedBuffer;

    double hostSampleRate = 44100.0;
    int playbackCursor = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrosoundSymphonyAudioProcessor)
};
