
using UnityEngine;

using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Core.Commands;
using Improbable.Gdk;
using Improbable.Worker.CInterop;
using Improbable.Gdk.TransformSynchronization;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Gdk.Subscriptions;

using Player;
using Npc;

namespace ProtoGame
{
    public class EntityCreationTemplate : MonoBehaviour
    {
        //[Require] private WorldCommandSender commandSender;
        void OnEnable()
        {
        }

        static public EntityTemplate CreateLandEntity()
        {
            var entityTemplate = new EntityTemplate();

            entityTemplate.AddComponent(new Position.Snapshot(), WorkerUtils.UnityGameLogic);
            entityTemplate.AddComponent(new Metadata.Snapshot { EntityType = "Low_Poly_Boat_Yard" }, WorkerUtils.UnityGameLogic);
            entityTemplate.AddComponent(new Persistence.Snapshot(), WorkerUtils.UnityGameLogic);
            entityTemplate.SetReadAccess(WorkerUtils.UnityGameLogic, WorkerUtils.UnityClient);
            entityTemplate.SetComponentWriteAccess(EntityAcl.ComponentId, WorkerUtils.UnityGameLogic);

            // send create entity command request without reserving an entity id
            // The SpatialOS Runtime will automatically assign a SpatialOS entity id to the newly created entity
            //commandSender.SendCreateEntityCommand(new WorldCommands.CreateEntity.Request(entityTemplate), OnCreateEntityResponse);
            return entityTemplate;
        }

        static public EntityTemplate CreateNPCEntity()
        {
            var entityTemplate = new EntityTemplate();
            Coordinates coord = new Coordinates((double)(Random.Range(16.0f, 60.0f)), (double)3.0f, (double)(Random.Range(3.0f, 46.0f)));
            entityTemplate.AddComponent(new Position.Snapshot { Coords = coord }, WorkerUtils.UnityGameLogic);
            entityTemplate.AddComponent(new Metadata.Snapshot { EntityType = "NPC" }, WorkerUtils.UnityGameLogic);
            entityTemplate.AddComponent(new Persistence.Snapshot(), WorkerUtils.UnityGameLogic);
            entityTemplate.AddComponent(new State.Snapshot(), WorkerUtils.AIWorker);
            TransformSynchronizationHelper.AddTransformSynchronizationComponents(entityTemplate, WorkerUtils.UnityGameLogic);

            entityTemplate.SetReadAccess(WorkerUtils.UnityGameLogic, WorkerUtils.UnityClient, WorkerUtils.AIWorker);
            entityTemplate.SetComponentWriteAccess(EntityAcl.ComponentId, WorkerUtils.AIWorker);

            // send create entity command request without reserving an entity id
            // The SpatialOS Runtime will automatically assign a SpatialOS entity id to the newly created entity
            //commandSender.SendCreateEntityCommand(new WorldCommands.CreateEntity.Request(entityTemplate), OnCreateEntityResponse);
            return entityTemplate;
        }

        static public EntityTemplate CreatePlayerEntityTemplate(string workerId, byte[] playerCreationArguments)
        {
            //Decide spawn position
            GameObject[] playerSpawnPoints = GameObject.FindGameObjectsWithTag("SpawnPoint");
            Vector3 spawnPos = playerSpawnPoints[Random.Range(0, playerSpawnPoints.Length - 1)].transform.position;

            //Setup SpatialOS entity and components
            var clientAttribute = $"workerId:{workerId}";
            var serverAttribute = WorkerUtils.UnityGameLogic;

            var template = new EntityTemplate();
            template.AddComponent(new Position.Snapshot { Coords = new Coordinates(spawnPos.x, spawnPos.y, spawnPos.z) }, serverAttribute);
            template.AddComponent(new Metadata.Snapshot { EntityType = "Player" }, serverAttribute);
            template.AddComponent(new PlayerInput.Snapshot { TargetPosition = Vector3f.Zero }, clientAttribute);
            template.AddComponent(new PlayerHealth.Snapshot { Health = 100 }, serverAttribute);
            template.AddComponent(new Chat.Snapshot(), WorkerUtils.ChatManager);
            TransformSynchronizationHelper.AddTransformSynchronizationComponents(template, serverAttribute);
            PlayerLifecycleHelper.AddPlayerLifecycleComponents(template, workerId, clientAttribute, serverAttribute);

            template.SetReadAccess(WorkerUtils.UnityClient, WorkerUtils.ChatManager, /*AndroidClientWorkerConnector.WorkerType, iOSClientWorkerConnector.WorkerType,*/ serverAttribute);
            template.SetComponentWriteAccess(EntityAcl.ComponentId, serverAttribute);

            return template;
        }

        static private void OnCreateEntityResponse(WorldCommands.CreateEntity.ReceivedResponse response)
        {
            if (response.StatusCode == StatusCode.Success)
            {
                var createdEntityId = response.EntityId.Value;
                // handle success
            }
            else
            {
                // handle failure
            }
        }
    }
}