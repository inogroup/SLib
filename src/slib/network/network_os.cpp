/*
 *   Copyright (c) 2008-2020 SLIBIO <https://github.com/SLIBIO>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "slib/network/os.h"

#include "slib/network/socket.h"
#include "slib/core/endian.h"
#include "slib/core/hash_map.h"
#include "slib/core/search.h"
#include "slib/core/scoped_buffer.h"

namespace slib
{

	SLIB_DEFINE_CLASS_DEFAULT_MEMBERS(NetworkInterfaceInfo)
	
	NetworkInterfaceInfo::NetworkInterfaceInfo()
	{
	}

	
	List<IPv4Address> Network::findAllIPv4Addresses()
	{
		List<IPv4Address> list;
		ListElements<NetworkInterfaceInfo> devices(Network::findAllInterfaces());
		for (sl_size i = 0; i < devices.count; i++) {
			ListElements<IPv4AddressInfo> addrs(devices[i].addresses_IPv4);
			for (sl_size k = 0; k < addrs.count; k++) {
				if (addrs[k].address.isHost()) {
					list.add_NoLock(addrs[k].address);
				}
			}
		}
		return list;
	}

	List<IPv4AddressInfo> Network::findAllIPv4AddressInfos()
	{
		List<IPv4AddressInfo> list;
		ListElements<NetworkInterfaceInfo> devices(Network::findAllInterfaces());
		for (sl_size i = 0; i < devices.count; i++) {
			ListElements<IPv4AddressInfo> addrs(devices[i].addresses_IPv4);
			for (sl_size k = 0; k < addrs.count; k++) {
				if (addrs[k].address.isHost()) {
					list.add_NoLock(addrs[k]);
				}
			}
		}
		return list;
	}

	List<IPv6Address> Network::findAllIPv6Addresses()
	{
		List<IPv6Address> list;
		ListElements<NetworkInterfaceInfo> devices(Network::findAllInterfaces());
		for (sl_size i = 0; i < devices.count; i++) {
			ListElements<IPv6Address> addrs(devices[i].addresses_IPv6);
			for (sl_size k = 0; k < addrs.count; k++) {
				if (addrs[k].isNotZero() && !(addrs[k].isLoopback()) && !(addrs[k].isIPv4Transition())) {
					list.add_NoLock(addrs[k]);
				}
			}
		}
		return list;
	}
	
	List<MacAddress> Network::findAllMacAddresses()
	{
		List<MacAddress> list;
		ListElements<NetworkInterfaceInfo> devices(Network::findAllInterfaces());
		for (sl_size i = 0; i < devices.count; i++) {
			if (devices[i].macAddress.isNotZero()) {
				list.add_NoLock(devices[i].macAddress);
			}
		}
		return list;
	}


	sl_bool Network::findInterface(const StringParam& _name, NetworkInterfaceInfo* pInfo)
	{
		StringData name(_name);
		ListElements<NetworkInterfaceInfo> devices(Network::findAllInterfaces());
		for (sl_size i = 0; i < devices.count; i++) {
			if (devices[i].name == name || devices[i].displayName == name) {
				if(pInfo) {
					*pInfo = devices[i];
				}
				return sl_true;
			}
		}
		return sl_false;
	}

}

#if defined(SLIB_PLATFORM_IS_WINDOWS)
#	if defined(SLIB_PLATFORM_IS_WIN32)

#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <netioapi.h>

#include "slib/core/platform.h"
#include "slib/network/dl/win32/iphlpapi.h"

#pragma comment(lib, "ws2_32.lib")

namespace slib
{

	template <>
	class Compare<MIB_IPADDRROW, IPv4Address>
	{
	public:
		sl_compare_result operator()(const MIB_IPADDRROW& a, const IPv4Address& b) const
		{
			return Compare<sl_uint32>()(a.dwAddr, Endian::swap32LE(b.getInt()));
		}
	};

	template <>
	class Equals<MIB_IPADDRROW, IPv4Address>
	{
	public:
		sl_bool operator()(const MIB_IPADDRROW& a, const IPv4Address& b) const
		{
			return Equals<sl_uint32>()(a.dwAddr, Endian::swap32LE(b.getInt()));
		}
	};

	List<NetworkInterfaceInfo> Network::findAllInterfaces()
	{
		auto funcGetIpAddrTable = iphlpapi::getApi_GetIpAddrTable();
		if (!funcGetIpAddrTable) {
			return sl_null;
		}
		auto funcGetAdaptersAddresses = iphlpapi::getApi_GetAdaptersAddresses();
		if (!funcGetAdaptersAddresses) {
			return sl_null;
		}

		Socket::initializeSocket();

		ULONG ulOutBufLen = 0;
		if (funcGetAdaptersAddresses(AF_UNSPEC, 0, NULL, sl_null, &ulOutBufLen) != ERROR_BUFFER_OVERFLOW) {
			return sl_null;
		}
		SLIB_SCOPED_BUFFER(sl_uint8, 4096, bufAdapter, (sl_size)ulOutBufLen)
		if (!bufAdapter) {
			return sl_null;
		}
		IP_ADAPTER_ADDRESSES* adapter = (IP_ADAPTER_ADDRESSES*)bufAdapter;
		if (funcGetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapter, &ulOutBufLen) != NO_ERROR) {
			return sl_null;
		}

		sl_bool flagVista = Win32::getVersion() >= WindowsVersion::Vista;
		ulOutBufLen = 0;
		if (!flagVista) {
			if (funcGetIpAddrTable(sl_null, &ulOutBufLen, TRUE) != ERROR_INSUFFICIENT_BUFFER) {
				return sl_null;
			}
		}
		SLIB_SCOPED_BUFFER(sl_uint8, 1024, bufIptable, (sl_size)ulOutBufLen);
		MIB_IPADDRTABLE* iptable = (MIB_IPADDRTABLE*)bufIptable;
		if (!flagVista) {
			if (!iptable) {
				return sl_null;
			}
			if (funcGetIpAddrTable(iptable, &ulOutBufLen, TRUE) != NO_ERROR) {
				return sl_null;
			}
		}

		List<NetworkInterfaceInfo> ret;

		do {

			NetworkInterfaceInfo device;

			device.name = adapter->AdapterName;
			device.displayName = String::create(adapter->FriendlyName);
			device.description = String::create(adapter->Description);

			IP_ADAPTER_UNICAST_ADDRESS* pip = adapter->FirstUnicastAddress;
			while (pip) {
				SocketAddress sa;
				sa.setSystemSocketAddress(pip->Address.lpSockaddr, pip->Address.iSockaddrLength);
				if (sa.ip.isIPv4()) {
					IPv4AddressInfo a4;
					a4.address = sa.ip.getIPv4();
					sl_uint32 networkPrefixLength = 0;
					if (flagVista) {
						networkPrefixLength = ((IP_ADAPTER_UNICAST_ADDRESS_LH*)pip)->OnLinkPrefixLength;
					} else {
						sl_size indexTable = 0;
						if (BinarySearch::search(iptable->table, iptable->dwNumEntries, a4.address, &indexTable)) {
							networkPrefixLength = IPv4Address(Endian::swap32LE(iptable->table[indexTable].dwMask)).getNetworkPrefixLengthFromMask();
						}
					}
					a4.networkPrefixLength = networkPrefixLength;
					device.addresses_IPv4.add_NoLock(a4);
				} else if (sa.ip.isIPv6()) {
					IPv6Address a6;
					a6 = sa.ip.getIPv6();
					device.addresses_IPv6.add_NoLock(a6);
				}
				pip = pip->Next;
			}

			if (adapter->PhysicalAddressLength == 6) {
				device.macAddress.setBytes(adapter->PhysicalAddress);
			} else {
				device.macAddress.setZero();
			}

			ret.add_NoLock(device);

			adapter = adapter->Next;

		} while (adapter);

		return ret;
	}

}

#	endif

#elif defined(SLIB_PLATFORM_IS_ANDROID)

#include "slib/core/platform.h"

namespace slib
{

	namespace priv
	{
		namespace network_os
		{

			SLIB_JNI_BEGIN_CLASS(JNetworkDevice, "slib/android/network/NetworkDevice")
				SLIB_JNI_STRING_FIELD(name);
				SLIB_JNI_STRING_FIELD(macAddress);
				SLIB_JNI_OBJECT_FIELD(addresses_IPv4, "[Ljava/lang/String;");
				SLIB_JNI_OBJECT_FIELD(addresses_IPv6, "[Ljava/lang/String;");
			SLIB_JNI_END_CLASS

			SLIB_JNI_BEGIN_CLASS(JNetworkAddress, "slib/android/network/Network")
				SLIB_JNI_STATIC_METHOD(getAllDevices, "getAllDevices", "()[Lslib/android/network/NetworkDevice;");
			SLIB_JNI_END_CLASS

		}
	}

	using namespace priv::network_os;

	List<NetworkInterfaceInfo> Network::findAllInterfaces()
	{
		List<NetworkInterfaceInfo> ret;
		if (JNetworkAddress::get() && JNetworkDevice::get()) {
			JniLocal<jobjectArray> jarr = JNetworkAddress::getAllDevices.callObject(sl_null);
			if (jarr.isNotNull()) {
				sl_uint32 n = Jni::getArrayLength(jarr);
				for (sl_uint32 i = 0; i < n; i++) {
					JniLocal<jobject> jdev = Jni::getObjectArrayElement(jarr, i);
					if (jdev.isNotNull()) {
						NetworkInterfaceInfo dev;
						dev.name = JNetworkDevice::name.get(jdev);
						dev.displayName = dev.name;
						dev.macAddress.setZero();
						dev.macAddress.parse(JNetworkDevice::macAddress.get(jdev));
						JniLocal<jobjectArray> jarrIPv4 = JNetworkDevice::addresses_IPv4.get(jdev);
						if (jarrIPv4.isNotNull()) {
							sl_uint32 nAddr = Jni::getArrayLength(jarrIPv4);
							for (sl_uint32 k = 0; k < nAddr; k++) {
								String saddr = Jni::getStringArrayElement(jarrIPv4, k);
								sl_int32 indexPrefix = saddr.indexOf("/");
								if (indexPrefix > 0) {
									IPv4AddressInfo ip;
									if (ip.address.parse(saddr.substring(0, indexPrefix))) {
										ip.networkPrefixLength = saddr.substring(indexPrefix+1).parseUint32();
										dev.addresses_IPv4.add_NoLock(ip);
									}
								}
							}

						}
						JniLocal<jobjectArray> jarrIPv6 = JNetworkDevice::addresses_IPv6.get(jdev);
						if (jarrIPv6.isNotNull()) {
							sl_uint32 nAddr = Jni::getArrayLength(jarrIPv6);
							for (sl_uint32 k = 0; k < nAddr; k++) {
								String saddr = Jni::getStringArrayElement(jarrIPv6, k);
								sl_int32 indexPrefix = saddr.indexOf("/");
								if (indexPrefix > 0) {
									IPv6Address ip;
									if (ip.parse(saddr.substring(0, indexPrefix))) {
										dev.addresses_IPv6.add_NoLock(ip);
									}
								}
							}
						}
						ret.add_NoLock(dev);
					}
				}
			}
		}
		return ret;
	}
}

#elif defined(SLIB_PLATFORM_IS_UNIX)

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>

#	if defined(SLIB_PLATFORM_IS_APPLE)
#		define	IFT_ETHER	0x6		/* Ethernet CSMACD */
#		include <net/if_dl.h>
#	endif
#	if defined(SLIB_PLATFORM_IS_LINUX)
#		include <linux/if_packet.h>
#	endif

namespace slib
{
	List<NetworkInterfaceInfo> Network::findAllInterfaces()
	{
		HashMap<String, NetworkInterfaceInfo> ret;

		struct ifaddrs * adapters = 0;
		getifaddrs(&adapters);

		if (adapters) {
			
			struct ifaddrs* adapter = adapters;
			
			while (adapter) {
				
				String name = adapter->ifa_name;
				
				NetworkInterfaceInfo* pdev = ret.getItemPointer(name);
				if (!pdev) {
					NetworkInterfaceInfo dev;
					dev.name = name;
					dev.displayName = name;
					dev.macAddress.setZero();
					ret.put_NoLock(name, dev);
					pdev = ret.getItemPointer(name);
				}
				if (pdev && adapter->ifa_addr) {
					if (adapter->ifa_addr->sa_family == AF_INET) {
						// ipv4 address
						sockaddr_in* addr = (sockaddr_in*)(adapter->ifa_addr);
						sockaddr_in* mask = (sockaddr_in*)(adapter->ifa_netmask);
						IPv4AddressInfo ip;
						ip.address = IPv4Address(Endian::swap32LE(addr->sin_addr.s_addr));
						ip.networkPrefixLength = IPv4Address(Endian::swap32LE(mask->sin_addr.s_addr)).getNetworkPrefixLengthFromMask();
						pdev->addresses_IPv4.add_NoLock(ip);
					} else if (adapter->ifa_addr->sa_family == AF_INET6) {
						// ipv6 address
						SocketAddress s;
						s.setSystemSocketAddress(adapter->ifa_addr);
						IPv6Address ip;
						ip = s.ip.getIPv6();
						pdev->addresses_IPv6.add_NoLock(ip);
					}
#	if defined(SLIB_PLATFORM_IS_APPLE)
					else if (adapter->ifa_addr->sa_family == AF_LINK) {
						sockaddr_dl* addr = (sockaddr_dl*)(adapter->ifa_addr);
						if (addr->sdl_type == IFT_ETHER) {
							sl_uint8* base = (sl_uint8*)(addr->sdl_data + addr->sdl_nlen);
							pdev->macAddress.setBytes(base);
						}
					}
#	endif
#	if defined(SLIB_PLATFORM_IS_LINUX)
					else if (adapter->ifa_addr->sa_family == AF_PACKET) {
						sockaddr_ll* addr = (sockaddr_ll*)(adapter->ifa_addr);
						if (addr->sll_halen == 6) {
							pdev->macAddress.setBytes((sl_uint8*)(addr->sll_addr));
						}
					}
#	endif
				}
				adapter = adapter->ifa_next;
			}
			freeifaddrs(adapters);
		}

		return ret.getAllValues();
	}
}

#endif

#if defined(SLIB_PLATFORM_IS_UNIX)

#include <netdb.h>
#include <net/if.h>

#endif

namespace slib
{

	sl_uint32 Network::getInterfaceIndexFromName(const StringParam& _name)
	{
		StringCstr name(_name);
#if defined(SLIB_PLATFORM_IS_WINDOWS)
		auto func = iphlpapi::getApi_if_nametoindex();
		if (func) {
			Socket::initializeSocket();
			return func(name.getData());
		}
		return 0;
#else
		return if_nametoindex(name.getData());
#endif
	}

	String Network::getInterfaceNameFromIndex(sl_uint32 index)
	{
		char buf[256];
		char* s;
#if defined(SLIB_PLATFORM_IS_WINDOWS)
		auto func = iphlpapi::getApi_if_indextoname();
		if (func) {
			Socket::initializeSocket();
			s = func(index, buf);
		} else {
			return sl_null;
		}
#else
		s = if_indextoname(index, buf);
#endif
		if (s) {
			return String(s);
		} else {
			return sl_null;
		}
	}

	List<IPAddress> Network::getIPAddressesFromHostName(const StringParam& _hostName)
	{
		Socket::initializeSocket();

		StringCstr hostName(_hostName);
		List<IPAddress> ret;

		addrinfo *addrs = sl_null;

		SocketAddress sa;
		int iRet = getaddrinfo(hostName.getData(), sl_null, sl_null, &addrs);
		if (iRet == 0) {
			addrinfo* addr = addrs;
			while (addr) {
				sl_int32 lenAddr = (sl_int32)(addr->ai_addrlen);
				if (lenAddr > 0) {
					sa.ip.setNone();
					sa.setSystemSocketAddress(addr->ai_addr, lenAddr);
					if (sa.ip.isNotNone()) {
						ret.add_NoLock(sa.ip);
					}
				}
				addr = addr->ai_next;
			}
			if (addrs) {
				freeaddrinfo(addrs);
			}
		}
		return ret;
	}

	IPAddress Network::getIPAddressFromHostName(const StringParam& hostName)
	{
		ListElements<IPAddress> list(getIPAddressesFromHostName(hostName));
		sl_size i;
		for (i = 0; i < list.count; i++) {
			if (list[i].isIPv4()) {
				return list[i];
			}
		}
		for (i = 0; i < list.count; i++) {
			if (list[i].isIPv6()) {
				return list[i];
			}
		}
		return IPAddress::none();
	}

	IPv4Address Network::getIPv4AddressFromHostName(const StringParam& hostName)
	{
		ListElements<IPAddress> list(getIPAddressesFromHostName(hostName));
		for (sl_size i = 0; i < list.count; i++) {
			if (list[i].isIPv4()) {
				return list[i].getIPv4();
			}
		}
		return IPv4Address::zero();
	}

	IPv6Address Network::getIPv6AddressFromHostName(const StringParam& hostName)
	{
		ListElements<IPAddress> list(getIPAddressesFromHostName(hostName));
		for (sl_size i = 0; i < list.count; i++) {
			if (list[i].isIPv6()) {
				return list[i].getIPv6();
			}
		}
		return IPv6Address::zero();
	}

}

#if defined(SLIB_PLATFORM_IS_APPLE) || defined(SLIB_PLATFORM_IS_FREEBSD)
#include <stdio.h>
#elif defined(SLIB_PLATFORM_IS_LINUX)
#include "slib/core/file.h"
#endif

namespace slib
{

	// Reference: PcapPlusPlus (https://github.com/seladb/PcapPlusPlus/blob/master/Pcap%2B%2B/src/PcapLiveDevice.cpp#setDefaultGateway)
	IPv4Address Network::getDefaultGateway(const StringParam& _interfaceName)
	{
#if defined(SLIB_PLATFORM_IS_WIN32)

		auto funcGetAdaptersInfo = iphlpapi::getApi_GetAdaptersInfo();
		if (!funcGetAdaptersInfo) {
			return IPv4Address::zero();
		}

		Socket::initializeSocket();

		ULONG ulOutBufLen = 0;
		if (funcGetAdaptersInfo(sl_null, &ulOutBufLen) != ERROR_BUFFER_OVERFLOW) {
			return IPv4Address::zero();
		}
		SLIB_SCOPED_BUFFER(sl_uint8, 1024, bufInfo, (sl_size)ulOutBufLen)
		if (!bufInfo) {
			return IPv4Address::zero();
		}
		IP_ADAPTER_INFO* info = (IP_ADAPTER_INFO*)bufInfo;
		if (funcGetAdaptersInfo(info, &ulOutBufLen) != NO_ERROR) {
			return IPv4Address::zero();
		}

		StringData interfaceName(_interfaceName);
		do {
			if (interfaceName.equals(info->AdapterName) || interfaceName.equals(info->Description)) {
				return IPv4Address(info->GatewayList.IpAddress.String);
			}
			info = info->Next;
		} while (info);

#elif defined(SLIB_PLATFORM_IS_MACOS) || defined(SLIB_PLATFORM_IS_FREEBSD)

		String command = String::concat("netstat -nr | grep default | grep ", _interfaceName);
		FILE* fp = popen(command.getData(), "r");
		char buf[1024];
		*buf = 0;
		if (fp) {
			fgets(buf, sizeof(buf), fp);
			pclose(fp);
		}
		if (!(*buf)) {
			return IPv4Address::zero();
		}
		buf[sizeof(buf) - 1] = 0;
		if (!(Base::equalsMemory(buf, "default ", 8))) {
			return IPv4Address::zero();
		}
		char* p = buf + 8;
		while (*p == ' ') {
			p++;
		}
		char* e = (char*)(Base::findMemory(p, buf + sizeof(buf) - 1 - p, ' '));
		if (e) {
			return IPv4Address(StringView(p, e - p));
		}

#elif defined(SLIB_PLATFORM_IS_LINUX)

		File file = File::openForRead("/proc/net/route");
		if (file.isNone()) {
			return IPv4Address::zero();
		}
		StringData interfaceName(_interfaceName);
		for (;;) {
			String line = file.readLine();
			if (line.isNull()) {
				break;
			}
			sl_reg index = line.indexOf('\t');
			if (index > 0) {
				StringView name = StringView(line).substring(0, index);
				if (interfaceName.equals(name)) {
					StringView remain = StringView(line).substring(index + 1);
					if (remain.startsWith("00000000\t")) {
						remain = remain.substring(9);
						sl_reg index = remain.indexOf('\t');
						if (index > 0) {
							StringView gateway = remain.substring(0, index);
							sl_uint32 nip = gateway.parseUint32(16);
							return IPv4Address(SLIB_GET_BYTE0(nip), SLIB_GET_BYTE1(nip), SLIB_GET_BYTE2(nip), SLIB_GET_BYTE3(nip));
						}
					}
				}
			}
		}

#endif
		return IPv4Address::zero();
	}

}
