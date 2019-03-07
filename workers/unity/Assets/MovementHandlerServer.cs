using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectRepresentation;
using Player;

namespace ProtoGame
{
    public class MovementHandlerServer : MonoBehaviour
    {
        [Require] private PlayerInput.Requirable.Reader playerInput;
        [Require] private Position.Requirable.Writer authority;
        private Vector3 targetPos;
        private bool tragetUpdated;
        private NavMeshAgent _agent;
        private void OnEnable()
        {
            playerInput.ComponentUpdated += OnPlayerInputUpdated;
            _agent = GetComponent<NavMeshAgent>();

            tragetUpdated = false;
        }

        void Update()
        {
            if (tragetUpdated)
            {
                _agent.destination = targetPos;
                tragetUpdated = false; // Consume target information
            }
        }

        private void OnPlayerInputUpdated(PlayerInput.Update update)
        {
            if (update.TargetPosition.HasValue)
            {
                targetPos = update.TargetPosition.Value.ToUnityVector();
                tragetUpdated = true;
            }
            
        }
    }
}