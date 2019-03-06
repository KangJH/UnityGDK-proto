
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
            //commandSender.CreateEntity(entityTemplate);
            return entityTemplate;
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

    }
}