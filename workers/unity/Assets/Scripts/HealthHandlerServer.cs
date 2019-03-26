using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Improbable.Gdk.Subscriptions;
using Player;

public class HealthHandlerServer : MonoBehaviour
{
    [Require] private PlayerHealthCommandReceiver commandReceiver;
    [Require] private PlayerHealthWriter playerHealth;

    // Start is called before the first frame update
    void OnEnable()
    {
        commandReceiver.OnDamageRequestReceived += OnDamage;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnDamage(PlayerHealth.Damage.ReceivedRequest request)
    {
        // retrieve information about the request
        var spatialEntity = GetComponent<LinkedEntityComponent>();
        Debug.Log("OnDamage(" + spatialEntity.EntityId + ", " + request.EntityId + ")");
        if (spatialEntity != null && spatialEntity.EntityId == request.EntityId)
        {
            var distance = Vector3.Distance(request.Payload.AttackerPosition.ToUnityVector(), transform.position);
            if (distance <= request.Payload.AttackDistance)
            {
                int amount = playerHealth.Data.Health - 10;
                playerHealth.SendUpdate(new PlayerHealth.Update
                {
                    Health = amount
                });
                Debug.Log("Damage");
            }
            else
            {
                Debug.Log("Miss");
            }
        }
            
    }
}
