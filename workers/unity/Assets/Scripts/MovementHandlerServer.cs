using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

using Improbable;
using Improbable.Gdk.Subscriptions;
using Player;

namespace ProtoGame
{
    public class MovementHandlerServer : MonoBehaviour
    {
        [Require] private PlayerInputReader playerInput;
        [Require] private PositionWriter authority;
        private Vector3 targetPos;
        private bool tragetUpdated;
        private NavMeshAgent _agent;
        private void OnEnable()
        {
            playerInput.OnUpdate += OnPlayerInputUpdated;
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