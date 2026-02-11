# Microsound Symphony (JUCE)

This project is a JUCE-based plugin/standalone synth that renders a very short high-sample-rate burst and unfolds it into multi-second textures with Granular, Spectral, or Hybrid modes.

## Build

### Configure (use local JUCE source tree, recommended in this environment)

```bash
cmake -S . -B build -DMICROSOUND_JUCE_SOURCE_DIR=/Users/md/JUCE
```

### Configure (auto-fetch JUCE)

```bash
cmake -S . -B build -DMICROSOUND_FETCH_JUCE=ON
```

### Configure (use local JUCE package)

```bash
cmake -S . -B build -DMICROSOUND_FETCH_JUCE=OFF -DCMAKE_PREFIX_PATH=/path/to/JUCE
```

### Build

```bash
cmake --build build --config Release
```

Artifacts are generated for `Standalone`, `VST3`, and `AU`.

## Source files

- `Source/PluginProcessor.*`
- `Source/PluginEditor.*`
