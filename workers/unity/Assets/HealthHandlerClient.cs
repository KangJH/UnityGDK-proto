using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

using Improbable.Gdk.Subscriptions;

using Player;

public class HealthHandlerClient : MonoBehaviour
{
    [Require] private PlayerHealthReader playerHealth;

    public Slider HealthUI;
    // Start is called before the first frame update
    void Start()
    {
        playerHealth.OnUpdate += OnHealthUpdated;
    }

    private void OnEnable()
    {
    }

    private void OnHealthUpdated(PlayerHealth.Update update)
    {
        if (update.Health.HasValue)
        {
            HealthUI.value = update.Health.Value;
        }
        
    }
}
