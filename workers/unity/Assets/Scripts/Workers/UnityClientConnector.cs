using Improbable.Gdk.Core;
using Improbable.Gdk.PlayerLifecycle;
using Improbable.Worker.CInterop;
using Improbable.Gdk.GameObjectCreation;
using Improbable.Gdk.GameObjectRepresentation;
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
            GameObjectCreationHelper.EnableStandardGameObjectCreation(Worker.World);
            GameObjectRepresentationHelper.AddSystems(Worker.World);
            TransformSynchronizationHelper.AddClientSystems(Worker.World);

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
