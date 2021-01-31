#include <WindowsBTTransmitter.h>
#include <WinSock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#include <stdexcept>
#include <RAII.h>
#include <wchar.h>
/**
 * Finds the bluetooth devices connected to the specified radio
 * @param predicate a function that takes a bluetooth info struct and returns a boolean
 * @param radio (optional) the handle to the radio to search or nullptr to search all radios
 * @return the Bluetooth address struct of the first device that evaluates to true by the predicate
 */
template<Functor<const BLUETOOTH_DEVICE_INFO_STRUCT&> pred> 
inline auto findBtDevice(pred predicate, HBLUETOOTH_RADIO_FIND radio = nullptr) {
	BLUETOOTH_DEVICE_SEARCH_PARAMS deviceParams = { 
		sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
		1, 0, 1, 1, 1, 15, NULL 
	};
	deviceParams.hRadio = radio;
	BLUETOOTH_ADDRESS_STRUCT addr{};
	BLUETOOTH_DEVICE_INFO deviceInfo;
	ZeroMemory(&deviceInfo, sizeof(deviceInfo));
	deviceInfo.dwSize = sizeof(deviceInfo);
	auto findDeviceHandle = ResourceWrapper{ BluetoothFindFirstDevice(&deviceParams, &deviceInfo),
		&BluetoothFindDeviceClose };
	while (findDeviceHandle.get()) {
		wprintf(L"\tDevice Name: %s\n", deviceInfo.szName);
		if (predicate(deviceInfo)) {
			addr = deviceInfo.Address;
			break;
		}
		if (!BluetoothFindNextDevice(findDeviceHandle.get(), &deviceInfo)) break;
	}
	return addr;
}
/**
 * @return the address of the bluetooth device to display the lights
 */
auto getBtDeviceAddr() {
	BLUETOOTH_FIND_RADIO_PARAMS radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	BLUETOOTH_RADIO_INFO radioInfo{sizeof(BLUETOOTH_RADIO_INFO), 0};
	BLUETOOTH_ADDRESS_STRUCT addr{};
	HANDLE radios;
	auto findRadioHandle = 
		ResourceWrapper{ BluetoothFindFirstRadio(&radioParams, &radios), &BluetoothFindRadioClose };
	while (findRadioHandle.get()) {
		const auto err = BluetoothGetRadioInfo(findRadioHandle.get(), &radioInfo);
		if(err == ERROR_SUCCESS) {
			wprintf(L"Radio Name: %s\n", radioInfo.szName);
			addr = findBtDevice([](auto&) {
				return false;
				},findRadioHandle.get());
		}
		else {
			fprintf(stderr, "Radio get info error: %d\n", err);
		}
		if (!BluetoothFindNextRadio(findRadioHandle.get(), &radios)) break;
	}
	return addr;
}

unsigned WindowsBT::refCount = 0;
inline Handle_t createBtSocket() {
	auto res = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (res == INVALID_SOCKET)
		throw std::runtime_error("Cannot create socket");
	return res;
}
inline auto createBTAddr(unsigned short port, BTH_ADDR addr) {
	SOCKADDR_BTH addrInfo;
	ZeroMemory(&addrInfo, sizeof(addrInfo));
	addrInfo.port = 1;
	addrInfo.addressFamily = AF_BTH;
	addrInfo.btAddr = addr;
	return addrInfo;
}
WindowsBT::WindowsBT()
{
	if (refCount++ == 0) {
		WSAData d;
		if(FAILED(WSAStartup(MAKEWORD(2, 1), &d)))
			throw std::bad_alloc();
	}
	auto addr = findBtDevice([](auto& device) {
		return wcsstr(device.szName, L"Mpow");
	});
	this->socket = createBtSocket();
	auto addrInfo = createBTAddr(1, addr.ullLong);
	if (connect(this->socket, (sockaddr*)&addrInfo, sizeof(addrInfo)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to connect\n");
		throw std::runtime_error("Failed to connect");
	}
}

WindowsBT::~WindowsBT()
{
	if (this->socket != INVALID_SOCKET)
		closesocket(this->socket);
	if (--refCount == 0) {
		WSACleanup();
	}
	
}
/**
 * Guaruntees the sending of data entirely, or an error
 * @return 0 or a WSA error code
 */
template<typename T>
inline int sendAll(Handle_t sock, const std::vector<T>& data) {
	const auto totalSize = data.size() * sizeof(T);
	int err = 0;
	decltype(totalSize) sent = 0;
	do {
		err = send(sock, reinterpret_cast<const char*>(&data[0]) + sent,
			totalSize - sent, 0);
		if (err > 0) sent += err;
		else {
			err = -WSAGetLastError();
		}
	} while (sent < totalSize && err > 0);
	return err < 0 ? -err : 0;
}

void WindowsBT::transmit(const std::vector<Visual>& visuals)
{
	/* Frame Layout ---
	* Size (8 bytes)
	* Data (Size bytes)
	*/
	const unsigned long long size = sizeof(Visual) * visuals.size();
	const auto v = std::vector<decltype(size)>{ size };
	int ret = 0;
	if ((ret = sendAll(this->socket, v)) != 0 ||
		(ret = sendAll(this->socket, visuals)) != 0) {
		fprintf(stderr, "Could not send data. Error code: %d\n", ret);
	}
}
