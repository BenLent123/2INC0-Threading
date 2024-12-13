# 2INC0-Threading
Smart traffic intersection assignment.
![{1830C751-0FBD-46B1-B279-4B3A915BADC4}](https://github.com/user-attachments/assets/a2d8c62e-69a5-4042-b0fd-3b5cda3efbf2)
![{6481685B-4292-4E19-ABC6-5AA71535FA26}](https://github.com/user-attachments/assets/1a4c976f-6cce-46d6-b7df-f1c5f3d1df0c)

Our idea Threading advanced:

make a matrix [i][j]-> moves claim the mutexs of their paths, so mutliple paths are allowed if the path mutex is free
![{EA18BDFD-1215-4078-93DA-430F6B2BD934}](https://github.com/user-attachments/assets/5ac844fb-3c1f-498e-bc12-72c062c78b30)

### printing format
```c++
traffic light <side> <direction> turns green at time <t> for car <ID>
```