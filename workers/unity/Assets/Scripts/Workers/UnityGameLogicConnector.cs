using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Gdk.TransformSynchronization;
using Improbable.Gdk;

using Player;

using UnityEngine;
namespace ProtoGame
{
    public class UnityGameLogicConnector : DefaultWorkerConnector
    {
        private async void Start()
        {
            PlayerLifecycleConfig.CreatePlayerEntityTemplate = EntityCreationTemplate.CreatePlayerEntityTemplate;
            await Connect(WorkerUtils.UnityGameLogic, new ForwardingDispatcher()).ConfigureAwait(false);
        }

        protected override void HandleWorkerConnectionEstablished()
        {
            Worker.World.GetOrCreateManager<MetricSendSystem>();
            PlayerLifecycleHelper.AddServerSystems(Worker.World);
            GameObjectCreationHelper.EnableStandardGameObjectCreation(Worker.World);
            //GameObjectRepresentationHelper.AddSystems(Worker.World);
            TransformSynchronizationHelper.AddServerSystems(Worker.World);
        }
    }
}
