#include "PluginEditor.h"

namespace
{
juce::Colour modeColour (int mode)
{
    switch (mode)
    {
        case 0: return juce::Colour (0xFFCACACA); // Granular
        case 1: return juce::Colour (0xFFBFBFBF); // Spectral
        case 2: return juce::Colour (0xFFD8D8D8); // Hybrid
        case 3: return juce::Colour (0xFFAFAFAF); // Xeno
        case 4: return juce::Colour (0xFFE2E2E2); // Morphogen
        case 5: return juce::Colour (0xFFC4C4C4); // Fennesz
        case 6: return juce::Colour (0xFFB4B4B4); // Noto
        case 7: return juce::Colour (0xFFF0F0F0); // Ikeda
        default: return juce::Colour (0xFFB8B8B8);
    }
}

class PresetModeLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    PresetModeLookAndFeel()
        : presetNames (MicrosoundSymphonyAudioProcessor::getPresetNames())
    {}

    void drawPopupMenuItem (juce::Graphics& g,
                            const juce::Rectangle<int>& area,
                            bool isSeparator,
                            bool isActive,
                            bool isHighlighted,
                            bool isTicked,
                            bool hasSubMenu,
                            const juce::String& text,
                            const juce::String& shortcutKeyText,
                            const juce::Drawable* icon,
                            const juce::Colour* textColourToUse) override
    {
        juce::ignoreUnused (isTicked, hasSubMenu, shortcutKeyText, icon, textColourToUse);
        if (isSeparator)
        {
            g.setColour (juce::Colour (0xFF8A8A8A).withAlpha (0.35f));
            g.fillRect (area.withTrimmedTop (area.getHeight() / 2).withHeight (1));
            return;
        }

        auto itemArea = area;
        const int idx = presetNames.indexOf (text);
        const int mode = MicrosoundSymphonyAudioProcessor::getPresetMode (idx);
        const juce::Colour c = modeColour (mode);

        g.setColour (isHighlighted ? juce::Colour (0xFFD9D9D9) : juce::Colour (0xFFC7C7C7));
        g.fillRect (itemArea);
        g.setColour (c.darker (0.4f));
        g.fillRect (itemArea.removeFromLeft (4));

        g.setColour (isActive ? juce::Colour (0xFF1C1C1C) : juce::Colour (0xFF575757));
        g.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::plain)));
        g.drawFittedText (text, itemArea.reduced (8, 0), juce::Justification::centredLeft, 1);
    }

private:
    juce::StringArray presetNames;
};

void paintBrushedMetal (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::ColourGradient bg (juce::Colour (0xFFCFCFCF), (float) bounds.getX(), (float) bounds.getY(),
                             juce::Colour (0xFFAFAFAF), (float) bounds.getX(), (float) bounds.getBottom(), false);
    bg.addColour (0.5, juce::Colour (0xFFBDBDBD));
    g.setGradientFill (bg);
    g.fillRect (bounds);

    g.setColour (juce::Colour (0x55FFFFFF));
    for (int y = bounds.getY(); y < bounds.getBottom(); y += 3)
        g.drawHorizontalLine (y, (float) bounds.getX(), (float) bounds.getRight());
}
}

MicrosoundSymphonyAudioProcessorEditor::MicrosoundSymphonyAudioProcessorEditor (MicrosoundSymphonyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (960, 620);

    titleLabel.setText ("unfoldings", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::Font (juce::FontOptions (34.0f, juce::Font::bold)));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xFF232323));
    addAndMakeVisible (titleLabel);

    modeBox.addItemList (juce::StringArray { "Granular", "Spectral", "Hybrid", "Xeno", "Morphogen", "Fennesz", "Noto", "Ikeda" }, 1);
    presetBox.addItemList (MicrosoundSymphonyAudioProcessor::getPresetNames(), 1);
    presetBox.setTextWhenNothingSelected ("Choose preset...");
    beautySceneBox.addItemList (juce::StringArray { "Off", "Lush", "Crystalline", "Dramatic" }, 1);
    microRateBox.addItemList (juce::StringArray { "192000", "384000", "768000", "1536000" }, 1);

    for (auto* c : { &modeBox, &presetBox, &beautySceneBox, &microRateBox })
    {
        c->setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFFD3D3D3));
        c->setColour (juce::ComboBox::textColourId, juce::Colour (0xFF222222));
        c->setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF6A6A6A));
        c->setColour (juce::ComboBox::arrowColourId, juce::Colour (0xFF2E2E2E));
        addAndMakeVisible (*c);
    }

    presetLookAndFeel = std::make_unique<PresetModeLookAndFeel>();
    presetBox.setLookAndFeel (presetLookAndFeel.get());
    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0)
            audioProcessor.applyPreset (idx);
        updatePresetColourTheme();
    };

    setupSlider (burstMsSlider, "Burst");
    setupSlider (densitySlider, "Density");
    setupSlider (outSecondsSlider, "Duration");
    setupSlider (grainMsSlider, "Grain");
    setupSlider (overlapSlider, "Overlap");
    setupSlider (stretchSlider, "Stretch");
    setupSlider (warpSlider, "Warp");
    setupSlider (spectralChaosSlider, "Chaos");
    setupSlider (hybridMixSlider, "Hybrid Mix");
    setupSlider (seedSlider, "Seed");

    loopButton.setColour (juce::ToggleButton::textColourId, juce::Colour (0xFF2A2A2A));
    addAndMakeVisible (loopButton);

    for (auto* b : { &renderButton, &applyBeautyButton, &exportButton })
    {
        b->setColour (juce::TextButton::buttonColourId, juce::Colour (0xFFC8C8C8));
        b->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xFFDCDCDC));
        b->setColour (juce::TextButton::textColourOffId, juce::Colour (0xFF1E1E1E));
        addAndMakeVisible (*b);
    }

    renderButton.onClick = [this] { audioProcessor.renderNow(); };
    applyBeautyButton.onClick = [this] { audioProcessor.applyBeautyScene(); };

    exportButton.onClick = [this]
    {
        exportChooser = std::make_unique<juce::FileChooser> ("Export rendered file", juce::File(), "*.wav");
        auto flags = juce::FileBrowserComponent::saveMode
                   | juce::FileBrowserComponent::canSelectFiles
                   | juce::FileBrowserComponent::warnAboutOverwriting;

        exportChooser->launchAsync (flags, [this] (const juce::FileChooser& chooser)
        {
            if (chooser.getResult() != juce::File())
                audioProcessor.exportLastRenderToWav (chooser.getResult());

            exportChooser.reset();
        });
    };

    auto& apvts = audioProcessor.apvts;
    modeAttachment = std::make_unique<ComboAttachment> (apvts, "mode", modeBox);
    beautySceneAttachment = std::make_unique<ComboAttachment> (apvts, "beautyScene", beautySceneBox);
    microRateAttachment = std::make_unique<ComboAttachment> (apvts, "microRate", microRateBox);
    burstMsAttachment = std::make_unique<SliderAttachment> (apvts, "burstMs", burstMsSlider);
    densityAttachment = std::make_unique<SliderAttachment> (apvts, "density", densitySlider);
    outSecondsAttachment = std::make_unique<SliderAttachment> (apvts, "outSeconds", outSecondsSlider);
    grainMsAttachment = std::make_unique<SliderAttachment> (apvts, "grainMs", grainMsSlider);
    overlapAttachment = std::make_unique<SliderAttachment> (apvts, "overlap", overlapSlider);
    stretchAttachment = std::make_unique<SliderAttachment> (apvts, "stretch", stretchSlider);
    warpAttachment = std::make_unique<SliderAttachment> (apvts, "warp", warpSlider);
    spectralChaosAttachment = std::make_unique<SliderAttachment> (apvts, "spectralChaos", spectralChaosSlider);
    hybridMixAttachment = std::make_unique<SliderAttachment> (apvts, "hybridMix", hybridMixSlider);
    seedAttachment = std::make_unique<SliderAttachment> (apvts, "seed", seedSlider);
    loopAttachment = std::make_unique<ButtonAttachment> (apvts, "loop", loopButton);

    updatePresetColourTheme();
}

MicrosoundSymphonyAudioProcessorEditor::~MicrosoundSymphonyAudioProcessorEditor()
{
    presetBox.setLookAndFeel (nullptr);
}

void MicrosoundSymphonyAudioProcessorEditor::setupSlider (juce::Slider& s, const juce::String& name)
{
    s.setName (name);
    s.setSliderStyle (juce::Slider::LinearVertical);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 78, 22);
    s.setColour (juce::Slider::backgroundColourId, juce::Colour (0xFFBEBEBE));
    s.setColour (juce::Slider::trackColourId, juce::Colour (0xFF8B8B8B));
    s.setColour (juce::Slider::thumbColourId, juce::Colour (0xFF5A5A5A));
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xFF202020));
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xFF7C7C7C));
    s.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xFFDADADA));
    addAndMakeVisible (s);
}

void MicrosoundSymphonyAudioProcessorEditor::updatePresetColourTheme()
{
    const int idx = presetBox.getSelectedId() - 1;
    const int mode = MicrosoundSymphonyAudioProcessor::getPresetMode (idx);
    const auto c = modeColour (mode);

    presetBox.setColour (juce::ComboBox::outlineColourId, c.darker (0.35f));
    presetBox.setColour (juce::ComboBox::arrowColourId, c.darker (0.55f));
    presetBox.setColour (juce::ComboBox::backgroundColourId, c.withAlpha (0.95f));
    repaint (presetBox.getBounds().expanded (6));
}

void MicrosoundSymphonyAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto full = getLocalBounds();
    paintBrushedMetal (g, full);

    auto r = full.reduced (12);
    auto hero = r.removeFromTop (82);
    auto body = r;
    auto left = body.removeFromLeft (300);
    body.removeFromLeft (10);

    auto drawBevelPanel = [&g] (juce::Rectangle<int> rect, float radius)
    {
        juce::ColourGradient fill (juce::Colour (0xFFD7D7D7), (float) rect.getX(), (float) rect.getY(),
                                   juce::Colour (0xFFBABABA), (float) rect.getX(), (float) rect.getBottom(), false);
        g.setGradientFill (fill);
        g.fillRoundedRectangle (rect.toFloat(), radius);

        g.setColour (juce::Colour (0x88FFFFFF));
        g.drawRoundedRectangle (rect.toFloat().reduced (0.5f), radius, 1.0f);
        g.setColour (juce::Colour (0xAA6E6E6E));
        g.drawRoundedRectangle (rect.toFloat().reduced (1.5f), radius - 1.0f, 1.0f);
    };

    drawBevelPanel (hero, 16.0f);

    auto leftTop = left.removeFromTop (300);
    auto leftBottom = left;
    drawBevelPanel (leftTop, 12.0f);
    drawBevelPanel (leftBottom, 12.0f);
    drawBevelPanel (body, 12.0f);

    g.setColour (juce::Colour (0xFF2D2D2D));
    g.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    g.drawFittedText ("CONTROL", leftTop.reduced (14, 8).removeFromTop (22), juce::Justification::centredLeft, 1);
    g.drawFittedText ("ACTIONS", leftBottom.reduced (14, 8).removeFromTop (22), juce::Justification::centredLeft, 1);

    auto bodyTop = body.reduced (14, 10).removeFromTop (24);
    g.drawFittedText ("STRUCTURE", bodyTop.removeFromLeft (220), juce::Justification::centredLeft, 1);
    g.drawFittedText ("TIMBRE", bodyTop.removeFromRight (180), juce::Justification::centredRight, 1);

    g.setColour (juce::Colour (0xFF1F1F1F));
    g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::plain)));
    g.drawFittedText ("Mode", modeBox.getBounds().translated (0, -18), juce::Justification::centredLeft, 1);
    g.drawFittedText ("Preset", presetBox.getBounds().translated (0, -18), juce::Justification::centredLeft, 1);
    g.drawFittedText ("Beauty Scene", beautySceneBox.getBounds().translated (0, -18), juce::Justification::centredLeft, 1);
    g.drawFittedText ("Micro Rate", microRateBox.getBounds().translated (0, -18), juce::Justification::centredLeft, 1);

    auto drawName = [&g] (juce::Slider& s)
    {
        auto b = s.getBounds();
        auto labelArea = b.withY (b.getY() - 18).withHeight (16);
        g.setColour (juce::Colour (0xE8FFFFFF));
        g.fillRoundedRectangle (labelArea.toFloat().expanded (2.0f, 1.0f), 3.0f);
        g.setColour (juce::Colour (0xFF1E1E1E));
        g.drawFittedText (s.getName(), labelArea, juce::Justification::centred, 1);
    };

    drawName (burstMsSlider);
    drawName (densitySlider);
    drawName (outSecondsSlider);
    drawName (grainMsSlider);
    drawName (overlapSlider);
    drawName (stretchSlider);
    drawName (warpSlider);
    drawName (spectralChaosSlider);
    drawName (hybridMixSlider);
    drawName (seedSlider);
}

void MicrosoundSymphonyAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);

    auto hero = area.removeFromTop (82);
    titleLabel.setBounds (hero.reduced (18, 10).withHeight (42));

    area.removeFromTop (10);

    auto left = area.removeFromLeft (300);
    area.removeFromLeft (10);
    auto right = area;

    auto leftTop = left.removeFromTop (300).reduced (14);
    left.removeFromTop (10);
    auto leftBottom = left.reduced (14);

    int y = leftTop.getY() + 28;
    modeBox.setBounds (leftTop.getX(), y, leftTop.getWidth(), 28); y += 46;
    presetBox.setBounds (leftTop.getX(), y, leftTop.getWidth(), 28); y += 46;
    beautySceneBox.setBounds (leftTop.getX(), y, leftTop.getWidth(), 28); y += 46;
    microRateBox.setBounds (leftTop.getX(), y, leftTop.getWidth(), 28); y += 46;
    loopButton.setBounds (leftTop.getX(), y, leftTop.getWidth(), 26);

    auto actionArea = leftBottom.withTrimmedTop (26);
    renderButton.setBounds (actionArea.removeFromTop (40));
    actionArea.removeFromTop (9);
    applyBeautyButton.setBounds (actionArea.removeFromTop (40));
    actionArea.removeFromTop (9);
    exportButton.setBounds (actionArea.removeFromTop (40));

    auto sliderPanel = right.reduced (14);
    sliderPanel.removeFromTop (30);

    auto row1 = sliderPanel.removeFromTop ((sliderPanel.getHeight() - 16) / 2);
    sliderPanel.removeFromTop (16);
    auto row2 = sliderPanel;

    auto layoutRow = [] (juce::Rectangle<int> row, std::array<juce::Slider*, 5> sliders)
    {
        constexpr int gap = 12;
        const int sliderW = (row.getWidth() - (gap * 4)) / 5;
        for (auto* s : sliders)
        {
            s->setBounds (row.removeFromLeft (sliderW));
            row.removeFromLeft (gap);
        }
    };

    layoutRow (row1, { &burstMsSlider, &densitySlider, &outSecondsSlider, &grainMsSlider, &overlapSlider });
    layoutRow (row2, { &stretchSlider, &warpSlider, &spectralChaosSlider, &hybridMixSlider, &seedSlider });
}
