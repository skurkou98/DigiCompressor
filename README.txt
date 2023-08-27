"DigiCompressor" plugin by Stanislau Kurkou

This plugin was built in Visual Studio (2019) using JUCE library code. The goal of this project is to build a multiband 
compressor with traditional controls split into 3 distinct adjustable bands. Currently, only the processing of the 
JUCE's compression algorithm has been implemented with the following controls:
-ATTACK: The time it takes for DigiCompressor to begin reducing gain after input signal goes beyond THRESHOLD. (5-500ms)
-RELEASE: The time it takes for DigiCompressor to stop reducing gain after input signal goes below THRESHOLD. (5-500ms)
-THRESHOLD: The level at which DigiCompressor will reduce gain on the input signal. (depends on definition of -dB in host app)
-RATIO: When the input signal goes beyond the THRESHOLD, this adjusts the strength of the reduction in output level. (1x-100x)
-BYPASS: Temporarily disables DigiCompressor to allow user to reference changes. (On/Off)

This plugin facilitates the use of a generic UI via JUCE library code and allows for the user to retain their previous 
input settings when reopening a session.

DigiCompressor is designed for use as a VST3 plugin. Simply download and add the file path of "DigiCompressor.vst3" to the 
plugins folder for your DAW and re-scan. TableTennis has been tested using Reaper v6.50 (2022).