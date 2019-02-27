using Improbable.Gdk.Core;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Worker.CInterop;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Gdk.GameObjectRepresentation;
using Improbable.Gdk.TransformSynchronization;

namespace ProtoGame
{
    public class UnityClientConnector : DefaultWorkerConnector
    { 
        private async void Start()
        {
            await Connect(WorkerUtils.UnityClient, new ForwardingDispatcher()).ConfigureAwait(false);
        }

        protected override void HandleWorkerConnectionEstablished()
        {
            PlayerLifecycleHelper.AddClientSystems(Worker.World);
            GameObjectCreationHelper.EnableStandardGameObjectCreation(Worker.World);
            GameObjectRepresentationHelper.AddSystems(Worker.World);
            TransformSynchronizationHelper.AddClientSystems(Worker.World);
        }

        protected override string SelectDeploymentName(DeploymentList deployments)
        {
            return deployments.Deployments[0].DeploymentName;
        }
    }
}
