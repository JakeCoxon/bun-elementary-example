#include <elem/Runtime.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

struct DeviceProxy;

std::vector<float> scratchData;
ma_device device;
std::unique_ptr<DeviceProxy> proxy;

extern "C" {

typedef void (*elem_callback_type)(const char* string);
elem_callback_type elem_callback;

void set_elem_callback(elem_callback_type callback) {
  elem_callback = callback;
}

// A simple struct to proxy between the audio device and the Elementary engine
struct DeviceProxy {
    DeviceProxy(double sampleRate, size_t blockSize)
        : scratchData(2 * blockSize), runtime(sampleRate, blockSize)
    {}

    void process(float* outputData, size_t numChannels, size_t numFrames)
    {
        // We might hit this the first time around, but after that should be fine
        if (scratchData.size() < (numChannels * numFrames))
            scratchData.resize(numChannels * numFrames);

        auto* deinterleaved = scratchData.data();
        std::array<float*, 2> ptrs {deinterleaved, deinterleaved + numFrames};

        // Elementary is happy to accept audio buffer data as an input to be
        // processed dynamically, such as applying effects, but here for simplicity
        // we're just going to produce output
        runtime.process(
            nullptr,
            0,
            ptrs.data(),
            numChannels,
            numFrames,
            nullptr
        );

        for (size_t i = 0; i < numChannels; ++i)
        {
            for (size_t j = 0; j < numFrames; ++j)
            {
                outputData[i + numChannels * j] = deinterleaved[i * numFrames + j];
            }
        }
    }

    std::vector<float> scratchData;
    elem::Runtime<float> runtime;
};

// Our main audio processing callback from the miniaudio device
void audioCallback(ma_device* pDevice, void* pOutput, const void* /* pInput */, ma_uint32 frameCount)
{
    auto* proxy = static_cast<DeviceProxy*>(pDevice->pUserData);

    auto numChannels = static_cast<size_t>(pDevice->playback.channels);
    auto numFrames = static_cast<size_t>(frameCount);

    proxy->process(static_cast<float*>(pOutput), numChannels, numFrames);
}

void app_init_audio() {
  

  ma_device_config deviceConfig;

  // XXX: I don't see a way to ask miniaudio for a specific block size. Let's just allocate
  // here for 1024 and resize in the first callback if we need to.
  proxy = std::make_unique<DeviceProxy>(44100.0, 1024);

  deviceConfig = ma_device_config_init(ma_device_type_playback);

  deviceConfig.playback.pDeviceID = nullptr;
  deviceConfig.playback.format    = ma_format_f32;
  deviceConfig.playback.channels  = 2;
  deviceConfig.sampleRate         = 44100;
  // deviceConfig.periodSizeInFrames = 1024;
  // deviceConfig.noFixedSizedCallback = false;
  deviceConfig.dataCallback       = audioCallback;
  deviceConfig.pUserData          = proxy.get();

  ma_result result = ma_device_init(nullptr, &deviceConfig, &device);

  if (result != MA_SUCCESS) {
      std::cout << "Failed to start the audio device! Exiting..." << std::endl;
      std::exit(0);
  }

  if (ma_device_start(&device) != MA_SUCCESS) {
    std::cout << "Failed to start the playback! Exiting..." << std::endl;
    ma_device_uninit(&device);
    std::exit(0);
  }
}

void app_uninit_audio() {
  ma_device_uninit(&device);
  proxy = nullptr;
}

void elem_post_instructions(const char* string) {
  proxy->runtime.applyInstructions(elem::js::parseJSON(string));
}

void elem_process_events() {

  elem::js::Array batch;

  proxy->runtime.processQueuedEvents([&batch](std::string const& type, elem::js::Value event) {

    auto object = elem::js::Object({
        {"type", type},
        {"event", event}
    });
    batch.push_back(object);

  });
  elem_callback(serialize(batch).c_str());
}

} // extern "C"