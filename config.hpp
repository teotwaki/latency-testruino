#ifndef LATENCY_TESTRUINO_CONFIG_HPP
#define LATENCY_TESTRUINO_CONFIG_HPP

// Tone generation
#define TONE_FREQUENCY 1000 // hz
#define TONE_DURATION  50 // ms

// Test stuff
#define MAX_TEST_DURATION    3000 // ms
#define TONE_POWER_THRESHOLD 50 // some loudness unit?
#define TONE_DURATION_THRESHOLD 30 // ms
#define TEST_COUNT 5 // iterations

#define    SAMPLING_FREQ (F_CPU / (128 * 13.0)) // 9.615 kHz
const long PHASE_INC = round(TONE_FREQUENCY / SAMPLING_FREQ * (1L << 16));
#define    LOG_TAU 6 // tau = 64 / SAMPLING_FREQ = 6.656 ms

#endif // LATENCY_TESTRUINO_CONFIG_HPP
