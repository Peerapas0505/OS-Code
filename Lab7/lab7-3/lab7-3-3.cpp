Scenario:

4 processes (P0, P1, P2, P3)
2 resource types (R0, R1)
Available: R0=0, R1=0
Allocation:
     R0  R1
P0:   1   0
P1:   0   1
P2:   1   0
P3:   0   1
Request:
     R0  R1
P0:   0   1
P1:   1   0
P2:   0   1
P3:   1   0
Questions:
1.Draw the wait-for graph
2.Is there a deadlock? If yes, which processes are deadlock?
3.Suggest a recovery strategy:
Answer:
1. Wait-For Graph:

P0 → P1 (P0 waits for R1 held by P1)
P1 → P0 (P1 waits for R0 held by P0)
P2 → P1 (P2 waits for R1 held by P1)
P3 → P0 (P3 waits for R0 held by P0)
2. Deadlock: YES. P0 and P1 are in a circular wait.

3. Recovery:

Terminate P0 or P1 (victim selection)
Rollback P0 or P1 to previous state
Preempt R0 from P0 or R1 from P1
