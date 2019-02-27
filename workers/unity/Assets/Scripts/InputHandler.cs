using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Improbable.Gdk.Core;
using Improbable.Gdk.GameObjectRepresentation;

using Player;

namespace ProtoGame
{
    //[WorkerType(WorkerUtils.UnityClient)]
    public class InputHandler : MonoBehaviour
    {
        [Require] private PlayerInput.Requirable.Writer playerInput;
        //private CharacterController _controller;

        void OnEnable()
        {
            //_controller = GetComponent<CharacterController>();
            Debug.Log("InputHandler start");
        }

        void Update()
        {
            if(playerInput != null)
            {
                playerInput.Send(new PlayerInput.Update
                {
                    Horizontal = new Option<float>(Input.GetAxis("Horizontal")),
                    Vertical = new Option<float>(Input.GetAxis("Vertical"))
                });
                //Debug.Log("User Updated.(" + Input.GetAxis("Horizontal") + ", " + Input.GetAxis("Vertical") + ")");

            }
            /* var workerSystem = World.GetExistingManager<WorkerSystem>();

             workerSystem.LogDispatcher.HandleLog(LogType.Error, new LogEvent(
                     "InputHandler Update.")
                 .WithField("User Input_H", Input.GetAxis("Horizontal"))
                 .WithField("User Input_V", Input.GetAxis("Vertical"))
                 );*/
            //Vector3 move = new Vector3(Input.GetAxis("Horizontal"), 0, Input.GetAxis("Vertical"));
            //_controller.Move(move * Time.deltaTime * Speed);
            //if (move != Vector3.zero)
            //    transform.forward = move;
        }
    }
}
