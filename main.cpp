#include <iostream>
#include <vector>
#include <portaudio.h>
#include <fstream> // Include the ofstream header for file writing

#define SAMPLE_RATE 10000
#define FRAMES_PER_BUFFER 1024
#define NUM_CHANNELS 1
#define RECORDING_DURATION_SECONDS 3
#define OUTPUT_FILE "output.csv"

int main() {
    PaStream *stream;
    PaError err;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Error initializing PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return -1;
    }

    // Set up input parameters
    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        std::cerr << "Error: No default input device." << std::endl;
        Pa_Terminate();
        return -1;
    }
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // Open the audio stream
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        nullptr,  // No output
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        nullptr,  // No callback
        nullptr   // No user data
    );

    if (err != paNoError) {
        std::cerr << "Error opening stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return -1;
    }

    // Start the audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Error starting stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1;
    }

    std::cout << "Recording..." << std::endl;

    // Create a vector to store the recorded audio data
    std::vector<float> audioData;
    audioData.reserve(SAMPLE_RATE * NUM_CHANNELS * RECORDING_DURATION_SECONDS);

    // Record for the specified duration
    for (int i = 0; i < RECORDING_DURATION_SECONDS * SAMPLE_RATE / FRAMES_PER_BUFFER; i++) {
        float buffer[FRAMES_PER_BUFFER];
        err = Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
        if (err != paNoError) {
            std::cerr << "Error reading stream: " << Pa_GetErrorText(err) << std::endl;
            break;
        }

        // Append the recorded data to the vector
        audioData.insert(audioData.end(), buffer, buffer + FRAMES_PER_BUFFER);
    }

    // Stop and close the audio stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Error stopping stream: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "Error closing stream: " << Pa_GetErrorText(err) << std::endl;
    }

    // Terminate PortAudio
    Pa_Terminate();

    std::cout << "Recording complete." << std::endl;

    // Use ofstream to save the audio data to a CSV file
    std::ofstream outputFile(OUTPUT_FILE);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << OUTPUT_FILE << std::endl;
        return -1;
    }

    // Write the audio data to the CSV file
    for (size_t i = 0; i < audioData.size(); i++) {
        outputFile << audioData[i];
        if (i < audioData.size() - 1) {
            outputFile << ",";
        }
    }

    outputFile.close();

    std::cout << "Audio saved to " << OUTPUT_FILE << std::endl;

    return 0;
}
