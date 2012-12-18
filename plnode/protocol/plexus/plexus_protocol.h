/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_PROTOCOL_H
#define	PLEXUS_PROTOCOL_H

#include <pthread.h>
#include <cstring>
#include <queue>
#include "../protocol.h"
#include "../../ds/cache.h"
#include "../../message/message_processor.h"
//#include "plexus_message_processor.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../../message/control/peer_initiate_get.h"
#include "../../message/control/peer_initiate_put.h"
#include "../../ds/host_address.h"
#include "../../message/message.h"

using namespace std;

class ABSProtocol;

class PlexusProtocol : public ABSProtocol {
        queue<ABSMessage*> incoming_message_queue;
        queue<ABSMessage*> outgoing_message_queue;

        pthread_mutex_t incoming_queue_lock;
        pthread_mutex_t outgoing_queue_lock;

        pthread_cond_t cond_incoming_queue_empty;
        pthread_cond_t cond_outgoing_queue_empty;

public:

        PlexusProtocol() :
        ABSProtocol() {
                //this->routing_table = new LookupTable<OverlayID, HostAddress > ();
                //this->index_table = new LookupTable<string, OverlayID > ();
                //this->msgProcessor = new PlexusMessageProcessor();
                pthread_mutex_init(&incoming_queue_lock, NULL);
                pthread_mutex_init(&outgoing_queue_lock, NULL);

                pthread_cond_init(&cond_incoming_queue_empty, NULL);
                pthread_cond_init(&cond_outgoing_queue_empty, NULL);
        }

        PlexusProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
                LookupTable<string, HostAddress>* index_table, Cache *cache,
                MessageProcessor* msgProcessor, Peer* container) :
        ABSProtocol(routing_table, index_table, cache, msgProcessor,
        container) {
                this->msgProcessor->setContainerProtocol(this);

                pthread_mutex_init(&incoming_queue_lock, NULL);
                pthread_mutex_init(&outgoing_queue_lock, NULL);

                pthread_cond_init(&cond_incoming_queue_empty, NULL);
                pthread_cond_init(&cond_outgoing_queue_empty, NULL);
        }

        void processMessage(ABSMessage *message) {
                msgProcessor->processMessage(message);
        }

        void initiate_join() {
        }

        void process_join() {
        }

        bool setNextHop(ABSMessage* msg) {
                int maxLengthMatch = 0, currentMatchLength = 0, currentNodeMathLength =
                        0;
                HostAddress next_hop;

                switch (msg->getMessageType()) {
                        case MSG_PEER_INIT:
                        case MSG_PEER_CONFIG:
                        case MSG_PEER_CHANGE_STATUS:
                        case MSG_PEER_START:
                        case MSG_PLEXUS_GET_REPLY:
                        case MSG_PEER_INITIATE_GET:
                        case MSG_PEER_INITIATE_PUT:
                                return false;
                                break;
                }

                if (msg->getOverlayTtl() == 0)
                        return false;

                Peer *container_peer = getContainerPeer();
                currentNodeMathLength =
                        container_peer->getOverlayID().GetMatchedPrefixLength(
                        msg->getDstOid());

                //cout << endl << "current node match : ";
                //container_peer->getOverlayID().printBits();
                //cout << " <> ";
                //msg->getOID().printBits();
                //cout << " = " << currentNodeMathLength << endl;

                //search in the RT
                //        OverlayID::MAX_LENGTH = GlobalData::rm->rm->k;
                //cout << "S OID M LEN " << OverlayID::MAX_LENGTH << endl;
                LookupTableIterator<OverlayID, HostAddress> rtable_iterator(
                        routing_table);
                rtable_iterator.reset_iterator();

                //routing_table->reset_iterator();
                while (rtable_iterator.hasMoreKey()) {
                        //while (routing_table->hasMoreKey()) {
                        //   OverlayID oid = routing_table->getNextKey();
                        OverlayID oid = rtable_iterator.getNextKey();
                        //cout << endl << "current match ";
                        //oid.printBits();
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(oid);
                        //cout << " ==== " << currentMatchLength << endl;

                        if (currentMatchLength > maxLengthMatch) {
                                maxLengthMatch = currentMatchLength;
                                routing_table->lookup(oid, &next_hop);
                                //printf("next host %s, next port %d\n", next_hop.GetHostName().c_str(), next_hop.GetHostPort());
                        }
                }
                //search in the Cache
                cache->reset_iterator();
                while (cache->has_next()) {
                        DLLNode *node = cache->get_next();
                        OverlayID *id = node->key;
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(*id);
                        if (currentMatchLength > maxLengthMatch) {
                                maxLengthMatch = currentMatchLength;
                                cache->lookup(msg->getDstOid(), next_hop);
                        }
                }

                cout << endl << "max match : = " << maxLengthMatch << endl;

                if (maxLengthMatch == 0 || maxLengthMatch < currentNodeMathLength) {
                        //msg->setDestHost("localhost");
                        //msg->setDestPort(container_peer->getListenPortNumber());
                        return false;
                } else {
                        msg->setDestHost(next_hop.GetHostName().c_str());
                        msg->setDestPort(next_hop.GetHostPort());
                        return true;
                }
        }

        void get(string name) {
                int hash_name_to_get = atoi(name.c_str());
                OverlayID destID(hash_name_to_get);

                MessageGET *msg = new MessageGET(container_peer->getHostName(),
                        container_peer->getListenPortNumber(), "", -1,
                        container_peer->getOverlayID(), destID, name);

                if (msgProcessor->processMessage(msg))
                        addToOutgoingQueue(msg);
        }

        void get_from_client(string name, HostAddress destination) {
                int hash_name_to_get = atoi(name.c_str());
                OverlayID destID(hash_name_to_get);

                cout << "id = " << hash_name_to_get << ends << " odi = ";
                destID.printBits();
                cout << endl;

                PeerInitiateGET *msg = new PeerInitiateGET(
                        container_peer->getHostName(),
                        container_peer->getListenPortNumber(),
                        destination.GetHostName(), destination.GetHostPort(),
                        container_peer->getOverlayID(), destID, name);
                msg->calculateOverlayTTL(getContainerPeer()->getNPeers());
                msg->message_print_dump();
                send_message(msg);
        }

        void put(string name, HostAddress hostAddress) {
                int hash_name_to_publish = atoi(name.c_str());
                OverlayID destID(hash_name_to_publish);

                MessagePUT *msg = new MessagePUT(container_peer->getHostName(),
                        container_peer->getListenPortNumber(), "", -1,
                        container_peer->getOverlayID(), destID, name,
                        hostAddress);

                if (msgProcessor->processMessage(msg))
                        addToOutgoingQueue(msg);
        }

        void put_from_client(string name, HostAddress hostAddress,
                HostAddress destination) {
                int hash_name_to_publish = atoi(name.c_str());
                OverlayID destID(hash_name_to_publish);

                cout << "id = " << hash_name_to_publish << ends << " odi = ";
                destID.printBits();
                cout << endl;

                PeerInitiatePUT *msg = new PeerInitiatePUT(
                        container_peer->getHostName(),
                        container_peer->getListenPortNumber(),
                        destination.GetHostName(), destination.GetHostPort(),
                        container_peer->getOverlayID(), destID, name, hostAddress);
                msg->calculateOverlayTTL(getContainerPeer()->getNPeers());

                msg->message_print_dump();
                send_message(msg);
        }

        void rejoin() {
        }

        void addToIncomingQueue(ABSMessage* message) {
                pthread_mutex_lock(&incoming_queue_lock);
                incoming_message_queue.push(message);
                pthread_cond_signal(&cond_incoming_queue_empty);
                pthread_mutex_unlock(&incoming_queue_lock);
        }

        bool isIncomingQueueEmpty() {
                bool status;
                pthread_mutex_lock(&incoming_queue_lock);
                status = incoming_message_queue.empty();
                pthread_mutex_unlock(&incoming_queue_lock);
                return status;
        }

        ABSMessage* getIncomingQueueFront() {
                pthread_mutex_lock(&incoming_queue_lock);

                while (incoming_message_queue.empty())
                        pthread_cond_wait(&cond_incoming_queue_empty, &incoming_queue_lock);

                ABSMessage* ret = incoming_message_queue.front();
                incoming_message_queue.pop();
                pthread_mutex_unlock(&incoming_queue_lock);
                return ret;
        }

        void addToOutgoingQueue(ABSMessage* message) {
                pthread_mutex_lock(&outgoing_queue_lock);
                outgoing_message_queue.push(message);
                pthread_cond_signal(&cond_outgoing_queue_empty);
                pthread_mutex_unlock(&outgoing_queue_lock);
        }

        bool isOutgoingQueueEmpty() {
                bool status;
                pthread_mutex_lock(&outgoing_queue_lock);
                status = outgoing_message_queue.empty();
                pthread_mutex_unlock(&outgoing_queue_lock);
                return status;
        }

        ABSMessage* getOutgoingQueueFront() {
                pthread_mutex_lock(&outgoing_queue_lock);

                while (outgoing_message_queue.empty()) {
                        //printf("Waiting for a message in outgoing queue");
                        pthread_cond_wait(&cond_outgoing_queue_empty, &outgoing_queue_lock);
                }

                ABSMessage* ret = outgoing_message_queue.front();
                //printf("Got a messge from the outgoing queue");
                outgoing_message_queue.pop();
                pthread_mutex_unlock(&outgoing_queue_lock);
                return ret;
        }

        ~PlexusProtocol() {
                pthread_mutex_destroy(&incoming_queue_lock);
                pthread_mutex_destroy(&outgoing_queue_lock);
                pthread_cond_destroy(&cond_incoming_queue_empty);
                pthread_cond_destroy(&cond_outgoing_queue_empty);
        }
};

#endif	/* PLEXUS_PROTOCOL_H */

