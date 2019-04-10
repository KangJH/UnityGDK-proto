using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Subscriptions;

using Player;
using UnityEngine.UI;
using Improbable.Gdk.Core.Commands;
using Improbable.Worker.CInterop;
using System;

namespace ProtoGame
{
    //[WorkerType(WorkerUtils.UnityClient)]
    public class InputHandler : MonoBehaviour
    {
        [Require] private PlayerInputWriter playerInput;
        [Require] private PlayerHealthCommandSender commandSender;
        [Require] private WorldCommandSender worldComm;
        [Require] private ChatCommandSender chatCommandSender;
        [Require] private ChatReader chatReader;
        private GameObject targetObject;
        private EntityId chatMgrEntityId;

        private Text outputText;

        public float attackDistance = 1.0f;

        void OnEnable()
        {
            InputField chatInput = GameObject.FindGameObjectWithTag("ChatInput").GetComponent<InputField>();
            chatInput.onEndEdit.AddListener(delegate { SendChatMessage(chatInput); });
            outputText = GameObject.FindGameObjectWithTag("ChatOutput").GetComponent<Text>();
            chatReader.OnIncomingMessageEvent += OnChatIncoming;
        }

        void Update()
        {
            if (Input.GetKeyDown("a"))
            {
                worldComm.SendCreateEntityCommand(new WorldCommands.CreateEntity.Request(EntityCreationTemplate.CreateChatManagerEntity()), OnCreateEntityResponse);
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
                if (chatCommandSender.IsValid)
                {
                    EntityId dummy = new EntityId(0);
                    Receiver toWhom = new Receiver(dummy, -1, true);
                    chatCommandSender.SendOutgoingMessageCommand(chatMgrEntityId, new MessageReq
                    {
                        Receiver = new Option<Receiver>(toWhom),
                        Message = new Option<string>(input.text)
                    });
                    Debug.Log("Send:" + chatMgrEntityId.ToString());
                }
                else
                {
                    Debug.Log("Fail to send.");
                }
                input.text = "";
            }
        }

        public void OnChatIncoming(MessageEvent input)
        {
            outputText.text = outputText.text + input.Message + Environment.NewLine;
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
