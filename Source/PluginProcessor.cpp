#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <array>
#include <cmath>
#include <complex>

namespace
{
constexpr float twoPi = juce::MathConstants<float>::twoPi;

int popcount32 (uint32_t v)
{
    int c = 0;
    while (v != 0u)
    {
        v &= (v - 1u);
        ++c;
    }
    return c;
}

struct PresetSpec
{
    const char* name;
    int mode;
    int microRateChoice;
    float burstMs;
    int density;
    float outSeconds;
    float grainMs;
    float overlap;
    float stretch;
    float warp;
    float spectralChaos;
    float hybridMix;
    int seed;
    int beautyScene;
};

const std::array<PresetSpec, 51>& getPresetBank()
{
    static const std::array<PresetSpec, 51> bank {{
        { "Porcelain Air", 1, 0, 6.5f, 2800, 3.6f, 8.0f, 1.9f, 14.0f, 5.6f, 0.08f, 0.04f, 1101, 0 },
        { "Tape Halo", 4, 1, 26.0f, 7400, 9.0f, 62.0f, 7.4f, 22.0f, 1.2f, 0.28f, 0.64f, 2202, 1 },
        { "Star Fracture", 3, 3, 9.0f, 7000, 5.6f, 12.0f, 2.4f, 74.0f, 5.6f, 0.98f, 0.08f, 3303, 2 },
        { "Coal Cathedral", 2, 2, 62.0f, 24000, 18.0f, 150.0f, 15.8f, 52.0f, 1.4f, 0.88f, 0.93f, 4404, 3 },
        { "Needle Rain", 0, 3, 16.0f, 5400, 7.2f, 22.0f, 4.2f, 14.0f, 2.5f, 0.36f, 0.30f, 5505, 0 },
        { "Salt Bells", 1, 2, 8.4f, 3900, 4.4f, 9.0f, 2.1f, 64.0f, 5.8f, 0.86f, 0.03f, 6606, 2 },
        { "Moss Choir", 4, 1, 44.0f, 11800, 13.5f, 102.0f, 12.4f, 30.0f, 1.6f, 0.54f, 0.82f, 7707, 1 },
        { "Rust Sparks", 3, 1, 26.0f, 18600, 9.8f, 26.0f, 6.8f, 18.0f, 1.4f, 0.67f, 0.62f, 8808, 0 },
        { "Frozen Organ", 2, 3, 36.0f, 15000, 12.0f, 66.0f, 8.9f, 40.0f, 2.9f, 0.73f, 0.69f, 9909, 3 },
        { "Paper Wind", 0, 0, 30.0f, 4100, 10.0f, 86.0f, 10.5f, 10.0f, 1.0f, 0.18f, 0.42f, 1110, 0 },
        { "Chrome Bloom", 1, 2, 14.0f, 8200, 7.5f, 16.0f, 3.3f, 50.0f, 3.9f, 0.67f, 0.11f, 1212, 2 },
        { "Velour Sea", 2, 2, 52.0f, 10400, 15.2f, 128.0f, 14.0f, 24.0f, 1.1f, 0.33f, 0.89f, 1313, 1 },
        { "Glitch Psalms", 3, 3, 31.0f, 28000, 12.2f, 34.0f, 4.2f, 92.0f, 3.0f, 1.00f, 0.12f, 1414, 3 },
        { "Ivory Mist", 4, 1, 18.0f, 5800, 8.8f, 20.0f, 3.8f, 28.0f, 2.1f, 0.31f, 0.18f, 1515, 0 },
        { "Tin Jungle", 0, 2, 28.0f, 17000, 11.0f, 40.0f, 6.0f, 12.0f, 2.2f, 0.62f, 0.38f, 1616, 0 },
        { "Night Plume", 2, 1, 58.0f, 19600, 17.0f, 140.0f, 15.4f, 36.0f, 1.9f, 0.77f, 0.87f, 1717, 3 },
        { "Opal Threads", 4, 2, 14.0f, 6200, 10.8f, 30.0f, 4.4f, 36.0f, 2.8f, 0.55f, 0.24f, 1818, 2 },
        { "Sunken Choir", 4, 0, 48.0f, 9000, 14.6f, 116.0f, 13.2f, 20.0f, 1.3f, 0.42f, 0.84f, 1919, 1 },
        { "Quartz Static", 3, 3, 6.0f, 3000, 4.2f, 10.0f, 1.8f, 54.0f, 6.0f, 0.90f, 0.02f, 2020, 0 },
        { "Monolith Flood", 2, 2, 64.0f, 28000, 19.5f, 160.0f, 16.0f, 56.0f, 2.2f, 1.00f, 0.95f, 2121, 3 },

        { "Glass Drizzle", 1, 0, 5.4f, 1800, 2.1f, 8.0f, 1.6f, 32.0f, 5.9f, 0.22f, 0.03f, 2301, 2 },
        { "Abyss Lantern", 4, 3, 72.0f, 25000, 20.0f, 174.0f, 16.0f, 58.0f, 0.9f, 0.71f, 0.91f, 2402, 3 },
        { "Helix Dust", 3, 0, 11.0f, 6200, 6.9f, 58.0f, 13.0f, 12.0f, 0.7f, 0.41f, 0.93f, 2503, 0 },
        { "Ion Choir", 2, 3, 41.0f, 9000, 16.3f, 132.0f, 15.0f, 18.0f, 1.0f, 0.26f, 0.98f, 2604, 1 },
        { "Broken Neutrino", 1, 3, 7.2f, 24000, 5.0f, 8.5f, 1.7f, 96.0f, 4.9f, 0.97f, 0.01f, 2705, 0 },
        { "Pollen Array", 0, 1, 22.0f, 9500, 8.2f, 26.0f, 3.3f, 6.0f, 3.1f, 0.52f, 0.21f, 2806, 0 },
        { "Silt Harmonics", 4, 2, 31.0f, 6800, 12.7f, 92.0f, 9.9f, 44.0f, 2.8f, 0.63f, 0.27f, 2907, 2 },
        { "Razor Kelp", 3, 1, 14.0f, 21000, 7.1f, 16.0f, 2.1f, 84.0f, 5.7f, 0.99f, 0.06f, 3008, 3 },
        { "Cathedral Frost", 2, 0, 80.0f, 5000, 19.0f, 180.0f, 16.0f, 8.0f, 0.6f, 0.11f, 0.99f, 3109, 1 },
        { "Grit Orbit", 0, 3, 13.0f, 26000, 6.5f, 14.0f, 2.0f, 4.0f, 4.7f, 0.83f, 0.12f, 3210, 0 },

        { "Coastal Melt", 5, 1, 46.0f, 9000, 14.0f, 118.0f, 12.8f, 24.0f, 1.4f, 0.32f, 0.82f, 3301, 1 },
        { "Burning Tape Sky", 5, 2, 58.0f, 13000, 17.2f, 146.0f, 14.6f, 30.0f, 1.1f, 0.58f, 0.91f, 3302, 3 },
        { "Soft Rust Choir", 5, 0, 34.0f, 7000, 11.8f, 96.0f, 10.2f, 18.0f, 1.7f, 0.24f, 0.73f, 3303, 1 },
        { "Solar Foam", 5, 3, 22.0f, 5400, 9.6f, 68.0f, 7.0f, 42.0f, 2.2f, 0.49f, 0.64f, 3304, 2 },
        { "Velvet Collapse", 5, 2, 64.0f, 18000, 20.0f, 172.0f, 16.0f, 54.0f, 0.9f, 0.77f, 0.95f, 3305, 3 },

        { "Pulse Lattice A", 6, 3, 8.0f, 12000, 6.0f, 12.0f, 2.2f, 48.0f, 4.0f, 0.68f, 0.08f, 3401, 0 },
        { "Pulse Lattice B", 6, 1, 10.0f, 9000, 7.4f, 16.0f, 3.0f, 22.0f, 2.0f, 0.42f, 0.06f, 3402, 0 },
        { "Sparse Click Field", 6, 0, 6.2f, 3500, 5.2f, 8.0f, 1.6f, 14.0f, 5.2f, 0.21f, 0.03f, 3403, 0 },
        { "Grid Failure", 6, 2, 12.0f, 20000, 8.8f, 20.0f, 3.4f, 70.0f, 3.6f, 0.92f, 0.15f, 3404, 3 },
        { "Binary Rain", 6, 3, 16.0f, 26000, 10.2f, 24.0f, 3.8f, 84.0f, 2.8f, 1.00f, 0.18f, 3405, 3 },

        { "Data Monolith", 7, 3, 7.0f, 5000, 6.6f, 10.0f, 2.0f, 40.0f, 5.6f, 0.84f, 0.02f, 3501, 0 },
        { "White Vector", 7, 2, 5.8f, 2800, 4.1f, 8.0f, 1.4f, 18.0f, 6.0f, 0.16f, 0.01f, 3502, 0 },
        { "Quantized Beam", 7, 1, 9.4f, 4600, 5.8f, 10.0f, 1.8f, 28.0f, 4.9f, 0.33f, 0.02f, 3503, 2 },
        { "Telemetry Storm", 7, 3, 14.0f, 14000, 9.4f, 18.0f, 2.9f, 76.0f, 3.1f, 0.96f, 0.05f, 3504, 3 },
        { "Atomic Corridor", 7, 0, 11.0f, 8000, 8.0f, 14.0f, 2.2f, 52.0f, 3.8f, 0.59f, 0.03f, 3505, 2 },

        { "Diskont Crackle Grid", 6, 2, 12.5f, 22000, 9.0f, 16.0f, 2.6f, 78.0f, 3.8f, 0.94f, 0.09f, 3601, 3 },
        { "Diskont Skipping Plate", 3, 1, 18.0f, 17000, 10.5f, 22.0f, 3.2f, 66.0f, 4.6f, 0.97f, 0.12f, 3602, 3 },
        { "Diskont Data Erosion", 7, 3, 9.0f, 12000, 8.4f, 14.0f, 2.4f, 58.0f, 4.9f, 0.88f, 0.04f, 3603, 0 },

        { "Ovalcommers Soft Mosaic", 2, 2, 29.0f, 11200, 13.8f, 76.0f, 9.6f, 34.0f, 2.7f, 0.56f, 0.72f, 3611, 2 },
        { "Ovalcommers Bloom Skip", 5, 2, 32.0f, 9400, 13.8f, 62.0f, 8.2f, 28.0f, 1.9f, 0.41f, 0.66f, 3612, 1 },
        { "Ovalcommers Neon Drift", 2, 1, 24.0f, 8600, 11.6f, 56.0f, 7.0f, 22.0f, 2.2f, 0.52f, 0.72f, 3613, 1 }
    }};
    return bank;
}
}

MicrosoundSymphonyAudioProcessor::MicrosoundSymphonyAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "MicrosoundSymphony", createParameterLayout())
{
}

void MicrosoundSymphonyAudioProcessor::prepareToPlay (double sampleRate, int)
{
    hostSampleRate = sampleRate;
    playbackCursor = 0;
}

void MicrosoundSymphonyAudioProcessor::releaseResources()
{
}

bool MicrosoundSymphonyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void MicrosoundSymphonyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const auto loop = apvts.getRawParameterValue ("loop")->load() > 0.5f;

    const juce::ScopedLock sl (renderedLock);

    if (renderedBuffer.getNumSamples() <= 0)
        return;

    const auto numOutChannels = buffer.getNumChannels();
    const auto numRenderChannels = renderedBuffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    const auto renderLen = renderedBuffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (playbackCursor >= renderLen)
        {
            if (! loop)
                break;

            playbackCursor = 0;
        }

        for (int ch = 0; ch < numOutChannels; ++ch)
        {
            const auto srcCh = juce::jmin (ch, numRenderChannels - 1);
            buffer.setSample (ch, sample, renderedBuffer.getSample (srcCh, playbackCursor));
        }

        ++playbackCursor;
    }
}

juce::AudioProcessorEditor* MicrosoundSymphonyAudioProcessor::createEditor()
{
    return new MicrosoundSymphonyAudioProcessorEditor (*this);
}

void MicrosoundSymphonyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void MicrosoundSymphonyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout MicrosoundSymphonyAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "mode", "Mode", juce::StringArray { "Granular", "Spectral", "Hybrid", "Xeno", "Morphogen", "Fennesz", "Noto", "Ikeda" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "beautyScene", "Beauty Scene", juce::StringArray { "Off", "Lush", "Crystalline", "Dramatic" }, 1));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "burstMs", "Burst (ms)", juce::NormalisableRange<float> (5.0f, 80.0f, 0.1f, 0.35f), 24.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "microRate", "Micro Rate",
        juce::StringArray { "192000", "384000", "768000", "1536000" }, 2));

    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "density", "Event Density", 500, 30000, 6000));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "outSeconds", "Output Seconds", juce::NormalisableRange<float> (1.0f, 20.0f, 0.01f, 0.4f), 6.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "grainMs", "Grain Out (ms)", juce::NormalisableRange<float> (8.0f, 180.0f, 0.1f, 0.4f), 42.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "overlap", "Granular Overlap", juce::NormalisableRange<float> (1.5f, 16.0f, 0.01f, 0.4f), 6.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "stretch", "Spectral Stretch", juce::NormalisableRange<float> (2.0f, 100.0f, 0.01f, 0.35f), 18.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "warp", "Spectral Warp", juce::NormalisableRange<float> (0.5f, 6.0f, 0.001f, 0.4f), 1.7f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "spectralChaos", "Spectral Chaos", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.45f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "hybridMix", "Hybrid Diffusion Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "loop", "Loop", true));

    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "seed", "Seed", 1, 999999, 12345));

    return { params.begin(), params.end() };
}

void MicrosoundSymphonyAudioProcessor::setParameterValue (const juce::String& paramID, float plainValue)
{
    if (auto* p = apvts.getParameter (paramID))
        p->setValueNotifyingHost (p->convertTo0to1 (plainValue));
}

void MicrosoundSymphonyAudioProcessor::applyBeautyScene()
{
    const int scene = (int) apvts.getRawParameterValue ("beautyScene")->load();

    if (scene == 1) // Lush
    {
        setParameterValue ("mode", 2.0f);
        setParameterValue ("burstMs", 38.0f);
        setParameterValue ("density", 12000.0f);
        setParameterValue ("outSeconds", 11.0f);
        setParameterValue ("grainMs", 78.0f);
        setParameterValue ("overlap", 10.0f);
        setParameterValue ("stretch", 28.0f);
        setParameterValue ("warp", 1.4f);
        setParameterValue ("spectralChaos", 0.62f);
        setParameterValue ("hybridMix", 0.72f);
    }
    else if (scene == 2) // Crystalline
    {
        setParameterValue ("mode", 1.0f);
        setParameterValue ("burstMs", 18.0f);
        setParameterValue ("density", 9000.0f);
        setParameterValue ("outSeconds", 8.0f);
        setParameterValue ("grainMs", 24.0f);
        setParameterValue ("overlap", 5.0f);
        setParameterValue ("stretch", 34.0f);
        setParameterValue ("warp", 2.9f);
        setParameterValue ("spectralChaos", 0.78f);
        setParameterValue ("hybridMix", 0.25f);
    }
    else if (scene == 3) // Dramatic
    {
        setParameterValue ("mode", 2.0f);
        setParameterValue ("burstMs", 52.0f);
        setParameterValue ("density", 18000.0f);
        setParameterValue ("outSeconds", 14.0f);
        setParameterValue ("grainMs", 95.0f);
        setParameterValue ("overlap", 12.5f);
        setParameterValue ("stretch", 42.0f);
        setParameterValue ("warp", 1.9f);
        setParameterValue ("spectralChaos", 0.93f);
        setParameterValue ("hybridMix", 0.84f);
    }
}

juce::StringArray MicrosoundSymphonyAudioProcessor::getPresetNames()
{
    juce::StringArray names;
    for (const auto& p : getPresetBank())
        names.add (p.name);
    return names;
}

int MicrosoundSymphonyAudioProcessor::getPresetMode (int presetIndex)
{
    const auto& bank = getPresetBank();
    if (presetIndex < 0 || presetIndex >= (int) bank.size())
        return 0;
    return bank[(size_t) presetIndex].mode;
}

void MicrosoundSymphonyAudioProcessor::applyPreset (int presetIndex)
{
    const auto& bank = getPresetBank();
    if (presetIndex < 0 || presetIndex >= (int) bank.size())
        return;

    const auto& p = bank[(size_t) presetIndex];
    setParameterValue ("mode", (float) p.mode);
    setParameterValue ("microRate", (float) p.microRateChoice);
    setParameterValue ("burstMs", p.burstMs);
    setParameterValue ("density", (float) p.density);
    setParameterValue ("outSeconds", p.outSeconds);
    setParameterValue ("grainMs", p.grainMs);
    setParameterValue ("overlap", p.overlap);
    setParameterValue ("stretch", p.stretch);
    setParameterValue ("warp", p.warp);
    setParameterValue ("spectralChaos", p.spectralChaos);
    setParameterValue ("hybridMix", p.hybridMix);
    setParameterValue ("seed", (float) p.seed);
    setParameterValue ("beautyScene", (float) p.beautyScene);
}

void MicrosoundSymphonyAudioProcessor::renderNow()
{
    const auto burstMs = apvts.getRawParameterValue ("burstMs")->load();
    const auto outSeconds = apvts.getRawParameterValue ("outSeconds")->load();
    const auto mode = (int) apvts.getRawParameterValue ("mode")->load();
    const auto density = (int) apvts.getRawParameterValue ("density")->load();
    const auto grainMs = apvts.getRawParameterValue ("grainMs")->load();
    const auto overlap = apvts.getRawParameterValue ("overlap")->load();
    const auto stretch = apvts.getRawParameterValue ("stretch")->load();
    const auto warp = apvts.getRawParameterValue ("warp")->load();
    const auto spectralChaos = apvts.getRawParameterValue ("spectralChaos")->load();
    const auto hybridMix = apvts.getRawParameterValue ("hybridMix")->load();
    const auto seed = (int) apvts.getRawParameterValue ("seed")->load();

    const int microRateChoice = (int) apvts.getRawParameterValue ("microRate")->load();
    const double microRate =
        microRateChoice == 0 ? 192000.0 :
        microRateChoice == 1 ? 384000.0 :
        microRateChoice == 2 ? 768000.0 :
        1536000.0;

    auto micro = renderMicroBurst (microRate, burstMs, density);

    juce::AudioBuffer<float> out;
    if (mode == 0)
    {
        out = unfoldGranular (micro, microRate, hostSampleRate, outSeconds, grainMs, overlap, seed);
    }
    else if (mode == 1)
    {
        out = unfoldSpectral (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, seed);
    }
    else if (mode == 2)
    {
        auto spectral = unfoldSpectral (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, seed);

        auto diffused = unfoldGranular (spectral,
                                        hostSampleRate,
                                        hostSampleRate,
                                        outSeconds,
                                        juce::jlimit (10.0f, 220.0f, grainMs * 1.35f),
                                        juce::jlimit (2.0f, 20.0f, overlap + 1.5f),
                                        seed + 9176);

        out.makeCopyOf (spectral);
        const float dryGain = juce::jmap (hybridMix, 1.0f, 0.55f);
        const float wetGain = juce::jmap (hybridMix, 0.0f, 0.85f);
        out.applyGain (dryGain);
        out.addFrom (0, 0, diffused, 0, 0, juce::jmin (out.getNumSamples(), diffused.getNumSamples()), wetGain);
        out.addFrom (1, 0, diffused, 1, 0, juce::jmin (out.getNumSamples(), diffused.getNumSamples()), wetGain);
    }
    else if (mode == 3)
    {
        const float chaos = juce::jlimit (0.0f, 1.0f, spectralChaos);
        const int xenoFlavor = ((seed * 3) + (int) std::round (warp * 17.0f) + (int) std::round (stretch * 3.0f) + (int) std::round (hybridMix * 100.0f)) & 3;
        auto spectral = unfoldSpectral (micro,
                                        microRate,
                                        hostSampleRate,
                                        outSeconds,
                                        juce::jlimit (8.0f, 120.0f, stretch * (1.1f + 0.7f * chaos)),
                                        juce::jlimit (0.7f, 6.5f, warp * (0.9f + 0.6f * chaos)),
                                        juce::jlimit (0.35f, 1.0f, spectralChaos + 0.22f),
                                        seed + 404);

        auto granular = unfoldGranular (micro,
                                        microRate,
                                        hostSampleRate,
                                        outSeconds,
                                        juce::jlimit (10.0f, 200.0f, grainMs * (0.9f + 1.1f * chaos)),
                                        juce::jlimit (1.5f, 20.0f, overlap * (0.8f + 1.0f * chaos)),
                                        seed + 9091);

        out.setSize (2, juce::jmax (spectral.getNumSamples(), granular.getNumSamples()));
        out.clear();

        const int outSamples = out.getNumSamples();
        juce::Random rng (seed + 31337);
        float hx = 0.11f + 0.03f * rng.nextFloat();
        float hy = -0.17f + 0.03f * rng.nextFloat();
        const uint32_t seedU = (uint32_t) seed;
        uint32_t ca = seedU * 747796405u + 2891336453u;
        const int maskBits = 6 + (int) std::round (18.0f * chaos);
        const uint32_t mask = maskBits >= 31 ? 0x7fffffffu : ((1u << maskBits) - 1u);
        const float spectralBlend = juce::jlimit (0.15f, 0.8f, 0.18f + 0.62f * (1.0f - hybridMix) + 0.07f * (float) xenoFlavor);
        const float granularBlend = juce::jlimit (0.10f, 0.8f, 0.15f + 0.55f * hybridMix + 0.06f * (float) (3 - xenoFlavor));
        const float xenoBlend = juce::jlimit (0.35f, 1.1f, 0.45f + 0.45f * chaos + 0.08f * (float) xenoFlavor);

        for (int i = 0; i < outSamples; ++i)
        {
            const float u = (float) i / (float) juce::jmax (1, outSamples - 1);

            if ((i & 7) == 0)
            {
                const uint32_t left = (ca << (1 + (xenoFlavor & 1))) | (ca >> (31 - (xenoFlavor & 1)));
                const uint32_t right = (ca >> (1 + ((xenoFlavor >> 1) & 1))) | (ca << (31 - ((xenoFlavor >> 1) & 1)));
                if (xenoFlavor == 0)
                    ca = left ^ (ca | right);
                else if (xenoFlavor == 1)
                    ca = (left & ~right) ^ (ca >> 3);
                else if (xenoFlavor == 2)
                    ca = (left + right) ^ (ca << 5);
                else
                    ca = (left ^ right) + (ca * 1664525u + 1013904223u);
            }

            const float gate = (float) popcount32 (ca & mask) / (float) juce::jmax (1, maskBits);
            const float a = (xenoFlavor == 2 ? 1.34f : 1.22f) + (0.42f + 0.26f * chaos) * std::sin (twoPi * ((0.21f + 0.11f * xenoFlavor) * u) + (2.3f + 1.7f * xenoFlavor) * gate);
            const float b = (xenoFlavor == 3 ? 0.22f : 0.15f) + (0.16f + 0.09f * chaos) * std::sin (twoPi * ((0.43f + 0.08f * xenoFlavor) * u) + (5.1f - 0.9f * xenoFlavor) * gate);
            const float px = hx;
            hx = 1.0f - a * hx * hx + hy;
            hy = b * px;
            if (! std::isfinite (hx) || ! std::isfinite (hy))
            {
                hx = 0.09f;
                hy = -0.13f;
            }
            hx = juce::jlimit (-2.0f, 2.0f, hx);
            hy = juce::jlimit (-2.0f, 2.0f, hy);

            const float noise = (rng.nextFloat() * 2.0f - 1.0f) * (0.08f + 0.28f * chaos);
            float xenoL = 0.0f;
            float xenoR = 0.0f;
            if (xenoFlavor == 0)
            {
                xenoL = std::tanh ((hx * (1.1f + 1.8f * chaos) + noise) * (0.6f + 1.1f * gate));
                xenoR = std::tanh ((hy * (1.3f + 1.6f * chaos) - noise) * (0.6f + 1.1f * (1.0f - gate)));
            }
            else if (xenoFlavor == 1)
            {
                xenoL = std::sin (twoPi * (0.15f * i + std::abs (hx) * (1.4f + 4.2f * chaos))) * std::tanh (hy * (1.2f + 1.3f * chaos) + noise);
                xenoR = std::sin (twoPi * (0.12f * i + std::abs (hy) * (1.7f + 3.8f * chaos))) * std::tanh (hx * (1.0f + 1.5f * chaos) - noise);
            }
            else if (xenoFlavor == 2)
            {
                const float q = std::tanh ((hx - hy) * (2.2f + 1.7f * chaos));
                xenoL = std::tanh ((hx + 0.6f * q + noise) * (1.0f + 1.9f * gate));
                xenoR = std::tanh ((hy - 0.6f * q - noise) * (1.0f + 1.9f * (1.0f - gate)));
            }
            else
            {
                const float c = std::cos (twoPi * (0.004f * i + gate * 3.0f));
                xenoL = std::tanh ((hx * c + hy * (1.0f - c) + noise) * (1.4f + 1.1f * chaos));
                xenoR = std::tanh ((hy * c - hx * (1.0f - c) - noise) * (1.4f + 1.1f * chaos));
            }

            const float sL = i < spectral.getNumSamples() ? spectral.getSample (0, i) : 0.0f;
            const float sR = i < spectral.getNumSamples() ? spectral.getSample (1, i) : 0.0f;
            const float gL = i < granular.getNumSamples() ? granular.getSample (0, i) : 0.0f;
            const float gR = i < granular.getNumSamples() ? granular.getSample (1, i) : 0.0f;

            out.setSample (0, i, spectralBlend * sL + granularBlend * gL + xenoBlend * xenoL);
            out.setSample (1, i, spectralBlend * sR + granularBlend * gR + xenoBlend * xenoR);
        }

        juce::AudioBuffer<float> folded;
        folded.makeCopyOf (out);
        const int segLen = juce::jmax (64, (int) std::round (hostSampleRate * ((0.010f + 0.020f * xenoFlavor) + (0.045f + 0.040f * xenoFlavor) * chaos)));
        const int hop = juce::jmax (16, segLen / (2 + (xenoFlavor % 3)));
        const int stride = 5 + ((seed + 3 * xenoFlavor) % 29);
        int cursor = (seed * 37) % juce::jmax (1, outSamples);

        for (int dst = 0; dst < outSamples; dst += hop)
        {
            cursor = (cursor + stride * segLen) % juce::jmax (1, outSamples);
            for (int n = 0; n < segLen; ++n)
            {
                const int di = dst + n;
                if (di >= outSamples)
                    break;

                const int si = (cursor + n) % juce::jmax (1, outSamples);
                const float w = 0.5f - 0.5f * std::cos (twoPi * (float) n / (float) juce::jmax (1, segLen - 1));
                out.addSample (0, di, folded.getSample (0, si) * w * (0.10f + 0.36f * chaos));
                out.addSample (1, di, folded.getSample (1, si) * w * (0.10f + 0.36f * chaos));
            }
        }

        // Symbolic phase-grammar transduction:
        // convert evolving bit/attractor states into a five-symbol rewrite process
        // that decides time reads, polarity, and non-linear fold strength.
        juce::AudioBuffer<float> grammarSrc;
        grammarSrc.makeCopyOf (out);
        const std::array<int, 8> primeHops { 2, 3, 5, 7, 11, 13, 17, 19 };
        uint32_t grammarState = seedU ^ 0x9e3779b9u;
        const int symbolCount = 5 + xenoFlavor;

        for (int i = 0; i < outSamples; ++i)
        {
            grammarState ^= grammarState << 13;
            grammarState ^= grammarState >> 17;
            grammarState ^= grammarState << 5;

            const uint32_t iU = (uint32_t) i;
            const int symbol = (int) ((grammarState ^ (iU * 2654435761u)) % (uint32_t) symbolCount);
            const int hopPrime = primeHops[(size_t) (grammarState & 7u)];
            const int back = (hopPrime * (17 + (7 + xenoFlavor) * symbol) + (i % (29 + (19 + 3 * xenoFlavor) * symbol))) % juce::jmax (1, outSamples);
            const int read = (i - back + outSamples) % juce::jmax (1, outSamples);

            const float srcL = grammarSrc.getSample (0, read);
            const float srcR = grammarSrc.getSample (1, read);
            const float srcMid = 0.5f * (srcL + srcR);
            const float polarity = ((symbol + xenoFlavor) & 1) ? -1.0f : 1.0f;
            const float fold = 0.9f + (2.6f + 0.7f * xenoFlavor) * chaos + 0.42f * (float) symbol;
            const float symBlend = (0.04f + (0.18f + 0.05f * xenoFlavor) * chaos)
                * (0.55f + 0.45f * (float) symbol / (float) juce::jmax (1, symbolCount - 1));

            const float injectL = std::tanh (fold * (srcL + 0.35f * srcMid)) * polarity;
            const float injectR = std::tanh (fold * (srcR - 0.35f * srcMid)) * (-polarity);

            out.addSample (0, i, injectL * symBlend);
            out.addSample (1, i, injectR * symBlend);
        }

        // Autocatalytic shadow resynthesis:
        // a self-referential moving-read process where local energy steers feedback gain.
        juce::AudioBuffer<float> shadow;
        shadow.makeCopyOf (out);
        const int baseD1 = juce::jmax (7, (int) std::round (hostSampleRate * ((0.001 + 0.0015 * xenoFlavor) + (0.007 + 0.002f * xenoFlavor) * chaos)));
        const int baseD2 = juce::jmax (11, (int) std::round (hostSampleRate * ((0.004 + 0.0020 * xenoFlavor) + (0.010 + 0.003f * xenoFlavor) * chaos)));
        const int baseD3 = juce::jmax (17, (int) std::round (hostSampleRate * ((0.008 + 0.0030 * xenoFlavor) + (0.014 + 0.004f * xenoFlavor) * chaos)));

        for (int i = 0; i < outSamples; ++i)
        {
            const float t = (float) i / (float) juce::jmax (1, outSamples - 1);
            const int d1 = baseD1 + (int) std::round ((0.5f + 0.5f * std::sin (twoPi * (7.1f * t))) * baseD2);
            const int d2 = baseD2 + (int) std::round ((0.5f + 0.5f * std::sin (twoPi * (13.7f * t + 0.3f))) * baseD3);
            const int d3 = baseD3 + (int) std::round ((0.5f + 0.5f * std::sin (twoPi * (3.9f * t + 1.1f))) * baseD1);

            const int r1 = (i - d1 + outSamples) % juce::jmax (1, outSamples);
            const int r2 = (i - d2 + outSamples) % juce::jmax (1, outSamples);
            const int r3 = (i - d3 + outSamples) % juce::jmax (1, outSamples);

            const float s1L = shadow.getSample (0, r1), s1R = shadow.getSample (1, r1);
            const float s2L = shadow.getSample (0, r2), s2R = shadow.getSample (1, r2);
            const float s3L = shadow.getSample (0, r3), s3R = shadow.getSample (1, r3);

            const float e = 0.5f * (std::abs (out.getSample (0, i)) + std::abs (out.getSample (1, i)));
            const float catalyst = juce::jlimit (0.0f, 1.0f, (0.15f + 0.85f * chaos) * (0.3f + 2.3f * e));
            const float mixA = 0.53f - 0.23f * catalyst;
            const float mixB = 0.31f + 0.17f * catalyst;
            const float mixC = 0.16f + 0.21f * catalyst;

            const float resynL = std::tanh ((mixA * s1L + mixB * s2R - mixC * s3L) * (1.0f + 2.0f * catalyst));
            const float resynR = std::tanh ((mixA * s1R + mixB * s2L - mixC * s3R) * (1.0f + 2.0f * catalyst));

            out.addSample (0, i, resynL * (0.05f + 0.27f * chaos));
            out.addSample (1, i, resynR * (0.05f + 0.27f * chaos));
        }
    }
    else if (mode == 4)
    {
        out = unfoldMorphogen (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, hybridMix, seed);
    }
    else if (mode == 5)
    {
        out = unfoldFennesz (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, hybridMix, seed);
    }
    else if (mode == 6)
    {
        out = unfoldNoto (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, seed);
    }
    else
    {
        out = unfoldIkeda (micro, microRate, hostSampleRate, outSeconds, stretch, warp, spectralChaos, seed);
    }

    sanitizeBufferInPlace (out);
    const float bloomAmount = juce::jlimit (0.15f, 1.0f, 0.35f + 0.35f * spectralChaos + (mode >= 2 ? 0.18f : 0.0f));
    applyBloomInPlace (out, hostSampleRate, seed + 11731, bloomAmount);
    sanitizeBufferInPlace (out);
    normalizeInPlace (out);

    const juce::ScopedLock sl (renderedLock);
    renderedBuffer = std::move (out);
    playbackCursor = 0;
}

bool MicrosoundSymphonyAudioProcessor::exportLastRenderToWav (const juce::File& file) const
{
    juce::AudioBuffer<float> copy;
    {
        const juce::ScopedLock sl (renderedLock);
        if (renderedBuffer.getNumSamples() <= 0)
            return false;

        copy.makeCopyOf (renderedBuffer);
    }

    if (file.existsAsFile())
        file.deleteFile();

    if (auto fileStream = file.createOutputStream())
    {
        std::unique_ptr<juce::OutputStream> stream (std::move (fileStream));
        juce::WavAudioFormat wav;
        auto options = juce::AudioFormatWriterOptions {}
            .withSampleRate (hostSampleRate)
            .withNumChannels (copy.getNumChannels())
            .withBitsPerSample (24)
            .withSampleFormat (juce::AudioFormatWriterOptions::SampleFormat::integral);

        if (auto writer = wav.createWriterFor (stream, options))
        {
            return writer->writeFromAudioSampleBuffer (copy, 0, copy.getNumSamples());
        }
    }

    return false;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::renderMicroBurst (double microRate,
                                                                              double burstMs,
                                                                              int density) const
{
    const int numSamples = juce::jmax (16, (int) std::round (microRate * burstMs * 0.001));
    juce::AudioBuffer<float> b (2, numSamples);
    b.clear();

    juce::Random rng ((int64) density * 1103515245 + numSamples);

    for (int i = 0; i < density; ++i)
    {
        const int start = rng.nextInt (numSamples);
        const int len = juce::jlimit (6, juce::jmax (8, (int) (0.0012 * microRate)), 6 + rng.nextInt ((int) (0.0038 * microRate) + 1));
        const float amp = std::pow (rng.nextFloat(), 2.1f) * 0.14f;
        const float pan = rng.nextFloat();
        const int eventType = rng.nextInt (4);

        const float f0 = 40.0f + std::pow (rng.nextFloat(), 2.0f) * (float) (0.47 * microRate);
        const float f1 = juce::jlimit (20.0f, (float) (0.49 * microRate), f0 * (0.35f + 3.7f * rng.nextFloat()));
        const float fmRate = 8.0f + 4200.0f * std::pow (rng.nextFloat(), 2.5f);
        const float fmDepth = 0.04f + 0.75f * rng.nextFloat();
        const float noiseBlend = std::pow (rng.nextFloat(), 1.4f);
        const int partialCount = 2 + rng.nextInt (5);

        for (int n = 0; n < len; ++n)
        {
            const int idx = start + n;
            if (idx >= numSamples)
                break;

            const float t = (float) n / (float) juce::jmax (1, len - 1);
            const float win = 0.5f - 0.5f * std::cos (twoPi * t);
            const float freq = juce::jmap (t, f0, f1);
            float s = 0.0f;

            if (eventType == 0)
            {
                const float ph = twoPi * freq * (float) idx / (float) microRate;
                s = std::sin (ph);
            }
            else if (eventType == 1)
            {
                const float mod = std::sin (twoPi * fmRate * (float) idx / (float) microRate);
                const float ph = twoPi * (freq * (1.0f + fmDepth * mod)) * (float) idx / (float) microRate;
                s = std::sin (ph);
            }
            else if (eventType == 2)
            {
                float sum = 0.0f;
                for (int p = 1; p <= partialCount; ++p)
                {
                    const float detune = 1.0f + (rng.nextFloat() * 2.0f - 1.0f) * 0.04f;
                    const float pf = juce::jlimit (30.0f, (float) (0.49 * microRate), freq * (float) p * detune);
                    const float ph = twoPi * pf * (float) idx / (float) microRate;
                    sum += std::sin (ph) / (float) p;
                }
                s = sum * 0.8f;
            }
            else
            {
                const float base = std::sin (twoPi * freq * (float) idx / (float) microRate);
                const float nse = rng.nextFloat() * 2.0f - 1.0f;
                s = juce::jmap (noiseBlend, base, nse);
            }

            const float shaped = amp * win * std::tanh (1.8f * s);
            b.addSample (0, idx, shaped * std::sqrt (1.0f - pan));
            b.addSample (1, idx, shaped * std::sqrt (pan));
        }
    }

    return b;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldGranular (const juce::AudioBuffer<float>& micro,
                                                                            double microRate,
                                                                            double outRate,
                                                                            double outSeconds,
                                                                            float grainOutMs,
                                                                            float overlap,
                                                                            int seed) const
{
    const int outSamples = juce::jmax (1, (int) std::round (outRate * outSeconds));
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();

    juce::Random rng (seed);

    const int microSamples = micro.getNumSamples();
    const int grainOutSamples = juce::jmax (8, (int) std::round (grainOutMs * 0.001 * outRate));
    const int hopOut = juce::jmax (1, (int) std::round ((double) grainOutSamples / juce::jmax (1.0f, overlap)));

    const int grainInSamples = juce::jlimit (8,
                                             juce::jmax (9, microSamples - 2),
                                             (int) std::round ((double) grainOutSamples * microRate / outRate * 0.2));

    for (int outPos = 0; outPos < outSamples; outPos += hopOut)
    {
        const int srcStart = rng.nextInt (juce::jmax (1, microSamples - grainInSamples));
        const float jitter = rng.nextFloat() * 2.0f - 1.0f;
        const float gain = 0.05f + 0.23f * std::pow (rng.nextFloat(), 1.6f);
        const float speed = juce::jlimit (0.3f, 2.6f, 0.45f + 2.1f * rng.nextFloat() * rng.nextFloat());
        const bool reverse = rng.nextFloat() < 0.17f;
        const float grainPan = rng.nextFloat();
        const float grainBrightness = 0.2f + 0.8f * rng.nextFloat();

        for (int i = 0; i < grainOutSamples; ++i)
        {
            const int write = outPos + i;
            if (write >= outSamples)
                break;

            const float u = (float) i / (float) juce::jmax (1, grainOutSamples - 1);
            const float readU = reverse ? (1.0f - u) : u;
            const float shaped = std::pow (readU, 0.62f + 0.32f * jitter);
            const float readPos = (float) srcStart + shaped * speed * (float) (grainInSamples - 1);
            const int r0 = juce::jlimit (0, microSamples - 1, (int) readPos);
            const int r1 = juce::jmin (microSamples - 1, r0 + 1);
            const float frac = readPos - (float) r0;
            const float win = 0.5f - 0.5f * std::cos (twoPi * u);
            const float lfo = 0.55f + 0.45f * std::sin (twoPi * (u * (1.5f + 3.0f * grainBrightness)));
            const float leftPan = std::sqrt (1.0f - grainPan);
            const float rightPan = std::sqrt (grainPan);

            const float x0L = micro.getSample (0, r0);
            const float x1L = micro.getSample (0, r1);
            const float x0R = micro.getSample (1, r0);
            const float x1R = micro.getSample (1, r1);
            const float sL = juce::jmap (frac, x0L, x1L);
            const float sR = juce::jmap (frac, x0R, x1R);
            const float mono = 0.5f * (sL + sR);
            const float airy = 0.65f * mono + 0.35f * (sL - sR);
            const float shapedOut = std::tanh ((0.9f + 1.4f * grainBrightness) * airy);

            out.addSample (0, write, shapedOut * win * gain * leftPan * lfo);
            out.addSample (1, write, shapedOut * win * gain * rightPan * (2.0f - lfo));
        }
    }

    return out;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::toMono (const juce::AudioBuffer<float>& in)
{
    juce::AudioBuffer<float> mono (1, in.getNumSamples());
    mono.clear();

    if (in.getNumChannels() == 1)
    {
        mono.copyFrom (0, 0, in, 0, 0, in.getNumSamples());
        return mono;
    }

    mono.addFrom (0, 0, in, 0, 0, in.getNumSamples(), 0.5f);
    mono.addFrom (0, 0, in, 1, 0, in.getNumSamples(), 0.5f);
    return mono;
}

float MicrosoundSymphonyAudioProcessor::princArg (float x)
{
    while (x > juce::MathConstants<float>::pi)
        x -= twoPi;
    while (x < -juce::MathConstants<float>::pi)
        x += twoPi;
    return x;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldSpectral (const juce::AudioBuffer<float>& micro,
                                                                            double microRate,
                                                                            double outRate,
                                                                            double outSeconds,
                                                                            float stretch,
                                                                            float spectralWarp,
                                                                            float spectralChaos,
                                                                            int seed) const
{
    auto mono = toMono (micro);
    juce::Random rng (seed);
    const float chaos = juce::jlimit (0.0f, 1.0f, spectralChaos);
    const float chaos2 = chaos * chaos;

    const int tinyOutSamples = juce::jmax (32, (int) std::round ((double) mono.getNumSamples() * outRate / microRate));
    juce::AudioBuffer<float> tiny (1, tinyOutSamples);

    for (int i = 0; i < tinyOutSamples; ++i)
    {
        const float srcPos = (float) i * (float) microRate / (float) outRate;
        const int a = juce::jlimit (0, mono.getNumSamples() - 1, (int) srcPos);
        const int b = juce::jmin (mono.getNumSamples() - 1, a + 1);
        const float t = srcPos - (float) a;
        tiny.setSample (0, i, juce::jmap (t, mono.getSample (0, a), mono.getSample (0, b)));
    }

    const int outSamples = juce::jmax (1, (int) std::round (outSeconds * outRate));
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();

    const int fftOrder = 11;
    const int fftSize = 1 << fftOrder;
    const int bins = fftSize / 2 + 1;

    juce::dsp::FFT fft (fftOrder);

    std::vector<float> window ((size_t) fftSize, 0.0f);
    for (int n = 0; n < fftSize; ++n)
        window[(size_t) n] = 0.5f - 0.5f * std::cos (twoPi * (float) n / (float) (fftSize - 1));

    const int hopIn = juce::jmax (32, fftSize / 8);
    const int hopOut = juce::jmax (32, (int) std::round ((double) hopIn * stretch));

    std::vector<float> lastPhase ((size_t) bins, 0.0f);
    std::vector<float> sumPhaseL ((size_t) bins, 0.0f);
    std::vector<float> sumPhaseR ((size_t) bins, 0.0f);

    std::vector<std::complex<float>> timeIn ((size_t) fftSize);
    std::vector<std::complex<float>> specIn ((size_t) fftSize);
    std::vector<std::complex<float>> specOutL ((size_t) fftSize);
    std::vector<std::complex<float>> specOutR ((size_t) fftSize);
    std::vector<std::complex<float>> timeOutL ((size_t) fftSize);
    std::vector<std::complex<float>> timeOutR ((size_t) fftSize);

    const int numFrames = 1 + juce::jmax (0, (outSamples - fftSize) / hopOut);
    const float seedPhaseA = rng.nextFloat() * twoPi;
    const float seedPhaseB = rng.nextFloat() * twoPi;
    const float seedPhaseC = rng.nextFloat() * twoPi;

    for (int frame = 0; frame < numFrames; ++frame)
    {
        const int outPos = frame * hopOut;
        const int inPos = (frame * hopIn) % juce::jmax (1, tiny.getNumSamples());
        const float frameU = (float) frame / (float) juce::jmax (1, numFrames - 1);
        const float dynWarp = juce::jlimit (0.45f,
                                            7.5f,
                                            spectralWarp
                                                * (1.0f
                                                    + (0.08f + 0.30f * chaos) * std::sin (twoPi * (0.23f * frameU) + seedPhaseA)
                                                    + (0.04f + 0.12f * chaos) * std::sin (twoPi * (0.97f * frameU) + seedPhaseB)));
        const float shimmer = (0.03f + 0.07f * chaos) + (0.10f + 0.20f * chaos)
            * (0.5f + 0.5f * std::sin (twoPi * (1.07f * frameU) + seedPhaseC));
        const float combRate = 0.06f + 0.31f * frameU;
        const float formantA = 0.13f + 0.24f * (0.5f + 0.5f * std::sin (twoPi * (0.21f * frameU) + seedPhaseA));
        const float formantB = 0.36f + 0.27f * (0.5f + 0.5f * std::sin (twoPi * (0.34f * frameU) + seedPhaseB));
        const float formantC = 0.64f + 0.21f * (0.5f + 0.5f * std::sin (twoPi * (0.18f * frameU) + seedPhaseC));
        const float formantWidthA = 0.032f;
        const float formantWidthB = 0.045f;
        const float formantWidthC = 0.055f;

        for (int n = 0; n < fftSize; ++n)
        {
            const int idx = (inPos + n) % tiny.getNumSamples();
            timeIn[(size_t) n] = { tiny.getSample (0, idx) * window[(size_t) n], 0.0f };
        }

        fft.perform (timeIn.data(), specIn.data(), false);

        std::fill (specOutL.begin(), specOutL.end(), std::complex<float> { 0.0f, 0.0f });
        std::fill (specOutR.begin(), specOutR.end(), std::complex<float> { 0.0f, 0.0f });

        for (int k = 0; k < bins; ++k)
        {
            const float normK = (float) k / (float) juce::jmax (1, bins - 1);
            const float srcA = (float) k / dynWarp;
            const float srcB = std::pow (normK, juce::jmax (0.2f, 1.15f / dynWarp)) * (float) (bins - 1);
            const float srcPos = juce::jlimit (0.0f,
                                               (float) (bins - 1),
                                               juce::jmap (0.5f + 0.5f * std::sin (twoPi * (0.17f * frameU + normK * 0.9f)),
                                                           srcA, srcB));
            const int srcK0 = juce::jlimit (0, bins - 1, (int) srcPos);
            const int srcK1 = juce::jmin (bins - 1, srcK0 + 1);
            const float srcFrac = srcPos - (float) srcK0;

            const auto v = specIn[(size_t) srcK0] + (specIn[(size_t) srcK1] - specIn[(size_t) srcK0]) * srcFrac;
            const float mag = std::abs (v);
            const float phase = std::atan2 (v.imag(), v.real());

            const float expected = twoPi * (float) hopIn * srcPos / (float) fftSize;
            const float d = princArg (phase - lastPhase[(size_t) srcK0] - expected);
            const float trueFreq = twoPi * srcPos / (float) fftSize + d / (float) hopIn;
            lastPhase[(size_t) srcK0] = phase;

            const float tilt = std::exp ((normK - 0.5f) * ((0.15f + 0.75f * chaos) * std::sin (twoPi * (0.31f * frameU) + seedPhaseA)));
            const float combDepth = 0.08f + 0.38f * chaos;
            const float comb = (1.0f - combDepth) + combDepth
                * (0.5f + 0.5f * std::cos (twoPi * (normK * (18.0f + (30.0f + 44.0f * chaos) * dynWarp) + combRate * frame)));
            const float formantBoostA = std::exp (-0.5f * std::pow ((normK - formantA) / formantWidthA, 2.0f));
            const float formantBoostB = std::exp (-0.5f * std::pow ((normK - formantB) / formantWidthB, 2.0f));
            const float formantBoostC = std::exp (-0.5f * std::pow ((normK - formantC) / formantWidthC, 2.0f));
            const float formants = (0.85f - 0.20f * chaos)
                + (0.20f + 0.62f * chaos) * formantBoostA
                + (0.26f + 0.80f * chaos) * formantBoostB
                + (0.22f + 0.66f * chaos) * formantBoostC;
            const float shapedMag = mag * tilt * comb * formants;

            const float stereoPhaseOffset = ((0.015f + 0.05f * chaos) + (0.07f + 0.45f * chaos2) * normK * normK)
                                            * std::sin (twoPi * (0.07f * frame + normK * 2.8f) + seedPhaseB);
            sumPhaseL[(size_t) k] += trueFreq * (float) hopOut;
            sumPhaseR[(size_t) k] += trueFreq * (float) hopOut
                * (1.0f + (0.0001f + 0.0026f * chaos) * std::sin (twoPi * (0.11f * frameU + normK)));

            const auto outCL = std::polar (shapedMag, sumPhaseL[(size_t) k]);
            const auto outCR = std::polar (shapedMag, sumPhaseR[(size_t) k] + stereoPhaseOffset);
            specOutL[(size_t) k] += outCL;
            specOutR[(size_t) k] += outCR;

            const float harmonicRatio = (1.08f + 0.45f * chaos)
                + (0.08f + 0.76f * chaos) * std::sin (twoPi * frameU + seedPhaseC);
            const int harmonicK = juce::jlimit (0, bins - 1, (int) std::round ((float) k * harmonicRatio));
            if (harmonicK > 0 && harmonicK < bins)
            {
                const float shimmerMag = shapedMag * shimmer * (1.0f - (0.15f + 0.45f * chaos) * normK);
                const float shimmerPhase = sumPhaseL[(size_t) k] * ((1.08f + 0.55f * chaos) + (0.1f + 0.95f * chaos) * frameU);
                specOutL[(size_t) harmonicK] += std::polar (shimmerMag, shimmerPhase);
                specOutR[(size_t) harmonicK] += std::polar (shimmerMag * 0.97f, shimmerPhase + 0.13f);
            }

            if (k > 0 && k < fftSize / 2)
            {
                specOutL[(size_t) (fftSize - k)] = std::conj (specOutL[(size_t) k]);
                specOutR[(size_t) (fftSize - k)] = std::conj (specOutR[(size_t) k]);
            }
        }

        fft.perform (specOutL.data(), timeOutL.data(), true);
        fft.perform (specOutR.data(), timeOutR.data(), true);

        for (int n = 0; n < fftSize; ++n)
        {
            const int w = outPos + n;
            if (w >= outSamples)
                break;

            const float win = window[(size_t) n] / (float) fftSize;
            out.addSample (0, w, timeOutL[(size_t) n].real() * win);
            out.addSample (1, w, timeOutR[(size_t) n].real() * win);
        }
    }

    return out;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldMorphogen (const juce::AudioBuffer<float>& micro,
                                                                             double microRate,
                                                                             double outRate,
                                                                             double outSeconds,
                                                                             float stretch,
                                                                             float spectralWarp,
                                                                             float spectralChaos,
                                                                             float hybridMix,
                                                                             int seed) const
{
    juce::ignoreUnused (microRate);
    auto mono = toMono (micro);
    juce::Random rng (seed + 2026);
    const float chaos = juce::jlimit (0.0f, 1.0f, spectralChaos);

    const int outSamples = juce::jmax (1, (int) std::round (outSeconds * outRate));
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();

    const int bins = juce::jlimit (48, 160, (int) std::round (64.0f + 64.0f * chaos));
    const int timeCells = juce::jlimit (40, 420, (int) std::round ((18.0f + 3.0f * stretch) * (1.0f + 0.6f * chaos)));
    const int subSteps = juce::jlimit (2, 14, (int) std::round (3.0f + 8.0f * chaos));
    const float baseWarp = juce::jlimit (0.6f, 6.0f, spectralWarp);

    std::vector<float> rdU ((size_t) bins, 1.0f), rdV ((size_t) bins, 0.0f), prevU ((size_t) bins, 1.0f), prevV ((size_t) bins, 0.0f);
    std::vector<float> nextU ((size_t) bins, 1.0f), nextV ((size_t) bins, 0.0f);
    std::vector<float> specProfile ((size_t) bins, 0.0f);
    std::vector<float> grid ((size_t) bins * (size_t) timeCells, 0.0f);

    // Build a spectral seed profile from tiny windows of the micro burst.
    const int fftOrder = 10;
    const int fftSize = 1 << fftOrder;
    juce::dsp::FFT fft (fftOrder);
    std::vector<std::complex<float>> timeBuf ((size_t) fftSize), specBuf ((size_t) fftSize);

    for (int n = 0; n < fftSize; ++n)
    {
        const int idx = (int) std::round ((double) n * (double) mono.getNumSamples() / (double) juce::jmax (1, fftSize - 1));
        const int i = juce::jlimit (0, mono.getNumSamples() - 1, idx);
        const float w = 0.5f - 0.5f * std::cos (twoPi * (float) n / (float) juce::jmax (1, fftSize - 1));
        timeBuf[(size_t) n] = { mono.getSample (0, i) * w, 0.0f };
    }
    fft.perform (timeBuf.data(), specBuf.data(), false);
    for (int k = 0; k < bins; ++k)
    {
        const float u = (float) k / (float) juce::jmax (1, bins - 1);
        const int src = juce::jlimit (0, fftSize / 2, (int) std::round (u * (fftSize / 2)));
        specProfile[(size_t) k] = std::pow (juce::jlimit (0.0f, 1.0f, std::abs (specBuf[(size_t) src]) * 6.0f), 0.65f);
    }

    for (int k = 0; k < bins; ++k)
    {
        const float impulse = (rng.nextFloat() < (0.06f + 0.08f * chaos)) ? (0.4f + 0.6f * rng.nextFloat()) : 0.0f;
        rdV[(size_t) k] = juce::jlimit (0.0f, 1.0f, 0.45f * specProfile[(size_t) k] + impulse);
        rdU[(size_t) k] = 1.0f - rdV[(size_t) k] * (0.6f + 0.3f * chaos);
        prevU[(size_t) k] = rdU[(size_t) k];
        prevV[(size_t) k] = rdV[(size_t) k];
    }

    const float diffUBase = 0.10f + 0.16f * chaos;
    const float diffVBase = 0.06f + 0.12f * chaos;
    const float temporalCouple = 0.08f + 0.26f * hybridMix;
    const float fBase = 0.012f + 0.028f * chaos;
    const float kBase = 0.038f + 0.034f * (1.0f - chaos);

    for (int t = 0; t < timeCells; ++t)
    {
        const float tu = (float) t / (float) juce::jmax (1, timeCells - 1);
        const float f = fBase + 0.018f * std::sin (twoPi * (0.13f * tu + 0.71f * chaos));
        const float kk = kBase + 0.016f * std::sin (twoPi * (0.21f * tu + 0.37f * chaos));

        for (int step = 0; step < subSteps; ++step)
        {
            for (int k = 0; k < bins; ++k)
            {
                const int km1 = (k - 1 + bins) % bins;
                const int kp1 = (k + 1) % bins;

                const float u = rdU[(size_t) k];
                const float v = rdV[(size_t) k];
                const float lapU = rdU[(size_t) km1] - 2.0f * u + rdU[(size_t) kp1];
                const float lapV = rdV[(size_t) km1] - 2.0f * v + rdV[(size_t) kp1];
                const float temporalU = prevU[(size_t) k] - u;
                const float temporalV = prevV[(size_t) k] - v;

                const float du = (diffUBase * lapU) - (u * v * v) + f * (1.0f - u) + temporalCouple * temporalU;
                const float dv = (diffVBase * lapV) + (u * v * v) - (kk + f) * v + temporalCouple * temporalV;

                nextU[(size_t) k] = juce::jlimit (0.0f, 1.5f, u + du * 0.9f);
                nextV[(size_t) k] = juce::jlimit (0.0f, 1.5f, v + dv * 0.9f);
            }

            rdU.swap (nextU);
            rdV.swap (nextV);
        }

        for (int k = 0; k < bins; ++k)
        {
            const float c = juce::jlimit (0.0f, 1.0f, rdV[(size_t) k] * (0.7f + 0.6f * specProfile[(size_t) k]));
            grid[(size_t) t * (size_t) bins + (size_t) k] = c;
            prevU[(size_t) k] = rdU[(size_t) k];
            prevV[(size_t) k] = rdV[(size_t) k];
        }
    }

    std::vector<float> phaseL ((size_t) bins, 0.0f), phaseR ((size_t) bins, 0.0f);
    const float nyquist = 0.5f * (float) outRate;
    const float warpPow = juce::jlimit (0.35f, 2.8f, 1.0f / juce::jmax (0.15f, baseWarp));
    const float stereoSpread = 0.02f + 0.28f * hybridMix + 0.2f * chaos;
    const float harmonicSkew = 0.15f + 0.6f * chaos;

    for (int s = 0; s < outSamples; ++s)
    {
        const float tu = (float) s / (float) juce::jmax (1, outSamples - 1);
        const float cellPos = tu * (float) juce::jmax (1, timeCells - 1);
        const int t0 = juce::jlimit (0, timeCells - 1, (int) cellPos);
        const int t1 = juce::jmin (timeCells - 1, t0 + 1);
        const float tf = cellPos - (float) t0;

        float sampleL = 0.0f;
        float sampleR = 0.0f;

        for (int k = 1; k < bins; ++k)
        {
            const float ku = (float) k / (float) juce::jmax (1, bins - 1);
            const float amp0 = grid[(size_t) t0 * (size_t) bins + (size_t) k];
            const float amp1 = grid[(size_t) t1 * (size_t) bins + (size_t) k];
            const float amp = juce::jlimit (0.0f, 1.0f, juce::jmap (tf, amp0, amp1));

            const float warped = std::pow (ku, warpPow);
            const float freq = juce::jlimit (12.0f, nyquist * 0.98f, warped * nyquist);
            const float detune = 1.0f + stereoSpread * (0.5f + 0.5f * std::sin (twoPi * (0.001f * (float) s + ku * 3.0f)));

            phaseL[(size_t) k] += twoPi * freq / (float) outRate;
            phaseR[(size_t) k] += twoPi * (freq * detune) / (float) outRate;

            const float morph = std::sin (phaseL[(size_t) k] * (1.0f + harmonicSkew * ku));
            const float morphR = std::sin (phaseR[(size_t) k] * (1.0f - harmonicSkew * (1.0f - ku)));
            const float w = std::pow (1.0f - ku, 0.35f + 0.25f * chaos);
            sampleL += amp * w * morph;
            sampleR += amp * w * morphR;
        }

        out.setSample (0, s, sampleL * (0.10f + 0.16f * chaos));
        out.setSample (1, s, sampleR * (0.10f + 0.16f * chaos));
    }

    return out;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldFennesz (const juce::AudioBuffer<float>& micro,
                                                                           double microRate,
                                                                           double outRate,
                                                                           double outSeconds,
                                                                           float stretch,
                                                                           float spectralWarp,
                                                                           float spectralChaos,
                                                                           float hybridMix,
                                                                           int seed) const
{
    auto spectral = unfoldSpectral (micro,
                                    microRate,
                                    outRate,
                                    outSeconds,
                                    juce::jlimit (8.0f, 120.0f, stretch * 1.25f),
                                    juce::jlimit (0.8f, 6.2f, spectralWarp * 0.9f),
                                    juce::jlimit (0.1f, 0.95f, spectralChaos * 0.75f + 0.15f),
                                    seed + 1001);
    auto granular = unfoldGranular (micro,
                                    microRate,
                                    outRate,
                                    outSeconds,
                                    juce::jlimit (18.0f, 220.0f, 52.0f + 90.0f * hybridMix),
                                    juce::jlimit (2.0f, 18.0f, 5.0f + 8.0f * hybridMix),
                                    seed + 2002);

    const int outSamples = juce::jmax (spectral.getNumSamples(), granular.getNumSamples());
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();
    juce::Random rng (seed + 3003);

    float phaseA = 0.0f, phaseB = 0.0f, phaseC = 0.0f;
    const float chordA = 110.0f * (1.0f + 0.08f * (rng.nextFloat() - 0.5f));
    const float chordB = chordA * 1.4983f;
    const float chordC = chordA * 2.2449f;

    const int d1 = juce::jmax (1, (int) std::round (outRate * (0.033 + 0.021 * spectralChaos)));
    const int d2 = juce::jmax (1, (int) std::round (outRate * (0.071 + 0.029 * spectralChaos)));
    std::vector<float> dl ((size_t) outSamples, 0.0f), dr ((size_t) outSamples, 0.0f);

    for (int i = 0; i < outSamples; ++i)
    {
        const float t = (float) i / (float) juce::jmax (1, outSamples - 1);
        const float env = 0.25f + 0.75f * std::pow (0.5f - 0.5f * std::cos (twoPi * t), 0.55f);
        const float flutter = 1.0f + 0.008f * std::sin (twoPi * (0.11f * t + 0.03f * spectralChaos));
        phaseA += twoPi * (chordA * flutter) / (float) outRate;
        phaseB += twoPi * (chordB * flutter) / (float) outRate;
        phaseC += twoPi * (chordC * flutter) / (float) outRate;
        const float drone = 0.35f * std::sin (phaseA) + 0.22f * std::sin (phaseB) + 0.18f * std::sin (phaseC);
        const float hiss = (rng.nextFloat() * 2.0f - 1.0f) * (0.01f + 0.05f * spectralChaos);

        const float sL = i < spectral.getNumSamples() ? spectral.getSample (0, i) : 0.0f;
        const float sR = i < spectral.getNumSamples() ? spectral.getSample (1, i) : 0.0f;
        const float gL = i < granular.getNumSamples() ? granular.getSample (0, i) : 0.0f;
        const float gR = i < granular.getNumSamples() ? granular.getSample (1, i) : 0.0f;
        float xL = 0.46f * sL + 0.36f * gL + env * (0.20f * drone + hiss);
        float xR = 0.46f * sR + 0.36f * gR + env * (0.20f * drone - hiss);

        const int i1 = i - d1, i2 = i - d2;
        const float fbL = (i1 >= 0 ? dl[(size_t) i1] : 0.0f) * 0.34f + (i2 >= 0 ? dr[(size_t) i2] : 0.0f) * 0.21f;
        const float fbR = (i1 >= 0 ? dr[(size_t) i1] : 0.0f) * 0.34f + (i2 >= 0 ? dl[(size_t) i2] : 0.0f) * 0.21f;
        dl[(size_t) i] = std::tanh (xL + fbL * (0.35f + 0.28f * hybridMix));
        dr[(size_t) i] = std::tanh (xR + fbR * (0.35f + 0.28f * hybridMix));

        out.setSample (0, i, dl[(size_t) i] * 0.76f);
        out.setSample (1, i, dr[(size_t) i] * 0.76f);
    }

    return out;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldNoto (const juce::AudioBuffer<float>& micro,
                                                                        double microRate,
                                                                        double outRate,
                                                                        double outSeconds,
                                                                        float stretch,
                                                                        float spectralWarp,
                                                                        float spectralChaos,
                                                                        int seed) const
{
    juce::ignoreUnused (microRate, spectralWarp);
    const int outSamples = juce::jmax (1, (int) std::round (outRate * outSeconds));
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();

    juce::Random rng (seed + 4004);
    auto mono = toMono (micro);
    uint32_t lfsr = (uint32_t) seed ^ 0xA5366B4Du;
    const int grid = juce::jmax (12, (int) std::round (outRate * (0.007 + 0.018 * (1.0f - juce::jlimit (0.0f, 1.0f, spectralChaos)))));
    const int microN = juce::jmax (1, mono.getNumSamples());
    float phase = 0.0f;
    const float baseHz = 200.0f + 3000.0f * juce::jlimit (0.0f, 1.0f, spectralChaos);

    for (int i = 0; i < outSamples; ++i)
    {
        if (i % grid == 0)
        {
            const uint32_t bit = ((lfsr >> 0u) ^ (lfsr >> 2u) ^ (lfsr >> 3u) ^ (lfsr >> 5u)) & 1u;
            lfsr = (lfsr >> 1u) | (bit << 31u);
        }

        const bool gateA = ((lfsr >> 2u) & 1u) != 0u;
        const bool gateB = ((lfsr >> 9u) & 1u) != 0u;
        const float sub = (float) (i % grid) / (float) juce::jmax (1, grid - 1);
        const float clickEnv = std::exp (-22.0f * sub) * (gateA ? 1.0f : 0.0f);
        const float burstEnv = std::exp (-8.0f * sub) * (gateB ? 1.0f : 0.0f);

        const float hz = baseHz * (1.0f + 2.0f * (float) ((lfsr >> 16u) & 7u) / 7.0f);
        phase += twoPi * hz / (float) outRate;
        const float sine = std::sin (phase);

        const int mi = (int) ((i * (3 + (seed % 11))) % microN);
        const float microTap = mono.getSample (0, mi);
        const float click = (rng.nextFloat() * 2.0f - 1.0f) * clickEnv * (0.12f + 0.18f * spectralChaos);
        const float body = burstEnv * (0.18f * sine + 0.12f * std::tanh (4.0f * microTap));
        const float sparse = ((i % (grid * (2 + (int) std::round (stretch * 0.2f)))) == 0) ? 0.6f : 0.0f;

        const float left = click + body + sparse * (rng.nextFloat() * 2.0f - 1.0f) * 0.08f;
        const float right = -click + body - sparse * (rng.nextFloat() * 2.0f - 1.0f) * 0.08f;
        out.setSample (0, i, left);
        out.setSample (1, i, right);
    }

    return out;
}

juce::AudioBuffer<float> MicrosoundSymphonyAudioProcessor::unfoldIkeda (const juce::AudioBuffer<float>& micro,
                                                                         double microRate,
                                                                         double outRate,
                                                                         double outSeconds,
                                                                         float stretch,
                                                                         float spectralWarp,
                                                                         float spectralChaos,
                                                                         int seed) const
{
    juce::ignoreUnused (microRate);
    const int outSamples = juce::jmax (1, (int) std::round (outRate * outSeconds));
    juce::AudioBuffer<float> out (2, outSamples);
    out.clear();

    auto mono = toMono (micro);
    const int microN = juce::jmax (1, mono.getNumSamples());
    const float chaos = juce::jlimit (0.0f, 1.0f, spectralChaos);
    const int banks = 6 + (int) std::round (10.0f * chaos);
    std::vector<float> freq ((size_t) banks, 0.0f), phase ((size_t) banks, 0.0f);
    uint32_t state = ((uint32_t) seed) ^ 0x7F4A7C15u;

    for (int b = 0; b < banks; ++b)
    {
        const float step = 40.0f + 220.0f * (float) b;
        const float quant = std::pow (2.0f, std::floor (std::log2 (step * juce::jlimit (0.5f, 6.0f, spectralWarp))));
        freq[(size_t) b] = juce::jlimit (20.0f, (float) (0.48 * outRate), quant);
    }

    const int gatePeriod = juce::jmax (2, (int) std::round (outRate * (0.0009 + 0.006 * (1.0f - chaos))));
    const int macro = juce::jmax (8, (int) std::round (outRate * (0.03 + 0.18 * (stretch / 100.0f))));

    for (int i = 0; i < outSamples; ++i)
    {
        if (i % gatePeriod == 0)
        {
            const uint32_t bit = ((state >> 0u) ^ (state >> 1u) ^ (state >> 21u) ^ (state >> 31u)) & 1u;
            state = (state >> 1u) | (bit << 31u);
        }

        float l = 0.0f, r = 0.0f;
        const float macroEnv = (((i / macro) & 1) == 0) ? 1.0f : (0.14f + 0.22f * chaos);
        for (int b = 0; b < banks; ++b)
        {
            const bool gate = ((state >> (b % 31)) & 1u) != 0u;
            phase[(size_t) b] += twoPi * freq[(size_t) b] / (float) outRate;
            const float s = std::sin (phase[(size_t) b]) * (gate ? 1.0f : 0.0f);
            const float w = 1.0f / (1.0f + (float) b * 0.35f);
            l += s * w;
            r += ((b & 1) == 0 ? s : -s) * w;
        }

        const int mi = (i * (5 + (seed % 13))) % microN;
        const float data = std::tanh (mono.getSample (0, mi) * (8.0f + 18.0f * chaos));
        const float amp = macroEnv * (0.05f + 0.35f * chaos);
        out.setSample (0, i, std::tanh ((l * amp) + data * 0.08f));
        out.setSample (1, i, std::tanh ((r * amp) - data * 0.08f));
    }

    return out;
}

void MicrosoundSymphonyAudioProcessor::applyBloomInPlace (juce::AudioBuffer<float>& b,
                                                          double sampleRate,
                                                          int seed,
                                                          float amount)
{
    if (b.getNumSamples() <= 8 || b.getNumChannels() < 2)
        return;

    juce::Random rng (seed);
    const int n = b.getNumSamples();
    const float amt = juce::jlimit (0.0f, 1.0f, amount);

    const int tap1 = juce::jmax (1, (int) std::round ((0.013 + 0.018 * rng.nextFloat()) * sampleRate));
    const int tap2 = juce::jmax (1, (int) std::round ((0.029 + 0.031 * rng.nextFloat()) * sampleRate));
    const int tap3 = juce::jmax (1, (int) std::round ((0.061 + 0.045 * rng.nextFloat()) * sampleRate));
    const int tap4 = juce::jmax (1, (int) std::round ((0.101 + 0.071 * rng.nextFloat()) * sampleRate));

    std::vector<float> dL ((size_t) n, 0.0f), dR ((size_t) n, 0.0f);
    float hpL = 0.0f, hpR = 0.0f, lpL = 0.0f, lpR = 0.0f;
    const float hpCoeff = 0.987f;
    const float lpCoeff = 0.08f + 0.10f * amt;

    for (int i = 0; i < n; ++i)
    {
        const float inL = b.getSample (0, i);
        const float inR = b.getSample (1, i);

        const int i1 = i - tap1, i2 = i - tap2, i3 = i - tap3, i4 = i - tap4;
        const float fbL = (i1 >= 0 ? dR[(size_t) i1] : 0.0f) * 0.41f
                        + (i2 >= 0 ? dL[(size_t) i2] : 0.0f) * 0.29f
                        - (i3 >= 0 ? dR[(size_t) i3] : 0.0f) * 0.18f
                        + (i4 >= 0 ? dL[(size_t) i4] : 0.0f) * 0.13f;
        const float fbR = (i1 >= 0 ? dL[(size_t) i1] : 0.0f) * 0.41f
                        + (i2 >= 0 ? dR[(size_t) i2] : 0.0f) * 0.29f
                        - (i3 >= 0 ? dL[(size_t) i3] : 0.0f) * 0.18f
                        + (i4 >= 0 ? dR[(size_t) i4] : 0.0f) * 0.13f;

        dL[(size_t) i] = std::tanh (inL + fbL * (0.28f + 0.42f * amt));
        dR[(size_t) i] = std::tanh (inR + fbR * (0.28f + 0.42f * amt));

        const float mid = 0.5f * (dL[(size_t) i] + dR[(size_t) i]);
        hpL = hpCoeff * (hpL + dL[(size_t) i] - mid);
        hpR = hpCoeff * (hpR + dR[(size_t) i] - mid);

        lpL += lpCoeff * (hpL - lpL);
        lpR += lpCoeff * (hpR - lpR);

        b.addSample (0, i, lpL * (0.11f + 0.28f * amt));
        b.addSample (1, i, lpR * (0.11f + 0.28f * amt));
    }
}

void MicrosoundSymphonyAudioProcessor::sanitizeBufferInPlace (juce::AudioBuffer<float>& b)
{
    for (int ch = 0; ch < b.getNumChannels(); ++ch)
    {
        auto* x = b.getWritePointer (ch);
        for (int i = 0; i < b.getNumSamples(); ++i)
        {
            float s = x[i];
            if (! std::isfinite (s))
                s = 0.0f;

            // Keep pathological values bounded before normalization.
            x[i] = std::tanh (s * 0.7f) * 1.35f;
        }
    }
}

void MicrosoundSymphonyAudioProcessor::normalizeInPlace (juce::AudioBuffer<float>& b, float peakTarget)
{
    float peak = 0.0f;
    for (int ch = 0; ch < b.getNumChannels(); ++ch)
        peak = juce::jmax (peak, b.getMagnitude (ch, 0, b.getNumSamples()));

    if (peak > 1.0e-7f)
        b.applyGain (peakTarget / peak);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MicrosoundSymphonyAudioProcessor();
}
