#include <AudioSource.h>
class PlaybackAudioSource : public AudioSource {
private:
	static PlaybackAudioSource* src;
	struct tWAVEFORMATEX* mixFmt;
	struct IAudioClient* audioClient;
	struct IAudioCaptureClient* captureClient;
	double bufferLenSec;
	unsigned char initializationStack; ///< Number that increments as it goes through initialization


	class {
		bool isReady = false; ///< True iff packetSize is the size of the next packet. False if it must be recalculated
		uint32_t packetSize = 0;
	public:
		/// Informs the next packet size
		void nextPacket(uint32_t size) {
			isReady = true;
			packetSize = size;
		}
		/// Indicates the read packet size has been consumed
		void consume() {
			isReady = false;
		}
		bool canRead() const {
			return isReady;
		}
		/// Requires canRead() return true
		uint32_t size() const {
			return packetSize;
		}
	} mutable nextPacket;

private:
	PlaybackAudioSource();
	~PlaybackAudioSource();
	void cleanup();
	uint32_t nextPacketSize() const;
public:
	signal_t sample(std::chrono::milliseconds duration) override;
	double samplingFrequency() const override;
	bool ready() const override;
	static PlaybackAudioSource* get();
};
