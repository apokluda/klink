/* 
 * File:   plexus_message_processor.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 2:45 PM
 */

#ifndef PLEXUS_MESSAGE_PROCESSOR_H
#define	PLEXUS_MESSAGE_PROCESSOR_H

#include "../../message/message.h"
#include "../../message/p2p/message_get.h"
#include "../../message/message_processor.h"
#include "../../message/control/peer_init_message.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../protocol.h"


class PlexusMessageProcessor: public MessageProcessor
{
public:

	void setup(LookupTable<OverlayID, HostAddress>* routing_table,
			LookupTable<string, HostAddress>* index_table)
	{
		MessageProcessor::setup(routing_table, index_table, NULL);
	}

	bool processMessage(ABSMessage* message)
	{
		message->incrementOverlayHops();
		message->decrementOverlayTtl();

		//INIT Message
		if(message->getMessageType() == MSG_PEER_INIT)
		{
			PeerInitMessage* pInitMsg = (PeerInitMessage*) message;
			ABSProtocol* container_protocol = this->getContainerProtocol();
			Peer* container_peer = container_protocol->getContainerPeer();

			container_protocol->setRoutingTable(&pInitMsg->getRoutingTable());
			container_peer->setNPeers(pInitMsg->getNPeers());
			container_peer->setOverlayID(pInitMsg->getOID());
                        GlobalData::oid = pInitMsg->getOID();

			this->setup(container_protocol->getRoutingTable(), container_protocol->getIndexTable());
			return false;
		}
		//PUT
		else if (message->getMessageType() == MSG_PLEXUS_PUT)
		{
			MessagePUT* putMsg = (MessagePUT*)message;
			Peer* container_peer = this->getContainerProtocol()->getContainerPeer();
			if( (container_peer->getOverlayID().GetOverlay_id() == putMsg->getOID().GetOverlay_id()) || putMsg->getOverlayTtl() == 0)
			{
				index_table->add(putMsg->GetDeviceName(), putMsg->GetHostAddress());
				return false;
			}
			else
			{
				return true;
			}
		}
		//GET
		else if (message->getMessageType() == MSG_PLEXUS_GET)
		{
			MessageGET *msg = ((MessageGET*) message);
			HostAddress hostAddress;
			if (index_table->lookup(msg->GetDeviceName(), &hostAddress))
			{
				MessageGET_REPLY *msg_reply = new MessageGET_REPLY();
				//msg_reply->setIP(ip);
				//send message
			} else
			{
				//send error message
			}
		}
		//GET REPLY
		else if (message->getMessageType() == MSG_PLEXUS_GET_REPLY)
		{
			MessageGET_REPLY *msg = ((MessageGET_REPLY*) message);
			cache->add(msg->getID(), msg->getIP());
		}
		return false;
	}
};

#endif	/* PLEXUS_MESSAGE_PROCESSOR_H */

