#include <gtest/gtest.h>
#include <WindowsBTReceiver.h>
#include <WindowsBTTransmitter.h>
#include <WS2tcpip.h>
#include <future>
#include <WSA.h>

auto genVisuals() {
	std::vector<Visual> visuals;
	int i = rand() % 8000;
	for (int j = 0; j < i; ++j) {
		Color c{ (unsigned char)(rand() % 255), (unsigned char)(rand() % 255), (unsigned char)(rand() % 255),(unsigned char)(rand() % 255) };
		Visual v{ c, rand() / (float)RAND_MAX * 2.f - 1, rand() / (float)RAND_MAX * 2.f - 1 };
		visuals.push_back(v);
	}
	return visuals;
}
bool operator==(Color a, Color b) {
	return memcmp(&a, &b, sizeof(Color)) == 0;
}
auto recvThread() {
	try {
		auto receiver
			= std::make_shared< WindowsBTReceiver<Ipv4TcpPolicy>>();
		receiver->accept();
		return receiver;
	}
	catch (std::exception& e) {
		fprintf(stderr, e.what());
		throw e;
	}
}
TEST(BTTest, transmissionTest) {
	//transmitter and receiver constructor are both blocking, 
	//start in two threads so they can happen at the same time
	auto fu = std::async(std::launch::async, &recvThread);
	unsigned long addr;
	inet_pton(AF_INET, "127.0.0.1", &addr);
	WindowsBT<Ipv4TcpPolicy> transmitter{addr};
	auto receiver = fu.get();
	for (int i = 0; i < 1000; ++i) {
		const auto visuals = genVisuals();
		transmitter.transmit(visuals);
		const auto recvVisuals = receiver->recv();
		ASSERT_EQ(recvVisuals.size(), visuals.size());
		for (int j = 0; j < visuals.size(); ++j) {
			ASSERT_EQ(visuals[j].color, recvVisuals[j].color);
			ASSERT_LE(abs(visuals[j].ndcX - recvVisuals[j].ndcX), 0.0001);
			ASSERT_LE(abs(visuals[j].ndcY - recvVisuals[j].ndcY), 0.0001);
		}

	}

}

int main(int argc, char** argv) {
	WSA::init();
	testing::InitGoogleTest(&argc, argv);
	const auto res = RUN_ALL_TESTS();
	WSA::cleanup();
	return res;
}
