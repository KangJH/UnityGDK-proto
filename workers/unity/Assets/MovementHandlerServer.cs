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
        private float mouseX, mouseY;
        private bool mouse0Button, mouse1Button;
        private NavMeshAgent _agent;
        private void OnEnable()
        {
            playerInput.ComponentUpdated += OnPlayerInputUpdated;
            _agent = GetComponent<NavMeshAgent>();
        }

        void Update()
        {
            if (mouse0Button)
            {
                Ray ray = Camera.main.ScreenPointToRay(new Vector3(mouseX, mouseY, 0));
                RaycastHit hit;
                if (Physics.Raycast(ray, out hit, 100))
                {
                    _agent.destination = hit.point;
                }
                mouse0Button = false; // consume mouse click
            }
        }

        private void OnPlayerInputUpdated(PlayerInput.Update update)
        {
            if(update.MouseX.HasValue)
            {
                mouseX = update.MouseX;
            }

            if (update.MouseY.HasValue)
            {
                mouseY = update.MouseY;
            }

            if (update.MouseBtn0.HasValue)
            {
                mouse0Button = update.MouseBtn0.Value;
            }
        }
    }
}