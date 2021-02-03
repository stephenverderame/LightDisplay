#include <WindowsBTTransmitter.h>
#include <WSA.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#include <stdexcept>
#include <RAII.h>
#include <wchar.h>
#include <WindowsBTImpl.h>
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
template<SocketPolicy sp>
WindowsBT<sp>::WindowsBT(typename sp::Address_t addr)
{
	WSA::init();
	this->socket = sp::createSocket();
	auto addrInfo = sp::createAddr(8008, addr);
	if (connect(this->socket, (sockaddr*)&addrInfo, sizeof(addrInfo)) == SOCKET_ERROR) {
		fprintf(stderr, "Failed to connect %d\n", WSAGetLastError());
		throw std::runtime_error("Failed to connect");
	}
}
template<SocketPolicy sp>
WindowsBT<sp>::WindowsBT() : WindowsBT(
	static_cast<typename sp::Address_t>(findBtDevice([](auto& device) {
		return wcsstr(device.szName, L"Mpow");
	}).ullLong)) {
	//Cannot be a compile time check because we manually initialize the valid templates to keep them in a cpp file
	if(!std::is_same_v<sp, BluetoothStreamPolicy>) 
		throw std::runtime_error("Default constructor must be applied on a bluetooth policy");
}

template<SocketPolicy sp>
WindowsBT<sp>::~WindowsBT()
{
	if (this->socket != INVALID_SOCKET)
		closesocket(this->socket);
	WSA::cleanup();
	
}

template<SocketPolicy sp>
void WindowsBT<sp>::transmit(const std::vector<Visual>& visuals)
{
	/* Frame Layout ---
	* Size (8 bytes)
	* Data (Size bytes)
	*/
	const unsigned long long size = sizeof(Visual) * visuals.size();
	int ret = 0;
	if ((ret = sendAll(this->socket, size)) != 0 ||
		(ret = sendAll(this->socket, visuals)) != 0) {
		fprintf(stderr, "Could not send data. Error code: %d\n", ret);
	}
}
// Initialize valid template instantiations to allow definition in cpp file
template WindowsBT<BluetoothStreamPolicy>;
template WindowsBT<Ipv4TcpPolicy>;
