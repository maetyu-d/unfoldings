# unfoldings

unfoldings is an experimental microsound instrument that generates ultra-dense bursts at extreme internal sample rates, then unfolds them into extended textures through spectral, granular, and morphogenetic processes. The project takes the form of a JUCE-based plugin and standalone synth.

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
