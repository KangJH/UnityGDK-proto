using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

using Npc;
using Improbable.Gdk.Subscriptions;

public class AIStateHandler : MonoBehaviour
{
    [Require] private StateCommandSender stateCommandSender;
    [Require] private StateReader stateReader;

    public float speed;

    private Vector3 target_pos = Vector3.zero;
    private GameObject target;
    private NavMeshAgent _agent;

    void OnEnable()
    {
        stateReader.OnUpdate += OnStateUpdated;
        target_pos = transform.position;
        _agent = GetComponent<NavMeshAgent>();
    }

    void OnDisable()
    {
        target_pos = Vector3.zero;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //Movement
        if (stateReader.Data.State == EState.MOVE)
        {
            if (Vector3.Distance(transform.position, target_pos) < 1.0f)
            {
                SendDoneCommand();
            }
            else
            {
                if(_agent.destination != target_pos)
                {
                    _agent.destination = target_pos;
                }
            }
        }
        //Attack
    }

    private void OnStateUpdated(State.Update update)
    {
        if (update.State.HasValue)
        {
            if(update.State.Value == EState.MOVE)
            {
                if(update.TargetPos.HasValue)
                {
                    target_pos = update.TargetPos.Value.ToUnityVector();
                }
            }
            else if(update.State.Value == EState.ATTACK)
            {
                if (update.TargetId.HasValue)
                {
                    //target = update.TargetId.Value;
                }
            }
        }

    }

    private void SendDoneCommand()
    {
        if(stateReader.Data.State != EState.IDLE)
        {
            LinkedEntityComponent entityInfo = GetComponent<LinkedEntityComponent>();
            TDoneReq doneReq;
            doneReq.Error = 0;
            stateCommandSender.SendDoneCommand(entityInfo.EntityId, doneReq);
        }
    }

}
