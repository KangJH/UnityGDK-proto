#include "AIWorker.h"

#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <npc/State.h>
#include <improbable/view.h>

using namespace npc;
using namespace worker;
using namespace improbable;

AIWorker::AIWorker()
	:m_pView(NULL), m_pConnection(NULL)
{
	
}

AIWorker::~AIWorker()
{
	m_pView->OnCommandRequest<State::Commands::Done>(NULL);
	m_pView = NULL;
	m_pConnection = NULL;
}

void AIWorker::Initialize(worker::View& view, worker::Connection& connection)
{
	m_pView = &view;
	m_pConnection = &connection;
	m_pView->OnCommandRequest<State::Commands::Done>(&AIWorker::OnCommandRequest);
}

void AIWorker::Process()
{
	View& view = *(GetInstance().m_pView);
	for each (auto var in view.Entities)
	{
		Entity& entity = var.second;
		worker::Option<State::Data&> state = entity.Get<State>();
		if (!state.empty())
		{
			if (state->state() == EState::IDLE)
			{
				EntityId id = HasNearActor(view, entity);
				if (id != -1)
				{
					// No near actor exist, Move entity
					GetInstance().UpdateState(var.first, EState::MOVE, -1, GetRandPos());
				}
				else
				{
					// Near actor exist, Attack
					GetInstance().UpdateState(var.first, EState::ATTACK, id);
				}
			}
		}
	}
}

void AIWorker::OnCommandRequest(const worker::CommandRequestOp<State::Commands::Done>& op)
{
	View& view = *(GetInstance().m_pView);
	Connection& connection = *(GetInstance().m_pConnection);
	auto error = op.Request.error();
	if (error == 0)
	{
		auto entity_id = op.EntityId;
		//const_iterator Map<EntityId, Entity>
		std::list<std::pair<const worker::EntityId, worker::Entity>>::iterator it = view.Entities.find(entity_id);
		for (; it != view.Entities.end(); it++)
		{
			GetInstance().UpdateState(it->first, EState::IDLE);
		}
	}
	State::Commands::Done::Response ret = State::Commands::Done::Response::Create();
	connection.SendCommandResponse<State::Commands::Done>(op.RequestId, ret);
}

void AIWorker::UpdateState(worker::EntityId entity_id, EState state, EntityId target_id, improbable::Coordinates target_coord)
{
	State::Update update;
	update.set_state(state);
	update.set_target_id(target_id);
	update.set_target_pos(target_coord);
	m_pConnection->SendComponentUpdate<State>(entity_id, update);
}

bool AIWorker::HasStateComponent(const Entity& entity)
{
	Option<const State::Data&> state = entity.Get<State>();
	return !state.empty();
}

EntityId AIWorker::HasNearActor(const View& view, const Entity& entity)
{
	EntityId ret = -1;
	double attack_able_distance = 1.0;
	worker::Option<const PositionData&> pos = entity.Get<Position>();
	for each (auto var in view.Entities)
	{
		Entity& target_entity = var.second;
		worker::Option<PositionData&> target_pos = target_entity.Get<Position>();
		if (Distance(pos->coords(), target_pos->coords()) <= attack_able_distance)
		{
			ret = var.first;
			break;
		}
	}
	return ret;
}

double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

Coordinates AIWorker::GetRandPos()
{
	double x = fRand(-10, 10);
	double y = 0;
	double z = fRand(-10, 10);
	Coordinates ret(x, y, z);
	return ret;
}

double AIWorker::Distance(const Coordinates A, const Coordinates B)
{
	return sqrt(pow(A.x() - B.x(), 2) + 
				pow(A.y() - B.y(), 2) + 
				pow(A.z() - B.z(), 2));
}
