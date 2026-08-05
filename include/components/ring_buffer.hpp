// Generic ring buffer structure to be used for the audio buffer

#pragma once

#include <array>
#include <atomic>
#include <cstddef>

template <typename T, size_t size>
class RingBuffer {
public:
	bool push(const T& value) {
		size_t current_write = write.load(std::memory_order_relaxed);
		size_t current_read = read.load(std::memory_order_acquire);

		buffer[current_write] = value;
		size_t next_write = (current_write + 1) % size;

		if (next_write == current_read) {
			read.store((current_read + 1) % size, std::memory_order_release);
		}

		write.store ( (current_write + 1) % size, std::memory_order_release);

		return true;
	}

	bool pop (T& value) {

		size_t current_read = read.load(std::memory_order_relaxed);
		size_t current_write = write.load(std::memory_order_acquire);

		if (current_read == current_write) {
			return false;
		}

		value = buffer[current_read];
		read.store ( (current_read + 1) % size, std::memory_order_release);

		return true;
	}

	size_t current_size() {
		size_t current_write = write.load(std::memory_order_acquire);
		size_t current_read = read.load(std::memory_order_acquire);

		if (current_write >= current_read) {
			return current_write - current_read;
		} else {
			return size - (current_read - current_write);
		}
	}

private:
	std::array<T, size> buffer {};

	std::atomic<size_t> write {0};
	std::atomic<size_t> read {0};
};