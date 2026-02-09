# Audio-Plugin-Template

## Overview

Audio-Plugin-Template is a JUCE-based audio plugin starter template designed for accelerating development. It provides a straightforward structure and automates common setup tasks, allowing you to focus on your plugin code rather than boilerplate configuration.

* Supports local JUCE checkout (via `JUCE_LIB`) or automatic fetching using CMake FetchContent (if environment variable `JUCE_LIB` is not set).

* JUCE modules are statically linked into the plugin for easy distribution.

* The project is structured to allow easy renaming of plugin classes, files, and CMake targets via a simple JSON configuration in `config/config.json`.

## Instructions

### Step 1 - Configure your Plugin (Optional)
Before building the plugin, customize your settings in `config/config.json`.

#### Example `config.json`:
```json
{
  "plugin_name": "NewPlugin",
  "company_name": "NewCompany",
  "plugin_code": "0000",
  "manufacturer_code": "AAAA",
  "is_synth": false,
  "needs_midi_input": false,
  "needs_midi_output": false,
  "formats": "VST3",
}
```

Notes:

* `plugin_name` is used for class names, includes, and CMake targets.

* `plugin_code` and `manufacturer_code` must be 4-character strings.

Once configured, run the script to apply all renames automatically:

```
python config.py
```


Note: This script should be run once on a fresh clone. It renames directories, files, class names, includes, and updates CMake targets according to `config.json`.

### Step 2 - Build Project
You can build the plugin in **Debug** or **Release** mode. In your project root directory execute:

#### Debug
```
cmake -B build
cmake --build build
```

#### Release
```
cmake -B build
cmake --build build --config Release
```

### Step 3 - Run and Test

The plugin will be available in the chosen format (VST3 by default). You can open it in your DAW for testing but make sure to copy `build\plugin\PluginTemplate_artefacts\Debug\VST3\PluginTemplate.vst3` folder in `C:\Program Files\Common Files\VST3` if you are using Windows, **or** just add the path to VST Plugin paths in your DAW and rescan for new plugins. [This](https://www.youtube.com/watch?v=aPMTYlMpbj0) video is helpful if you want to see how to use the Visual Studio Debugger with Reaper (but of course you can choose any other DAW of your preference).

## Notes and Best Practices

* Valid Plugin Name: Ensure `plugin_name` is a valid C++ identifier (letters, digits, underscores; cannot start with a digit).

* Cleaning Builds: After renaming via `config.py`, delete the build directory before rebuilding to avoid stale CMake references.