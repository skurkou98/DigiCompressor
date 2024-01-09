"DigiCompressor" plugin by Stanislau Kurkou

This plugin was built in Visual Studio (2019) using JUCE library code. The compression algorithm has been implemented with 
the following controls:
-ATTACK: How quickly DigiCompressor begins reducing gain after input signal goes above THRESHOLD. (5-500ms)
-RELEASE: How quickly DigiCompressor stops reducing gain after input signal goes below THRESHOLD. (5-500ms)
-THRESHOLD: Level at which DigiCompressor will reduce gain on the input signal. (-dB)
-RATIO: The strength of the reduction in output level after THRESHOLD is breached. (1.0x-100.0x)
-BYPASS: Temporarily disables DigiCompressor to allow user to reference changes. (On/Off)

This plugin facilitates the use of a generic UI and allows for the user to retain their previous input settings when 
reopening a session.

DigiCompressor is designed for use as a VST3 plugin. Simply download and add the file path of "DigiCompressor.vst3" to the 
plugins folder for your DAW and re-scan. DigiCompressor has been tested using Reaper v6.50 (2022).
