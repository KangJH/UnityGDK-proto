using Improbable;
using Improbable.Gdk.Core;
using Improbable.PlayerLifecycle;
using UnityEngine;
using Snapshot = Improbable.Gdk.Core.Snapshot;

//using ProtoGame;

namespace ProtoGame.Editor
{
    internal static class SnapshotGenerator
    {
        public struct Arguments
        {
            public string OutputPath;
        }

        public static void Generate(Arguments arguments)
        {
            var snapshot = CreateSnapshot();
            snapshot.WriteToFile(arguments.OutputPath);
        }

        private static Snapshot CreateSnapshot()
        {
            var snapshot = new Snapshot();
            AddPlayerSpawner(snapshot);

            snapshot.AddEntity(EntityCreationTemplate.CreateLandEntity());
            for(int i = 0; i < 10; i++)
            {
                snapshot.AddEntity(EntityCreationTemplate.CreateNPCEntity());
            }
            return snapshot;
        }

        private static void AddPlayerSpawner(Snapshot snapshot)
        {
            var serverAttribute = WorkerUtils.UnityGameLogic;
            
            var template = new EntityTemplate();
            template.AddComponent(new Position.Snapshot(), serverAttribute);
            template.AddComponent(new Metadata.Snapshot { EntityType = "PlayerCreator" }, serverAttribute);
            template.AddComponent(new Persistence.Snapshot(), serverAttribute);
            template.AddComponent(new PlayerCreator.Snapshot(), serverAttribute);

            template.SetReadAccess(WorkerUtils.UnityClient, WorkerUtils.UnityGameLogic/*, AndroidClientWorkerConnector.WorkerType, iOSClientWorkerConnector.WorkerType*/);
            template.SetComponentWriteAccess(EntityAcl.ComponentId, serverAttribute);

            snapshot.AddEntity(template);
        }
    }
}
