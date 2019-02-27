
using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Core.Commands;
using Improbable.Gdk.GameObjectRepresentation;
using Improbable.Worker.CInterop;
using UnityEngine;

using Player;

namespace ProtoGame
{
    public class EntityCreationTemplate : MonoBehaviour
    {
        [Require] private WorldCommands.Requirable.WorldCommandRequestSender commandSender;
        [Require] private WorldCommands.Requirable.WorldCommandResponseHandler responseHandler;

        void OnEnable()
        {
            // Register callback for listening to any incoming create entity command responses for this entity
            responseHandler.OnCreateEntityResponse += OnCreateEntityResponse;
        }

        public void CreateExampleEntity()
        {
            /*var entityTemplate = new EntityTemplate();

            entityTemplate.AddComponent(new Position.Snapshot(), "UnityGameLogic");
            entityTemplate.AddComponent(new Metadata.Snapshot { EntityType = "MyPrefab" }, "UnityGameLogic");
            entityTemplate.AddComponent(new ExampleComponent.Snapshot(), "UnityGameLogic");
            entityTemplate.SetReadAccess("UnityGameLogic", "UnityClient");
            entityTemplate.SetComponentWriteAccess(EntityAcl.ComponentId, "UnityGameLogic");

            // send create entity command request without reserving an entity id
            // The SpatialOS Runtime will automatically assign a SpatialOS entity id to the newly created entity
            commandSender.CreateEntity(entityTemplate);*/
        }

        void OnCreateEntityResponse(WorldCommands.CreateEntity.ReceivedResponse response)
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

        /*public static EntityTemplate CreatePlayerEntityTemplate(Coordinates coords)
        {
            var entityTemplate = new EntityTemplate();

            entityTemplate.AddComponent(new Position.Snapshot { Coords = coords }, "UnityGameLogic");
            entityTemplate.AddComponent(new Metadata.Snapshot { EntityType = "Player" }, "UnityGameLogic");
            entityTemplate.AddComponent(new Persistence.Snapshot(), "UnityGameLogic");
            entityTemplate.AddComponent(new PlayerInput.Snapshot { Horizontal = 0.0f, Vertical = 0.0f, Running = false }, "UnityClient");
            entityTemplate.AddComponent(new PlayerMovement.Snapshot { X = 0.0f, Y = 0.0f, Z = 0.0f }, "UnityGameLogic");
            entityTemplate.SetReadAccess("UnityGameLogic", "UnityClient");
            entityTemplate.SetComponentWriteAccess(EntityAcl.ComponentId, "UnityGameLogic");

            return entityTemplate;
        }*/
    }
}