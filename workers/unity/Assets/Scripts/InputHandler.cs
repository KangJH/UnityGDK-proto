﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Subscriptions;

using Player;
using UnityEngine.UI;
using Improbable.Gdk.Core.Commands;
using Improbable.Worker.CInterop;

namespace ProtoGame
{
    //[WorkerType(WorkerUtils.UnityClient)]
    public class InputHandler : MonoBehaviour
    {
        [Require] private PlayerInputWriter playerInput;
        [Require] private PlayerHealthCommandSender commandSender;
        [Require] private WorldCommandSender worldComm;
        [Require] private ChatCommandSender chatCommandSender;
        private GameObject targetObject;
        private EntityId chatMgrEntityId;

        public string mytempstring = "";
        public float attackDistance = 1.0f;

        void OnEnable()
        {
        }

        void Update()
        {
            if (Input.GetKeyDown("a"))
            {
                worldComm.SendCreateEntityCommand(new WorldCommands.CreateEntity.Request(EntityCreationTemplate.CreateChatManagerEntity()), OnCreateEntityResponse);
            }

            if (!string.IsNullOrEmpty(mytempstring))
            {
                EntityId dummy = new EntityId(0);
                Receiver toWhom = new Receiver(dummy, -1, true);
                chatCommandSender.SendOutgoingMessageCommand(chatMgrEntityId, new MessageReq
                {
                    Receiver = new Option<Receiver>(toWhom),
                    Message = new Option<string>(mytempstring)
                });
                mytempstring = "";
                Debug.Log("Send.");
            }
            if (playerInput != null)
            {

                if (Input.GetMouseButtonDown(0))
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
                    if (distance <= attackDistance)
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


        public void SendChatMessage(InputField input)
        {
            // To do: Target message
            if (input.text.Length > 0)
            {
                mytempstring = string.Copy(input.text);
//                 if (chatCommandSender.IsValid)
//                 {
//                     EntityId dummy = new EntityId(0);
//                     Receiver toWhom = new Receiver(dummy, -1, true);
//                     chatCommandSender.SendOutgoingMessageCommand(chatMgrEntityId, new MessageReq
//                     {
//                         Receiver = new Option<Receiver>(toWhom),
//                         Message = new Option<string>(input.text)
//                     });
//                     mytempstring = input.text;
//                 }
//                 else
//                 {
//                     Debug.Log("Fail to send.");
//                 }
                //                 chatManager = GameObject.FindGameObjectWithTag("ChatManager").GetComponent<ChatMananger>();
                //                 if (chatManager != null && chatManager.isActiveAndEnabled)
                //                 {
                //                     EntityId dummy = new EntityId(0);
                //                     Receiver toWhom = new Receiver(dummy, -1, true); 
                // 
                //                     chatManager.SendChatMessage(toWhom, input.text);
                //                 }
                //                 else
                //                 {
                //                     Debug.Log("Fail to send.");
                //                 }
                input.text = "";
                Debug.Log(mytempstring);
            }
        }


        private void OnCreateEntityResponse(WorldCommands.CreateEntity.ReceivedResponse response)
        {
            if (response.StatusCode == StatusCode.Success)
            {
                chatMgrEntityId = response.EntityId.Value;
                // handle success
            }
            else
            {
                // handle failure
            }
        }
    }
}
