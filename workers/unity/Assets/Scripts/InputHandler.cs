using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Improbable;
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
                if(Input.GetMouseButtonDown(0))
                {
                    Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
                    RaycastHit hit;
                    if (Physics.Raycast(ray, out hit, 100))
                    {
                        playerInput.Send(new PlayerInput.Update
                        {
                            TargetPosition = new Option<Vector3f>(Vector3f.FromUnityVector(hit.point)),
                        });
                    }
                }
            }
        }
    }
}
