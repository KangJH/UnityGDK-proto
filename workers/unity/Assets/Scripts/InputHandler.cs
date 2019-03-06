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

        void OnEnable()
        {
        }

        void Update()
        {
            if(playerInput != null)
            {
                if(Input.GetMouseButtonDown(0) || Input.GetMouseButtonUp(0) ||
                   Input.GetMouseButtonDown(1) || Input.GetMouseButtonUp(1))
                {
                    playerInput.Send(new PlayerInput.Update
                    {
                        MouseX = new Option<float>(Input.mousePosition.x),
                        MouseY = new Option<float>(Input.mousePosition.y),
                        MouseBtn0 = new Option<BlittableBool>(Input.GetMouseButtonDown(0)),
                        MouseBtn1 = new Option<BlittableBool>(Input.GetMouseButtonDown(1))
                    });
                }
            }
        }
    }
}
