using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Subscriptions;

using Player;

namespace ProtoGame
{
    //[WorkerType(WorkerUtils.UnityClient)]
    public class InputHandler : MonoBehaviour
    {
        [Require] private PlayerInputWriter playerInput;
        [Require] private PlayerHealthCommandSender commandSender;
        //[Require] private PlayerInput.Requirable.CommandRequestSender playerInputCommandSender;
        private GameObject targetObject;
        public float attackDistance = 1.0f;

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
                    if (Physics.Raycast(ray, out hit))
                    {
                        // The clicked object is Player, track the other player's target
                        if (hit.transform.root.tag == "Player")
                        {
                            targetObject = hit.transform.root.gameObject;
                        }
                        else
                        {
                            SendTargetPosition(hit.point);
                        }
                    }
                }

                if (targetObject != null)
                {
                    var distance = Vector3.Distance(transform.position, targetObject.transform.position);
                    if(distance <= attackDistance)
                    {
                        //var inputhandler = targetObject.GetComponent<InputHandler>();
                        SendAttack(targetObject, attackDistance, transform.position);
                        targetObject = null;
                    }
                    else
                    {
                        SendTargetPosition(targetObject.transform.position);
                    }
                }
            }
        }

        void SendTargetPosition(Vector3 pos)
        {
            playerInput.SendUpdate(new PlayerInput.Update
            {
                TargetPosition = new Option<Vector3f>(Vector3f.FromUnityVector(pos)),
            });
        }

        public void SendAttack(GameObject target, float attackDistance, Vector3 attckerPos)
        {
            if (commandSender != null)
            {
                var spatialEntity = target.GetComponent<LinkedEntityComponent>();
                if (spatialEntity != null)
                {
                    commandSender.SendDamageCommand(spatialEntity.EntityId, new DamageRequest
                    {
                        AttackDistance = new Option<float>(attackDistance),
                        AttackerPosition = new Option<Vector3f>(Vector3f.FromUnityVector(attckerPos))
                    });
                }
            }
        }
    }
}
