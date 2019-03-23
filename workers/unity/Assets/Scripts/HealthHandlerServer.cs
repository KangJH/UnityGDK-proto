using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Improbable.Gdk.GameObjectRepresentation;
using Player;

public class HealthHandlerServer : MonoBehaviour
{
    [Require] private PlayerInput.Requirable.Reader playerInputReader;
    [Require] private PlayerHealth.Requirable.Writer playerHealth;

    // Start is called before the first frame update
    void OnEnable()
    {
        playerInputReader.OnAttack += OnAttack;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnAttack(AttackInfo info)
    {
        // retrieve information about the request
        var spatialEntity = GetComponent<SpatialOSComponent>();
        Debug.Log("OnAttack(" + spatialEntity.SpatialEntityId + ", " + info.Target + ")");
        if (spatialEntity != null && spatialEntity.SpatialEntityId == info.Target)
        {
            var distance = Vector3.Distance(info.AttackerPosition.ToUnityVector(), transform.position);
            if (distance <= info.AttackDistance)
            {
                int amount = playerHealth.Data.Health - 10;
                playerHealth.Send(new PlayerHealth.Update
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
