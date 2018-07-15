#include <benchmark/benchmark.h>
#include <CamEncoder/av_yuv/av_rgb2yuv.h>
#include <CamEncoder/av_yuv/av_rgba2yuv.h>
#include <CamEncoder/av_yuv/av_rgba2yuv_ssse3.h>

class rgb2yuv_fixture : public ::benchmark::Fixture
{
public:
    void SetUp(const ::benchmark::State& state)
    {
        width = state.range(0);
        height = state.range(1);
        const auto total = width * height;

        rgb_buffer.resize(total * 4);
        yuv420_buffer.resize(total * 3);

        source[0] = rgb_buffer.data();
        source_stride[0] = width * 4;

        auto y = yuv420_buffer.data();
        auto u = y + total;
        auto v = u + (total >> 1);

        destination[0] = y;
        destination[1] = u;
        destination[2] = v;

        destination_stride[0] = width;
        destination_stride[1] = width >> 1;
        destination_stride[2] = width >> 1;
    }

    void TearDown(const ::benchmark::State& state)
    {
    }

public:
    std::vector<uint8_t> rgb_buffer;
    uint8_t * source[3] = {};
    int source_stride[3] = {};

    std::vector<uint8_t> yuv420_buffer;
    uint8_t *destination[8] = {};
    int destination_stride[3] = {};

    int width{0};
    int height{0};
};


BENCHMARK_DEFINE_F(rgb2yuv_fixture, bgra2yuv420p)(benchmark::State& st)
{
    for (auto _ : st) {
        bgra2yuv420p(destination, destination_stride, source, width, height, source_stride);
    }
}

BENCHMARK_REGISTER_F(rgb2yuv_fixture, bgra2yuv420p)
    ->Args({ 128, 128})
    ->Args({ 256, 256})
    ->Args({ 512, 512})
    ->Args({ 1024, 1024})
    ->Args({ 2048, 2048})
    ->Args({ 4096, 4096});

BENCHMARK_DEFINE_F(rgb2yuv_fixture, bgra2yuv420p_sse)(benchmark::State& st)
{
    for (auto _ : st) {
        bgra2yuv420p_sse(destination, destination_stride, source, width, height, source_stride);
    }
}

BENCHMARK_REGISTER_F(rgb2yuv_fixture, bgra2yuv420p_sse)
    ->Args({ 128, 128})
    ->Args({ 256, 256})
    ->Args({ 512, 512})
    ->Args({ 1024, 1024})
    ->Args({ 2048, 2048})
    ->Args({ 4096, 4096});
