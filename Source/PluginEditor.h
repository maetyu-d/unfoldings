#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class MicrosoundSymphonyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MicrosoundSymphonyAudioProcessorEditor (MicrosoundSymphonyAudioProcessor&);
    ~MicrosoundSymphonyAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MicrosoundSymphonyAudioProcessor& audioProcessor;

    juce::Label titleLabel;

    juce::ComboBox modeBox;
    juce::ComboBox presetBox;
    juce::ComboBox beautySceneBox;
    juce::ComboBox microRateBox;
    juce::Slider burstMsSlider;
    juce::Slider densitySlider;
    juce::Slider outSecondsSlider;
    juce::Slider grainMsSlider;
    juce::Slider overlapSlider;
    juce::Slider stretchSlider;
    juce::Slider warpSlider;
    juce::Slider spectralChaosSlider;
    juce::Slider hybridMixSlider;
    juce::Slider seedSlider;
    juce::ToggleButton loopButton { "Loop Playback" };

    juce::TextButton renderButton { "Render" };
    juce::TextButton applyBeautyButton { "Apply Beauty" };
    juce::TextButton exportButton { "Export WAV" };
    std::unique_ptr<juce::FileChooser> exportChooser;
    std::unique_ptr<juce::LookAndFeel_V4> presetLookAndFeel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ComboAttachment> modeAttachment;
    std::unique_ptr<ComboAttachment> beautySceneAttachment;
    std::unique_ptr<ComboAttachment> microRateAttachment;
    std::unique_ptr<SliderAttachment> burstMsAttachment;
    std::unique_ptr<SliderAttachment> densityAttachment;
    std::unique_ptr<SliderAttachment> outSecondsAttachment;
    std::unique_ptr<SliderAttachment> grainMsAttachment;
    std::unique_ptr<SliderAttachment> overlapAttachment;
    std::unique_ptr<SliderAttachment> stretchAttachment;
    std::unique_ptr<SliderAttachment> warpAttachment;
    std::unique_ptr<SliderAttachment> spectralChaosAttachment;
    std::unique_ptr<SliderAttachment> hybridMixAttachment;
    std::unique_ptr<SliderAttachment> seedAttachment;
    std::unique_ptr<ButtonAttachment> loopAttachment;

    void setupSlider (juce::Slider& s, const juce::String& name);
    void updatePresetColourTheme();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrosoundSymphonyAudioProcessorEditor)
};
