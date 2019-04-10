using System.Collections;
using System.Collections.Generic;

using UnityEngine;
using UnityEngine.UI;

using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Subscriptions;

using Player;
using System;

public class ChatHandler : MonoBehaviour
{
    [Require] private ChatCommandSender chatCommandSender;
    [Require] private ChatReader chatReader;

    private Text outputText;

    void OnEnable()
    {
        InputField chatInput = GameObject.FindGameObjectWithTag("ChatInput").GetComponent<InputField>();
        chatInput.onEndEdit.AddListener(delegate { SendChatMessage(chatInput); });
        outputText = GameObject.FindGameObjectWithTag("ChatOutput").GetComponent<Text>();
        chatReader.OnIncomingMessageEvent += OnChatIncoming;
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
                var spatialEntity = GetComponent<LinkedEntityComponent>();
                chatCommandSender.SendOutgoingMessageCommand(spatialEntity.EntityId, new MessageReq
                {
                    Receiver = new Option<Receiver>(toWhom),
                    Message = new Option<string>(input.text)
                });
                Debug.Log("Send:" + spatialEntity.EntityId.ToString());
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
        outputText.text = outputText.text + input.Sender.ToString() + " : " + input.Message + Environment.NewLine;
    }
}
