using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Improbable.Gdk.Subscriptions;
using Player;

public class HealthHandlerServer : MonoBehaviour
{
    [Require] private PlayerInputReader playerInputReader;
    [Require] private PlayerHealthWriter playerHealth;

    // Start is called before the first frame update
    void OnEnable()
    {
        playerInputReader.OnAttackEvent += OnAttack;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnAttack(AttackInfo info)
    {
        // retrieve information about the request
        var spatialEntity = GetComponent<LinkedEntityComponent>();
        Debug.Log("OnAttack(" + spatialEntity.EntityId + ", " + info.Target + ")");
        if (spatialEntity != null && spatialEntity.EntityId == info.Target)
        {
            var distance = Vector3.Distance(info.AttackerPosition.ToUnityVector(), transform.position);
            if (distance <= info.AttackDistance)
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
