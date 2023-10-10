#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1024
#define NUM_CHANNELS 1
#define OUTPUT_FILE "output.wav"

int main() {
    PaStream *stream;
    PaError err;
    PaStreamParameters inputParameters;
    PaSampleFormat sampleFormat = paFloat32;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "Error initializing PortAudio: %s\n", Pa_GetErrorText(err));
        return -1;
    }

    // Set up input parameters
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        Pa_Terminate();
        return -1;
    }
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = sampleFormat;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    // Open stream
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL,  // No output
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        NULL,  // No callback
        NULL   // No user data
    );

    if (err != paNoError) {
        fprintf(stderr, "Error opening stream: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return -1;
    }

    // Start stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Error starting stream: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return -1;
    }

    printf("Recording...\n");

    // Record for 5 seconds
    Pa_Sleep(5000);

    // Stop stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Error stopping stream: %s\n", Pa_GetErrorText(err));
    }

    // Close stream
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Error closing stream: %s\n", Pa_GetErrorText(err));
    }

    // Terminate PortAudio
    Pa_Terminate();

    printf("Recording complete. Audio saved to %s\n", OUTPUT_FILE);

    return 0;
}