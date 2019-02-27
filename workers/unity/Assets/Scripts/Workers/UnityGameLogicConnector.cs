using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Gdk.TransformSynchronization;
using Improbable.Gdk.GameObjectRepresentation;

using Player;

using UnityEngine;
namespace ProtoGame
{
    public class UnityGameLogicConnector : DefaultWorkerConnector
    {
        private async void Start()
        {
            PlayerLifecycleConfig.CreatePlayerEntityTemplate = CreatePlayerEntityTemplate;
            await Connect(WorkerUtils.UnityGameLogic, new ForwardingDispatcher()).ConfigureAwait(false);
        }

        protected override void HandleWorkerConnectionEstablished()
        {
            Worker.World.GetOrCreateManager<MetricSendSystem>();
            PlayerLifecycleHelper.AddServerSystems(Worker.World);
            GameObjectCreationHelper.EnableStandardGameObjectCreation(Worker.World);
            GameObjectRepresentationHelper.AddSystems(Worker.World);
            TransformSynchronizationHelper.AddServerSystems(Worker.World);
        }

        private static EntityTemplate CreatePlayerEntityTemplate(string workerId, Improbable.Vector3f position)
        {
            var clientAttribute = $"workerId:{workerId}";
            var serverAttribute = WorkerUtils.UnityGameLogic;
            Debug.Log("Create Entity: " + clientAttribute);
            var template = new EntityTemplate();
            template.AddComponent(new Position.Snapshot { Coords = new Coordinates(position.X, position.Y, position.Z) }, serverAttribute);
            template.AddComponent(new Metadata.Snapshot { EntityType = "Player" }, serverAttribute);
            template.AddComponent(new PlayerInput.Snapshot { Horizontal = 0.0f, Vertical = 0.0f, Running = false }, clientAttribute);
            //template.AddComponent(new PlayerMovement.Snapshot { X = 0.0f, Y = 0.0f, Z = 0.0f }, serverAttribute);
            TransformSynchronizationHelper.AddTransformSynchronizationComponents(template, serverAttribute);
            PlayerLifecycleHelper.AddPlayerLifecycleComponents(template, workerId, clientAttribute, serverAttribute);

            template.SetReadAccess(WorkerUtils.UnityClient, /*AndroidClientWorkerConnector.WorkerType, iOSClientWorkerConnector.WorkerType,*/ serverAttribute);
            template.SetComponentWriteAccess(EntityAcl.ComponentId, serverAttribute);

            return template;
        }
    }
}
