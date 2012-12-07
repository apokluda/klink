/*
 * protocol.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <cstring>

#include "../message/message.h"
#include "../message/message_processor.h"
#include "../ds/lookup_table.h"
#include "../ds/cache.h"
#include "../ds/overlay_id.h"
#include "../ds/host_address.h"
#include "../peer/peer.h"

class ABSProtocol
{
protected:
	LookupTable<OverlayID, HostAddress>* routing_table;
	LookupTable<string, OverlayID>* index_table;
	Peer* container_peer;
	Cache* cache;
	MessageProcessor* msgProcessor;

public:

	ABSProtocol()
	{
		this->routing_table = NULL;
		this->index_table = new LookupTable <string, OverlayID>();
		this->container_peer = NULL;
		this->cache = new Cache();
		this->msgProcessor = NULL;
	}

	ABSProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
			LookupTable<string, OverlayID>* index_table,
			Cache *cache,
			MessageProcessor* msgProcessor,
			Peer* container)
	{
		this->routing_table = routing_table;
		this->index_table = index_table;
		this->cache = cache;
		this->container_peer = container;
		msgProcessor->setup(routing_table, index_table, cache);
	}

	void setContainerPeer(Peer* peer)
	{
		container_peer = peer;
	}
	Peer* getContainerPeer()
	{
		return container_peer;
	}

	void setCache(Cache* cahce)
	{
		this->cache = cache;
	}
	Cache* getCache()
	{
		return cache;
	}

	void setMessageProcess(MessageProcessor* processor)
	{
		msgProcessor = processor;
	}
	MessageProcessor* getMessageProcessor()
	{
		return msgProcessor;
	}

	void setRoutingTable(LookupTable <OverlayID, HostAddress>* table)
	{
		if(routing_table != NULL) delete routing_table;
		routing_table = new LookupTable <OverlayID, HostAddress>();
		*routing_table = *table;
	}

	void setIndexTable(LookupTable <string, OverlayID>* table)
	{
		if(index_table != NULL) delete index_table;
		index_table = new LookupTable <string, OverlayID>();
		*index_table = *table;
	}

	virtual ~ABSProtocol(){}

	virtual void initiate_join() = 0;
	virtual void process_join() = 0;
	virtual void setNextHop(ABSMessage*) = 0;
	virtual void get(string name) = 0;
	virtual void put(string name, HostAddress my_address) = 0;
	virtual void rejoin() = 0;
};

#endif /* PROTOCOL_H_ */
