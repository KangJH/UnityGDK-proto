using Improbable.Gdk.Core;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Worker.CInterop;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Gdk.TransformSynchronization;

using UnityEngine;
using UnityEngine.UI;

namespace ProtoGame
{
    public class UnityClientConnector : DefaultWorkerConnector
    {
        public GameObject JoinButton;
        public GameObject LeaveButton;
        public GameObject ErrorMsg;

        private const string AuthPlayer = "Prefabs/UnityClient/Authoritative/Player";
        private const string NonAuthPlayer = "Prefabs/UnityClient/NonAuthoritative/Player";

        private void Start()
        {
        }

        public async void ConnectToSpatialOS()
        {
            JoinButton.GetComponent<Button>().interactable = false;
            ErrorMsg.GetComponent<Text>().text = "";
            await Connect(WorkerUtils.UnityClient, new ForwardingDispatcher()).ConfigureAwait(false);

        }

        public void DisconnectToSpatialOS()
        {
            Dispose();
            LeaveButton.SetActive(false);
            JoinButton.SetActive(true);
        }

        protected override void HandleWorkerConnectionEstablished()
        {
            PlayerLifecycleHelper.AddClientSystems(Worker.World);
            TransformSynchronizationHelper.AddClientSystems(Worker.World);

            // Set the Worker gameObject to the ClientWorker so it can access PlayerCreater reader/writers
            var fallback = new GameObjectCreatorFromMetadata(Worker.WorkerType, Worker.Origin, Worker.LogDispatcher);
            GameObjectCreationHelper.EnableStandardGameObjectCreation(
                Worker.World,
                new AdvancedEntityPipeline(Worker, AuthPlayer, NonAuthPlayer, fallback),
                gameObject);



            JoinButton.GetComponent<Button>().interactable = true;
            JoinButton.SetActive(false);
            LeaveButton.SetActive(true);
        }

        protected override void HandleWorkerConnectionFailure(string errorMessage)
        {
            ErrorMsg.GetComponent<Text>().text = errorMessage;
            JoinButton.GetComponent<Button>().interactable = true;
        }

        protected override string SelectDeploymentName(DeploymentList deployments)
        {
            return deployments.Deployments[0].DeploymentName;
        }

        public override void Dispose()
        {
            Worker?.Dispose();
            Worker = null;
        }
    }
}
