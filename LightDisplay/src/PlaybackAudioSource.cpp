#include <PlaybackAudioSource.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <RAII.h>

PlaybackAudioSource* PlaybackAudioSource::src = nullptr;

inline void assertSuccess(HRESULT res, const char * msg = "WSAPI Failure") {
	if (FAILED(res)) {
		const auto s = std::string(msg) + " :" + std::to_string(res);
		fprintf(stderr, s.c_str());
		throw std::runtime_error(s);
	}
}
constexpr auto clsid_mm_device_enumerator = __uuidof(MMDeviceEnumerator);
constexpr auto iid_imm_device_enumerator = __uuidof(IMMDeviceEnumerator);
constexpr auto iid_audio_client = __uuidof(IAudioClient);
constexpr auto iid_audio_capture_client = __uuidof(IAudioCaptureClient);
constexpr auto reftime_millisecond = 10000;


inline auto initDeviceEnum() {
	IMMDeviceEnumerator* deviceEnum;
	const auto res = CoCreateInstance(clsid_mm_device_enumerator, nullptr, CLSCTX_ALL,
		iid_imm_device_enumerator, reinterpret_cast<void**>(&deviceEnum));
	assertSuccess(res, "Init failure");
	return ResourceWrapper(deviceEnum, [](decltype(deviceEnum) ptr) {
		ptr->Release();
	});
}

template<typename T>
inline auto initDevice(ResourceWrapper<IMMDeviceEnumerator*, T>& deviceEnum) {
	IMMDevice* device;
	//eRender for loopback, eCapture for normal mic
	const auto res = deviceEnum->GetDefaultAudioEndpoint(eRender, eConsole, &device);
	assertSuccess(res, "Can't get default endpoint");
	return ResourceWrapper(device, [](decltype(device) ptr) {
		ptr->Release();
	});
}
template<typename T>
inline auto activateDevice(ResourceWrapper<IMMDevice*, T>& device) {
	IAudioClient* audioClient;
	const auto res = device->Activate(iid_audio_client, CLSCTX_ALL, nullptr,
		reinterpret_cast<void**>(&audioClient));
	assertSuccess(res, "Can't activate device");
	return audioClient;
}
inline void initAudioClient(IAudioClient* audioClient, tWAVEFORMATEX * fmt) {
	const REFERENCE_TIME requestDuration = 1000 * reftime_millisecond;
	//0 instead of AUDCLNT_STREAMFLAGS_LOOPBACK for normal
	const auto res = 
	audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, requestDuration, 0,
		fmt, nullptr);
	assertSuccess(res, "Can't initialize audio client");
}
inline auto initCaptureClient(IAudioClient* audioClient) {
	IAudioCaptureClient* captureClient;
	const auto res = 
		audioClient->GetService(iid_audio_capture_client, reinterpret_cast<void**>(&captureClient));
	assertSuccess(res, "Can't get capture client service");
	return captureClient;
}
inline auto getBufferSeconds(IAudioClient* audioClient, tWAVEFORMATEX * mixFmt) {
	uint32_t bufferSize;
	assertSuccess(audioClient->GetBufferSize(&bufferSize), "Can't get buffer size");
	return 1000.0 * reftime_millisecond * bufferSize / mixFmt->nSamplesPerSec;
}

PlaybackAudioSource::PlaybackAudioSource() : initializationStack(0)
{
	try {
		auto deviceEnum = initDeviceEnum();
		auto device = initDevice(deviceEnum);

		audioClient = activateDevice(device);
		initializationStack = 1;
		assertSuccess(audioClient->GetMixFormat(&mixFmt), "Failed to get mix fmt");
		initializationStack = 2;

		initAudioClient(audioClient, mixFmt);
		captureClient = initCaptureClient(audioClient);
		initializationStack = 3;

		bufferLenSec = getBufferSeconds(audioClient, mixFmt);
		assertSuccess(audioClient->Start(), "Cannot start");
		initializationStack = 4;
	}
	catch (const std::exception& e) {
		cleanup();
		throw e;
	}

}
PlaybackAudioSource::~PlaybackAudioSource() {
	cleanup();
}
void PlaybackAudioSource::cleanup()
{
	switch (initializationStack) {
	case 4:
		audioClient->Stop();
		[[fallthrough]];
	case 3:
		captureClient->Release();
		[[fallthrough]];
	case 2:
		CoTaskMemFree(mixFmt);
		[[fallthrough]];
	case 1:
		audioClient->Release();
	}
}

uint32_t PlaybackAudioSource::nextPacketSize() const {
	if (nextPacket.canRead()) return nextPacket.size();
	else {
		uint32_t r;
		assertSuccess(captureClient->GetNextPacketSize(&r));
		nextPacket.nextPacket(r);
		return r;
	}
}
inline signal_t rawDataToSignal(uint8_t* data, uint32_t sampleNum, tWAVEFORMATEX * fmt) {
	signal_t result;
	result.reserve(sampleNum);
	const auto increment = fmt->nChannels * fmt->wBitsPerSample / 8;
	const auto bytesPerSamp = fmt->wBitsPerSample / 8;
	for (decltype(sampleNum) i = 0; i < sampleNum * increment; i += increment) {
		int64_t res = 0;
		for (auto j = 0; j < fmt->nChannels; ++j) {
			int32_t a;
			memcpy(&a, &data[i + j * bytesPerSamp], bytesPerSamp);
			res += a;
		}
		res /= fmt->nChannels;
		result.emplace_back((double)res);
	}
	return result;
}
signal_t PlaybackAudioSource::sample(std::chrono::milliseconds)
{
	uint32_t pSize;
	signal_t samples;
	if ((pSize = nextPacketSize()) != 0) {
		uint8_t* data;
		uint32_t numFramesAvailable;
		DWORD flags;
		assertSuccess(captureClient->GetBuffer(&data, &numFramesAvailable, &flags, nullptr, nullptr));
		const auto sig = rawDataToSignal(data, numFramesAvailable, mixFmt);
		captureClient->ReleaseBuffer(numFramesAvailable);
		nextPacket.consume();
		return sig;
	}
	return signal_t();
}

double PlaybackAudioSource::samplingFrequency() const
{
	return mixFmt->nSamplesPerSec;
}

bool PlaybackAudioSource::ready() const
{
	return nextPacketSize() > 0;
}

PlaybackAudioSource* PlaybackAudioSource::get()
{
	if (src == nullptr) {
		static PlaybackAudioSource audio;
		src = &audio;
	}
	return src;
}
