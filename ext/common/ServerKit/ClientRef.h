/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014 Phusion
 *
 *  "Phusion Passenger" is a trademark of Hongli Lai & Ninh Bui.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SERVER_KIT_CLIENT_REF_H_
#define _PASSENGER_SERVER_KIT_CLIENT_REF_H_

#include <boost/move/core.hpp>

namespace Passenger {
namespace ServerKit {


template<typename Server, typename Client>
class ClientRef {
private:
	BOOST_COPYABLE_AND_MOVABLE(ClientRef);
	Client *client;
	const char *file;
	unsigned int line;

	static Server *getServer(Client *client) {
		return static_cast<Server *>(
			static_cast<typename Server::BaseClass *>(client->getServerBaseClassPointer())
		);
	}

public:
	explicit
	ClientRef(Client *_client, const char *_file, unsigned int _line)
		: client(_client),
		  file(_file),
		  line(_line)
	{
		if (_client != NULL) {
			getServer(_client)->_refClient(_client, _file, _line);
		}
	}

	ClientRef(const ClientRef &ref)
		: client(ref.client),
		  file(ref.file),
		  line(ref.line)
	{
		if (ref.client != NULL) {
			getServer(ref.client)->_refClient(ref.client, ref.file, ref.line);
		}
	}

	ClientRef(const ClientRef &ref, const char *_file, unsigned int _line)
		: client(ref.client),
		  file(_file),
		  line(_line)
	{
		if (ref.client != NULL) {
			getServer(ref.client)->_refClient(ref.client, _file, _line);
		}
	}

	explicit
	ClientRef(BOOST_RV_REF(ClientRef) ref)
		: client(ref.client)
	{
		ref.client = NULL;
	}

	~ClientRef() {
		if (client != NULL) {
			getServer(client)->_unrefClient(client, file, line);
		}
	}

	Client *get() const {
		return client;
	}

	ClientRef &operator=(BOOST_COPY_ASSIGN_REF(ClientRef) ref) {
		if (client == ref.client) {
			Client *oldClient = client;
			const char *oldFile = file;
			unsigned int oldLine = line;
			client = ref.client;
			file = ref.file;
			line = ref.line;
			if (client != NULL) {
				getServer(client)->_refClient(ref.client, ref.file, ref.line);
			}
			if (oldClient != NULL) {
				getServer(oldClient)->_unrefClient(oldClient, oldFile, oldLine);
			}
		}
		return *this;
	}

	ClientRef &operator=(BOOST_RV_REF(ClientRef) ref) {
		Client *oldClient = client;
		client = ref.client;
		ref.client = NULL;
		if (oldClient != NULL) {
			getServer(oldClient)->_unrefClient(oldClient, file, line);
		}
		return *this;
	}
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_CLIENT_REF_H_ */
