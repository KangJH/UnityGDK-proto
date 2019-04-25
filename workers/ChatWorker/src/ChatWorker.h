#pragma once

#include <improbable/worker.h>
#include "player/chat.h"
#include <improbable/view.h>
#include <improbable/standard_library.h>

class ChatWorker
{
public:
	static ChatWorker& GetInstance()
	{
		static ChatWorker chatWorker;
		return chatWorker;
	}
	void Initialize(worker::View& view, worker::Connection& connection);
	static void Process();
private:
	ChatWorker();
	~ChatWorker();
	ChatWorker(const ChatWorker&);
	ChatWorker& operator=(const ChatWorker&);

	worker::View* m_pView;
	worker::Connection* m_pConnection;

	void TransmitMessage(worker::EntityId senderID, worker::EntityId receiverID, std::string& message);

	static void OnCommandRequest(const worker::CommandRequestOp<player::Chat::Commands::OutgoingMessage>& op);
	static bool HasChatComponent(const worker::Entity& entity);
	
};

