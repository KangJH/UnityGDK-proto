#pragma once

#include <improbable/worker.h>
#include "npc/state.h"
#include <improbable/view.h>
#include <improbable/standard_library.h>

class AIWorker
{
public:
	static AIWorker& GetInstance()
	{
		static AIWorker aiWorker;
		return aiWorker;
	}
	void Initialize(worker::View& view, worker::Connection& connection);
	static void Process();
private:
	AIWorker();
	~AIWorker();
	AIWorker(const AIWorker&);
	AIWorker& operator=(const AIWorker&);

	worker::View* m_pView;
	worker::Connection* m_pConnection;

	void UpdateState(worker::EntityId entity_id, npc::EState state, worker::EntityId target_id = -1, improbable::Coordinates target_coord = { 0, 0, 0 });

	static void OnCommandRequest(const worker::CommandRequestOp<npc::State::Commands::Done>& op);
	static bool HasStateComponent(const worker::Entity& entity);
	static worker::EntityId HasNearActor(const worker::View& view, const worker::Entity& entity);
	static improbable::Coordinates GetRandPos();
	static double Distance(const improbable::Coordinates A, const improbable::Coordinates B);
	
};

