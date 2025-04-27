# 🎶 SOUND_COMPOSER

A simple C project for managing and visualizing sound and image assets using [raylib](https://www.raylib.com/) and [miniaudio](https://github.com/mackron/miniaudio).  
This project sets the foundation for building a **basic sound composer** or audio visualization tool.

---

## 📂 Project Structure

```plaintext
SOUND_COMPOSER/
├── assets/            # Images and audio files
│   ├── Group 1 (1).png
│   ├── Group 2 (1).png
│   ├── Group 3 (1).png
│   ├── Group 5.png
│   ├── sample1.wav
│   └── sample2.wav
├── docs/               # (Optional) Project documentation
├── output/             # (Optional) Output files
├── src/                # Source code
│   ├── build/          # Compiled executables and object files
│   ├── main.c          # Main application logic
│   ├── util.c          # Utility functions
│   ├── util.h          # Utility function headers
│   ├── miniaudio.h     # Embedded audio library (single header)
│   └── Makefile        # Makefile for building the project
├── .vscode/            # VSCode project settings (optional)
├── .idea/              # JetBrains IDE settings (optional)
├── .cache/             # Build cache (optional)
├── README.md           # Project documentation (this file)

```
## Features

* Read WAV file headers and PCM data.
* Write PCM data (implementation details needed).
* Reverse audio data.
* Apply various low-pass filters (simple, Butterworth 3rd & 4th order).
* Visualize audio waveforms using raylib.
* Zoom functionality for waveform display.
* Basic audio recording using miniaudio.
* Data structures for managing audio tracks and UI elements.

## Dependencies

* **raylib.h:** For graphics and UI elements (like waveform drawing and draggable boxes).
* **miniaudio.h:** For cross-platform audio playback and recording.
* Standard C libraries: `stdio.h`, `stdlib.h`, `string.h`, `stdint.h`, `math.h`.

## Data Structures

* **`WAVHeader`**: Holds the metadata of a WAV file according to the standard WAV format specification.
* **`Node`**: A node for a linked list, likely used to store chunks of PCM audio data (`int16_t *pcm`).
* **`Track`**: Represents an audio track, potentially holding the head of a linked list (`Node *head`) of audio data chunks or a single continuous block (`int16_t *pcm`).
* **`DraggableBox`**: A structure likely used with raylib for creating draggable UI elements, containing position (`Rectangle`), state (`dragging`, `offset`), and appearance (`Color`).
* **`Screen` (enum)**: Defines different states or screens for the application (e.g., `Load`, `Real`, `Static`).

## Functions

### File I/O & Data Reading

* **`void read_wav_header(const char* filename)`**: Reads the header information from the specified WAV file.
* **`int16_t* read_pcm_data(const char* filename)`**: Reads the Pulse Code Modulation (PCM) audio data from the specified WAV file. Returns a pointer to the data.
* **`void write_pcm_data()`**: Writes PCM data back to a file (details like filename and data source need clarification based on implementation).

### Data Structure Management

* **`Node* insert(int16_t *data)`**: Inserts a block of PCM data into the linked list. Returns the new node.
* **`void free_list()`**: Frees the memory allocated for the linked list storing audio data.

### Audio Manipulation & Effects

* **`void reverse(const char *input_file, const char *output_file)`**: Reverses the audio data from `input_file` and saves it to `output_file`.
* **`void low_filter(const char *input_file, const char *output_file2)`**: Applies a simple low-pass filter to the audio data.
* **`void butterworth_low_filter(const char *input_file, const char *output_file)`**: Applies a Butterworth low-pass filter (order not specified in name, likely 2nd order based on other function names).
* **`void butterworth_filter_4th_order(const char *input_file, const char *output_file)`**: Applies a 4th order Butterworth low-pass filter.
* **`void butterworth_filter_3rd_order(const char *input_file, const char *output_file)`**: Applies a 3rd order Butterworth low-pass filter.

### Waveform Visualization (Requires raylib)

* **`void draw_waveform(const char *filepath, int num_samples)`**: Draws the waveform of the audio data from the specified file path. `num_samples` likely controls the section or resolution of the waveform displayed.
* **`void zoom(int *num_samples)`**: Modifies the `num_samples` variable, likely used to zoom in or out of the waveform display handled by `draw_waveform`.

### Audio Recording (Requires miniaudio)

* **`void StartAudioRecording(void)`**: Initializes and starts the audio recording process.
* **`void StopAudioRecording(void)`**: Stops the audio recording process.
* **`void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)`**: The callback function required by miniaudio to handle incoming audio data during recording.

## Global Variables

* **`extern WAVHeader header;`**: A global instance of the WAVHeader struct, likely holding the header info of the currently loaded file.
* **`extern Node* head;`**: A global pointer to the head of the linked list storing audio data chunks.

## Building

*(Instructions on how to compile the code would go here. This typically involves using a C compiler like GCC and linking against raylib and miniaudio.)*

Example using GCC (assuming source files `main.c`, `util.c` and libraries are set up):

```bash
gcc main.c util.c -o sound_composer -lraylib -lminiaudio -lm # Add other necessary flags


## Building

This project uses `make` for building. Ensure you have `gcc`, `make`, and the necessary libraries (`raylib`) installed.

1.  Navigate to the directory containing the `Makefile` (likely the `src` directory based on your file structure image).
2.  Run the make command:
    ```bash
    make
    ```
    or
    ```bash
    make all
    ```
    This will compile the source files (`main.c`, `util.c`) into object files located in `../build/obj/` and then link them into the final executable located at `../build/bin/sound_composer`.

3.  *(Optional)* To clean up build files (object files and the executable):
    ```bash
    make clean
    ```

**Note:** If you encounter linking errors related to `miniaudio`, you may need to add `-lminiaudio` to the `LIBS` variable in your `Makefile`.

## Usage

After successfully building the project:

1.  Run the executable directly from the directory containing the `Makefile`:
    ```bash
    make run
    ```
    This command first ensures the project is built (`all` dependency) and then executes `../build/bin/sound_composer`.

2.  Alternatively, you can run the executable directly by specifying its path:
    ```bash
    ../build/bin/sound_composer
    ```
   

