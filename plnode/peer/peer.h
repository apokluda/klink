/*
 * peer.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PEER_H_
#define PEER_H_

#include <string.h>
#include "../protocol/protocol.h"
#include "../ds/overlay_id.h"
#include "../ds/ip_address.h"
#include "../../communication/server_socket.h"

class ABSProtocol;

class ABSProtocol;

class Peer
{
	int n_peers;

	int peer_id;
	int code_word;
	OverlayID overlay_id;

	IPAddress ip_address;
	string host_name;
	int listen_port_number;

	int name_range_start;
	int name_range_end;
	double alpha;
	int k;

	ABSProtocol* protocol;
	ServerSocket* server_socket;

public:
	Peer()
	{
		char hostname[100];
		gethostname(hostname, 100);
		host_name = hostname;
		listen_port_number = 0;
	}

	Peer(int port)
	{
		char hostname[100];
		gethostname(hostname, 100);
		host_name = hostname;
		listen_port_number = port;
		server_socket = new ServerSocket(listen_port_number);
	}

	Peer(const char* hosts_file)
	{
		char hostname[100];
		gethostname(hostname, 100);
		host_name = hostname;
		listen_port_number = 0;
		FILE* hosts_ptr = fopen(hosts_file, "r");
		char host_name[200];
		int port = -1;
		int n_hosts;
		fscanf(hosts_ptr, "%d", &n_hosts);

		for(int i = 0; i < n_hosts; i++)
		{
			fscanf(hosts_ptr, "%s %d", host_name, &port);
			if(strncmp(this->getHostName().c_str(), host_name, strlen(this->getHostName().c_str())))
				break;
		}
		fclose(hosts_ptr);
		if(port != -1)
		{
			listen_port_number = port;
			server_socket = new ServerSocket(listen_port_number);
		}
	}

	~Peer()
	{
		;
	}

	double getAlpha()
	{
		return alpha;
	}

	void setAlpha(double alpha)
	{
		this->alpha = alpha;
	}

	int getCodeWord()
	{
		return code_word;
	}

	void setCodeWord(int codeWord)
	{
		code_word = codeWord;
	}

	IPAddress getIpAddress()
	{
		return ip_address;
	}

	int getIntIpAddress()
	{
		return ip_address.getIpAddress();
	}

	string getStrIpAddress()
	{
		return ip_address.getStrIpAddress();
	}

	void setIpAddress(char* ipAddress)
	{
		ip_address.setIp(ipAddress);
	}

	int getK()
	{
		return k;
	}

	void setK(int k)
	{
		this->k = k;
	}

	int getNameRangeEnd()
	{
		return name_range_end;
	}

	void setNameRangeEnd(int nameRangeEnd)
	{
		name_range_end = nameRangeEnd;
	}

	int getNameRangeStart()
	{
		return name_range_start;
	}

	void setNameRangeStart(int nameRangeStart)
	{
		name_range_start = nameRangeStart;
	}

	int getPeerId()
	{
		return peer_id;
	}

	void setPeerId(int peerId)
	{
		peer_id = peerId;
	}

	void setHostName(string host_name)
	{
		this->host_name = host_name;
	}

	string getHostName()
	{
		return host_name;
	}

	int getListenPortNumber()
	{
		return listen_port_number;
	}

	void setListenPortNumber(int port)
	{
		listen_port_number = port;
	}

	ABSProtocol* getProtocol()
	{
		return protocol;
	}

	void setProtocol(ABSProtocol* protocol)
	{
		this->protocol = protocol;
	}

	ServerSocket* getServerSocket()
	{
		return server_socket;
	}

	void setServerSocket(ServerSocket* socket)
	{
		server_socket = socket;
	}

	void setNPeers(int n)
	{
		n_peers = n;
	}

	int getNPeers()
	{
		return n_peers;
	}

	OverlayID getOverlayID()
	{
		return overlay_id;
	}

	void setOverlayID(OverlayID id)
	{
		overlay_id = id;
	}

};

#endif /* PEER_H_ */
