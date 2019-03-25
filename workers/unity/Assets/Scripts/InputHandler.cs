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
        //[Require] private PlayerInput.Requirable.CommandRequestSender playerInputCommandSender;
        private GameObject targetObject;
        public float AttachDistance = 1.0f;
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
                            targetObject = hit.transform.gameObject;
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
                    if(distance <= AttachDistance)
                    {
                        SendAttack(AttachDistance, transform.position);
                        //targetObject.GetComponent<InputHandler>().SendAttack(AttachDistance, transform.position);
                        /*var spatialEntity = targetObject.transform.root.GetComponent<SpatialOSComponent>();
                        if (spatialEntity != null)
                        {
                            
                           // playerInputCommandSender.SendAttackRequest(spatialEntity.SpatialEntityId, new AttackInfo(0, Vector3f.FromUnityVector(transform.position), AttachDistance));

                            playerInput.SendAttack(new AttackInfo
                            {
                                Target = spatialEntity.SpatialEntityId,
                                AttackDistance = AttachDistance,
                                AttackerPosition = new Option<Vector3f>(Vector3f.FromUnityVector(transform.position))
                            });
                            Debug.Log("Attack");
                        }*/
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

        public void SendAttack(float attackDistance, Vector3 attckerPos)
        {
            var spatialEntity = targetObject.transform.root.GetComponent<LinkedEntityComponent>();
            if(spatialEntity != null)
            {
                playerInput.SendAttackEvent(new AttackInfo
                {
                    Target = spatialEntity.EntityId,
                    AttackDistance = attackDistance,
                    AttackerPosition = new Option<Vector3f>(Vector3f.FromUnityVector(attckerPos))
                });

            }
        }
    }
}
