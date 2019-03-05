using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class MovementHandlerClient : MonoBehaviour
{
    public float Speed;
    public float Gravity;
    private CharacterController _controller;
    private Vector3 moveDirection = Vector3.zero;
    private void OnEnable()
    {
        _controller = GetComponent<CharacterController>();
        // prevMove = Vector3.zero;
    }

    void Update()
    {
        if (_controller.isGrounded)
        {
            // We are grounded, so recalculate
            // move direction directly from axes

            moveDirection = new Vector3(Input.GetAxis("Horizontal"), 0.0f, Input.GetAxis("Vertical"));
            moveDirection = transform.TransformDirection(moveDirection);
            moveDirection = moveDirection * Speed;

        }

        // Apply gravity
        moveDirection.y = moveDirection.y - (Gravity * Time.deltaTime);

        // Move the controller
        _controller.Move(moveDirection * Time.deltaTime);
    }
}

