using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectRepresentation;

using Player;

namespace ProtoGame
{
    public class MovementHandlerClient : MonoBehaviour
    {
        [Require] private PlayerMovement.Requirable.Reader playerMovement;
        private CharacterController _controller;
        private Vector3 move;

        void Start()
        {
            _controller = GetComponent<CharacterController>();
        }
        private void OnEnable()
        {
            playerMovement.ComponentUpdated += OnPlayerMovementUpdated;
            move = Vector3.zero;
        }
        // Update is called once per frame
        void Update()
        {
            _controller.Move(move);
            if (move != Vector3.zero)
                transform.forward = move;
        }

        private void OnPlayerMovementUpdated(PlayerMovement.Update update)
        {
            move = new Vector3(update.X, update.Y, update.Z);
            Debug.Log("move updated.(" + move.x + ", " + move.y + ", " + move.z + ")");
        }
    }
}
