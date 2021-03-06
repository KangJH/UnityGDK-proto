﻿using System;

using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Worker;
using UnityEngine;

using Player;
using Improbable.Gdk.Subscriptions;

public class AdvancedEntityPipeline : IEntityGameObjectCreator
{
    private const string GameobjectNameFormat = "{0}(SpatialOS {1}, Worker: {2})";
    private const string WorkerAttributeFormat = "workerId:{0}";
    private const string PlayerMetadata = "Player";

    private readonly GameObject cachedAuthPlayer;
    private readonly GameObject cachedNonAuthPlayer;

    private readonly IEntityGameObjectCreator fallback;
    private readonly string workerIdAttribute;
    private readonly Worker worker;


    public AdvancedEntityPipeline(Worker worker, string authPlayer, string nonAuthPlayer,
        IEntityGameObjectCreator fallback)
    {
        this.worker = worker;
        this.fallback = fallback;
        workerIdAttribute = string.Format(WorkerAttributeFormat, worker.WorkerId);
        cachedAuthPlayer = Resources.Load<GameObject>(authPlayer);
        cachedNonAuthPlayer = Resources.Load<GameObject>(nonAuthPlayer);
    }

    public void OnEntityCreated(SpatialOSEntity entity, EntityGameObjectLinker linker)
    {
        if (!entity.HasComponent<Metadata.Component>())
        {
            return ;
        }

        var prefabName = entity.GetComponent<Metadata.Component>().EntityType;
        if (prefabName.Equals(PlayerMetadata))
        {
            var clientInput = entity.GetComponent<PlayerInput.Component>();
            if (entity.GetComponent<EntityAcl.Component>().ComponentWriteAcl
                .TryGetValue(clientInput.ComponentId, out var clientMovementWrite))
            {
                var authority = false;
                foreach (var attributeSet in clientMovementWrite.AttributeSet)
                {
                    if (attributeSet.Attribute.Contains(workerIdAttribute))
                    {
                        authority = true;
                    }
                }

                var positionComp = entity.GetComponent<Position.Component>();
                var position = positionComp.Coords.ToUnityVector();

                var prefab = authority ? cachedAuthPlayer : cachedNonAuthPlayer;
                var gameObject = UnityEngine.Object.Instantiate(prefab, position, Quaternion.identity);

                gameObject.name = GetGameObjectName(prefab, entity, worker);

                Type[] componentsToAdd =
                {
                    typeof(UnityEngine.Transform),
                    typeof(Rigidbody),
                    typeof(MeshRenderer)
                };
                linker.LinkGameObjectToSpatialOSEntity(entity.SpatialOSEntityId, gameObject, componentsToAdd);
                return;
            }
        }

        fallback.OnEntityCreated(entity, linker);
    }

    private static string GetGameObjectName(GameObject prefab, SpatialOSEntity entity, Worker worker)
    {
        return string.Format(GameobjectNameFormat, prefab.name, entity.SpatialOSEntityId, worker.WorkerType);
    }

    public void OnEntityRemoved(EntityId entityId)
    {
        fallback.OnEntityRemoved(entityId);
    }
}
