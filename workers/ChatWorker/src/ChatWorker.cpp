#include "ChatWorker.h"

#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <improbable/view.h>

using namespace player;
using namespace worker;
using namespace improbable;

ChatWorker::ChatWorker()
	:m_pView(NULL), m_pConnection(NULL)
{
	
}

ChatWorker::~ChatWorker()
{
	m_pView->OnCommandRequest<Chat::Commands::OutgoingMessage>(NULL);
	m_pView = NULL;
	m_pConnection = NULL;
}

void ChatWorker::Initialize(worker::View& view, worker::Connection& connection)
{
	m_pView = &view;
	m_pConnection = &connection;
	m_pView->OnCommandRequest<Chat::Commands::OutgoingMessage>(&ChatWorker::OnCommandRequest);
}

void ChatWorker::Process()
{
	
}

void ChatWorker::OnCommandRequest(const worker::CommandRequestOp<player::Chat::Commands::OutgoingMessage>& op)
{
	View& view = *(GetInstance().m_pView);
	Connection& connection = *(GetInstance().m_pConnection);
	auto message_receiver = op.Request.receiver();
	auto sender = op.EntityId;
	auto message = op.Request.message();
	if (message_receiver.all())
	{
		for (auto& entityData : view.Entities)
		{
			auto entity_id = entityData.first;
			worker::Entity& entity = entityData.second;
			if (HasChatComponent(entity) && view.GetAuthority<player::Chat>(entity_id) == worker::Authority::kAuthoritative)
			{
				GetInstance().TransmitMessage(sender, entity_id, message);
			}
		}
	}
	else
	{
		if (message_receiver.group_id() != -1)
		{
			// To do
		}
		else
		{
			GetInstance().TransmitMessage(sender, message_receiver.single_target(), message);
		}
	}
	Chat::Commands::OutgoingMessage::Response ret = Chat::Commands::OutgoingMessage::Response::Create();
	connection.SendCommandResponse<Chat::Commands::OutgoingMessage>(op.RequestId, ret);
}

void ChatWorker::TransmitMessage(worker::EntityId senderID, worker::EntityId receiverID, std::string& message)
{
	Chat::Update update;
	MessageEvent event(senderID, message);
	update.add_incoming_message(event);
	m_pConnection->SendComponentUpdate<Chat>(receiverID, update);
}

bool ChatWorker::HasChatComponent(const Entity& entity)
{
	Option<const Chat::Data&> state = entity.Get<Chat>();
	return !state.empty();
}

