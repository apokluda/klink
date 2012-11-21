/*
 * protocol.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../message/message.h"

class ABSProtocol
{
public:
	ABSProtocol();
	virtual ~ABSProtocol();

	virtual void initiate_join() = 0;
	virtual void process_join() = 0;
	virtual void forward(const ABSMessage*) = 0;
	virtual void get() = 0;
	virtual void put() = 0;
	virtual void rejoin() = 0;
};


#endif /* PROTOCOL_H_ */
