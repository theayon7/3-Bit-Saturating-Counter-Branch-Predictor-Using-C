# 3-Bit-Saturating-Counter-Branch-Predictor
- Presentation: https://www.canva.com/design/DAGxtjpcY74/J1HLky1CJpS3vlExSI7MMQ/view?utm_content=DAGxtjpcY74&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=hedc32f17e7


## **Why branch prediction?**

In a CPU, whenever we hit a **branch instruction** (like `if`, `for`, `while`, `goto`), the processor has to guess:

* Will the branch **be taken** (jump happens)?

* Or will it **not be taken** (next sequential instruction runs)?

Modern CPUs don’t like to wait for the branch to resolve, so they **predict**. If the guess is correct, execution runs smoothly. If not, the CPU has to throw away work (a **pipeline stall/misprediction penalty**).

##  **Saturating counter idea**

Instead of just guessing "always taken" or "always not-taken," we keep a **counter per branch** that learns from history.

* **3-bit counter** → range `0 … 7`

* **Prediction rule**:

  * If counter ≥ 4 → predict **TAKEN**

  * If counter ≤ 3 → predict **NOT-TAKEN**

So the predictor is like a **7-point confidence meter**:

`0   1   2   3   |   4   5   6   7`  
`<-- predict N --|-- predict T -->`

## **Update logic**

Every time the branch executes:

* If branch was **taken** → increment the counter (but don’t exceed 7).

* If branch was **not taken** → decrement the counter (but don’t go below 0).

This means:

* If a branch is usually taken → the counter drifts upward → predictor starts saying "taken."

* If a branch is usually not-taken → the counter drifts downward → predictor starts saying "not-taken."

* Occasional exceptions (like a loop exit) don’t flip the prediction immediately — because you’d need multiple outcomes in a row to cross the threshold. That’s the **saturation effect**.

##  **Example walk-through**

Suppose PC=100 has counter starting at **3** (weakly not-taken):

1. Branch executes, outcome \= **TAKEN**

   * Counter goes 3 → 4

   * Prediction flips to **taken**.

2. Next time, branch is **TAKEN** again

   * Counter 4 → 5 (still predict taken).

3. Suddenly branch is **NOT-TAKEN** (rare case, like loop exit)

   * Counter 5 → 4 → still predicts taken next time\!

   * So predictor is **resistant to noise**.

Only if the branch keeps being **not-taken** will the counter drift down past 3 and change the prediction.

##  **Why 3 bits?**

* **1-bit counter**: too sensitive, flips prediction after one mispredict.

* **2-bit counter**: classic (used in real CPUs), requires 2 mispredictions in a row to flip.

* **3-bit counter**: even stronger memory, requires up to 4 mispredictions to flip.  
   Useful if branches are highly biased (e.g., loops almost always taken except at exit).

So more bits \= more inertia (slower to adapt, but more stable).

**In short**:  
 The 3-bit saturating counter is like a **confidence dial** for each branch.

* Counter high → strongly believe the branch is taken.

* Counter low → strongly believe not-taken.

* Updates gradually so the predictor isn’t fooled by one odd branch outcome.

When a CPU encounters a branch instruction (like `if` or `loop`), it must decide **whether the branch will be taken or not taken** before the actual condition is known.  
 To reduce pipeline stalls, CPUs use a **branch predictor**.

The **saturating counter predictor** is a common technique:

### **1\. Counter States**

We use a small counter (2-bit, 3-bit, etc.). Here, we use a **3-bit counter** (0–7).

* **0–1** → Strongly Not Taken

* **2–3** → Weakly Not Taken

* **4–5** → Weakly Taken

* **6–7** → Strongly Taken

The prediction is:

* **Taken** if counter ≥ 4

* **Not Taken** if counter ≤ 3

### **2\. How It Works**

* Start at **weakly not taken (3)**.

* For every branch outcome:

  * If the branch was **Taken**, increase the counter (up to max 7).

  * If the branch was **Not Taken**, decrease the counter (down to min 0).

* This way the predictor gradually “learns” a branch’s behavior.

### **3\. Example**

* If a branch is taken 10 times in a row → counter shifts to **strongly taken (7)**.

* If later the branch flips once to **not taken**, it won’t immediately switch prediction → it only moves to **weakly taken (5)**.

* This prevents “flipping” from one wrong case, which makes it **stable**.

# **Code Explanation**

Now, let’s connect this to the code I gave you.

### **1\. Setup**

`#define TABLE_SIZE 8  // 8-entry table for 3-bit saturating counter`  
`int counters[TABLE_SIZE] = {0};`

We create an **8-entry prediction table**.  
 Each entry is a 3-bit counter (values 0–7).  
 All counters are initialized to **3 (weakly not taken)**.

### **2\. State Function**

`const char* getState(int counter) {`  
    `if (counter <= 1) return "Strongly Not Taken";`  
    `else if (counter <= 3) return "Weakly Not Taken";`  
    `else if (counter <= 5) return "Weakly Taken";`  
    `else return "Strongly Taken";`  
`}`

This converts the counter value into a **human-readable state**.

### **3\. Branch Patterns**

`char *patterns[4] = {`  
    `"TTTTTTTTTT",   // always taken`  
    `"NNNNNNNNNN",   // always not taken`  
    `"TNTNTNTNTN",   // alternating`  
    `"TTTTTNNNNN"    // biased taken`  
`};`

We simulate **4 members**, each with their own branch behavior.

### **4\. Prediction Logic**

`int prediction = (counters[index] >= 4) ? 'T' : 'N';`  
`char actual = patterns[i][j];`  
`int correct = (prediction == actual);`

* If counter ≥ 4 → predict Taken.

* If counter ≤ 3 → predict Not Taken.

* Compare prediction with the **actual branch outcome**.

### **5\. Update Counter**

`if (actual == 'T' && counters[index] < 7) counters[index]++;`  
`else if (actual == 'N' && counters[index] > 0) counters[index]--;`

* If branch was **Taken**, increase counter (up to 7).

* If **Not Taken**, decrease counter (down to 0).

This is the **saturating behavior** → counter never goes below 0 or above 7\.

### **6\. Statistics**

`total_predictions++;`  
`if (correct) total_correct++;`

We track **accuracy** (how often prediction \= actual).

At the end, we print:

`printf("Team Prediction Accuracy: %.2f%%\n",`  
       `(total_correct / total_predictions) * 100);`

### **7\. Output**

At the end, we display:

* For each step: prediction, actual, counter value, and correctness.

* Final state of each counter (with Strong/Weak labels).

* Total team accuracy %.

# **In Simple Words**

* **We built a learning system** that guesses whether a branch will be taken or not.

* **It remembers past outcomes** in a small counter (0–7).

* **Strongly Taken/Not Taken** → requires several wrong outcomes to flip.

* **Weakly Taken/Not Taken** → easy to flip with one wrong outcome.

The program shows how accurate this method is on different branch patterns.
