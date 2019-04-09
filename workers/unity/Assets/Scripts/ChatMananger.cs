using System.Collections;
using System.Collections.Generic;

using UnityEngine;

using Improbable;
using Improbable.Gdk.Core;
using Improbable.Gdk.Subscriptions;

using Player;

public class ChatMananger : MonoBehaviour
{
    [Require] private ChatCommandSender chatCommandSender;
    //[Require] private ChatE

    public bool SendChatMessage(Receiver receiver, string message)
    {
        if(isActiveAndEnabled)
        {
            var spatialEntity = GetComponent<LinkedEntityComponent>();
            if (spatialEntity != null)
            {
                chatCommandSender.SendOutgoingMessageCommand(spatialEntity.EntityId, new MessageReq
                {
                    Receiver = new Option<Receiver>(receiver),
                    Message = new Option<string>(message)
                });
                return true;
            }
        }
        return false;
    }
}
