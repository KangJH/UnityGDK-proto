using System.Collections;
using System.Collections.Generic;
using UnityEngine;

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
        private float horizontal_input;
        private float vertical_input;
       // private Vector3 prevMove;
        public float Speed;
        public float Gravity;
        private CharacterController _controller;
        private Vector3 moveDirection = Vector3.zero;
        private void OnEnable()
        {
            playerInput.ComponentUpdated += OnPlayerInputUpdated;
            _controller = GetComponent<CharacterController>();
           // prevMove = Vector3.zero;
        }

        void Update()
        {
            //Debug.Log("("+ horizontal_input + ", " + vertical_input + ")(" + transform.position.x + ", " + transform.position.y + ", " + transform.position.z + ")");
            //Vector3 move = new Vector3(horizontal_input, 0, vertical_input);
            //move = move * Time.deltaTime * Speed;
            //_controller.Move(move);
            //if (move != Vector3.zero)
            //    transform.forward = move;


            if (_controller.isGrounded)
            {
                // We are grounded, so recalculate
                // move direction directly from axes

                moveDirection = new Vector3(horizontal_input, 0.0f, vertical_input);
                moveDirection = transform.TransformDirection(moveDirection);
                moveDirection = moveDirection * Speed;

                //if (Input.GetButton("Jump"))
                //{
                //    moveDirection.y = jumpSpeed;
                //}
            }

            // Apply gravity
            moveDirection.y = moveDirection.y - (Gravity * Time.deltaTime);

            // Move the controller
            _controller.Move(moveDirection * Time.deltaTime);
        }

        private void OnPlayerInputUpdated(PlayerInput.Update update)
        {
            if(update.Horizontal.HasValue)
            {
                horizontal_input = update.Horizontal;
            }

            if (update.Vertical.HasValue)
            {
                vertical_input = update.Vertical;
            }
        }
    }
}