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
    [SerializeField]
    List<Message> messageList = new List<Message>();
    public int maxMessages = 25;
    [Require] private ChatCommandSender chatCommandSender;
    [Require] private ChatReader chatReader;
    private GameObject chatPanel;
    public GameObject outputText;


    void OnEnable()
    {
        InputField chatInput = GameObject.FindGameObjectWithTag("ChatInput").GetComponent<InputField>();
        chatInput.onEndEdit.AddListener(delegate { SendChatMessage(chatInput); });
        chatPanel = GameObject.FindGameObjectWithTag("ChatOutput");
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
        if(messageList.Count >= maxMessages)
        {
            Destroy(messageList[0].textObject.gameObject);
            messageList.Remove(messageList[0]);
        }
        GameObject newText = Instantiate(outputText, chatPanel.transform);

        Message newMessage = new Message();
        newMessage.textObject = newText.GetComponent<Text>();
        newMessage.textObject.text = input.Sender.ToString() + " : " + input.Message;
        messageList.Add(newMessage);
    }
}

[System.Serializable]
public class Message
{
    public Text textObject;
}
