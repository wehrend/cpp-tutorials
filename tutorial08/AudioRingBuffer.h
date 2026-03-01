#ifndef AUDIO_RING_BUFFER_H
#define AUDIO_RING_BUFFER_H

#include <atomic>
#include <cstddef>

/**
 * @brief A Single-Producer Single-Consumer (SPSC) Ring Buffer.
 * Designed for passing audio samples from the Audio Thread to the UI.
 */
template <typename T, size_t Size>
class AudioRingBuffer {
private:
    T buffer[Size] = {0};
    // Atomic index to prevent race conditions between threads
    std::atomic<size_t> writeIndex{0};

public:
    AudioRingBuffer() = default;

    /**
     * @brief Writes a single sample to the buffer.
     * Called by the "Producer" (Audio Thread).
     */
    void write(T sample) {
        size_t currentWrite = writeIndex.load(std::memory_order_relaxed);
        buffer[currentWrite] = sample;
        
        // Ensure the sample is written before the index is updated
        writeIndex.store((currentWrite + 1) % Size, std::memory_order_release);
    }

    /**
     * @brief Reads a sample at a specific relative index.
     * Called by the "Consumer" (UI/Renderer).
     */
    T read(size_t index) const {
        return buffer[index % Size];
    }

    /**
     * @brief Returns the current write position.
     * Use acquire semantics to ensure we see the most recent writes.
     */
    size_t getWritePointer() const {
        return writeIndex.load(std::memory_order_acquire);
    }

    size_t getSize() const { return Size; }
};

#endif