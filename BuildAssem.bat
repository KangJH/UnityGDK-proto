@echo off

spatial cloud upload UnityProto --force
spatial cloud delete myunityfirstproject
spatial cloud launch UnityProto cloud_launch.json myunityfirstproject --snapshot=snapshots/default.snapshot
