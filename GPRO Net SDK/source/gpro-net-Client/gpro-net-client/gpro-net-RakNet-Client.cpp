/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet-Client.cpp
	Source for RakNet client management.
*/

#include "gpro-net/gpro-net-client/gpro-net-RakNet-Client.hpp"


namespace gproNet
{
	cRakNetClient::cRakNetClient()
	{
		RakNet::SocketDescriptor sd;
		char SERVER_IP[16] = "127.0.0.1";

		peer->Startup(1, &sd, 1);
		peer->SetMaximumIncomingConnections(0);
		peer->Connect(SERVER_IP, SET_GPRO_SERVER_PORT, 0, 0);
	}

	cRakNetClient::~cRakNetClient()
	{
		peer->Shutdown(0);
	}

	bool cRakNetClient::ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID)
	{
		if (cRakNetManager::ProcessMessage(bitstream, sender, dtSendToReceive, msgID))
			return true;

		// client-specific messages
		switch (msgID)
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			//printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			//printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			//printf("Another client has connected.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			//printf("The server is full.\n");
			return true;
		case ID_DISCONNECTION_NOTIFICATION:
			//printf("We have been disconnected.\n");
			return true;
		case ID_CONNECTION_LOST:
			//printf("Connection lost.\n");
			return true;

		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			// client connects to server, send greeting
			RakNet::BitStream bitstream_w;
			WriteTest(bitstream_w, "Hello server from client");
			peer->Send(&bitstream_w, MEDIUM_PRIORITY, UNRELIABLE_SEQUENCED, 0, sender, false);
		}	return true;

			// test message
		case ID_GPRO_MESSAGE_COMMON_BEGIN:
		{
			// client receives greeting, just print it
			ReadTest(bitstream);
		}	return true;

		}
		return false;
	}

	/*
struct sSpatialPose
{
	float scale[3];     // non-uniform scale 4bytes
	float rotate[3];    // orientation as Euler angles 4bytes
	float translate[3]; // translation 4bytes

	// read from stream
	RakNet::BitStream& Read(RakNet::BitStream& bitstream)
	{
		scale = DecompressScale(scale[]);
		rotate = DecompressRotation(rotate[]);
		translate = DecompressTranslation(translate[]);

		bitstream.Read(scale[0]);
		bitstream.Read(scale[1]);
		bitstream.Read(scale[2]);
		bitstream.Read(rotate[0]);
		bitstream.Read(rotate[1]);
		bitstream.Read(rotate[2]);
		bitstream.Read(translate[0]);
		bitstream.Read(translate[1]);
		bitstream.Read(translate[2]);
		return bitstream;
	}

	// write to stream
	RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
	{
		scale = CompressScale(scale[]);
		rotate = CompressRotation(rotate[]);
		translate = CompressTranslation(translate[]);

		bitstream.Write(scale[0]);
		bitstream.Write(scale[1]);
		bitstream.Write(scale[2]);
		bitstream.Write(rotate[0]);
		bitstream.Write(rotate[1]);
		bitstream.Write(rotate[2]);
		bitstream.Write(translate[0]);
		bitstream.Write(translate[1]);
		bitstream.Write(translate[2]);
		return bitstream;
	}

	// Compression functions
	float[] CompressScale(float[] _scale)
	{
		// Each float is 4bytes, 4 * 3 = 12 
		// Compress 12 bytes down to 4 bytes (32 bits) 
		// Alot 10 bits of 32 to each float
	}

	float[] CompressRotation(float[] _rotate)
	{
		// Each float is 4bytes, 4 * 3 = 12
		// Compress 12 bytes down to 4 bytes (32 bits)
		// Alot 10 bits of 32 to each float
	}

	float[] CompressTranslation(float[] _translate)
	{
		// Each float is 4bytes, 4 * 3 = 12 
		// Compress 12 bytes down to 4 bytes (32 bits) 
		// Alot 10 bits of 32 to each float
	}

	// Decompression functions
	float[] DecompressScale(float[] _scale)
	{
		// Extract the float from the alotted space
	}

	float[] DecompressRotation(float[] _rotate)
	{
		// Extract the float from the alotted space
	}

	float[] DecompressTranslation(float[] _translate)
	{
		// Extract the float from the alotted space
	}
};

*/

}