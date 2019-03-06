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
            //Decide spawn position
            GameObject[] playerSpawnPoints = GameObject.FindGameObjectsWithTag("SpawnPoint");
            Vector3 spawnPos = playerSpawnPoints[Random.Range(0, playerSpawnPoints.Length - 1)].transform.position;
            foreach (GameObject go in playerSpawnPoints)
                Debug.Log("Spawn candidate:" + go.transform.position);
            Debug.Log("Spawned Pos:" + spawnPos);
            //Setup SpatialOS entity and components
            var clientAttribute = $"workerId:{workerId}";
            var serverAttribute = WorkerUtils.UnityGameLogic;
            Debug.Log("Create Entity: " + clientAttribute);
            var template = new EntityTemplate();
            template.AddComponent(new Position.Snapshot { Coords = new Coordinates(spawnPos.x, spawnPos.y, spawnPos.z) }, serverAttribute);
            template.AddComponent(new Metadata.Snapshot { EntityType = "Player" }, serverAttribute);
            template.AddComponent(new PlayerInput.Snapshot { MouseX = 0.0f, MouseY = 0.0f, MouseBtn0 = false, MouseBtn1 = false }, clientAttribute);
            TransformSynchronizationHelper.AddTransformSynchronizationComponents(template, serverAttribute);
            PlayerLifecycleHelper.AddPlayerLifecycleComponents(template, workerId, clientAttribute, serverAttribute);

            template.SetReadAccess(WorkerUtils.UnityClient, /*AndroidClientWorkerConnector.WorkerType, iOSClientWorkerConnector.WorkerType,*/ serverAttribute);
            template.SetComponentWriteAccess(EntityAcl.ComponentId, serverAttribute);

            return template;
        }
    }
}
